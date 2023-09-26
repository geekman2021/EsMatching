er

      g.cPointer.className = 'cPointer';
      $(g.cPointer).css('visibility', 'hidden'); // set visibility to hidden instead of calling hide() to force browsers to cache the image in cPointer class
      // assign column reordering hint

      g.reorderHint = Messages.strColOrderHint; // get data columns in the first row of the table

      var $firstRowCols = $(g.t).find('tr').first().find('th.draggable'); // initialize column order

      var $colOrder = $(g.o).find('.col_order'); // check if column order is passed from PHP

      var i;

      if ($colOrder.length > 0) {
        g.colOrder = $colOrder.val().split(',');

        for (i = 0; i < g.colOrder.length; i++) {
          g.colOrder[i] = parseInt(g.colOrder[i], 10);
        }
      } else {
        g.colOrder = [];

        for (i = 0; i < $firstRowCols.length; i++) {
          g.colOrder.push(i);
        }
      } // register events


      $(g.t).find('th.draggable').on('mousedown', function (e) {
        $(g.o).addClass('turnOffSelect');

        if (g.visibleHeadersCount > 1) {
          g.dragSta