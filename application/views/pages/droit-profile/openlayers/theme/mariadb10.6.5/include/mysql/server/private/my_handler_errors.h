unction to get values for Enum or Set Columns
     *
     * @param string $db         the current database
     * @param string $table      the current table
     * @param string $columnType whether enum or set
     *
     * @return void
     */
    public function getEnumOrSetValues($db, $table, $columnType)
    {
        $column = $_POST['column'];
        $curr_value = $_POST['curr_value'];
        $response = Response::getInstance();
        if ($columnType == "enum") {
            $dropdown = $this->getHtmlForEnumColumnDropdown(
                $db, $table, $column, $curr_value
            );
            $response->addJSON('dropdown', $dropdown);
        } else {
            $select = $this->getHtmlForSetColumn(
                $db, $table, $column, $curr_value
            );
            $response->addJSON('select', $select);
        }
        exit;
    }

    /**
     * Function to get the default sql query for browsing page
     *
     * @param string $db    the current database
     * @param string $table the current table
     *
     * @return string $sql_query the default $sql_query for browse page
     */
    public function getDefaultSqlQueryForBrowse($db, $table)
    {
        $bookmark = Bookmark::get(
            $GLOBALS['dbi'],
            $GLOBALS['cfg']['Server']['user'],
            $db,
            $table,
            'label',
            false,
            true
        );

        if (! empty($bookmark) && ! empty($bookmark->getQuery())) {
            $GLOBALS['using_bookmark_message'] = Message::notice(
                __('Using bookmark "%s" as default browse query.')
            );
            $GLOBALS['using_bookmark_message']->addParam($table);
            $GLOBALS['using_bookmark_message']->addHtml(
                Util::showDocu('faq', 'faq6-22')
            );
            $sql_query = $bookmark->getQuery();
        } else {

            $defaultOrderByClause = '';

            if (isset($GLOBALS['cfg']['TablePrimaryKeyOrder'])
                && ($GLOBALS['cfg']['TablePrimaryKeyOrder'] !== 'NONE')
            ) {

                $primaryKey     = null;
                $primary        = Index::getPrimary($table, $db);

                if ($primary !== false) {

                    $primarycols    = $primary->getColumns();

                    foreach ($primarycols as $col) {
                        $primaryKey = $col->getName();
                        break;
                    }

                    if ($primaryKey != null) {
                        $defaultOrderByClause = ' ORDER BY '
                            . Util::backquote($table) . '.'
                            . Util::backquote($primaryKey) . ' '
                            . $GLOBALS['cfg']['TablePrimaryKeyOrder'];
                    }

                }

            }

            $sql_query = 'SELECT * FROM ' . Util::backquote($table)
                . $defaultOrderByClause;

        }

        return $sql_query;
    }

    /**
     * Responds an error when an error happens when executing the query
     *
     * @param boolean $is_gotofile    whether goto file or not
     * @param string  $error          error after executing the query
     * @param string  $full_sql_query full sql query
     *
     * @return void
     */
    private function handleQueryExecuteError($is_gotofile, $error, $full_sql_query)
    {
        if ($is_gotofile) {
            $message = Message::rawError($error);
            $response = Response::getInstance();
            $response->setRequestStatus(false);
            $response->addJSON('message', $message);
        } else {
            Util::mysqlDie($error, $full_sql_query, '', '');
        }
        exit;
    }

    /**
     * Function to store the query as a bookmark
     *
     * @param string  $db                     the current database
     * @param string  $bkm_user               the bookmarking user
     * @param string  $sql_query_for_bookmark the query to be stored in bookmark
     * @param string  $bkm_label              bookmark label
     * @param boolean $bkm_replace            whether to replace existing bookmarks
     *
     * @return void
     */
    public function storeTheQueryAsBookmark($db, $bkm_user, $sql_query_for_bookmark,
        $bkm_label, $bkm_replace
    ) {
        $bfields = array(
            'bkm_database' => $db,
            'bkm_user'  => $bkm_user,
            'bkm_sql_query' => $sql_query_for_bookmark,
            'bkm_label' => $bkm_label,
        );

        // Should we replace bookmark?
        if (isset($bkm_replace)) {
            $bookmarks = Bookmark::getList(
                $GLOBALS['dbi'],
                $GLOBALS['cfg']['Server']['user'],
                $db
            );
  