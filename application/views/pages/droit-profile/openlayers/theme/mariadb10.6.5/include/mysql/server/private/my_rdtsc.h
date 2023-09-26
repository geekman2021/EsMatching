 = $this->getHtmlForPreviousUpdateQuery(
            isset($disp_query) ? $disp_query : null,
            $GLOBALS['cfg']['ShowSQL'], isset($sql_data) ? $sql_data : null,
            isset($disp_message) ? $disp_message : null
        );

        $profiling_chart_html = $this->getHtmlForProfilingChart(
            $url_query, $db, isset($profiling_results) ? $profiling_results :array()
        );

        $missing_unique_column_msg = $this->getMessageIfMissingColumnIndex(
            $table, $db, $editable, $has_unique
        );

        $bookmark_created_msg = $this->getBookmarkCreatedMessage();

        $table_html = $this->getHtmlForSqlQueryResultsTable(
            $displayResultsObject,
            $pmaThemeImage, $url_query, $displayParts,
            $editable, $unlim_num_rows, $num_rows, $showtable, $result,
            $analyzed_sql_results
        );

        $indexes_problems_html = $this->getHtmlForIndexesProblems(
            isset($query_type) ? $query_type : null,
            isset($selectedTables) ? $selectedTables : null, $db
        );

        $cfgBookmark = Bookmark::getParams($GLOBALS['cfg']['Server']['user']);
        if ($cfgBookmark) {
            $bookmark_support_html = $this->getHtmlForBookmark(
                $displayParts,
                $cfgBookmark,
                $sql_query, $db, $table,
                isset($complete_query) ? $complete_query : $sql_query,
                $cfgBookmark['user']
            );
        } else {
            $bookmark_support_html = '';
        }

        $html_output = isset($table_maintenance_html) ? $table_maintenance_html : '';

        $html_output .= $this->getHtmlForSqlQueryResults(
            $previous_update_query_html, $profiling_chart_html,
            $missing_unique_column_msg, $bookmark_created_msg,
            $table_html, $indexes_problems_html, $bookmark_support_html
        );

        return $html_output;
    }

    /**
     * Function to execute the query and send the response
     *
     * @param array      $analyzed_sql_results   analysed sql results
     * @param bool       $is_gotofile            whether goto file or not
     * @param string     $db                     current database
     * @param string     $table                  current table
     * @param bool|null  $find_real_end          whether to find real end or not
     * @param string     $sql_query_for_bookmark the sql query to be stored as bookmark
     * @param array|null $extra_data             extra data
     * @param string     $message_to_show        message to show
     * @param string     $message                message
     * @param array|null $sql_data               sql data
     * @param string     $goto                   goto page url
     * @param string     $pmaThemeImage          uri of the PMA theme image
     * @param string     $disp_query             display query
     * @param string     $disp_message           display message
     * @param string     $query_type             query type
     * @param string     $sql_query              sql query
     * @param array|null $selectedTables         array of table names selected from the
     *                                           database structure page, for an action
     *                                           like check table, optimize table,
     *                                           analyze table or repair table
     * @param string     $complete_query         complete query
     *
     * @return void
     */
    public function executeQueryAndSendQueryResponse($analyzed_sql_results,
        $is_gotofile, $db, $table, $find_real_end, $sql_query_for_bookmark,
        $extra_data, $message_to_show, $message, $sql_data, $goto, $pmaThemeImage,
        $disp_query, $disp_message, $query_type, $sql_query, $selectedTables,
        $complete_query
    ) {
        if ($analyzed_sql_results == null) {
            // Parse and analyze the query
            list(
                $analyzed_sql_results,
                $db,
                $table_from_sql
            ) = ParseAnalyze::sqlQuery($sql_query, $db);
            // @todo: possibly refactor
            extract($analyzed_sql_results);

            if ($table != $table_from_sql && !empty($table_from_sql)) {
                $table = $table_from_sql;
            }
        }

        $html_output = $this->executeQueryAndGetQueryResponse(
            $analyzed_sql_results, // analyzed_sql_results
            $is_gotofile, // is_gotofile
            $db, // db
            $table, // table
            $find_real_end, // find_real_end
            $sql_query_for_bookmark, // sql_query_for_bookmark
            $extra_data, // extra_data
            $message_to_show, // message_to_show
            $message, // message
            $sql_data, // sql_data
            $goto, // goto
            $pmaThemeImage, // pmaThemeImage
            $disp_query, // disp_query
            $disp_message, // disp_message
            $query_type, // query_type
            $sql_query, // sql_query
            $selectedTables, // selectedTables
            $complete_query // complete_query
        );

        $response = Response::getInstance();
        $response->addHTML($html_output);
    }

    /**
     * Function to execute the query and send the response
     *
     * @param array      $analyzed_sql_results   analysed sql results
     * @param bool       $is_gotofile            whether goto file or not
     * @param string     $db                     current database
     * @param string     $table                  current table
     * @param bool|null  $find_real_end          whether to find real end or not
     * @param string     $sql_query_for_bookmark the sql query to be stored as bookmark
     * @param array|null $extra_data             extra data
     * @param string     $message_to_show        message to show
     * @param string     $message                message
     * @param array|null $sql_data               sql data
     * @param string     $goto                   goto page url
     * @param string     $pmaThemeImage          uri of the PMA theme image
     * @param string     $disp_query             display query
     * @param string     $disp_message           display message
     * @param string     $query_type             query type
     * @param string     $sql_query              sql query
     * @param array|null $selectedTables         array of table names selected from the
     *                                           database structure page, for an action
     *                                           like check table, optimize table,
     *                                           analyze table or repair table
     * @param string     $complete_query         complete query
     *
     * @return string html
     */
    public function executeQueryAndGetQueryResponse(array $analyzed_sql_results,
        $is_gotofile, $db, $table, $find_real_end, $sql_query_for_bookmark,
        $extra_data, $message_to_show, $message, $sql_data, $goto, $pmaThemeImage,
        $disp_query, $disp_message, $query_type, $sql_query, $selectedTables,
        $complete_query
    ) {
        // Handle disable/enable foreign key checks
        $default_fk_check = Util::handleDisableFKCheckInit();

        // Handle remembered sorting order, only for single table query.
        // Handling is not required when it's a union query
        // (the parser never sets the 'union' key to 0).
        // Handling is also not required if we came from the "Sort by key"
        // drop-down.
        if (! empty($analyzed_sql_results)
            && $this->isRememberSortingOrder($analyzed_sql_results)
            && empty($analyzed_sql_results['union'])
            && ! isset($_POST['sort_by_key'])
        ) {
            if (! isset($_SESSION['sql_from_query_box'])) {
                $this->handleSortOrder($db, $table, $analyzed_sql_results, $sql_query);
            } else {
                unset($_SESSION['sql_from_query_box']);
        