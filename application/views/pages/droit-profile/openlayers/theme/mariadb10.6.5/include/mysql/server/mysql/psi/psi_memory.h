  switch ($this->shapeType) {
            case 0:
                // Nothing to save
                break;
            case 1:
                $this->_savePointRecord();
                break;
            case 21:
                $this->_savePointMRecord();
                break;
            case 11:
                $this->_savePointZRecord();
                break;
            case 3:
                $this->_savePolyLineRecord();
                break;
            case 23:
                $this->_savePolyLineMRecord();
                break;
            case 13:
                $this->_savePolyLineZRecord();
                break;
            case 5:
                $this->_savePolygonRecord();
                break;
            case 25:
                $this->_savePolygonMRecord();
                break;
            case 15:
                $this->_savePolygonZRecord();
                break;
            case 8:
                $this->_saveMultiPointRecord();
                break;
            case 28:
                $this->_saveMultiPointMRecord();
                break;
            case 18:
                $this->_saveMultiPointZRecord();
                break;
            default:
                $this->setError(sprintf('The Shape Type "%s" is not supported.', $this->shapeType));
                break;
        }
        if (ShapeFile::supports_dbase() && !is_null($this->DBFFile)) {
            $this->_saveDBFData();
        }
    }

    /**
     * Updates DBF data to match header.
     *
     * @param array $header DBF structure header
     */
    public function updateDBFInfo($header)
    {
        $tmp = $this->DBFData;
        unset($this->DBFData);
        $this->DBFData = array();
        foreach ($header as $value) {
            $this->DBFData[$value[0]] = (isset($tmp[$value[0]])) ? $tmp[$value[0]] : '';
        }
    }

    /**
     * Reads data.
     *
     * @param string $type  type for unpack()
     * @param int    $count number of bytes
     *
     * @return mixed
     */
    private function _loadData($type, $count)
    {
        $data = $this->ShapeFile->readSHP($count);
        if ($data === false) {
            return false;
        }
        $this->read += strlen($data);

        return Util::loadData($type, $data);
    }

    /**
     * Loads metadata header from a file.
     */
    private function _loadHeaders()
    {
        $this->shapeType = false;
        $this->recordNumber = $this->_loadData('N', 4);
        if ($this->recordNumber === false) {
            return;
        }
        // We read the length of the record
        $this->size = $this->_loadData('N', 4);
        if ($this->size === false) {
            return;
        }
        $this->size = $this->size * 2 + 8;
        $this->shapeType = $this->_loadData('V', 4);
    }

    /**
     * Saves metadata header to a file.
     */
    private function _saveHeaders()
    {
        fwrite($this->SHPFile, pack('N', $this->recordNumber));
        fwrite($this->SHPFile, pack('N', $this->getContentLength()));
        fwrite($this->SHPFile, pack('V', $this->shapeType));
    }

    private function _loadPoint()
    {
        $data = array();

        $data['x'] = $this->_loadData('d', 8);
        $data['y'] = $this->_loadData('d', 8);

        return $data;
    }

    private function _loadPointM()
    {
        $data = $this->_loadPoint();

        $data['m'] = $this->_loadData('d', 8);

        return $data;
    }

    private function _loadPointZ()
    {
        $data = $this->_loadPoint();

        $data['z'] = $this->_loadData('d', 8);
        $data['m'] = $this->_loadData('d', 8);

        return $data;
    }

    private function _savePoint($data)
    {
        fwrite($this->SHPFile, Util::packDouble($data['x']));
        fwrite($this->SHPFile, Util::packDouble($data['y']));
    }

    private function _savePointM($data)
    {
        fwrite($this->SHPFile, Util::packDouble($data['x']));
        fwrite($this->SHPFile, Util::packDouble($data['y']));
        fwrite($this->SHPFile, Util::packDouble($data['m']));
    }

    private function _savePointZ($data)
    {
        fwrite($this->SHPFile, Util::packDouble($data['x']));
        fwrite($this->SHPFile, Util::packDouble($data['y']));
        fwrite($this->SHPFile, Util::packDouble($data['z']));
        fwrite($this->SHPFile, Util::packDouble($data['m']));
    }

    private function _loadNullRecord()
    {
        $this->SHPData = array();
    }

    private function _loadPointRecord()
    {
        $this->SHPData = $this->_loadPoint();
    }

    private function _loadPointMRecord()
    {
        $this->SHPData = $this->_loadPointM();
    }

    private function _loadPointZRecord()
    {
        $this->SHPData = $this->_loadPointZ();
    }

    private function _savePointRecord()
    {
        $this->_s