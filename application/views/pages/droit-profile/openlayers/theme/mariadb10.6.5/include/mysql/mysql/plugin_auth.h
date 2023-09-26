       var cols = $('th.column_heading').map(function () {
            return $(this).text().trim();
        }).get();
        $.uiTableFilter($('#table_columns'), $(this).val(), cols, null, 'td span');
    });
    $('.edit').on('click', function () {
        var rownum = $(this).parent().data('rownum');
        $('#save_' + rownum).show();
        $(this).hide();
        $('#f_' + rownum + ' td span').hide();
        $('#f_' + rownum + ' input, #f_' + rownum + ' select, #f_' + rownum + ' .open_enum_editor').show();
        var attributeVal = $('#f_' + rownum + ' td[name=col_attribute] span').html();
        $('#f_' + rownum + ' select[name=field_attribute\\[' + rownum + '\\] ] option[value="' + attributeVal + '"]').attr('selected','selected');
        if ($('#f_' + rownum + ' .default_type').val() === 'USER_DEFINED') {
            $('#f_' + rownum + ' .default_type').siblings('.default_value').show();
        } else {
            $('#f_' + rownum + ' .default_type').siblings('.default_value').hide();
        }
    });
    $('.del_row').on('click', function (event) {
        event.preventDefault();
        event.stopPropagation();
        var $td = $(this);
        var question = Messages.strDeleteCentralColumnWarning;
        $td.confirm(question, null, function () {
            var rownum = $td.data('rownum');
            $('#del_col_name').val('selected_fld%5B%5D=' + $('#checkbox_row_' + rownum).val());
            $('#del_form').trigger('submit');
        });
    });
    $('.edit_cancel_form').on('click', function (event) {
        event.preventDefault();
        event.stopPropagation();
        var rownum = $(this).data('rownum');
        $('#save_' + rownum).hide();
        $('#edit_' + rownum).show();
        $('#f_' + rownum + ' td span').show();
        $('#f_' + rownum + ' input, #f_' + rownum + ' select,#f_' + rownum + ' .default_value, #f_' + rownum + ' .open_enum_editor').hide();
        $('#tableslistcontainer').find('.checkall').show();
    });
    $('.edit_save_form').on('click', function (event) {
        event.preventDefault();
        event.stopPropagation();
        var rownum = $(this).data('rownum');
        $('#f_' + rownum + ' td').each(function () {
            if ($(this).attr('name') !== 'undefined') {
                $(this).find(':input[type!="hidden"],select').first()
                    .attr('name', $(this).attr('name'));
            }
        });

        if ($('#f_' + rownum + ' .default_type').val() === 'USER_DEFINED') {
            $('#f_' + rownum + ' .default_type').attr('name','col_default_sel');
        } else {
            $('#f_' + rownum + ' .default_value').attr('name','col_default_val');
        }

        var datastring = $('#f_' + rownum + ' :input').serialize();
        $.ajax({
            type: 'POST',
            url: 'index.php?route=/database/central-columns',
            data: datastring + CommonParams.get('arg_separator') + 'ajax_request=true',
            dataType: 'json',
            success: function (data) {
                if (data.message !== '1') {
                    Functions.ajaxShowMessage(
                        '<div class="alert alert-danger" role="alert">' +
                        data.message +
                        '</div>',
                        false
                    );
                } else {
                    $('#f_' + rownum + ' td input[id=checkbox_row_' + rownum + ']').val($('#f_' + rownum + ' input[name=col_name]').val()).html();
                    $('#f_' + rownum + ' td[name=col_name] span').text($('#f_' + rownum + ' input[name=col_name]').val()).html();
                    $('#f_' + rownum + ' td[name=col_type] span').text($('#f_' + rownum + ' select[name=col_type]').val()).html();
                    $('#f_' + rownum + ' td[name