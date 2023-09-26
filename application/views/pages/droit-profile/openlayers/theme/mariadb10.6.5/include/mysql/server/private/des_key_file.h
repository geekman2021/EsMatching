      var checkboxValue = $checkbox.val();
                $checkbox.attr('name', checkboxName.replace(oldClause, newClause));
                $checkbox.val(checkboxValue.replace(decodedOldClause, decodedNewClause));
              });
            }); // update the display of executed SQL query command

            if (typeof data.sql_query !== 'undefined') {
              // extract query box
              var $resultQuery = $($.parseHTML(data.sql_query));
              var sqlOuter = $resultQuery.find('.sqlOuter').wrap('<p>').parent().html();
              var tools = $resultQuery.find('.tools').wrap('<p>').parent().html(); // sqlOuter and tools will not be present if 'Show SQL queries' configuration is off

              if (typeof sqlOuter !== 'undefined' && typeof tools !== 'undefined') {
                $(g.o).find('.result_query').not($(g.o).find('.result_query').last()).remove();
                var $existingQuery = $(g.o).find('.result_query'); // If two query box exists update query in second else add a second box

                if ($existingQuery.find('div.sqlOuter').length > 1) {
                  $existingQuery.children().eq(3).remove();
                  $existingQuery.children().eq(3).remove();
              