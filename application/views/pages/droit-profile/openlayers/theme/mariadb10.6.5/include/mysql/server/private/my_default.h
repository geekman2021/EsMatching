property_to_set = '';
        }
        $retval = $pmatable->setUiProp(
            $property_to_set,
            $property_value,
            isset($_POST['table_create_time']) ? $_POST['table_create_time'] : null
        );
        if (gettype($retval) != 'boolean') {
            $response = Response::getInstance();
            $response->setRequestStatus(false);
            $response->addJSON('message', $retval->getString());
            exit;
        }

        return $retval;
    }

    /**
     * Function to check the request for setting the column order or visibility
     *
     * @param string $table the current table
     * @param string $db    the current database
     *
     * @return void
     */
    public function setColumnOrderOrVisibility($table, $db)
    {
        $pmatable = new Table($table, $db);
        $retval = false;

        // set column order
        if (isset($_POST['col_order'])) {
            $retval = $this->setColumnProperty($pmatable, 'col_order');
        }

        // set column visibility
        if ($retval === true && isset($_POST['col_visib'])) {
            $retval = $this->setColumnProperty($pmatable, 'col_visib');
        }

        $response = Response::getInstance();
        $response->setRequestStatus($retval == true);
        exit;
    }

    /**
     * Function to add a bookmark
     *
     * @param string $goto goto page URL
     *
     * @return void
     */
    public function addBookmark($goto)
    {
        $bookmark = Bookmark::createBookmark(
            $GLOBALS['dbi'],
            $GLOBALS['cfg']['Server']['user'],
            $_POST['bkm_fields'],
            (isset($_POST['bkm_all_users'])
                && $_POST['bkm_all_users'] == 'true' ? true : false
            )
        );
        $result = $bookmark->save();
        $response = Response::getInstance();
        if ($response->isAjax(