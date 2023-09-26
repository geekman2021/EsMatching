             g.hideColList();
            }
          });
          $(g.cDrop).append(cd);
        }); // add column visibility control

        g.cList.innerHTML = '<div class="lDiv"></div>';
        var $listDiv = $(g.cList).find('div');

        var tempClick = function tempClick() {
          if (g.toggleCol($(this).index())) {
            g.afterToggleCol();
          }
        };

        for (i = 0; i < $firstRowCols.length; i++) {
          var currHeader = $firstRowCols[i];
          var listElmt = document.createElement('div');
          $(listElmt).text($(currHeader).text()).prepend('<input type="checkbox" ' + (g.colVisib[i] ? 'checked="checked" ' : '') + '>');
          $listDiv.append(listElmt); // add event on click

          $(listElmt).on('click', tempClick);
        } // add "show all column" button


        var showAll = document.createElement('div');
        $(showAll).addClass('showAllColBtn').text(g.showAllColText);
        $(g.cList).append(showAll);
        $(showAll).on('click', function () {
          g.showAllColumns();
        }); // prepend "show all column" button at top if the list is too long

        if ($firstRowCols.length > 10) {
          var clone = showAll.cloneNode(true);
          $(g.cList).prepend(clone);
          $(clone).on('click', function () {
            g.showAllColumns();
          });
        }
      } // hide column visibility list if we move outside the list


      $(g.t).find('td, th.draggable').on('mouseenter', function () {
        g.hideColList();
      }); // attach to first row first col of the grid

      var thFirst = $(g.t).find('th.print_ignore');
      $(thFirst).append(g.cDrop);
      $(thFirst).append(g.cList); // some adjustment

      g.reposDrop();
    },

    /**
     * Move currently Editing Cell to Up
     */
    moveUp: function moveUp(e) {
      e.preventDefault();
      var $thisField = $(g.currentEditCell);
      var fieldName = Sql.getFieldName($(g.t), $thisField);
      var whereClause = $thisField.parents('tr').first().find('.where_clause').val();

      if (typeof whereClause === 'undefined') {
        whereClause = '';
      }

      var found = false;
      var $prevRow;
      $thisField.parents('tr').first().parents('tbody').children().each(function () {
        if ($(this).find('.where_clause').val() === whereClause) {
          found = true;
        }

        if (!found) {
          $prevRow = $(this);
        }
      });
      var newCell;

      if (found && $prevRow) {
        $prevRow.children('td').each(function () {
          if (Sql.getFieldName($(g.t), $(this)) === fieldName) {
            newCell = this;
          }
        });
      }

      if (newCell) {
        g.hideEditCell(false, false, false, {
          move: true,
          cell: newCell
        });
      }
    },

    /**
     * Move currently Editing Cell to Down
     */
    moveDown: function moveDown(e) {
      e.preventDefault();
      var $thisField = $(g.currentEditCell);
      var fieldName = Sql.getFieldName($(g.t), $thisField);
      var whereClause = $thisField.parents('tr').first().find('.where_clause').val();

      if (typeof whereClause === 'undefined') {
        whereClause = '';
      }

      var found = false;
      var $nextRow;
      var j = 0;
      var nextRowFound = false;
      $thisField.parents('tr').first().parents('tbody').children().each(function () {
        if ($(this).find('.where_clause').val() === whereClause) {
          found = true;
        }

        if (found) {
          if (j >= 1 && !nextRowFound) {
            $nextRow = $(this);
            nextRowFound = true;
          } else {
            j++;
          }
        }
      });
      var newCell;

      if (found && $nextRow) {
        $nextRow.children('td').each(function () {
          if (Sql.getFieldName($(g.t), $(this)) === fieldName) {
            newCell = this;
          }
        });
      }

      if (newCell) {
        g.hideEditCell(false, false, false, {
          move: true,
          cell: newCell
        });
      }
    },

    /**
     * Move currently Editing Cell to Left
     */
    moveLeft: function moveLeft(e) {
      e.preventDefault();
      var $thisField = $(g.currentEditCell);
      var fieldName = Sql.getFieldName($(g.t), $thisField);
      var whereClause = $thisField.parents('tr').first().find('.where_clause').val();

      if (typeof whereClause === 'undefined') {
        whereClause = '';
      }

      var found = false;
      var $foundRow;
      $thisField.parents('tr').first().parents('tbody').children().each(function () {
        if ($(this).find('.where_clause').val() === whereClause) {
          found = true;
          $foundRo