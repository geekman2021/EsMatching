'); // remember current datetime value in $input_field, if it is not null

          var datetimeValue = !isNull ? $inputField.val() : '';
          var showMillisec = false;
          var showMicrosec = false;
          var timeFormat = 'HH:mm:ss'; // check for decimal places of seconds

          if ($td.attr('data-decimals') > 0 && $td.attr('data-type').indexOf('time') !== -1) {
            if (datetimeValue && datetimeValue.indexOf('.') === false) {
              datetimeValue += '.';
            }

            if ($td.attr('data-decimals') > 3) {
              showMillisec = true;
              showMicrosec = true;
              timeFormat = 'HH:mm:ss.lc';

              if (datetimeValue) {
                datetimeValue += '000000';
                datetimeValue = datetimeValue.substring(0, datetimeValue.indexOf('.') + 7);
                $inputField.val(datetimeValue);
              }
            } else {
              showMillisec = true;
              timeFormat = 'HH:mm:ss.l';

              if (datetimeValue) {
                datetimeValue += '000';
                datetimeValue = datetimeValue.substring(0, datetimeValue.indexOf('.') + 4);
                $inputField.val(datetimeValue);
              }
            }
          } // add datetime picker


          Functions.addDatepicker($inputField, $td.attr('data-type'), {
            showMillisec: showMillisec,
            showMicrosec: showMicrosec,
            timeFormat: timeFormat,
            firstDay: firstDayOfCalendar
          });
          $inputField.on('keyup', function (e) {
            if (e.which === 13) {
              // post on pressing "Enter"
              e.preventDefault();
              e.stopPropagation();
              g.saveOrPostEditedCell();
            } else if (e.which !== 27) {
              Functions.toggleDatepickerIfInvalid($td, $inputField);
            }
          });
          $inputField.datepicker('show');
          Functions.toggleDatepickerIfInvalid($td, $inputField); // unbind the mousedown event to preven