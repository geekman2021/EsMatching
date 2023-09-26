query) ? $url_query : null;

        $response = Response::getInstance();
        $header   = $response->getHeader();
        $scripts  = $header->getScripts();

        $just_one_table = $this->resultSetHasJustOneTable($fields_meta);

        // hide edit and delete links:
        // - for information_schema
        // - if the result set does not contain all the columns of a unique key
        //   (unless this is an updatable view)
        // - if the SELECT query contains a join or a subquery

        $updatableView = false;

        $statement = isset($analyzed_sql_results['statement']) ? $analyzed_sql_results['statement'] : null;
        if ($statement instanceof SelectStatement) {
            if (!empty($statement->expr)) {
                if ($statement->expr[0]->expr === '*') {
                    $_table = new Table($table, $db);
                    $updatableView = $_table->isUpdatableView();
                }
            }

            if ($analyzed_sql_results['join']
                || $analyzed_sql_results['is_subquery']
                || count($analyzed_sql_results['select_tables']) !== 1
            ) {
                $just_one_table = false;
            }
        }

        $has_unique = $this->resultSetContainsUniqueKey(
            $db, $table, $fields_meta
        );

        $editable = ($has_unique
            || $GLOBALS['cfg']['RowActionLinksWithoutUnique']
            || $updatableView)
