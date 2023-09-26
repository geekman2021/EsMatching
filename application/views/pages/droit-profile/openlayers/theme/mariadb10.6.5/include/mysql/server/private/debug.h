      $editArea.removeClass('edit_area_loading');
            $editArea.append(data.dropdown);
            $editArea.append('<div class="cell_edit_hint">' + g.cellEditHint + '</div>');
          }); // end $.post()

          $editArea.show();
          $editArea.on('change', 'select', function () {
            $(g.cEdit).find('.edit_box').val($(this).val());
          });
        } else if ($td.is('.set')) {
          // handle set fields
          $editArea.addClass('edit_area_loading'); // if the data is truncated, get the full data

          if ($td.is('.truncated')) {
            postParams = {
              'ajax_request': true,
              'server': g.server,
              'db': g.db,
              'table': g.table,
              'column': fieldName,
              'curr_value': currValue,
              'get_full_values': true,
              'where_clause': whereClause
            };
          } else {
            postParams = {
              'ajax_request': true,
              'server': g.server,
              'db': g.db,
              'table': g.table,
              'column': fieldName,
              'curr_value': currValue
            };
          }

          g.lastXHR = $.post('index.php?route=/sql/ge