->escapeString($db) . '\''
                . ' AND table_name = \'' . $this->dbi->escapeString($table)
                . '\'';
            $this->relation->queryAsControlUser($remove_query);
        }

        if ($cfgRelation['pdfwork']) {
            $remove_query = 'DELETE FROM '
                . Util::backquote($cfgRelation['db'])
                . '.' . Util::backquote($cfgRelation['table_coords'])
                . ' WHERE db_name  = \'' . $this->dbi->escapeString($db) . '\''
                . ' AND table_name = \'' . $this->dbi->escapeString($table)
                . '\'';
            $this->relation->queryAsControlUser($remove_query);
        }

        if ($cfgRelation['relwork']) {
            $remove_query = 'DELETE FROM '
                . Util::backquote($cfgRelation['db'])
                . '.' . Util::backquote($cfgRelation['relation'])
                . ' WHERE master_db  = \'' . $this->dbi->escapeString($db)
                . '\''
                . ' AND master_table = \'' . $this->dbi->escapeString($table)
                . '\'';
            $this->relation->queryAsControlUser($remove_query);

            $remove_query = 'DELETE FROM '
                . Util::backquote($cfgRelation['db'])
                . '.' . Util::backquote($cfgRelation['relation'])
                . ' WHERE foreign_db  = \'' . $this->dbi->escapeString($db)
                . '\''
                . ' AND foreign_table = \'' . $this->dbi->escapeString($table)
                . '\'';
            $this->relation->queryAsControlUser($remove_query);
        }

        if ($cfgRelation['uiprefswork']) {
            $remove_query = 'DELETE FROM '
                . Util::backquote($cfgRelation['db'])
                . '.' . Util::backquote($cfgRelation['table_uiprefs'])
                . ' WHERE db_name  = \'' . $this->dbi->escapeString($db) . '\''
                . ' AND table_name = \'' . $this->dbi->escapeString($table)
                . '\'';
            $this->relation->queryAsControlUser($remove_query);
        }

        if (! $cfgRelation['navwork']) {
            return;
        }

        $remove_query = 'DELETE FROM '
            . Util::backquote($cfgRelation['db'])
            . '.' . Util::backquote($cfgRelation['navigationhiding'])
            . ' WHERE db_name  = \'' . $this->dbi->escapeString($db) . '\''
            . ' AND (table_name = \'' . $this->dbi->escapeString($table)
            . '\''
            . ' OR (item_name = \'' . $this->dbi->escapeString($table)
            . '\''
            . ' AND item_type = \'table\'))';
        $this->relation->queryAsControlUser($remove_query);
    }

    /**
     * Cleanup database related relation stuff
     *
     * @param string $db database name
     *
     * @return void
     */
    public function database($db)
    {
        $cfgRelation = $this->relation->getRelationsParam();

        if ($cfgRelation['commwork']) {
            $remove_query = 'DELETE FROM '
                . Util::backquote($cfgRelation['db'])
                . '.' . Util::backquote($cfgRelation['column_info'])
                . ' WHERE db_name  = \'' . $this->dbi->escapeString($db) . '\'';
            $this->relation->queryAsControlUser($remove_query);
        }

        if ($cfgRelation['bookmarkwork']) {
            $remove_query = 'DELETE FROM '
                . Util::backquote($cfgRelation['db'])
                . '.' . Util::backquote($cfgRelation['bookmark'])
                . ' WHERE dbase  = \'' . $this->dbi->escapeString($db) . '\'';
            $this->relation->queryAsControlUser($remove_query);
        }

        if ($cfgRelation['displaywork']) {
            $remove_query = 'DELETE FROM '
                . Util::backquote($cfgRelation['db'])
                . '.' . Util::backquote($cfgRelation['table_info'])
                . ' WHERE db_name  = \'' . $this->dbi->escapeString($db) . '\'';
            $this->relation->queryAsControlUser($remove_query);
        }

        if ($cfgRelation['pdfwork']) {
            $remove_query = 'DELETE FROM '
                . Util::backquote($cfgRelation['db'])
                . '.' . Util::backquote($cfgRelation['pdf_pages'])
                . ' WHERE db_name  = \'' . $this->dbi->escapeString($db) . '\'';
            $this->relation->queryAsControlUser($remove_query);

            $remove_query = 'DELETE FROM '
                . Util::backquote($cfgRelation['db'])
                . '.' . Util::backquote($cfgRelation['table_coords'])
                . ' WHERE db_name  = \'' . $this->dbi->escapeString($db) . '\'';
            $this->relation->queryAsControlUser($remove_query);
        }

        if ($cfgRelation['relwork']) {
            $remove_query = 'DELETE FROM '
                . Util::backquote($cfgRelation['db'])
                . '.' . Util::backquote($cfgRelation['relation'])
                . ' WHERE master_db  = \''
                . $this->dbi->escapeStrin