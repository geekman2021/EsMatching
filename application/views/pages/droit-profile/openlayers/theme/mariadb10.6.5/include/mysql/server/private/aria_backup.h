; ++$i) {
            if ($part + 1 < $numparts && $this->SHPData['parts'][$part + 1] == $i) {
                ++$part;
            }
            if (!isset($this->SHPData['parts'][$part]['points']) || !is_array($this->SHPData['parts'][$part]['points'])) {
                $this->SHPData['parts'][$part] = array('points' => array());
            }
            $this->SHPData['parts'][$part]['points'][] = $this->_loadPoint();
        }
    }

    /**
     * @param string $type
     */
    private function _loadPolyLineMZRecord($type)
    {
        /* The m dimension is optional, depends on bounding box data */
        if ($type == 'm' && !$this->ShapeFile->hasMeasure()) {
            return;
        }

        $this->SHPData[$type . 'min'] = $this->_loadData('d', 8);
        $this->SHPData[$type . 'max'] = $this->_loadData('d', 8);

        $numparts = $this->SHPData['numparts'];
        $numpoints = $this->SHPData['numpoints'];

        $part = 0;
        for ($i = 0; $i < $numpoints; ++$i) {
            if ($part + 1 < $numparts && $this->SHPData['parts'][$part + 1] == $i) {
                ++$part;
            }
            $this->SHPData['parts'][$part]['points'][$i][$type] = $this->_loadData('d', 8);
        }
    }

    private function _loadPolyLineMRecord()
    {
        $this->_loadPolyLineRecord();

        $this->_loadPolyLineMZRecord('m');
    }

    private function _loadPolyLineZRecord()
    {
        $this->_loadPolyLineRecord();

        $this->_loadPolyLineMZRecord('z');
        $this->_loadPolyLineMZRecord('