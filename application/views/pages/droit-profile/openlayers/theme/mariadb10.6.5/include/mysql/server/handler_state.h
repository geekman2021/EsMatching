ove_query);
        }

        if ($cfgRelation['favoritework']) {
            $remove_query = 'DELETE FROM '
                . Util::backquote($cfgRelation['db'])
                . '.' . Util::backquote($cfgRelation['favorite'])
                . " WHERE `username`  = '" . $this->dbi->escapeString($username)
                . "'";
            $this->relation->queryAsControlUser($remove_query);
        }

        if ($cfgRelation['uiprefswork']) {
            $remove_query = 'DELETE FROM '
                . Util::backquote($cfgRelation['db'])
                . '.' . Util::backquote($cfgRelation['table_uiprefs'])
                . " WHERE `username`  = '" . $this->dbi->escapeString($username)
                . "'";
            $this->relation->query