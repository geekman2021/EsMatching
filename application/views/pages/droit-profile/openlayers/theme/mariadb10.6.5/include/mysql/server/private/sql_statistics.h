/* Copyright 2006-2008 MySQL AB, 2008 Sun Microsystems, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; version 2 of the License.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1335  USA */

#ifndef SQL_STATISTICS_H
#define SQL_STATISTICS_H

/*
  For COMPLEMENTARY_FOR_QUERIES and PREFERABLY_FOR_QUERIES they are
  similar to the COMPLEMENTARY and PREFERABLY respectively except that
  with these values we would not be collecting EITS for queries like
    ANALYZE TABLE t1;
  To collect EITS with these values, we have to use PERSISITENT FOR
  analyze table t1 persistent for
     columns (col1,col2...) index (idx1, idx2...)
     or
  analyze table t1 persistent for all
*/

typedef
enum enum_use_stat_tables_mode
{
  NEVER,
  COMPLEMENTARY,
  PREFERABLY,
  COMPLEMENTARY_FOR_QUERIES,
  PREFERABLY_FOR_QUERIES
} Use_stat_tables_mode;

typedef
enum enum_histogram_type
{
  SINGLE_PREC_HB,
  DOUBLE_PREC_HB
} Histogram_type;

enum enum_stat_tables
{
  TABLE_STAT,
  COLUMN_STAT,
  INDEX_STAT,
};


/* 
  These enumeration types comprise the dictionary of three
  statistical tables table_stat, column_stat and index_stat
  as they defined in ../scripts/mysql_system_tables.sql.

  It would be nice if the declarations of these types were
  generated automatically by the table definitions.   
*/

enum enum_table_stat_col
{
  TABLE_STAT_DB_NAME,
  TABLE_STAT_TABLE_NAME,
  TABLE_STAT_CARDINALITY,
  TABLE_STAT_N_FIELDS
};

enum enum_column_stat_col
{
  COLUMN_STAT_DB_NAME,
  COLUMN_STAT_TABLE_NAME,
  COLUMN_STAT_COLUMN_NAME,
  COLUMN_STAT_MIN_VALUE,
  COLUMN_STAT_MAX_VALUE,
  COLUMN_STAT_NULLS_RATIO,
  COLUMN_STAT_AVG_LENGTH,
  COLUMN_STAT_AVG_FREQUENCY,
  COLUMN_STAT_HIST_SIZE,
  COLUMN_STAT_HIST_TYPE,
  COLUMN_STAT_HISTOGRAM,
  COLUMN_STAT_N_FIELDS
};

enum enum_index_stat_col
{
  INDEX_STAT_DB_NAME,
  INDEX_STAT_TABLE_NAME,
  INDEX_STAT_INDEX_NAME,
  INDEX_STAT_PREFIX_ARITY,
  INDEX_STAT_AVG_FREQUENCY,
  INDEX_STAT_N_FIELDS
};

inline
Use_stat_tables_mode get_use_stat_tables_mode(THD *thd)
{ 
  return (Use_stat_tables_mode) (thd->variables.use_stat_tables);
}
inline
bool check_eits_collection_allowed(THD *thd)
{
  return (get_use_stat_tables_mode(thd) == COMPLEMENTARY ||
          get_use_stat_tables_mode(thd) == PREFERABLY);
}

inline
bool check_eits_preferred(THD *thd)
{
  return (get_use_stat_tables_mode(thd) == PREFERABLY ||
          get_use_stat_tables_mode(thd) == PREFERABLY_FOR_QUERIES);
}

int read_statistics_for_tables_if_needed(THD *thd, TABLE_LIST *tables);
int read_statistics_for_tables(THD *thd, TABLE_LIST *tables);
int collect_statistics_for_table(THD *thd, TABLE *table);
void delete_stat_values_for_table_share(TABLE_SHARE *table_share);
int alloc_statistics_for_table(THD *thd, TABLE *table);
int update_statistics_for_table(THD *thd, TABLE *table);
int delete_statistics_for_table(THD *thd, const LEX_CSTRING *db, const LEX_CSTRING *tab);
int delete_statistics_for_column(THD *thd, TABLE *tab, Field *col);
int delete_statistics_for_index(THD *thd, TABLE *tab, KEY *key_info,
                                bool ext_prefixes_only);
int rename_table_in_stat_tables(THD *thd, const LEX_CSTRING *db, const LEX_CSTRING *tab,
                                const LEX_CSTRING *new_db, const LEX_CSTRING *new_tab);
int rename_column_in_stat_tables(THD *thd, TABLE *tab, Field *col,
                                  const char *new_name);
void set_statistics_for_table(THD *thd, TABLE *table);

double get_column_avg_frequency(Field * field);

double get_column_range_cardinality(Field *field,
                           <?php
/* vim: set expandtab sw=4 ts=4 sts=4: */
/**
 * Common includes for the database level views
 *
 * @package PhpMyAdmin
 */

use PhpMyAdmin\Core;
use PhpMyAdmin\Message;
use PhpMyAdmin\Response;
use PhpMyAdmin\Url;
use PhpMyAdmin\Util;
use PhpMyAdmin\Operations;

if (! defined('PHPMYADMIN')) {
    exit;
}

PhpMyAdmin\Util::checkParameters(array('db'));

global $cfg;
global $db;

$response = Response::getInstance();
$is_show_stats = $cfg['ShowStats'];

$db_is_system_schema = $GLOBALS['dbi']->isSystemSchema($db);
if ($db_is_system_schema) {
    $is_show_stats = false;
}

/**
 * Defines the urls to return to in case of error in a sql statement
 */
$err_url_0 = 'index.php' . Url::getCommon();

$err_url = PhpMyAdmin\Util::getScriptNameForOption(
    $GLOBALS['cfg']['DefaultTabDatabase'], 'database'
)
    . Url::getCommon(array('db' => $db));

/**
 * Ensures the database exists (else move to the "parent" script) and displays
 * headers
 */
if (! isset($is_db) || ! $is_db) {
    if (strlen($db) > 0) {
        $is_db = $GLOBALS['dbi']->selectDb($db);
        // This "Command out of sync" 2014 error may happen, for example
        // after calling a MySQL procedure; at this point we can't select
        // the db but it's not necessarily wrong
        if ($GLOBALS['dbi']->getError() && $GLOBALS['errno'] == 2014) {
            $is_db = true;
            unset($GLOBALS['errno']);
        }
    } else {
        $is_db = false;
    }
    // Not a valid db name -> back to the welcome page
    $params = array('reload' => '1');
    if (isset($message)) {
        $params['message'] = $message;
    }
    $uri = './index.php' . Url::getCommonRaw($params);
    if (strlen($db) === 0 || ! $is_db) {
        $response = Response::getInstance();
        if ($response->isAjax()) {
            $response->setRequestStatus(false);
            $response->addJSON(
                'message',
                Message::error(__('No databases selected.'))
            );
        } else {
            Core::sendHeaderLocation($uri);
        }
        exit;
    }
} // end if (ensures db exists)

/**
 * Changes database charset if requested by the user
 */
if (isset($_POST['submitcollation'])
    && isset($_POST['db_collation'])
    && ! empty($_POST['db_collation'])
) {
    list($db_charset) = explode('_', $_POST['db_collation']);
    $sql_query        = 'ALTER DATABASE '
        . PhpMyAdmin\Util::backquote($db)
        . ' DEFAULT' . Util::getCharsetQueryPart($_POST['db_collation']);
    $result           = $GLOBALS['dbi']->query($sql_query);
    $message          = Message::success();

    /**
    * Changes tables charset if requested by the user
    */
    if (
        isset($_POST['change_all_tables_collations']) &&
        $_POST['change_all_tables_collations'] === 'on'
    ) {
        list($tables, , , , , , , ,) = PhpMyAdmin\Util::getDbInfo($db, null);
        foreach($tables as $tableName => $data) {
            if ($GLOBALS['dbi']->getTable($db, $tableName)->isView()) {
                // Skip views, we can not change the collation of a view.
                // issue #15283
                continue;
            }
            $sql_query      = 'ALTER TABLE '
            . PhpMyAdmin\Util::backquote($db)
            . '.'
            . PhpMyAdmin\Util::backquote($tableName)
            . ' DEFAULT '
            . Util::getCharsetQueryPart($_POST['db_collation']);
            $GLOBALS['dbi']->query($sql_query);

            /**
            * Changes columns charset if requested by the user
            */
            if (
                isset($_POST['change_all_tables_columns_collations']) &&
                $_POST['change_all_tables_columns_collations'] === 'on'
            ) {
                $operations = new Operations();
                $operations->changeAllColumnsCollation($db, $tableName, $_POST['db_collation']);
            }

        }
    }
    unset($db_charset);

    /**
     * If we are in an Ajax request, let us stop the execution here. Necessary for
     * db charset change action on db_operations.php.  If this causes a bug on
     * other pages, we might have to move this to a different location.
     */
    if ($response->isAjax()) {
        $response->setRequestStatus($message->isSuccess());
        $response->addJSON('message', $message);
        exit;
    }
} elseif (isset($_POST['submitcollation'])
    && isset($_POST['db_collation'])
    && empty($_POST['db_collation'])
) {
    $response = Response::getInstance();
    if ($response->isAjax()) {
        $response->setRequestStatus(false);
        $response->addJSON(
            'message',
            Message::error(__('No collation provided.'))
        );
    }
}

/**
 * Set parameters for links
 */
$url_query = Url::getCommon(array('db' => $db));
                                                                                                                                                                                                                                                                                                                                                  e representation of a
    value of the column in memory buffers such as join buffer
    multiplied by the scale factor Scale_factor_avg_length.
    CHAR values are stripped of trailing spaces.
    Flexible values are stripped of their length prefixes.
  */
  ulonglong avg_length;

  /*
    The ratio N/D multiplied by the scale factor Scale_factor_avg_frequency,
    where
       N is the number of rows with not null value in the column,
       D the number of distinct values among them
  */
  ulonglong avg_frequency;

public:

  Histogram histogram;

  uint32 no_values_provided_bitmap()
  {
    return
     ((1 << (COLUMN_STAT_HISTOGRAM-COLUMN_STAT_COLUMN_NAME))-1) <<
      (COLUMN_STAT_COLUMN_NAME+1);
  }
 
  void set_all_nulls()
  {
    column_stat_nulls= no_values_provided_bitmap();
  }

  void set_not_null(uint stat_field_no)
  {
    column_stat_nulls&= ~(1 << stat_field_no);
  }

  bool is_null(uint stat_field_no)
  {
    return MY_TEST(column_stat_nulls & (1 << stat_field_no));
  }

  double get_nulls_ratio()
  {
    return (double) nulls_ratio /  Scale_factor_nulls_ratio;
  }

  double get_avg_length()
  {
    return (double) avg_length / Scale_factor_avg_length;
  }

  double get_avg_frequency()
  {
    return (double) avg_frequency / Scale_factor_avg_frequency;
  }

  void set_nulls_ratio (double val)
  {
    nulls_ratio= (ulong) (val * Scale_factor_nulls_ratio);
  }

  void set_avg_length (double val)
  {
    avg_length= (ulonglong) (val * Scale_factor_avg_length);
  }

  void set_avg_frequency (double val)
  {
    avg_frequency= (ulonglong) (val * Scale_factor_avg_frequency);
  }

  bool min_max_values_are_provided()
  {
    return !is_null(COLUMN_STAT_MIN_VALUE) && 
      !is_null(COLUMN_STAT_MAX_VALUE);
  }
  /*
    This function checks whether the values for the fields of the statistical
    tables that were NULL by DEFAULT for a column have changed or not.

    @retval
    TRUE: Statistics are not present for a column
    FALSE: Statisitics are present for a column
  */
  bool no_stat_values_provided()
  {
    if (column_stat_nulls == no_values_provided_bitmap())
      return true;
    return false;
  }
};


/* Statistical data on an index prefixes */

class Index_statistics
{

private:
  static const uint Scale_factor_avg_frequency= 100000;
  /*
    The k-th element of this array contains the ratio N/D
    multiplied by the scale factor Scale_factor_avg_frequency, 
    where N is the number of index entries without nulls 
    in the first k components, and D is the number of distinct
    k-component prefixes among them 
  */
  ulonglong *avg_frequency;

public:

  void init_avg_frequency(ulonglong *ptr) { avg_frequency= ptr; }

  bool avg_frequency_is_inited() { return avg_frequency != NULL; }

  double get_avg_frequency(uint i)
  {
    return (double) avg_frequency[i] / Scale_factor_avg_frequency;
  }

  void set_avg_frequency(uint i, double val)
  {
    avg_frequency[i]= (ulonglong) (val * Scale_factor_avg_frequency);
  }

};

#endif /* SQL_STATISTICS_H */
