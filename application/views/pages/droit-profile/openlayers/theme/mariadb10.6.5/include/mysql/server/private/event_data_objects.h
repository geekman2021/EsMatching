d('.save_cells_at_once').val();
      g.maxTruncatedLen = CommonParams.get('LimitChars'); // register events

      $(g.t).find('td.data.click1').on('click', function (e) {
        startGridEditing(e, this); // prevent default action when clicking on "link" in a table

        if ($(e.target).is('.grid_edit a')) {
          e.preventDefault();
        }
      });
      $(g.t).find('td.data.click2').on('click', function (e) {
        var $cell = $(this); // In the case of relational link, We want single click on the link
        // to goto the link and double click to start grid-editing.

        var $link = $(e.target);

        if ($link.is('.grid_edit.relation a')) {
          e.preventDefault(); // get the click count and increase

          var clicks = $cell.data('clicks');
          clicks = typeof clicks === 'undefined' ? 1 : clicks + 1;

          if (clicks === 1) {
            // if there are no previous clicks,
            // start the single click timer
            var timer = setTimeout(function () {
              // temporarily remove ajax class so the page loader will not handle it,
              // submit and then add it back
              $link.removeClass('ajax');
              AJAX.requestHandler.call($link[0]);
              $link.addClass('ajax');
              $cell.data('clicks', 0);
            }, 700);
            $cell.data('clicks', clicks);
            $cell.data('timer', timer);
          } else {
            // When double clicking a link, switch to edit mode
            // this is a double click, cancel the single click timer
            // and make the click count 0
            clearTimeout($cell.data('timer'));
            $cell.data('clicks', 0); // start grid-editing

            startGridEditing(e, this);
          }
        }
      }).on('dblclick', function (e) {
        if ($(e.target).is('.grid_edit a')) {
          e.preventDefault();
        } else {
          startGridEditing(e, this);
        }
      });
      $(g.cEditStd).on('keydown', 'input.edit_box, select', handleCtrlNavigation);
      $(g.cEditStd).find('.edit_box').on('focus', function () {
        g.showEditArea();
      });
      $(g.cEditStd).on('keydown', '.edit_box, select', function (e) {
        if (e.which === 13) {
          // post on pressing "Enter"
          e.preventDefault();
          g.saveOrPostEditedCell();
        }
      });
      $(g.cEditStd).on('keydown', function (e) {
        if (!g.isEditCellTextEditable) {
          // prevent text editing
          e.preventDefault();
        }
      });
      $(g.cEditTextarea).on('keydown', 'textarea.edit_box, select', handleCtrlNavigation);
      $(g.cEditTextarea).find('.edit_box').on('focus', function () {
        g.showEditArea();
      });
      $(g.cEditTextarea).on('keydown', '.edit_box, select', function (e) {
        if (e.which === 13 && !e.shiftKey) {
          // post on pressing "Enter"
          e.preventDefault();
          g.saveOrPostEditedCell();
        }
      });
      $(g.cEditTextarea).on('keydown', function (e) {
        if (!g.isEditCellTextEditable) {
          // prevent text editing
          e.preventDefault();
        }
      });
      $('html').on('click', function (e) {
        // hide edit cell if the click is not fromDat edit area
        if ($(e.target).parents().index($(g.cEdit)) === -1 && !$(e.target).parents('.ui-datepicker-header').length && !$('.browse_foreign_modal.ui-dialog:visible').length && !$(e.target).closest('.dismissable').length) {
          g.hideEditCell();
        }
      }).on('keydown', function (e) {
        if (e.which === 27 && g.isCellEditActive) {
          // cancel on pressing "Esc"
          g.hideEditCell(true);
        }
      });
      $(g.o).find('div.save_edited').on('click', function () {
        g.hideEditCell();
        g.postEditedCell();
      });
      $(window).on('beforeunload', function () {
        if (g.isCellEdited) {
          return g.saveCellWarning;
        }
      }); // attach to global div

      $(g.gDiv).append(g.cEditStd);
      $(g.gDiv).append(g.cEditTextarea); // add hint for grid editing feature when hovering "Edit" link in ea