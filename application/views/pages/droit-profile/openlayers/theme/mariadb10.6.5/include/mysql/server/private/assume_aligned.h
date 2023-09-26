n];
            }
            if (!isset($this->SHPData[$max]) || ($this->SHPData[$max] < $point[$direction])) {
                $this->SHPData[$max] = $point[$direction];
            }
        }
    }

    /**
     * Sets dimension to 0 if not set.
     *
     * @param array  $point     Point to check
     * @param string $dimension Dimension to check
     *
     * @return array
     */
    private function _fixPoint($point, $dimension)
    {
        if (!isset($point[$dimension])) {
            $point[$dimension] = 0.0; // no_value
        }

        return $point;
    }

    /**
     * Adjust point and bounding box when adding point.
     *
     * @param array $point Point data
     *
     * @return array Fixed point data
     */
    private function _adjustPoint($point)
    {
        $type = $this->shapeType / 10;
        if ($type >= 2) {
            $point = $this->_fixPoint($point, 'm');
        } elseif ($type >= 1) {
            $point = $this->_fixPoint($point, 'z');
            $point = $this->_fixPoint($point, 'm');
        }

        return $point;
    }

    /**
     * Adds point to a record.
     *
     * @param array $point     Point data
     * @param int   $partIndex Part index
     */
    public function addPoint($point, $partIndex = 0)
    {
        $point = $this->_adjustPoint($point);
        switch ($this->shapeType) {
            case 0:
                //Don't add anything
                return;
            case 1:
            case 11:
            case 21:
                //Substitutes the value of the current point
                $this->SHPData = $point;
                break;
            case 3:
            case 5:
            case 13:
            case 15:
            case 23:
            case 25:
                //Adds a new point to the selected part
                $this->SHPData['parts'][$partIndex]['points'][] = $point;
                $this->SHPData['numparts'] = count($this->SHPData['parts']);
                $this->SHPData['numpoints'] = 1 + (isset($this->SHPData['numpoints']) ? $this->SHPData['numpoints'] : 0);
                break;
            case 8:
            case 18:
            case 28:
                //Adds a new point
                $this->SHPData['points'][] = $point;
                $this->SHPData['numpoints'] = 1 + (isset($this->SHPData['numpoints']) ? $thi