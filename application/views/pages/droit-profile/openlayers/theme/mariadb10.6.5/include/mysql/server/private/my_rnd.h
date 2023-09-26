layResultsObject->setConfigParamsForDisplayTable();

        // assign default full_sql_query
        $full_sql_query = $sql_query;

        // Do append a "LIMIT" clause?
        if ($this->isAppendLimitClause($analyzed_sql_results)) {
            $full_sql_query = $this->getSqlWithLimitClause($analyzed_sql_results);
        }

        $GLOBALS['reload'] = $this->hasCurrentDbChanged($db);
        $GLOBALS['dbi']->selectDb($db);

        // Execute the query
        list($result, $num_rows, $unlim_num_rows, $profiling_results, $extra_data)
            = $this->executeTheQuery(
                $analyzed_sql_results,
                $full_sql_query,
                $is_gotofile,
                $db,
                $table,
                isset($find_real_end) ? $find_real_end : null,
                isset($sql_query_for_bookmark) ? $sql_query_for_bookmark : null,
                isset($extra_data) ? $extra_data : null
            );

        if ($GLOBALS['dbi']->moreResults()) {
            $GLOBALS['dbi']->nextResult();
        }

        $operatio