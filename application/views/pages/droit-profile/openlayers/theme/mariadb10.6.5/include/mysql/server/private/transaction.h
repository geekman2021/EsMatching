pe'  => PMA_ENGINE_DETAILS_TYPE_NUMERIC,
            ),
            'pbxt_log_buffer_size'         => array(
                'title' => __('Log buffer size'),
                'desc'  => __(
                    'The size of the buffer used when writing a data'
                    . ' log. The default is 256MB. The engine allocates one'
                    . ' buffer per thread, but only if the thread is required'
                    . ' to write a data log.'
                ),
                'type'  => PMA_ENGINE_DETAILS_TYPE_SIZE,
            ),
            'pbxt_data_file_grow_size'     => array(
                'title' => __('Data file grow size'),
                'desc'  => __('The grow size of the handle data (.xtd) files.'),
                'type'  => PMA_ENGINE_DETAILS_TYPE_SIZE,
            ),
            'pbxt_row_file_grow_size'      => array(
                'title' => __('Row file grow size'),
                'desc'  => __('The grow size of the row pointer (.xtr) files.'),
                'type'  => PMA_ENGINE_DETAILS_TYPE_SIZE,
            ),
            'pbxt_log_file_count'          => array(
                'title' => __('Log file count'),
                'desc'  => __(
                    'This is the number of transaction log files'
                    . ' (pbxt/system/xlog*.xt) the system will maintain. If the'
                    . ' number of logs exceeds this value then old logs will be'
                    . ' deleted, 