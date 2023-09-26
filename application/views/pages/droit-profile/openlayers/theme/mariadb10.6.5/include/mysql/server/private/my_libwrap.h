okmark_created_msg = Message::success(
                __('Bookmark %s has been created.')
            );
            $bookmark_created_msg->addParam($_GET['label']);
        } else {
            $bookmark_created_msg = null;
        }

        return $bookmark_created_msg;
    }

    /**
     * Function to get html for the sql query results table
     *
     * @param DisplayResults $displayResultsObject instance of DisplayResult
     * @param string         $pmaThemeImage        theme image uri
     * @param string         $url_query            url query
     * @param array          $displayParts         the parts to display
     * @param bool           $editable             whether the result table is
     *                                             editable or not
     * @param int            $unlim_num_rows       unlimited number of rows
     * @param int            $num_rows             number of rows
     * @param bool           $showtable            whether to show table or not
     * @param object         $result               result of the executed query
     * @param array          $analyzed_sql_results analyzed sql results
     * @param bool           