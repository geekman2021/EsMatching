h.draggable').eq(n).add($(this).find('td').eq(g.actionSpan + n)).hide();
          });
          g.colVisib[n] = 0;
          $(g.cList).find('.lDiv div').eq(n).find('input').prop('checked', false);
        } else {
          // cannot hide, force the checkbox to stay checked
          $(g.cList).find('.lDiv div').eq(n).find('input').prop('checked', true);
          return false;
        }
      } else {
        // column n is not visible
        $(g.t).find('tr').each(function () {
          $(this).find('th.draggable').eq(n).add($(this).find('td').eq(g.actionSpan + n)).show();
        });
        g.colVisib[n] = 1;
        $(g.cList).find('.lDiv div').eq(n).find('input').prop('checked', true);
      }

      return true;
    },

    /**
     * This must be called if toggleCol() returns is true.
     *
     * This function is separated from toggleCol because, sometimes, we want to toggle
     * some columns together at one time and do just one adjustment after it, e.g. in showAllColumns().
     */
    afterToggleCol: function afterToggleCol() {
      // some adjustments after hiding column
      g.reposRsz();
      g.reposDrop();
      g.sendColPrefs(); // check visible first row headers count

      g.visibleHeadersCount = $(g.t).find('tr').first().find('th.draggable:visible').length;
      g.refreshRestoreButton();
    },

    /**
     * Show columns' visibility list.
     *
     * @param obj The drop down arrow of column visibility list
     */
    showColList: function showColList(obj) {
      // only show when not resizing or reordering
      if (!g.colRsz && !g.colReorder) {
        var pos = $(obj).position();
        $(g.cList).css({
          top: pos.top + $(obj).outerHeight(true)
        }).show();
        $(obj).addClass('coldrop-hover');
      }
    },

    /**
     * Hide columns' visibility list.
     */
    hideColList: function hideColList() {
      $(g.cList).hide();
      $(g.cDrop).find('.coldrop-hover').removeClass('coldrop-hover');
    },

    /**
     * Reposition the column visibility drop-down arrow.
     */
    reposDrop: function reposDrop() {
      var $th = $(t).find('th:not(.draggable)');

      for (var i = 0; i < $th.length; i++) {
        var $cd = $(g.cDrop).find('div').eq(i); // column drop-down arrow

        var pos = $($th[i]).position();
        $cd.css({
          left: pos.left + $($th[i]).width() - $cd.width(),
          top: pos.top
        });
      }
    },

    /**
     * Show all hidden columns.
     */
    showAllColumns: function showAllColumns() {
      for (var i = 0; i < g.colVisib.length; i++) {
        if (!g.colVisib[i]) {
          g.toggleCol(i);
        }
      }

      g.afterToggleCol();
    },

    /**
     * Show edit cell, if it can be shown
     *
     * @param cell <td> element to be edited
     */
    showEditCell: function showEditCell(cell) {
      if ($(cell).is('.grid_edit') && !g.colRsz && !g.colReorder) {
        if (!g.isCellEditActive) {
          var $cell = $(cell);

          if ('string' === $cell.attr('data-type') || 'blob' === $cell.attr('data-type') || 'json' === $cell.attr('data-type')) {
            g.cEdit = g.cEditTextarea;
          } else {
            g.cEdit = g.cEditStd;
          } // remove all edit area and hide it


          $(g.cEdit).find('.edit_area').empty().hide(); // reposition the cEdit element

          $(g.cEdit).css({
            top: $cell.position().top,
            left: $cell.position().left
          }).show().find('.edit_box').css({
            width: $cell.outerWidth(),
            height: $cell.outerHeight()
          }); // fill the cell edit with text from <td>

          var value = Functions.getCellValue(cell);

          if ($cell.attr('data-type') === 'json' && $cell.is('.truncated') === false) {
            try {
              value = JSON.stringify(JSON.parse(value), null, 4);
            } catch (e) {// Show as is
            }
          }

          $(g.cEdit).find('.edit_box').val(value);
          g.currentEditCell = cell;
          $(g.cEdit).find('.edit_box').trigger('focus');
          moveCursorToEnd($(g.cEdit).find('.edit_box'));
          $(g.cEdit).find('*').prop('disabled', false);
        }
      }

      function moveCursorToEnd(input) {
        var originalValue = input.val();
        var originallength = originalValue.length;
        input.val('');
        input.trigger('blur').trigger('focus').val(originalValue);
        input[0].setSelectionRange(originallength, originallength);
      }
    },

    /**
     * Remove edit cell and the edit area, if it is shown.
     *
     * @param force Optional, force to hide edit cell without saving edited field.
     * @param data  Optional, data from the POST AJAX request to save the edited field
     *              or just specify "true", if we want to replace the edited field with the new value.
     * @param field Optional, the edited <td>. If not specified, the function will
     *              use currently edited <td> from g.currentEditCell.
     * @param field Optional, this object contains a boolean named move (true, if called from move* functions)
     *              and a <td> to which the grid_edit should move
     */
    hideEditCell: function hideEditCell(force, data, field, options) {
      if (g.isCellEditActive && !force) {
        // cell is being edited, save or post the edited data
        if (options !== undefined) {
          g.saveOrPostEditedCell(options);
        } else {
          g.saveOrPostEditedCell();
        }

        return;
      } // cancel any previous request


      if (g.lastXHR !== null) {
        g.lastXHR.abort();
        g.lastXHR = null;
      }

      if (data) {
        if (g.currentEditCell) {
          // save value of currently edited cell
          // replace current edited field with the new value
          var $thisField = $(g.currentEditCell);
          var isNull = $thisField.data('value') === null;

          if (isNull) {
            $thisField.find('span').html('NULL');
            $thisField.addClass('null');
          } else {
            $thisField.removeClass('null');
            var value = data.isNeedToRecheck ? data.truncatableFieldValue : $thisField.data('value'); // Truncates the text.

            $thisField.removeClass('truncated');

            if (CommonParams.get('pftext') === 'P' && value.length > g.maxTruncatedLen) {
              $thisField.addClass('truncated');
              value = value.substring(0, g.maxTruncatedLen) + '...';
            } // Add <br> before carriage return.


            var newHtml = Functions.escapeHtml(value);
            newHtml = newHtml.replace(/\n/g, '<br>\n');
            var decimals = parseInt($thisField.attr('data-decimals')); // remove decimal places if column type not supported

            if (decimals === 0 && $thisField.attr('data-type').indexOf('time') !== -1) {
              newHtml = newHtml.substring(0, newHtml.indexOf('.'));
            } // remove additional decimal places


            if (decimals > 0 && $thisField.attr('data-type').indexOf('time') !== -1) {
              newHtml = newHtml.substring(0, newHtml.length - (6 - decimals));
            }

            var selector = 'span';

            if ($thisField.hasClass('hex') && $thisField.find('a').length) {
              selector = 'a';
            } // Updates the code keeping highlighting (if any).


            var $target = $thisField.find(selector);

            if (!Functions.updateCode($target, newHtml, value)) {
              $target.html(newHtml);
            }
          }

          if ($thisField.is('.bit')) {
            $thisField.find('span').text($thisField.data('value'));
          }
        }

        if (data.transformations !== undefined) {
          $.each(data.transformations, function (cellIndex, value) {
            var $thisField = $(g.t).find('.to_be_saved').eq(cellIndex);
            $thisField.find('span').html(value);
          });
        }

        if (data.relations !== undefined) {
          $.each(data.relations, function (cellIndex, value) {
            var $thisField = $(g.t).find('.to_be_saved').eq(cellIndex);
            $thisField.find('span').html(value);
          });
        } // refresh the grid


        g.reposRsz();
        g.reposDrop();
      } // hide the cell editing area


      $(g.cEdit).hide();
      $(g.cEdit).find('.edit_box').trigger('blur');
      g.isCellEditActive = false;
      g.currentEditCell = null; // destroy datepicker in edit area, if exist

      var $dp = $(g.cEdit).find('.hasDatepicker');

      if ($dp.length > 0) {
        // eslint-disable-next-line no-underscore-dangle
        $(document).on('mousedown', $.datepicker._checkExternalClick);
        $dp.datepicker('destroy'); // change the cursor in edit box back to normal
        // (the cursor become a hand pointer when we add datepicker)

        $(g.cEdit).find('.edit_box').css('cursor', 'inherit');
      }
    },

    /**
     * Show drop-down edit area when edit cell is focused.
     */
    showEditArea: function showEditArea() {
      if (!g.isCellEditActive) {
        // make sure the edit area has not been shown
        g.isCellEditActive = true;
        g.isEditCellTextEditable = false;
        /**
         * @var $td current edited cell
         */

        var $td = $(g.currentEditCell);
        /**
         * @var $editArea the editing area
         */

        var $editArea = $(g.cEdit).find('.edit_area');
        /**
         * @var where_clause WHERE clause for the edited cell
         */

        var whereClause = $td.parent('tr').find('.where_clause').val();
        /**
         * @var field_name  String containing the name of this field.
         * @see Sql.getFieldName()
         */

        var fieldName = Sql.getFieldName($(t), $td);
        /**
         * @var relation_curr_value String current value of the field (for fields that are foreign keyed).
         */

        var relationCurrValue = $td.text();
        /**
         * @var relation_key_or_display_column String relational key if in 'Relational display column' mode,
         * relational display column if in 'Relational key' mode (for fields that are foreign keyed).
         */

        var relationKeyOrDisplayColumn = $td.find('a').attr('title');
        /**
         * @var curr_value String current value of the field (for fields that are of type enum or set).
         */

        var currValue = $td.find('span').text(); // empty all edit area, then rebuild it based on $td classes

        $editArea.empty(); // remember this instead of testing more than once

        var isNull = $td.is('.null'); // add goto link, if this cell contains a link

        if ($td.find('a').length > 0) {
          var gotoLink = document.createElement('div');
          gotoLink.className = 'goto_link';
          $(gotoLink).append(g.gotoLinkText + ' ').append($td.find('a').clone());
          $editArea.append(gotoLink);
        }

        g.wasEditedCellNull = false;

        if ($td.is(':not(.not_null)')) {
          // append a null checkbox
          $editArea.append('<div class="null_div"><label>NULL:<input type="checkbox"></label></div>');
          var $checkbox = $editArea.find('.null_div input'); // check if current <td> is NULL

          if (isNull) {
            $checkbox.prop('checked', true);
            g.wasEditedCellNull = true;
          } // if the select/editor is changed un-check the 'checkbox_null_<field_name>_<row_index>'.


          if ($td.is('.enum, .set')) {
            $editArea.on('change', 'select', function () {
              $checkbox.prop('checked', false);
            });
          } else if ($td.is('.relation')) {
            $editArea.on('change', 'select', function () {
              $checkbox.prop('checked', false);
            });
            $editArea.on('click', '.browse_foreign', function () {
              $checkbox.prop('checked', false);
            });
          } else {
            $(g.cEdit).on('keypress change paste', '.edit_box', function () {
              $checkbox.prop('checked', false);
            }); // Capture ctrl+v (on IE and Chrome)

            $(g.cEdit).on('keydown', '.edit_box', function (e) {
              if (e.ctrlKey && e.which === 86) {
                $checkbox.prop('checked', false);
              }
            });
            $editArea.on('keydown', 'textarea', function () {
              $checkbox.prop('checked', false);
            });
          } // if some text is written in textbox automatically unmark the null checkbox and if it is emptied again mark the checkbox.


          $(g.cEdit).find('.edit_box').on('input', function () {
       