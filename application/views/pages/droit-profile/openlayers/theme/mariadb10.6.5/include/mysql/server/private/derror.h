 '\'\'') + '\'';
                break;
              }
            }
          }
        }); // end of loop for every edited cells in a row
        // save new_clause

        var newClause = '';

        for (var field in conditionArray) {
          newClause += field + ' ' + conditionArray[field] + ' AND ';
        }

        newClause = newClause.substring(0, newClause.length - 5); // remove the last AND

        $tr.data('new_clause', newClause); // save condition_array

        $tr.find('.condition_array').val(JSON.stringify(conditionArray));
        multiEditFieldsName.push(fieldsName);
        multiEditFieldsType.push(fieldsType);
        multiEditFields.push(fields);
        multiEditFieldsNull.push(fieldsNull);
      }); // end of loop for every edited rows

      relFieldsList = $.param(relationFields);
      transformFieldsList = $.param(transformFields); // Make the Ajax post after setting all parameters

      /**
       * @var post_params Object conta