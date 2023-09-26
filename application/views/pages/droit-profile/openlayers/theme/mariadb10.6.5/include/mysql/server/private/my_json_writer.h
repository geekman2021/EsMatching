t    $num_rows             number of rows affected/changed by the query
     * @param bool   $justBrowsing         whether just browsing or not
     * @param string $db                   the current database
     * @param string $table                the current table
     * @param array  $analyzed_sql_results the analyzed query and other variables set
     *                                     after analyzing the query
     *
     * @return int $unlim_num_rows unlimited number of rows
     */
    private function countQueryResults(
        $num_rows, $justBrowsing, $db, $table, array $analyzed_sql_results
    ) {

        /* Shortcut for not analyzed/empty query */
        if (empty($analyzed_sql_results)) {
            return 0;
        }

        if (!$this->isAppendLimitClause($analyzed_sql_results)) {
            // if we did not append a limit, set this to get a correct
            // "Showing rows..." message
            // $_SESSION['tmpval']['max_rows'] = 'all';
            $unlim_num_rows = $num_rows;
        } elseif ($this->isAppendLimitClause($analyzed_sql_results) && $_SESSION['tmpval']['max_rows'] > $num_rows) {
            // When user has not defined a limit in query and total rows in
            // result are less than max_rows to display, there is no need
            // to count total rows for that query again
            $unlim_num_rows = $_SESSION['tmpval']['pos'] + $num_rows;
        } elseif ($analyzed_sql_results['querytype'] == 'SELECT'
            || $analyzed_sql_results['is_subquery']
        ) {
            //    c o u n t    q u e r y

            // If we are "just browsing", there is only one table (and no join),
            // and no WHERE clause (or just 'WHERE 1 '),
            // we do a quick count (which uses MaxExactCount) because
            // SQL_CALC_FOUND_ROWS is not quick on large InnoDB tables

            // However, do not count again if we did it previously
            // due to $find_real_end == true
            if ($justBrowsing) {
                // Get row count (is approximate for InnoDB)
                $unlim_num_rows = $GLOBALS['dbi']->getTable($db, $table)->countRecords();
                /**
                 * @todo Can we know at this point that this is InnoDB,
                 *       (in this case there would be no need for getting
                 *       an exact count)?
                 */
                if ($unlim_num_rows < $GLOBALS['cfg']['MaxExactCount']) {
                    // Get the exact count if approximate count
                    // is less than MaxExactCount
                    /**
                     * @todo In countRecords(), MaxExactCount is also verified,
                     *       so can we avoid checking it twice?
                     */
                    $unlim_num_rows = $GLOBALS['dbi']->getTable($db, $table)
                        ->countRecords(true);
                }

            } else {

                // The SQL_CALC_FOUND_ROWS option of the SELECT statement is used.

                // For UNION statements, only a SQL_CALC_FOUND_ROWS is required
                // after the first SELECT.

                $count_query = Query::replaceClause(
                    $analyzed_sql_results['statement'],
                    $analyzed_sql_results['parser']->list,
                    'SELECT SQL_CALC_FOUND_ROWS',
                    null,
                    true
                );

                // Another LIMIT clause is added to avoid long delays.
                // A complete result will be returned anyway, but the LIMIT would
                // stop the query as soon as the result that is required has been
                // computed.

                if (empty($analyzed_sql_results['union'])) {
                    $count_query .= ' LIMIT 1';
                }

                // Running the count query.
                $GLOBALS['dbi']->tryQuery($count_query);

                $unlim_num_rows = $GLOBALS['dbi']->fetchValue('SELECT FOUND_ROWS()');
            } // end else "just browsing"
        } else {// not $is_select
            $unlim_num_rows = 0;
        }

        return $unlim_num_rows;
    }

    /**
     * Function to handle all aspects relating to executing the query
     *
     * @param array   $analyzed_sql_results   analyzed sql results
     * @param string  $full_sql_query         full sql query
     * @param boolean $is_gotofile            whether to go to a file
     * @param string  $db                     current database
     * @param string  $table                  current table
     * @param boolean $find_real_end          whether to find the real end
     * @param string  $sql_query_for_bookmark sql query to be stored as bookmark
     * @param array   $extra_data             extra data
     *
     * @return mixed
     */
    private function executeTheQuery(array $analyzed_sql_results, $full_sql_query, $is_gotofile,
        $db, $table, $find_real_end, $sql_query_for_bookmark, $extra_data
    ) {
        $response = Response::getInstance();
        $response->getHeader()->getMenu()->setTable($table);

        // Only if we ask to see the php code
        if (isset($GLOBALS['show_as_php'])) {
            $result = null;
            $num_rows = 0;
            $unlim_num_rows = 0;
        } else { // If we don't ask to see the php code
            if (isset($_SESSION['profiling'])
                && Util::profilingSupported()
            ) {
                $GLOBALS['dbi']->query('SET PROFILING=1;');
            }

            list(
                $result,
                $GLOBALS['querytime']
            ) = $this->executeQueryAndMeasureTime($full_sql_query);

            // Displays an error message if required and stop parsing the script
            $error = $GLOBALS['dbi']->getError();
            if ($error && $GLOBALS['cfg']['IgnoreMultiSubmitErrors']) {
                $extra_data['error'] = $error;
            } elseif ($error) {
                $this->handleQueryExecuteError($is_gotofile, $error, $full_sql_query);
            }

            // If there are no errors and bookmarklabel was given,
            // store the query as a bookmark
            if (! empty($_POST['bkm_label']) && ! empty($sql_query_for_bookmark)) {
                $cfgBookmark = Bookmark::getParams($GLOBALS['cfg']['Server']['user']);
                $this->storeTheQueryAsBookmark(
                    $db, $cfgBookmark['user'],
                    $sql_query_for_bookmark, $_POST['bkm_label'],
                    isset($_POST['bkm_replace']) ? $_POST['bkm_replace'] : null
                );
            } // end store bookmarks

            // Gets the number of rows affected/returned
            // (This must be done immediately after the query because
            // mysql_affected_rows() reports about the last query done)
            $num_rows = $this->getNumberOfRowsAffectedOrChanged(
                $analyzed_sql_results['is_affected'], $result
            );

            // Grabs the profiling results
            if (isset($_SESSION['profiling'])
                && Util::profilingSupported()
            ) {
                $profiling_results = $GLOBALS['dbi']->fetchResult('SHOW PROFILE;');
            }

            $justBrowsing = $this->isJustBrowsing(
                $analyzed_sql_results, isset($find_real_end) ? $find_real_end : null
            );

            $unlim_num_rows = $this->countQueryResults(
                $num_rows, $justBrowsing, $db, $table, $analyzed_sql_results
            );

            $this->cleanupRelations(
                isset($db) ? $db : '',
                isset($table) ? $table : '',
                isset($_POST['dropped_column']) ? $_POST['dropped_column'] : null,
                isset($_POST['purge']) ? $_POST['purge'] : null
            );

            if (isset($_POST['dropped_column'])
                && strlen($db) > 0
                && strlen($table) > 0
            ) {
                // to refresh the list of indexes (Ajax mode)
                $extra_data['indexes_list'] = Index::getHtmlForIndexes(
                    $table,
                    $db
                );
            }
        }

        return array($result, $num_rows, $unlim_num_rows,
            isset($profiling_results) ? $profiling_results : null, $extra_data
        );
    }
    /**
     * Delete related transformation information
     *
     * @param string $db                   current database
     * @param string $table                current table
     * @param array  $analyzed_sql_results analyzed sql results
     *
     * @return void
     */
    private function deleteTransformationInfo($db, $table, array $analyzed_sql_results)
    {
        if (! isset($analyzed_sql_results['statement'])) {
            return;
        }
        $statement = $analyzed_sql_results['statement'];
        if ($statement instanceof AlterStatement) {
            if (!empty($statement->altered[0])
                && $statement->altered[0]->options->has('DROP')
            ) {
                if (!empty($statement->altered[0]->field->column)) {
                    Transformations::clear(
                        $db,
                        $table,
                        $statement->altered[0]->field->column
                    );
                }
            }
        } elseif ($statement instanceof DropStatement) {
            Transformations::clear($db, $table);
        }
    }

    /**
     * Function to get the message for the no rows returned case
     *
     * @param string $message_to_show      message to show
     * @param array  $analyzed_sql_results analyzed sql results
     * @param int    $num_rows             number of rows
     *
     * @return string $message
     */
    private function getMessageForNoRowsReturned($message_to_show,
        array $analyzed_sql_results, $num_rows
    ) {
        if ($analyzed_sql_results['querytype'] == 'DELETE"') {
            $message = Message::getMessageForDeletedRows($num_rows);
        } elseif ($analyzed_sql_results['is_insert']) {
            if ($analyzed_sql_results['querytype'] == 'REPLACE') {
                // For REPLACE we get DELETED + INSERTED row count,
                // so we have to call it affected
                $message = Message::getMessageForAffectedRows($num_rows);
            } else {
                $message = Message::getMessageForInsertedRows($num_rows);
            }
            $insert_id = $GLOBALS['dbi']->insertId();
            if ($insert_id != 0) {
                // insert_id is id of FIRST record inserted in one insert,
                // so if we inserted multiple rows, we had to increment this
                $message->addText('[br]');
                // need to use a temporary because the Message class
                // currently supports adding parameters only to the first
                // message
                $_inserted = Message::notice(__('Inserted row id: %1$d'));
                $_inserted->addParam($insert_id + $num_rows - 1);
                $message->addMessage($_inserted);
            }
        } elseif ($analyzed_sql_results['is_affected']) {
            $message = Message::getMessageForAffectedRows($num_rows);

            // Ok, here is an explanation for the !$is_select.
            // The form generated by PhpMyAdmin\SqlQueryForm
            // and db_sql.php has many submit buttons
            // on the same form, and some confusion arises from the
            // fact that $message_to_show is sent for every case.
            // The $message_to_show containing a success message and sent with
            // the form should not have priority over errors
        } elseif (! empty($message_to_show)
            && $analyzed_sql_results['querytype'] != 'SELECT'
        ) {
            $message = Message::rawSuccess(htmlspecialchars($message_to_show));
        } elseif (! empty($GLOBALS['show_as_php'])) {
            $message = Message::success(__('Showing as PHP code'));
        } elseif (isset($GLOBALS['show_as_php'])) {
            /* User disable showing as PHP, query is only displayed */
            $message = Message::notice(__('Showing SQL query'));
        } else {
            $message = Message::success(
                __('MySQL returned an empty result set (i.e. zero rows).')
            );
        }

        if (isset($GLOBALS['querytime'])) {
            $_querytime = Message::notice(
                '(' . __('Query took %01.4f seconds.') . ')'
            );
            $_querytime->addParam($GLOBALS['querytime']);
            $message->addMessage($_querytime);
        }

        // In case of ROLLBACK, notify the user.
        if (isset($_POST['rollback_query'])) {
            $message->addText(__('[ROLLBACK occurred.]'));
        }

        return $message;
    }

    /**
     * Function to respond back when the query returns zero rows
     * This method is called
     * 1-> When browsing an empty table
     * 2-> When executing a query on a non empty table which returns zero results
     * 3-> When executing a query on an empty table
     * 4-> When executing an INSERT, UPDATE, DELETE query from the SQL tab
     * 5-> When deleting a row from BROWSE tab
     * 6-> When searching using the SEARCH tab which returns zero results
     * 7-> When changing the structure of the table except change operation
     *
     * @param array          $analyzed_sql_results analyzed sql results
     * @param string         $db                   current database
     * @param string         $table                current table
     * @param string         $message_to_show      message to show
     * @param int            $num_rows             number of rows
     * @param DisplayResults $displayResultsObject DisplayResult instance
     * @param array          $extra_data           extra data
     * @param string         $pmaThemeImage        uri of the theme image
     * @param array|null     $profiling_results    profiling results
     * @param object         $result               executed query results
     * @param string         $sql_query            sql query
     * @param string         $complete_query       complete sql query
     *
     * @return string html
     */
    private function getQueryResponseForNoResultsReturned(array $analyzed_sql_results, $db,
        $table, $message_to_show, $num_rows, $displayResultsObject, $extra_data,
        $pmaThemeImage, $profiling_results, $result, $sql_query, $complete_query
    ) {
        if ($this->isDeleteTransformationInfo($analyzed_sql_results)) {
            $this->deleteTransformationInfo($db, $table, $analyzed_sql_results);
        }

        if (isset($extra_data['error'])) {
            $message = Message::rawError($extra_data['error']);
        } else {
            $message = $this->getMessageForNoRowsReturned(
                isset($message_to_show) ? $message_to_show : null,
                $analyzed_sql_results, $num_rows
            );
        }

        $html_output = '';
        $html_message = Util::getMessage(
            $message, $GLOBALS['sql_query'], 'success'
        );
        $html_output .= $html_message;
        if (!isset($GLOBALS['show_as_php'])) {

            if (! empty($GLOBALS['reload'])) {
                $extra_data['reload'] = 1;
                $extra_data['db'] = $GLOBALS['db'];
            }

            // For ajax requests add message and sql_query as JSON
            if (empty($_REQUEST['ajax_page_request'])) {
                $extra_data['message'] = $message;
                if ($GLOBALS['cfg']['ShowSQL']) {
                    $extra_data['sql_query'] = $html_message;
                }
            }

            $response = Response::getInstance();
            $response->addJSON(isset($extra_data) ? $extra_data : array());

            if (!empty($analyzed_sql_results['is_select']) &&
                    !isset($extra_data['error'])) {
                $url_query = isset($url_query) ? $url_query : null;

                $displayParts = array(
                    'edit_lnk' => null,
                    'del_lnk' => null,
                    'sort_lnk' => '1',
                    'nav_bar'  => '0',
                    'bkm_form' => '1',
                    'text_btn' => '1',
                    'pview_lnk' => '1'
                );

                $html_output .= $this->getHtmlForSqlQueryResultsTable(
                    $displayResultsObject,
                    $pmaThemeImage, $url_query, $displayParts,
                    false, 0, $num_rows, true, $result,
                    $analyzed_sql_results, true
                );

                if (isset($profiling_results)) {
                    $header   = $response->getHeader();
                    $scripts  = $header->getScripts();
                    $scripts->addFile('sql.js');
                    $html_output .=