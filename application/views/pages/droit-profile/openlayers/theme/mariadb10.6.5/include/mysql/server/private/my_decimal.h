sults(
        $profiling_results
    ) {
        $profiling_stats = array(
            'total_time' => 0,
            'states' => array(),
        );
        $chart_json = Array();
        $i = 1;
        $table = '';
        foreach ($profiling_results as $one_result) {
            if (isset($profiling_stats['states'][ucwords($one_result['Status'])])) {
                $states = $profiling_stats['states'];
                $states[ucwords($one_result['Status'])]['total_time']
                    += $one_result['Duration'];
                $states[ucwords($one_result['Status'])]['calls']++;
            } else {
                $profiling_stats['states'][ucwords($one_result['Status'])] = array(
                    'total_time' => $one_result['Duration'],
                    'calls' => 1,
                );
            }
            $profiling_stats['total_time'] += $one_result['Duration'];

            $table .= ' <tr>' . "\n";
            $table .= '<td>' . $i++ . '</td>' . "\n";
            $table .= '<td>' . ucwords($one_result['Status'])
                . '</td>' . "\n";
            $table .= '<td class="right">'
                . (Util::formatNumber($one_result['Duration'], 3, 1))
                . 's<span class="rawvalue hide">'
                . $one_result['Duration'] . '</span></td>' . "\n";
            if (isset($chart_json[ucwords($one_result['Status'])])) {
                $chart_json[ucwords($one_result['Status'])]
                    += $one_result['Duration'];
            } else {
                $chart_json[ucwords($one_result['Status'])]
                    = $one_result['Duration'];
            }
        }
        return array($table, $chart_json, $profiling_stats);
    }

    /**
     * Function to get HTML for summary by state table
     *
     * @param array $profiling_stats profiling stats
     *
     * @return string $table html for the table
     */
    private function getTableHtmlForProfilingSummaryByState(array $profiling_stats)
    {
        $table = '';
        foreach ($profiling_stats['states'] as $name => $stats) {
            $table .= ' <tr>' . "\n";
            $table .= '<td>' . $name . '</td>' . "\n";
            $table .= '<td align="right">'
                . Util::formatNumber($stats['total_time'], 3, 1)
                . 's<span class="rawvalue hide">'
                . $stats['total_time'] . '</span></td>' . "\n";
            $table .= '<td align="right">'
                . Util::formatNumber(
                    100 * ($stats['total_time'] / $profiling_stats['total_time']),
                    0, 2
                )
            . '%</td>' . "\n";
            $table .= '<td align="right">' . $stats['calls'] . '</td>'
                . "\n";
            $table .= '<td align="right">'
                . Util::formatNumber(
                    $stats['total_time'] / $stats['calls'], 3, 1
                )
                . 's<span class="rawvalue hide">'
                . number_format($stats['total_time'] / $stats['calls'], 8, '.', '')
                . '</span></td>' . "\n";
            $table .= ' </tr>' . "\n";
        }
        return $table;
    }

    /**
     * Get the HTML for the enum column dropdown
     * During grid edit, if we have a enum field, returns the html for the
     * dropdown
     *
     * @param string $db         current database
     * @param string $table      current table
     * @param string $column     current column
     * @param string $curr_value currently selected value
     *
     * @return string $dropdown html for the dropdown
     */
    private function getHtmlForEnumColumnDropdown($db, $table, $column, $curr_value)
    {
        $values = $this->getValuesForColumn($db, $table, $column);
        $dropdown = '<option value="">&nbsp;</option>';
        $dropdown .= $this->getHtmlForOptionsList($values, array($curr_value));
        $dropdown = '<select>' . $dropdown . '</select>';
        return $dropdown;
    }

    /**
     * Get value of a column for a specific row (marked by $where_clause)
     *
     * @param string $db           current database
     * @param string $table        current table
     * @param string $column       current column
     * @param string $where_clause where clause to select a particular row
     *
     * @return string with value
     */
    private function getFullValuesForSetColumn($db, $table, $column, $where_clause)
    {
        $result = $GLOBALS['dbi']->fetchSingleRow(
            "SELECT `$column` FROM `$db`.`$table` WHERE $where_clause"
        );

        return $result[$column];
    }

    /**
     * Get the HTML for the set column dropdown
     * During grid edit, if we have a set field, returns the html for the
     * dropdown
     *
     * @param string $db         current database
     * @param string $table      current table
     * @param string $column     current column
     * @param string $curr_value currently selected value
     *
     * @return string $dropdown html for the set column
     */
    private function getHtmlForSetColumn($db, $table, $column, $curr_value)
    {
        $values = $this->getValuesForColumn($db, $table, $column);
        $dropdown = '';
        $full_values =
            isset($_POST['get_full_values']) ? $_POST['get_full_values'] : false;
        $where_clause =
            isset($_POST['where_clause']) ? $_POST['where_clause'] : null;

        // If the $curr_value was truncated, we should
        // fetch the correct full values from the table
        if ($full_values && ! empty($where_clause)) {
            $curr_value = $this->getFullValuesForSetColumn(
                $db, $table, $column, $where_clause
            );
        }

        //converts characters of $curr_value to HTML entities
        $converted_curr_value = htmlentities(
            $curr_value, ENT_COMPAT, "UTF-8"
        );

        $selected_values = explode(',', $converted_curr_value);

        $dropdown .= $this->getHtmlForOptionsList($values, $selected_values);

        $select_size = (sizeof($values) > 10) ? 10 : sizeof($values);
        $dropdown = '<select multiple="multiple" size="' . $select_size . '">'
            . $dropdown . '</select>';

        return $dropdown;
    }

    /**
     * Get all the values for a enum column or set column in a table
     *
     * @param string $db     current database
     * @param string $table  current table
     * @param string $column current column
     *
     * @return array $values array containing the value list for the column
     */
    private function getValuesForColumn($db, $table, $column)
    {
        $field_info_query = $GLOBALS['dbi']->getColumnsSql($db, $table, $column);

        $field_info_result = $GLOBALS['dbi']->fetchResult(
            $field_info_query,
            null,
            null,
            DatabaseInterface::CONNECT_USER,
            DatabaseInterface::QUERY_STORE
        );

        $values = Util::parseEnumSetValues($field_info_result[0]['Type']);

        return $values;
    }

    /**
     * Get HTML for options list
     *
     * @param array $values          set of values
     * @param array $selected_values currently selected values
     *
     * @return string $options HTML for options list
     */
    private function getHtmlForOptionsList(array $values, array $selected_values)
    {
        $options = '';
        foreach ($values as $value) {
            $options .= '<option value="' . $value . '"';
            if (in_array($value, $selected_values, true)) {
                $options .= ' selected="selected" ';
            }
            $options .= '>' . $value . '</option>';
        }
        return $options;
    }

    /**
     * Function to get html for bookmark support if bookmarks are enabled. Else will
     * return null
     *
     * @param array  $displayParts   the parts to display
     * @param array  $cfgBookmark    configuration setting for bookmarking
     * @param string $sql_query      sql query
     * @param string $db             current database
     * @param string $table          current table
     * @param string $complete_query complete query
     * @param string $bkm_user       bookmarking user
     *
     * @return string $html
     */
    public function getHtmlForBookmark(array $displayParts, array $cfgBookmark, $sql_query, $db,
        $table, $complete_query, $bkm_user
    ) {
        if ($displayParts['bkm_form'] == '1'
            && (! empty($cfgBookmark) && empty($_GET['id_bookmark']))
            && ! empty($sql_query)
        ) {
            $goto = 'sql.php'
                . Url::getCommon(
                    array(
                        'db' => $db,
                        'table' => $table,
                        'sql_query' => $sql_query,
                        'id_bookmark'=> 1,
                    )
                );
            $bkm_sql_query = isset($complete_query) ? $complete_query : $sql_query;
            $html = '<form action="sql.php" method="post"'
                . ' onsubmit="return ! emptyCheckTheField(this,'
                . '\'bkm_fields[bkm_label]\');"'
                . ' class="bookmarkQueryForm print_ignore">';
            $html .= Url::getHiddenInputs();
            $html .= '<input type="hidden" name="db"'
                . ' value="' . htmlspecialchars($db) . '" />';
            $html .= '<input type="hidden" name="goto" value="' . $goto . '" />';
            $html .= '<input type="hidden" name="bkm_fields[bkm_database]"'
                . ' value="' . htmlspecialchars($db) . '" />';
            $html .= '<input type="hidden" name="bkm_fields[bkm_user]"'
                . ' value="' . $bkm_user . '" />';
            $html .= '<input type="hidden" name="bkm_fields[bkm_sql_query]"'
                . ' value="'
                . htmlspecialchars($bkm_sql_query)
                . '" />';
            $html .= '<fieldset>';
            $html .= '<legend>';
            $html .= Util::getIcon(
                'b_bookmark', __('Bookmark this SQL query'), true
            );
            $html .= '</legend>';
            $html .= '<div class="formelement">';
            $html .= '<label>' . __('Label:');
            $html .= '<input type="text" name="bkm_fields[bkm_label]" value="" />' .
                '</label>';
            $html .= '</div>';
            $html .= '<div class="formelement">';
            $html .= '<label>' .
                '<input type="checkbox" name="bkm_all_users" value="true" />';
            $html .=  __('Let every user access this bookmark') . '</label>';
            $html .= '</div>';
            $html .= '<div class="clearfloat"></div>';
            $html .= '</fieldset>';
            $html .= '<fieldset class="tblFooters">';
            $html .= '<input type="hidden" name="store_bkm" value="1" />';
            $html .= '<input type="submit"'
                . ' value="' . __('Bookmark this SQL query') . '" />';
            $html .= '</fieldset>';
            $html .= '</form>';

        } else {
            $html = null;
        }

        return $html;
    }

    /**
     * Function to check whether to remember the sorting order or not
     *
     * @param array $analyzed_sql_results the analyzed query and other variables set
     *                                    after analyzing the query
     *
     * @return boolean
     */
    private function isRememberSortingOrder(array $analyzed_sql_results)
    {
        return $GLOBALS['cfg']['RememberSorting']
            && ! ($analyzed_sql_results['is_count']
                || $analyzed_sql_results['is_export']
                || $analyzed_sql_results['is_func']
                || $analyzed_sql_results['is_analyse'])
            && $analyzed_sql_results['select_from']
            && isset($analyzed_sql_results['select_expr'])
            && isset($analyzed_sql_results['select_tables'])
            && ((empty($analyzed_sql_results['select_expr']))
                || ((count($analyzed_sql_results['select_expr']) == 1)
                    && ($analyzed_sql_results['select_expr'][0] == '*')))
            && count($analyzed_sql_results['select_tables']) == 1;
    }

    /**
     * Function to check whether the LIMIT clause should be appended or not
     *
     * @param array $analyzed_sql_results the analyzed query and other variables set
     *                                    after analyzing the query
     *
     * @return boolean
     */
    private function isAppendLimitClause(array $analyzed_sql_results)
    {
        // Assigning LIMIT clause to an syntactically-wrong query
        // is not needed. Also we would want to show the true query
        // and the true error message to the query executor

        return (isset($analyzed_sql_results['parser'])
            && count($analyzed_sql_results['parser']->errors) === 0)
            && ($_SESSION['tmpval']['max_rows'] != 'all')
            && ! ($analyzed_sql_results['is_export']
            || $analyzed_sql_results['is_analyse'])
            && ($analyzed_sql_results['select_from']
                || $analyzed_sql_results['is_subquery'])
            && empty($analyzed_sql_results['limit']);
    }

    /**
     * Function to check whether this query is for just browsing
     *
     * @param array   $analyzed_sql_results the analyzed query and other variables set
     *                                      after analyzing the query
     * @param boolean $find_real_end        whether the real end should be found
     *
     * @return boolean
     */
    public function isJustBrowsing(array $analyzed_sql_results, $find_real_end)
    {
        return ! $analyzed_sql_results['is_group']
            && ! $analyzed_sql_results['is_func']
            && empty($analyzed_sql_results['union'])
            && empty($analyzed_sql_results['distinct'])
            && $analyzed_sql_results['select_from']
            && (count($analyzed_sql_results['select_tables']) === 1)
            && (empty($analyzed_sql_results['statement']->where)
                || (count($analyzed_sql_results['statement']->where) == 1
                    && $analyzed_sql_results['statement']->where[0]->expr ==='1'))
            && empty($analyzed_sql_results['group'])
            && ! isset($find_real_end)
            && ! $analyzed_sql_results['is_subquery']
            && ! $analyzed_sql_results['join']
            && empty($analyzed_sql_results['having']);
    }

    /**
     * Function to check whether the related transformation information shoul