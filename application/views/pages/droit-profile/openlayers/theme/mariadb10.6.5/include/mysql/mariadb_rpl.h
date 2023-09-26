<?php

declare(strict_types=1);

namespace PhpMyAdmin;

use function array_pop;
use function count;
use function htmlspecialchars;
use function strlen;

/**
 * Index manipulation class
 */
class Index
{
    public const PRIMARY  = 1;
    public const UNIQUE   = 2;
    public const INDEX    = 4;
    public const SPATIAL  = 8;
    public const FULLTEXT = 16;

    /**
     * Class-wide storage container for indexes (caching, singleton)
     *
     * @var array
     */
    private static $registry = [];

    /** @var string The name of the schema */
    private $schema = '';

    /** @var string The name of the table */
    private $table = '';

    /** @var string The name of the index */
    private $name = '';

    /**
     * Columns in index
     *
     * @var array
     */
    private $columns = [];

    /**
     * The index method used (BTREE, HASH, RTREE).
     *
     * @var string
     */
    private $type = '';

    /**
     * The index choice (PRIMARY, UNIQUE, INDEX, SPATIAL, FULLTEXT)
     *
     * @var string
     */
    private $choice = '';

    /**
     * Various remarks.
     *
     * @var string
     */
    private $remarks = '';

    /**
     * Any comment provided for the index with a COMMENT attribute when the
     * index was created.
     *
     * @var string
     */
    private $comment = '';

    /** @var int 0 if the index cannot contain duplicates, 1 if it can. */
    private $nonUnique = 0;

    /**
     * Indicates how the key is packed. NULL if it is not.
     *
     * @var string
     */
    private $packed = null;

    /**
     * Block size for the index
     *
     * @var int
     */
    private $keyBlockSize = null;

    /**
     * Parser option for the index
     *
     * @var string
     */
    private $parser = null;

    /**
     * @param array $params parameters
     */
    public function __construct(array $params = [])
    {
        $this->set($params);
    }

    /**
     * Creates(if not already created) and returns the corresponding Index object
     *
     * @param string $schema     database name
     * @param string $table      table name
     * @param string $index_name index name
     *
     * @return Index corresponding Index object
     */
    public static function singleton($schema, $table, $index_name = '')
    {
        self::loadIndexes($table, $schema);
        if (! isset(self::$registry[$schema][$table][$index_name])) {
            $index = new Index();
            if (strlen($index_name) > 0) {
                $index->setName($index_name);
                self::$registry[$schema][$table][$index->getName()] = $index;
            }

            return $index;
        }

        return self::$registry[$schema][$table][$index_name];
    }

    /**
     * returns an array with all indexes from the given table
     *
     * @param string $table  table
     * @param string $schema schema
     *
     * @return Index[]  array of indexes
     */
    public static function getFromTable($table, $schema)
    {
        self::loadIndexes($table, $schema);

        if (isset(self::$registry[$schema][$table])) {
            return self::$registry[$schema][$table];
        }

        return [];
    }

    /**
     * Returns an array with all indexes from the given table of the requested types
     *
     * @param string $table   table
     * @param string $schema  schema
     * @param int    $choices choices
     *
     * @return Index[] array of indexes
     */
    public static function getFromTableByChoice($table, $schema, $choices = 31)
    {
        $indexes = [];
        foreach (self::getFromTable($table, $schema) as $index) {
            if (($choices & self::PRIMARY)
                && $index->getChoice() === 'PRIMARY'
            ) {
                $indexes[] = $index;
            }
            if (($choices & self::UNIQUE)
                && $index->getChoice() === 'UNIQUE'
            ) {
                $indexes[] = $index;
            }
            if (($choices & self::INDEX)
                && $index->getChoice() === 'INDEX'
            ) {
                $indexes[] = $index;
            }
            if (($choices & self::SPATIAL)
                && $index->getChoice() === 'SPATIAL'
            ) {
                $indexes[] = $index;
            }
            if ((! ($choices & self::FULLTEXT))
                || $index->getChoice() !== 'FULLTEXT'
            ) {
                continue;
            }

            $indexes[] = $index;
        }

        return $indexes;
    }

    /**
     * return primary if set, false otherwise
     *
     * @param string $table  table
     * @param string $schema schema
     *
     * @return Index|false primary index or false if no one exists
     */
    public static function getPrimary($table, $schema)
    {
        self::loadIndexes($table, $schema);

        if (isset(self::$registry[$schema][$table]['PRIMARY'])) {
            return self::$registry[$schema][$table]['PRIMARY'];
        }

        return false;
    }

    /**
     * Load index data for table
     *
     * @param string $table  table
     * @param string $schema schema
     *
     * @return bool whether loading was successful
     */
    private static function loadIndexes($table, $schema)
    {
        global $dbi;

        if (isset(self::$registry[$schema][$table])) {
            return true;
        }

        $_raw_indexes = $dbi->getTableIndexes($schema, $table);
        foreach ($_raw_indexes as $_each_index) {
            $_each_index['Schema'] = $schema;
            $keyName = $_each_index['Key_name'];
            if (! isset(self::$registry[$schema][$table][$keyName])) {
                $key = new Index($_each_index);
                self::$registry[$schema][$table][$keyName] = $key;
            } else {
                $key = self::$registry[$schema][$table][$keyName];
            }

            $key->addColumn($_each_index);
        }

        return true;
    }

    /**
     * Add column to index
     *
     * @param array $params column params
     *
     * @return void
     */
    public function addColumn(array $params)
    {
        if (! isset($params['Column_name'])
            || strlen($params['Column_name']) <= 0
        ) {
            return;
        }

        $this->columns[$params['Column_name']] = new IndexColumn($params);
    }

    /**
     * Adds a list of columns to the index
     *
     * @param array $columns array containing details about the columns
     *
     * @return void
     */
    public function addColumns(array $columns)
    {
        $_columns = [];

        if (isset($columns['names'])) {
            // coming from form
            // $columns[names][]
            // $columns[sub_parts][]
            foreach ($columns['names'] as $key => $name) {
                $sub_part = $columns['sub_parts'][$key] ?? '';
                $_columns[] = [
                    'Column_name'   => $name,
                    'Sub_part'      => $sub_part,
                ];
            }
        } else {
            // coming from SHOW INDEXES
            // $columns[][name]
            // $columns[][sub_part]
            // ...
            $_columns = $columns;
        }

        foreach ($_columns as $column) {
            $this->addColumn($column);
        }
    }

    /**
     * Returns true if $column indexed in this index
     *
     * @param string $column the column
     *
     * @return bool true if $column indexed in this index
     */
    public function hasColumn($column)
    {
        return isset($this->columns[$column]);
    }

    /**
     * Sets index details
     *
     * @param array $params index details
     *
     * @return void
     */
    public function set(array $params)
    {
        if (isset($params['columns'])) {
            $this->addColumns($params['columns']);
        }
        if (isset($params['Schema'])) {
