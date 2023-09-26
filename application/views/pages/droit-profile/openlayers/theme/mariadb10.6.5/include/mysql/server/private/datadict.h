 i++) {
        var x = g.colOrder[i];
        var j = i - 1;

        while (j >= 0 && x < g.colOrder[j]) {
          j--;
        }

        if (j !== i - 1) {
          g.shiftCol(i, j + 1);
        }
      }

      if (g.tableCreateTime) {
        // send request to server to remember the column order
        g.sendColPrefs();
      }

      g.refreshRestoreButton();
    },

    /**
     * Send column preferences (column order and visibility) to the server.
     */
    sendColPrefs: function sendColPrefs() {
      if ($(g.t).is('.ajax')) {
        // only send preferences if ajax class
        if (typeof g.db !== 'string' && typeof g.table !== 'string') {
          // The server has nothing to do with it
          // Issue: https://github.com/phpmyadmin/phpmyadmin/issues/15658
          return;
        }

        var postParams = {
          'ajax_request': true,
          'db': g.db,
          'table': g.table,
          'token': g.token,
          'server': g.server,
          'table_create_time': g.tableCreateTime
        };

        if (g.colOrder.length > 0) {
          $.extend(postParams, {
            'col_order': g.colOrder.toString()
          });
        }

        if (g.colVisib.length > 0) {
          $.extend(postParams, {
            'col_visib': g.colVisib.toString()
          });
        }

        $.post('index.php?route=/sql/set-column-preferences', postParams, function (data) {
          if (data.success !== true) {
            var $tempDiv = $(document.createElement('div'));
            $tempDiv.html(data.error);
            $tempDiv.addClass('alert alert-danger');
            Functions.ajaxShowMessage($tempDiv, false);
          }
        });
   