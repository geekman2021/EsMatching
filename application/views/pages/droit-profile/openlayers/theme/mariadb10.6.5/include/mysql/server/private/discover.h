me] = $(g.cEdit).find('.edit_box').val();
        } else if ($thisField.hasClass('hex')) {
          if ($(g.cEdit).find('.edit_box').val().match(/^(0x)?[a-f0-9]*$/i) !== null) {
            thisFieldParams[fieldName] = $(g.cEdit).find('.edit_box').val();
          } else {
            var hexError = '<div class="alert alert-danger" role="alert">' + Messages.strEnterValidHex + '</div>';
            Functions.ajaxShowMessage(hexError, false);
            thisFieldParams[fieldName] = Functions.getCellValue(g.currentEditCell);
          }
        } else {
          thisFieldParams[fieldName] = $(g.cEdit).find('.edit_box').val();
        }

        if (g.wasEditedCellNull || thisFieldParams[fieldName] !== Functions.getCellValue(g.currentEditCell)) {
          needToPost = true;
        }
      }

      if (needToPost) {
        $(g.currentEditCell).addClass('to_be_saved').data('value', thisFieldParams[fieldName]);

        if (g.saveCellsAtOnce) {
          $(g.o).find('div.save_edited').show();
        }

        g.isCellEdited = true;
      }

      return needToPost;
    },

    /**
     * Save or post currently edited cell, depending on the "saveCellsAtOnce" configuration.
     *
     * @param field Optional, this object contains a boolean named move (true, if called from move* functions)
     *              and a <td> to which the grid_edit should move
     */
    saveOrPostEditedCell: function saveOrPostEditedCell(options) {
      var saved = g.saveEditedCell(); // Check if $cfg['SaveCellsAtOnce'] is false

      if (!g.s