he profiling info
     *
     * @return string $profiling_table html for the profiling table and chart
     */
    private function getHtmlForProfilingChart($url_query, $db, $profiling_results)
    {
        if (! empty($profiling_results)) {
            $url_query = isset($url_query)
                ? $url_query
                : Url::getCommon(array('db' => $db));

            $profiling_table = '';
            $profiling_table .= '<fieldset><legend>' . __('Profiling')
                . '</legend>' . "\n";
            $profiling_table .= '<div class="floatleft">';
            $profiling_table .= '<h3>' . __('Detailed profile') . '</h3>';
            $profiling_table .= '<table id="profiletable"><thead>' . "\n";
            $profiling_table .= ' <tr>' . "\n";
            $profiling_table .= '  <th>' . __('Order')
                . '<div class="sorticon"></div></th>' . "\n";
            $pro