_unique_column_msg = null;
        }

        return $missing_unique_column_msg;
    }

    /**
     * Function to get html to display problems in indexes
     *
     * @param string     $query_type     query type
     * @param array|null $selectedTables array of table names selected from the
     *                                   database structure page, for an action
     *                                   like check table, optimize table,
     *                                   analyze table or repair table
     * @param string     $db             current database
     *
     * @return string
     */
    private function getHtmlForIndexesProblems($query_type, $selectedTables, $db)
    {
        // BEGIN INDEX CHECK See if indexes should be checked.
        if (isset($query_type)
            && $query_type == 'check_tbl'
         