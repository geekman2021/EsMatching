e column n to new width "nw"
     *
     * @param n zero-based column index
     * @param nw new width of the column in pixel
     */
    resize: function resize(n, nw) {
      $(g.t).find('tr').each(function () {
        $(this).find('th.draggable:visible').eq(n).find('span').add($(this).find('td:visible').eq(g.actionSpan + n).find('span')).css('width', nw);
      });
    },

    /**
     * Reposition column resize bars.
     */
    reposRsz: function reposRsz() {
      $(g.cRsz).find('div').hide();
      var $firstRowCols = $(g.t).find('tr').first().find('th.draggable:visible');
      var $resizeHandles = $(g.cRsz).find('div').removeClass('condition');
      $(g.t).find('table.pma_table').find('thead th').first().removeClass('before-condition');

      for (var n = 0, l = $firstRowCols.length; n < l; n++) {
        var $col = $($firstRowCols[n]);
        var colWidth;

        if (navigator.userAgent.toLowerCase().indexOf('safari') !== -1) {
          colWidth = $col.outerWidth();
        } else {
          colWidth = $col.outerWidth(true);
        }

        $($r