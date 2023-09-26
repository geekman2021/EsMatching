<?php
/* vim: set expandtab sw=4 ts=4 sts=4: */
/**
 * The PBXT storage engine
 *
 * @package PhpMyAdmin-Engines
 */
namespace PhpMyAdmin\Engines;

use PhpMyAdmin\Core;
use PhpMyAdmin\StorageEngine;
use PhpMyAdmin\Util;

/**
 * The PBXT storage engine
 *
 * @package PhpMyAdmin-Engines
 */
class Pbxt extends StorageEngine
{
    /**
     * Returns array with variable names dedicated to PBXT storage engine
     *
     * @return array   variable names
     */
    public function getVariables()
    {
        return array(
            'pbxt_index_cache_size'        => array(
                'title' => __('Index cache size'),
                'desc'  => __(
                    'This is the amount of memory allocated to the'
                    . ' index cache. Default value is 32MB. The memory'
                    . ' allocated here is used only for caching index pages.'
                ),
                'type'  => PMA_ENGINE_DETAILS_TYPE_SIZE,
            ),
            'pbxt_record_cache_size'       => array(
                'title' => __('Record cache size'),
                'desc'  => __(
                    'This is the amount of memory allocated to the'
                    . ' record cache used to cache table data. The default'
                    . ' value is 32MB. This memory is used to cache changes to'
                    . ' the handle data (.xtd) and row pointer (.xtr) files.'
                ),
                'type'  => PMA_ENGINE_DETAILS_TYPE_SIZE,
            ),
            'pbxt_log_cache_size'          => array(
              