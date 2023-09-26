"use strict";

/* global firstDayOfCalendar */
// templates/javascript/variables.twig

/**
 * Create advanced table (resize, reorder, and show/hide columns; and also grid editing).
 * This function is designed mainly for table DOM generated from browsing a table in the database.
 * For using this function in other table DOM, you may need to:
 * - add "draggable" class in the table header <th>, in order to make it resizable, sortable or hidable
 * - have at least one non-"draggable" header in the table DOM for placing column visibility drop-down arrow
 * - pass the value "false" for the parameter "enableGridEdit"
 * - adjust other parameter value, to select which features that will be enabled
 *
 * @param t the table DOM element
 * @param enableResize Optional, if false, column resizing feature will be disabled
 * @param enableReorder Optional, if false, column reordering feature will be disabled
 * @param enableVisib Optional, if false, show/hide column feature will be disabled
 * @param enableGridEdit Optional, if false, grid editing feature will be disabled
 */
// eslint-disable-next-line no-unused-vars
var makeGrid = function makeGrid(t, enableResize, enableReorder, enableVisib, enableGridEdit) {
  var isResizeEnabled = enableResize === undefined ? true : enableResize;
  var isReorderEnabled = enableReorder === undefined ? true : enableReorder;
  var isVisibEnabled = enableVisib === undefined ? true : enableVisib;
  var isGridEditEnabled = enableGridEdit === undefined ? true : enableGridEdit;
  var g = {
    /** *********
     * Constant
     ***********/
    minColWidth: 15,

    /** *********
     * Variables, assigned with default value, changed later
     ***********/
    actionSpan: 5,
    // number of colspan in Actions header in a table
    tableCreateTime: null,
    // table creation time, used for saving column order and visibility to server, only available in "Browse tab"
    // Column reordering variables
    colOrder: [],
    // array of column order
    // Column visibility variables
    colVisib: [],
    // array of column visibility
    showAllColText: '',
    // string, text for "show all" button under column visibility list
    visibleHeadersCount: 0,
    // number of visible data headers
    // Table hint variables
    reorderHint: '',
    // string, hint for column reordering
    sortHint: '',
    // string, hint for column sorting
    markHint: '',
    // string, hint for column marking
    copyHint: '',
    // string, hint for copy column name
    showReorderHint: false,
    showSortHint: false,
    showMarkHint: false,
    // Grid editing
    isCellEditActive: false,
    // true if current focus is in edit cell
    isEditCellTextEditable: false,
    // true if current edit cell is editable in the text input box (not textarea)
    currentEditCell: null,
    // reference to <td> that currently being edited
    cellEditHint: '',
    // hint shown when doing grid edit
    gotoLinkText: '',
    // "Go to link" text
    wasEditedCellNull: false,
    // true if last value of the edited cell was NULL
    maxTruncatedLen: 0,
    // number of characters that can be displayed in a cell
    saveCellsAtOnce: false,
    // $cfg[saveCellsAtOnce]
    isCellEdited: false,
    // true if at least one cell has been edited
    saveCellWarning: '',
    // string, warning text when user want to leave a page with unsaved edited data
    lastXHR: null,
    // last XHR object used in AJAX request
    isSaving: false,
    // true when currently saving edited data, used to handle double posting caused by pressing ENTER in grid edit text box in Chrome browser
    alertNonUnique: '',
    // string, alert shown when saving edited nonunique table
    // Common hidden inputs
    token: null,
    server: null,
    db: null,
    table: null,

    /** **********
     * Functions
     ************/

    /**
     * Start to resize column. Called when clicking on column separator.
     *
     * @param e event
     * @param obj dragged div object
     */
    dragStartRsz: function dragStartRsz(e, obj) {
      var n = $(g.cRsz).find('div').index(obj); // get the index of separator (i.e., column index)

      $(obj).addClass('colborder_active');
      g.colRsz = {
        x0: e.pageX,
        n: n,
        obj: obj,
        objLeft: $(obj).position().left,
        objWidth: $(g.t).find('th.dragga