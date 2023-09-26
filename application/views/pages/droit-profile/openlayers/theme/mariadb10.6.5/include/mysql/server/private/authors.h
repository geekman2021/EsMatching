'][$partIndex]['points'][count($this->SHPData['parts'][$partIndex]['points']) - 1]);

                    $this->SHPData['numparts'] = count($this->SHPData['parts']);
                    --$this->SHPData['numpoints'];
                }
                break;
            case 8:
            case 18:
            case 28:
                //Deletes the point, if exists
                if (isset($this->SHPData['points'][$pointIndex])) {
                    $count = count($this->SHPData['points']) - 1;
                    for ($i = $pointIndex; $i < $count; ++$i) {
                        $this->SHPData['points'][$i] = $this->SHPData['points'][$i + 1];
                    }
                    unset($this->SHPData['points'][count($this->SHPData['points']) - 1]);

                    --$this->SHPData['numpoints'];
                }
                break;
            default:
                $this->setError(sprintf('The Shape Type "%s" is not supported.', $this->shapeType));
                break;
        }
    }

    /**
     * Returns length of content.
     *
     * @return int
     */
    public function getContentLength()
    {
        // The content length for a record is the length of the record contents section measured in 16-bit words.
        // one coordinate makes 4 16-bit words (64 bit double)
        switch ($this->shapeType) {
            case 0:
                $result = 0;
                break;
            case 1:
                $result = 10;
                break;
            case 21:
                $result = 10 + 4;
                break;
            case 11:
                $result = 10 + 8;
                break;
            case 3:
            case 5:
                $count = count($this->SHPData['parts']);
                $result = 22 + 2 * $count;
                for ($i = 0; $i < $count; ++$i) {
                    $result += 8 * count($this->SHPData['parts'][$i]['points']);
                }
                break;
            case 23:
            case 25:
                $count = count($this->SHPData['parts']);
                $result = 22 + (2 * 4) + 2 * $count;
                for ($i = 0; $i < $count; ++$i) {
                    $result += (8 + 4) * count($this->SHPData['parts'][$i]['points']);
                }
                break;
            case 13:
            case 15:
                $count = count($this->SHPData['parts']);
                $result = 22 + (4 * 4) + 2 * $count;
                for ($i = 0; $i < $count; ++$i) {
                    $result += (8 + 8) * count($this->SHPData['parts'][$i]['points']);
                }
                break;
            case 8:
                $result = 20 + 8 * count($this->SHPData['points']);
                break;
            case 28:
                $result = 20 + (2 * 4) + (8 + 4) * count($this->SHPData['points']);
                break;
            case 18:
                $result = 20 + (4 * 4) + (8 + 8) * count($this->SHPData['points']);
                break;
            default:
                $result = false;
                $this->setError(sprintf('The Shape Type "%s" is not supported.', $this->shapeType));
                break;
        }

        return $result;
    }

    private function _loadDBFData()
    {
        $this->DBFData = @dbase_get_record_with_names($this->DBFFile, $this->recordNumber);
        unset($this->DBFData['deleted']);
    }

    private function _saveDBFData()
    {
        if (count($this->DBFData) == 0) {
            return;
        }
        unset($this->DBFData['deleted']);
        if ($this->recordNumber <= dbase_numrecords($this->DBFFile)) {
            if (!dbase_replace_record($this->DBFFile, array_values($this->DBFData), $this->recordNumber)) {
                $this->setError('I wasn\'t possible to update the information in the DBF file.');
            }
        } else {
            if (!dbase_add_record($this->DBFFile, array_values($this->DBFData))) {
                $this->setError('I wasn\'t possible to add the information to the DBF file.');
            }
        }
    }

    /**
     * Sets error message.
     *
     * @param string $error
     */
    public function setError($error)
    {
        $this->lastError = $error;
    }

    /**
     * Returns shape name.
     *
     * @return string
     */
    public function getShapeName()
    {
        return Util::nameShape($this->shapeType);
    }
}
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                          <?php
/**
 * phpMyAdmin ShapeFile library
 * <https://github.com/phpmyadmin/shapefile/>.
 *
 * Copyright 2006-2007 Ovidio <ovidio AT users.sourceforge.net>
 * Copyright 2016 - 2017 Michal Čihař <michal@cihar.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, you can download one from
 * https://www.gnu.org/copyleft/gpl.html.
 */

namespace PhpMyAdmin\ShapeFile;

class Util
{
    private static $little_endian = null;

    private static $shape_names = array(
        0 => 'Null Shape',
        1 => 'Point',
        3 => 'PolyLine',
        5 => 'Polygon',
        8 => 'MultiPoint',
        11 => 'PointZ',
        13 => 'PolyLineZ',
        15 => 'PolygonZ',
        18 => 'MultiPointZ',
        21 => 'PointM',
        23 => 'PolyLineM',
        25 => 'PolygonM',
        28 => 'MultiPointM',
        31 => 'MultiPatch',
    );

    /**
     * Reads data.
     *
     * @param string $type type for unpack()
     * @param string $data Data to process
     *
     * @return mixed
     */
    public static function loadData($type, $data)
    {
        if ($data === false || strlen($data) == 0) {
            return false;
        }
        $tmp = unpack($type, $data);

        return current($tmp);
    }

    /**
     * Changes endianity.
     *
     * @param string $binValue Binary value
     *
     * @return string
     */
    public static function swap($binValue)
    {
        $result = $binValue[strlen($binValue) - 1];
        for ($i = st