<?php
/* vim: set expandtab sw=4 ts=4 sts=4: */
/**
 * Set of functions for the SQL executor
 *
 * @package PhpMyAdmin
 */
namespace PhpMyAdmin;

use PhpMyAdmin\Bookmark;
use PhpMyAdmin\Core;
use PhpMyAdmin\DatabaseInterface;
use PhpMyAdmin\Display\Results as DisplayResults;
use PhpMyAdmin\Index;
use PhpMyAdmin\Message;
use PhpMyAdmin\Operations;
use PhpMyAdmin\ParseAnalyze;
use PhpMyAdmin\Relation;
use PhpMyAdmin\RelationCleanup;
use PhpMyAdmin\Response;
use PhpMyAdmin\SqlParser\Statements\AlterStatement;
use PhpMyAdmin\SqlParser\Statements\DropStatement;
use PhpMyAdmin\SqlParser\Statements\SelectStatement;
use PhpMyAdmin\SqlParser\Utils\Query;
use PhpMyAdmin\Table;
use PhpMyAdmin\Transformations;
use PhpMyAdmin\Url;
use PhpMyAdmin\Util;

/**
 * Set of functions for the SQL executor
 *
 * @package PhpMyAdmin
 */
class Sql
{
    /**
     * @var Relation $relation
     */
    private $relation;

    /**
     * Constructor
     */
    public function __construct()
    {
        $this->relation = new Relation();
    }

    /**
     * Parses and analyzes the given SQL query.
     *
     * @param string $sql_query SQL query
     * @param string $db        DB name
     *
     * @return mixed
     */
    public function parseAndAnalyze($sql_query, $db = null)
    {
        if (is_null($db) && isset($GLOBALS['db']) && strlen($GLOBALS['db'])) {
            $db = $GLOBALS['db'];
        }
        list($analyzed_sql_results,,) = ParseAnalyze::sqlQuery($sql_query, $db);
        return $analyzed_sql_results;
    }

    /**
     * Handle remembered sorting order, only for single table query
     *
     * @param string $db                    database name
     * @param string $table                 table name
     * @param array  &$analyzed_sql_results the analyzed query results
     * @param string &$full_sql_query       SQL query
     *
     * @return void
     */
    private function handleSortOrder(
        $db, $table, array &$analyzed_sql_results, &$full_sql_query
    ) {
        $pmatable = new Table($table, $db);

        if (empty($analyzed_sql_results['order'])) {

            // Retrieving the name of the column we should sort after.
            $sortCol = $pmatable->getUiProp(Table::PROP_SORTED_COLUMN);
            if (empty($sortCol)) {
                return;
            }

            // Remove the name of the table from the retrieved field name.
            $sortCol = str_replace(
                Util::backquote($table) . '.',
                '',
                $sortCol
            );

            // Create the new query.
            $full_sql_query = Query::replaceClause(
                $analyzed_sql_results['statement'],
                $analyzed_sql_results['parser']->list,
                'ORDER BY ' . $sortCol
            );

            // TODO: Avoid reparsing the query.
            $analyzed_sql_results = Query::getAll($full_sql_query);
        } else {
            // Store the remembered table into session.
            $pmatable->setUiProp(
                Table::PROP_SORTED_COLUMN,
                Query::getClause(
                    $analyzed_sql_results['statement'],
                    $analyzed_sql_results['parser']->list,
                    'ORDER BY'
                )
            );
        }
    }

    /**
     * Append limit clause to SQL query
     *
     * @param array &$analyzed_sql_results the analyzed query results
     *
     * @return string limit clause appended SQL query
     */
    private function getSqlWithLimitClause(array &$analyzed_sql_results)
    {
        return Query::replaceClause(
            $analyzed_sql_results['statement'],
            $analyzed_sql_results['parser']->list,
            'LIMIT ' . $_SESSION['tmpval']['pos'] . ', '
            . $_SESSION['tmpval']['max_rows']
        );
    }

    /**
     * Verify whether the result set has columns from just one table
     *
     * @param array $fields_meta meta fields
     *
     * @return boolean whether the result set has columns from just one table
     */
    private function resultSetHasJustOneTable(array $fields_meta)
    {
        $just_one_table = tr