<?php
/* vim: set expandtab sw=4 ts=4 sts=4: */
/**
 * Library for extracting information about the partitions
 *
 * @package PhpMyAdmin
 */
namespace PhpMyAdmin;

use PhpMyAdmin\SubPartition;

/**
 * base Partition Class
 *
 * @package PhpMyAdmin
 */
class Partition extends SubPartition
{
    /**
     * @var string partition description
     */
    protected $description;
    /**
     * @var SubPartition[] sub partitions
     */
    protected $subPartitions = array();

    /**
     * Loads data from the fetched row from information_schema.PARTITIONS
     *
     * @param array $row fetched row
     *
     * @return void
     */
    protected function loadData(array $row)
    {
        $this->name = $row['PARTITION_NAME'];
        $this->ordinal = $row['PARTITION_ORDINAL_POSITION'];
        $this->method = $row['PARTITION_METHOD'];
        $this->expression = $row['PARTITION_EXPRESSION'];
        $this->description = $row['PARTITION_DESCRIPTION'];
        // no sub partitions, load all data to this object
        if (empty($row['SUBPARTITION_NAME'])) {
            $this->loadCommonData($row);
        }
    }

    /**
     * Returns the partiotion description
     *
     * @return string partition description
     */
    public function getDescription()
    {
        return $this->description;
    }

    /**
     * Add a sub partition
     *
     * @param SubPartition $partition Sub partition
     *
     * @return void
     */
    public function addSubPartition(SubPartition $partition)
    {
        $this->subPartitions[] = $partition;
    }

    /**
     * Whether there are sub partitions
     *
     * @return boolean
     */
    public function hasSubPartitions()
    {
        return ! empty($this->subPartitions);
    }

    /**
     * Returns the number of data rows
     *
     * @return integer number of rows
     */
    public function getRows()
    {
        if (empty($this->subPartitions)) {
            return $this->rows;
        }

        $rows = 0;
        foreach ($this->subPartitions as $subPartition) {
            $rows += $subPartition->rows;
        }
        return $rows;
    }

    /**
     * Returns the total data length
     *
     * @return integer data length
     */
    public function getDataLength()
    {
        if (empty($this->subPartitions)) {
            return $this->dataLength;
        }

        $dataLength = 0;
        foreach ($this->subPartitions as $subPartition) {
            $dataLength += $subPartition->dataLength;
        }
        return $dataLength;
    }

    /**
     * Returns the tatal index length
     *
     * @return integer index length
     */
    public function getIndexLength()
    {
        if (empty($this->subPartitions)) {
            return $this->indexLength;
        }

        $indexLength = 0;
        foreach ($this->subPartitions as $subPartition) {
            $indexLength += $subPartition->indexLength;
        }
        return $indexLength;
    }

    /**
     * Returns the list of sub partitions
     *
     * @return SubPartition[]
     */
    public function getSubPartitions()
    {
        return $this->subPartitions;
    }

    /**
     * Returns array of partitions for a specific db/table
     *
     * @param string $db    database name
     * @param string $table table name
     *
     * @access  public
     * @return Partition[]
     */
    static public function getPartitions($db, $table)
    {
        if (Partition::havePartitioning()) {
            $result = $GLOBALS['dbi']->fetchResult(
                "SELECT * FROM `information_schema`.`PARTITIONS`"
                . " WHERE `TABLE_SCHEMA` = '" . $GLOBALS['dbi']->escapeString($db)
                . "' AND `TABLE_NAME` = '" . $GLOBALS['dbi']->escapeString($table) . "'"
            );
            if ($result) {
                $partitionMap = array();
                foreach ($result as $row) {
                    if (isset($partitionMap[$row['PARTITION_NAME']])) {
                        $partition = $partitionMap[$row['PARTITION_NAME']];
                    } else {
                        $partition = new Partition($row);
                        $partitionMap[$row['PARTITION_NAME']] = $partition;
                    }

                    if (! empty($row['SUBPARTITION_NAME'])) {
                        $parentPartition = $partition;
                        $partition = new SubPartition($row);
                        $parentPartition->addSubPartition($partition);
                    }
                }
                return array_values($partitionMap);
            }
            return array();
        }

        return array();
    }

    /**
     * returns array of partition names for a specific db/table
     *
     * @param string $db    database name
     * @param string $table table name
     *
     * @access  public
     * @return array   of partition names
     */
    static public function getPartitionNames($db, $table)
    {
        if (Partition::havePartitioning()) {
            return $GLOBALS['dbi']->fetchResult(
                "SELECT DISTINCT `PARTITION_NAME` FROM `information_schema`.`PARTITIONS`"
                . " WHERE `TABLE_SCHEMA` = '" . $GLOBALS['dbi']->escapeString($db)
                . "' AND `TABLE_NAME` = '" . $GLOBALS['dbi']->escapeString($table) . "'"
            );
        }

        return array();
    }

    /**
     * returns the partition method used by the table.
     *
     * @param string $db    database name
     * @param string $table table name
     *
     * @return string partition method
     */
    static public function getPartitionMethod($db, $table)
    {
        if (Partition::havePartitioning()) {
            $partition_method = $GLOBALS['dbi']->fetchResult(
                "SELECT `PARTITION_METHOD` FROM `information_schema`.`PARTITIONS`"
                . " WHERE `TABLE_SCHEMA` = '" . $GLOBALS['dbi']->escapeString($db) . "'"
                . " AND `TABLE_NAME` = '" . $GLOBALS['dbi']->escapeString($table) . "'"
                . " LIMIT 1"
            );
            if (! empty($partition_method)) {
                return $partition_method[0];
            }
        }
        return null;
    }

    /**
     * checks if MySQL server supports partitioning
     *
     * @static
     * @staticvar boolean $have_partitioning
     * @staticvar boolean $already_checked
     * @access  public
     * @return boolean
     */
    static public function havePartitioning()
    {
        static $have_partitioning = false;
        static $already_checked = false;

        if (! $already_checked) {
            if ($GLOBALS['dbi']->getVersion() < 50600) {
                if ($GLOBALS['dbi']->fetchValue(
                    "SELECT @@have_partitioning;"
                )) {
                    $have_partitioning = true;
                }
            } else if ($GLOBALS['dbi']->getVersion() >= 80000) {
                $have_partitioning = true;
            } else {
                // see https://dev.mysql.com/doc/refman/5.6/en/partitioning.html
                $plugins = $GLOBALS['dbi']->fetchResult("SHOW PLUGINS");
                foreach ($plugins as $value) {
                    if ($value['Name'] == 'partition') {
                        $have_partitioning = true;
                        break;
                    }
                }
            }
            $already_checked = true;
        }
        return $have_partitioning;
    }
}
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                  , "" },
{ ER_SP_ALREADY_EXISTS                    ,"42000", "" },
{ ER_SP_DOES_NOT_EXIST                    ,"42000", "" },
{ ER_SP_LILABEL_MISMATCH                  ,"42000", "" },
{ ER_SP_LABEL_REDEFINE                    ,"42000", "" },
{ ER_SP_LABEL_MISMATCH                    ,"42000", "" },
{ ER_SP_UNINIT_VAR                        ,"01000", "" },
{ ER_SP_BADSELECT                         ,"0A000", "" },
{ ER_SP_BADRETURN                         ,"42000", "" },
{ ER_SP_BADSTATEMENT                      ,"0A000", "" },
{ ER_UPDATE_LOG_DEPRECATED_IGNORED        ,"42000", "" },
{ ER_UPDATE_LOG_DEPRECATED_TRANSLATED     ,"42000", "" },
{ ER_QUERY_INTERRUPTED                    ,"70100", "" },
{ ER_SP_WRONG_NO_OF_ARGS                  ,"42000", "" },
{ ER_SP_COND_MISMATCH                     ,"42000", "" },
{ ER_SP_NORETURN                          ,"42000", "" },
{ ER_SP_NORETURNEND                       ,"2F005", "" },
{ ER_SP_BAD_CURSOR_QUERY                  ,"42000", "" },
{ ER_SP_BAD_CURSOR_SELECT                 ,"42000", "" },
{ ER_SP_CURSOR_MISMATCH                   ,"42000", "" },
{ ER_SP_CURSOR_ALREADY_OPEN               ,"24000", "" },
{ ER_SP_CURSOR_NOT_OPEN                   ,"24000", "" },
{ ER_SP_UNDECLARED_VAR                    ,"42000", "" },
{ ER_SP_FETCH_NO_DATA                     ,"02000", "" },
{ ER_SP_DUP_PARAM                         ,"42000", "" },
{ ER_SP_DUP_VAR                           ,"42000", "" },
{ ER_SP_DUP_COND                          ,"42000", "" },
{ ER_SP_DUP_CURS                          ,"42000", "" },
{ ER_SP_SUBSELECT_NYI                     ,"0A000", "" },
{ ER_STMT_NOT_ALLOWED_IN_SF_OR_TRG        ,"0A000", "" },
{ ER_SP_VARCOND_AFTER_CURSHNDLR           ,"42000", "" },
{ ER_SP_CURSOR_AFTER_HANDLER              ,"42000", "" },
{ ER_SP_CASE_NOT_FOUND                    ,"20000", "" },
{ ER_DIVISION_BY_ZERO                     ,"22012", "" },
{ ER_TRUNCATED_WRONG_VALUE_FOR_FIELD      ,"22007", "" },
{ ER_ILLEGAL_VALUE_FOR_TYPE               ,"22007", "" },
{ ER_VIEW_CHECK_FAILED                    ,"44000", "" },
{ ER_PROCACCESS_DENIED_ERROR              ,"42000", "" },
{ ER_XAER_NOTA                            ,"XAE04", "" },
{ ER_XAER_INVAL                           ,"XAE05", "" },
{ ER_XAER_RMFAIL                          ,"XAE07", "" },
{ ER_XAER_OUTSIDE                         ,"XAE09", "" },
{ ER_XAER_RMERR                           ,"XAE03", "" },
{ ER_XA_RBROLLBACK                        ,"XA100", "" },
{ ER_NONEXISTING_PROC_GRANT               ,"42000", "" },
{ ER_DATA_TOO_LONG                        ,"22001", "" },
{ ER_SP_BAD_SQLSTATE                      ,"42000", "" },
{ ER_CANT_CREATE_USER_WITH_GRANT          ,"42000", "" },
{ ER_SP_DUP_HANDLER                       ,"42000", "" },
{ ER_SP_NOT_VAR_ARG                       ,"42000", "" },
{ ER_SP_NO_RETSET                         ,"0A000", "" },
{ ER_CANT_CREATE_GEOMETRY_OBJECT          ,"22003", "" },
{ ER_TOO_BIG_SCALE                        ,"42000", "S1009" },
{ ER_TOO_BIG_PRECISION                    ,"42000", "S1009" },
{ ER_M_BIGGER_THAN_D                      ,"42000", "S1009" },
{ ER_TOO_LONG_BODY                        ,"42000", "S1009" },
{ ER_TOO_BIG_DISPLAYWIDTH                 ,"42000", "S1009" },
{ ER_XAER_DUPID                           ,"XAE08", "" },
{ ER_DATETIME_FUNCTION_OVERFLOW           ,"22008", "" },
{ ER_MALFORMED_DEFINER                    ,"0L000", "" },
{ ER_ROW_IS_REFERENCED_2                  ,"23000", "" },
{ ER_NO_REFERENCED_ROW_2                  ,"23000", "" },
{ ER_SP_BAD_VAR_SHADOW                    ,"42000", "" },
{ ER_SP_WRONG_NAME                        ,"42000", "" },
{ ER_SP_NO_AGGREGATE                      ,"42000", "" },
{ ER_MAX_PREPARED_STMT_COUNT_REACHED      ,"42000", "" },
{ ER_NON_GROUPING_FIELD_USED              ,"42000", "" },
{ ER_CANT_CHANGE_TX_CHARACTERISTICS       ,"25001", "" },
{ ER_WRONG_PARAMCOUNT_TO_NATIVE_FCT       ,"42000", "" },
{ ER_WRONG_PARAMETERS_TO_NATIVE_FCT       ,"42000", "" },
{ ER_WRONG_PARAMETERS_TO_STORED_FCT       ,"42000", "" },
{ ER_DUP_ENTRY_WITH_KEY_NAME              ,"23000", "S1009" },
{ ER_XA_RBTIMEOUT                         ,"XA106", "" },
{ ER_XA_RBDEADLOCK                        ,"XA102", "" },
{ ER_FUNC_INEXISTENT_NAME_COLLISION       ,"42000", "" },
{ ER_DUP_SIGNAL_SET                       ,"42000", "" },
{ ER_SIGNAL_WARN                          ,"01000", "" },
{ ER_SIGNAL_NOT_FOUND                     ,"02000", "" },
{ ER_SIGNAL_EXCEPTION                     ,"HY000", "" },
{ ER_RESIGNAL_WITHOUT_ACTIVE_HANDLER      ,"0K000", "" },
{ ER_SPATIAL_MUST_HAVE_GEOM_COL           ,"42000", "" },
{ ER_DATA_OUT_OF_RANGE                    ,"22003", "" },
{ ER_ACCESS_DENIED_NO_PASSWORD_ERROR      ,"28000", "" },
{ ER_TRUNCATE_ILLEGAL_FK                  ,"42000", "" },
{ ER_DA_INVALID_CONDITION_NUMBER          ,"35000", "" },
{ ER_FOREIGN_DUPLICATE_KEY_WITH_CHILD_INFO,"23000", "S1009" },
{ ER_FOREIGN_DUPLICATE_KEY_WITHOUT_CHILD_INFO,"23000", "S1009" },
{ ER_CANT_EXECUTE_IN_READ_ONLY_TRANSACTION,"25006", "" },
{ ER_ALTER_OPERATION_NOT_SUPPORTED        ,"0A000", "" },
{ ER_ALTER_OPERATION_NOT_SUPPORTED_REASON ,"0A000", "" },
{ ER_DUP_UNKNOWN_IN_INDEX                 ,"23000", "" },
{ ER_ACCESS_DENIED_CHANGE_USER_ERROR      ,"28000", "" },
{ ER_DATA_OVERFLOW                        ,"22003", "" },
{ ER_DATA_TRUNCATED                       ,"22003", "" },
{ ER_BAD_DATA                             ,"22007", "" },
{ ER_DYN_COL_DATA                         ,"22007", "" },
{ ER_CONNECTION_KILLED                    ,"70100", "" },
{ ER_NO_SUCH_TABLE_IN_ENGINE              ,"42S02", "" },
{ ER_INVALID_ROLE                         ,"OP000", "" },
{ ER_INVALID_CURRENT_USER                 ,"0L000", "" },
{ ER_IT_IS_A_VIEW                         ,"42S02", "" },
{ ER_STATEMENT_TIMEOUT                    ,"70100", "" },
{ ER_SUBQUERIES_NOT_SUPPORTED             ,"42000", "" },
{ ER_SET_STATEMENT_NOT_SUPPORTED          ,"42000", "" },
{ ER_INVALID_DEFAULT_VALUE_FOR_FIELD      ,"22007", "" },
{ ER_GET_STACKED_DA_WITHOUT_ACTIVE_HANDLER,"0Z002", "" },
{ ER_INVALID_ARGUMENT_FOR_LOGARITHM       ,"2201E", "" },
{ ER_GIS_INVALID_DATA                     ,"22023", "" },
{ ER_USER_LOCK_WRONG_NAME                 ,"42000", "" },
{ ER_UNUSED_26                            ,"0A000", "" },
{ ER_CONSTRAINT_FAILED                    ,"23000", "" },
{ ER_EXPRESSION_REFERS_TO_UNINIT_FIELD    ,"01000", "" },
{ ER_WRONG_PARAMCOUNT_TO_CURSOR           ,"42000", "" },
{ ER_NOT_SEQUENCE                         ,"42S02", "" },
{ ER_NOT_SEQUENCE2                        ,"42S02", "" },
{ ER_UNKNOWN_SEQUENCES                    ,"42S02", "" },
{ ER_UNKNOWN_VIEW                         ,"42S02", "" },
{ ER_TOO_LONG_KEYPART                     ,"42000", "S1009" },
