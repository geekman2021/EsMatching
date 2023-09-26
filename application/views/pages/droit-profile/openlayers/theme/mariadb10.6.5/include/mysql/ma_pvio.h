ype = $params['Index_type'];
        }
        if (isset($params['Comment'])) {
            $this->remarks = $params['Comment'];
        }
        if (isset($params['Index_comment'])) {
            $this->comment = $params['Index_comment'];
        }
        if (isset($params['Non_unique'])) {
            $this->nonUnique = $params['Non_unique'];
        }
        if (isset($params['Packed'])) {
            $this->packed = $params['Packed'];
        }
        if (isset($params['Index_choice'])) {
            $this->choice = $params['Index_choice'];
        } elseif ($this->name === 'PRIMARY') {
            $this->choice = 'PRIMARY';
        } elseif ($this->type === 'FULLTEXT') {
            $this->choice = 'FULLTEXT';
            $this->type = '';
        } elseif ($this->type === 'SPATIAL') {
            $this->choice = 'SPATIAL';
            $this->type = '';
        } elseif ($this->nonUnique == '0') {
            $this->choice = 'UNIQUE';
        } else {
            $this->choice = 'INDEX';
        }
        if (isset($params['Key_block_size'])) {
            $this->keyBlockSize = $params['Key_block_size'];
        }
        if (! isset($params['Parser'])) {
            return;
        }

        $this->parser = $params['Parser'];
    }

    /**
     * Returns the number of columns of the index
     *
     * @return int the number of the columns
     */
    public function getColumnCount()
    {
        return count($this->columns);
    }

    /**
     * Returns the index comment
     *
     * @return string index comment
     */
    public function getComment()
    {
        return $this->comment;
    }

    /**
     * Returns index remarks
     *
     * @return string index remarks
     */
    public function getRemarks()
    {
        return $this->remarks;
    }

    /**
     * Return the key block size
     *
     * @return int
     */
    public function getKeyBlockSize()
    {
        return $this->keyBlockSize;
    }

    /**
     * Return the parser
     *
     * @return string
     */
    public function getParser()
    {
        return $this->parser;
    }

    /**
     * Returns concatenated remarks and comment
     *
     * @return string concatenated remarks and comment
     */
    public function getComments()
    {
        $comments = $this->getRemarks();
        if (strlen($comments) > 0) {
            $comments .= "\n";
        }
        $comments .= $this->getComment();

        return $comments;
    }

    /**
     * Returns index type (BTREE, HASH, RTREE)
     *
     * @return string index type
     */
    public function getType()
    {
        return $this->type;
    }

    /**
     * Returns index choice (PRIMARY, UNIQUE, INDEX, SPATIAL, FULLTEXT)
     *
     * @return string index choice
     */
    public function getChoice()
    {
        return $this->choice;
    }

    /**
     * Returns a lit of all index types
     *
     * @return string[] index types
     */
    public static function getIndexTypes()
    {
        return [
            'BTREE',
            'HASH',
        ];
    }

    public function hasPrimary(): bool
    {
        return (bool) self::getPrimary($this->table, $this->schema);
    }

    /**
     * Returns how the index is packed
     *
     * @return string how the index is packed
     */
    public function getPacked()
    {
        return $this->packed;
    }

    /**
     * Returns 'No' if the index is not packed,
     * how the index is packed if packed
     *
     * @return string
     */
    public function isPacked()
    {
        if ($this->packed === null) {
            return __('No');
        }

        return htmlspecialchars($this->packed);
    }

    /**
     * Returns integer 0 if the index cannot contain duplicates, 1 if it can
     *
     * @return int 0 if the index cannot contain duplicates, 1 if it can
     */
    public function getNonUnique()
    {
        return $this->nonUnique;
    }

    /**
     * Returns whether the index is a 'Unique' index
     *
     * @param bool $as_text whether to output should be in text
     *
     * @return mixed whether the index is a 'Unique' index
     */
    public function isUnique($as_text = false)
    {
        if ($as_text) {
            $r = [
                '0' => __('Yes'),
                '1' => __('No'),
            ];
        } else {
            $r = [
                '0' => true,
                '1' => false,
            ];
        }

        return $r[$this->nonUnique];
    }

    /**
     * Returns the name of the index
     *
