/**
 * @fileoverview   events handling from central columns page
 * @name            Central columns
 *
 * @requires    jQuery
 */

/**
 * AJAX scripts for /database/central-columns
 *
 * Actions ajaxified here:
 * Inline Edit and save of a result row
 * Delete a row
 * Multiple edit and delete option
 *
 */

AJAX.registerTeardown('database/central_columns.js', function () {
    $('.edit').off('click');
    $('.edit_save_form').off('click');
    $('.edit_cancel_form').off('click');
    $('.del_row').off('click');
    $(document).off('keyup', '.filter_rows');
    $('.edit_cancel_form').off('click');
    $('#table-select').off('change');
    $('#column-select').off('change');
    $('#add_col_div').find('>a').off('click');
    $('#add_new').off('submit');
    $('#multi_edit_central_columns').off('submit');
    $('select.default_type').off('change');
    $('button[name=\'delete_central_columns\']').off('click');
    $('button[name=\'edit_central_columns\']').off('click');
});

AJAX.registerOnload('database/central_columns.js', function () {
    $('#tableslistcontainer input,#tableslistcontainer select,#tableslistcontainer .default_value,#tableslistcontainer .open_enum_editor').hide();
    $('#tableslistcontainer').find('.checkall').show();
    $('#tableslistcontainer').find('.checkall_box').show();
    if ($('#table_columns').find('tbody tr').length > 0) {
        $('#table_columns').tablesorter({
            headers: {
                0: { sorter: false },
                1: { sorter: false }, // hidden column
                4: { sorter: 'integer' }
            }
        });
    }
    $('#tablesli