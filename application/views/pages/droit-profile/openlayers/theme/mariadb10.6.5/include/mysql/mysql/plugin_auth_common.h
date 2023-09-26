+ rownum + ' select[name=col_attribute]').val()).html();
                    $('#f_' + rownum + ' td[name=col_isNull] span').text($('#f_' + rownum + ' input[name=col_isNull]').is(':checked') ? 'Yes' : 'No').html();
                    $('#f_' + rownum + ' td[name=col_extra] span').text($('#f_' + rownum + ' input[name=col_extra]').is(':checked') ? 'auto_increment' : '').html();
                    $('#f_' + rownum + ' td[name=col_default] span').text($('#f_' + rownum + ' :input[name=col_default]').val()).html();
                }
                $('#save_' + rownum).hide();
                $('#edit_' + rownum).show();
                $('#f_' + rownum + ' td span').show();
                $('#f_' + rownum + ' input, #f_' + rownum + ' select,#f_' + rownum + ' .default_value, #f_' + rownum + ' .open_enum_editor').hide();
                $('#tableslistcontainer').find('.checkall').show();
            },
            error: function () {
                Functions.ajaxShowMessage(
                    '<div class="alert alert-danger" role="alert">' +
                        Messages.strErrorProcessingRequest +
                        '</div>',
                    false
                );
            }
        });
    });
    $('#table-select').on('change', function () {
        var selectValue = $(this).val();
        var defaultColumnSelect = $('#column-select').find('option').first();
        var href = 'index.php?route=/database/central-columns/populate';
        var params = {
            'ajax_request' : true,
            'server' : CommonParams.get('server'),
            'db' : CommonParams.get('db'),
            'selectedTable' : selectValue
        };
        $('#column-select').html('<option value="">' + Messages.strLoading + '</option>');
        if (selectValue !== '') {
            $.post(href, params, function (data) {
                $('#column-select').empty().append(defaultColumnSelect);
                $('#column-select').append(data.message);
            });
        }
    });
    $('#add_column').on('submit', function (e) {
        var selectvalue = $('#column-select').val();
        if (selectvalue === '') {
            e.preventDefault();
            e.stopPropagation();
        }
    });
    $('#add_col_div').find('>a').on('click', function () {
        $('#add_new').slideToggle('slow');
        var $addColDivLinkSpan = $('#add_col_div').find('>a span');
        if ($addColDivLinkSpan.html() === '+') {
            $addColDivLinkSpan.html('-');
        } else {
            $addColDivLinkSpan.html('+');
        }
    });
    $('#add_new').on('submit', function () {
        $('#add_new').toggle();
    });
    $('#tableslistcontainer').find('select.default_type').on('change', function () {
        if ($(this).val() === 'USER_DEFINED') {
            $(this).siblings('.default_value').attr('name','col_default');
            $(this).attr('name','col_default_sel');
        } else {
            $(this).attr('name','col_default');
            $(this).siblings('.default_value').attr('name','col_default_val');
        }
    });
});
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                 