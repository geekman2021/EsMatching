nsformed fields in the edited cells
       */

      var transformationFields = false;
      /**
       * @var full_sql_query String containing the complete SQL query to update this table
       */

      var fullSqlQuery = '';
      /**
       * @var rel_fields_list  String, url encoded representation of {@link relations_fields}
       */

      var relFieldsList = '';
      /**
       * @var transform_fields_list  String, url encoded representation of {@link transformFields}
       */

      var transformFieldsList = '';
      /**
       * @var where_clause Array containing where clause for updated fields
       */

      var fullWhereClause = [];
      /**
       * @var is_unique   Boolean, whether the rows in this table is unique or not
       */

      var isUnique = $(g.t).find('td.edit_row_anchor').is('.nonunique') ? 0 : 1;
      /**
       * multi edit variables
       */

      var multiEditFieldsName = [];
      var multiEditFieldsType = [];
      var multiEditFields = [];
      var multiEditFieldsNull = []; // alert user if edited table is not unique

      if (!isUnique) {
        alert(g.alertNonUnique);
      } // loop each edited row


      $(g.t).find('td.to_be_saved').parents('tr').each(function () {
        var $tr = $(this);
        var whereClause = $tr.find('.where_clause').val();

        if (typeof whereClause === 'undefined') {
          whereClause = '';
        }

        fullWhereClause.push(whereClause);
        var conditionArray = JSON.parse($tr.find('.condition_array').val());
        /**
         * multi edit variables, for current row
         * @TODO array indices are still not correct, they should be md5 of field's name
         */

        var fieldsName = [];
        var fieldsType = [];
        var fields = [];
        var fieldsNull = []; // loop each edited cell in a row

        $tr.find('.to_be_saved').each(function () {
          /**
           * @var $this_field    Object referring to the td that is being edited
           */
          var $thisField = $(this);
          /**
           * @var field_name  String containing the name of this field.
           * @see Sql.getFieldName()
           */

          var fieldName = Sql.getFieldName($(g.t), $thisField);
          /**
           * @var this_field_params   Array temporary storage for the name/value of current field
           */

 