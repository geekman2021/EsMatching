} while ($GLOBALS['dbi']->moreResults() && $GLOBALS['dbi']->nextResult());

        } else {
            $fields_meta = array();
            if (isset($result) && ! is_bool($result)) {
                $fields_meta = $GLOBALS['dbi']->getFieldsMeta($result);
            }
            $fields_cnt = count($fields_meta);
            $_SESSION['is_multi_query'] = false;
            $displayResultsObject->setProperties(
                $unlim_num_rows,
                $fields_meta,
                $analyzed_sql_results['is_count'],
                $analyzed_sql_results['is_export'],
                $analyzed_sql_results['is_func'],
                $analyzed_sql_results['is_analyse'],
                $num_rows,
                $fields_cnt, $GLOBALS['querytime'],
                $pmaThemeImage, $GLOBALS['text_dir'],
                $analyzed_sql_results['is_maint'],
                $analyzed_sql_results['is_explain'],
                $analyzed_sql_results['is_show'],
                $showtable,
                $printview,
                $url_query,
                $editable,
                $browse_dist
            );

            if (! is_bool($result)) {
                $table_html .= $displayResultsObject->getTable(
                    $result,
                    $displayParts,
                    $analyzed_sql_results,
                    $is_limited_display
                );
            }
            $GLOBALS['dbi']->freeResult($result);
        }

        return