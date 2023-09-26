<tr id="row_tbl_{{ curr }}"{{ table_is_view ? ' class="is_view"' }} data-filter-row="{{ current_table['TABLE_NAME']|upper }}">
    <td class="text-center d-print-none">
        <input type="checkbox"
            name="selected_tbl[]"
            class="{{ input_class }}"
            value="{{ current_table['TABLE_NAME'] }}"
            id="checkbox_tbl_{{ curr }}">
    </td>
    <th>
        <a href="{{ url('/sql', table_url_params|merge({'pos': 0})) }}" title="{{ browse_table_label_title }}">
            {{ browse_table_label_truename }}
        </a>
        {{ tracking_icon|raw }}
    </th>
    {% if server_slave_status %}
        <td class="text-center">
            {{ ignored ? get_image('s_cancel', 'Not replicated'|trans) }}
            {{ do ? get_image('s_success', 'Replicated'|trans) }}
        </td>
    {% endif %}

    {# Favorite table anchor #}
    {% if num_favorite_tables > 0 %}
        <td class="text-center d-print-none">
            {# Check if current table is already in favorite list #}
            {% set fav_params = {
                'db': db,
                'ajax_request': true,
                'favorite_table': current_table['TABLE_NAME'],
                ((already_favorite ? 'remove' : 'add') ~ '_favorite'): true
            } %}
            {% include 'database/structure/favorite_anchor.twig' with {
                'table_name_hash': table_name_hash,
                'db_table_name_hash': db_table_name_hash,
                'fav_params': fav_params,
                'already_favorite': already_favorite,
            } only %}
        </td>
    {% endif %}

    <td class="text-center d-print-none">
        <a href="{{ url('/sql', table_url_params|merge({'pos': 0})) }}">
          {{ may_have_rows ? get_icon('b_browse', 'Browse'|trans) : get_icon('bd_browse', 'Browse'|trans) }}
        </a>
    </td>
    <td class="text-center d-print-none">
        <a href="{{ url('/table/structure', table_url_params) }}">
          {{ get_icon('b_props', 'Structure'|trans) }}
        </a>
    </td>
    <td class="text-center d-print-none">
        <a href="{{ url('/table/search', table_url_params) }}">
          {{ may_have_rows ? get_icon('b_select', 'Search'|trans) : get_icon('bd_select', 'Search'|trans) }}
        </a>
    </td>

    {% if not db_is_system_schema %}
        <td class="insert_table text-center d-print-none">
            <a href="{{ url('/table/change', table_url_params) }}">{{ get_icon('b_insrow', 'Insert'|trans) }}</a>
        </td>
        {% if table_is_view %}
            <td class="text-center d-print-none">
                <a href="{{ url('/view/create', {
                    'db': db,
                    'table': current_table['TABLE_NAME']
                }) }}">{{ get_icon('b_edit', 'Edit'|trans) }}</a>
            </td>
        {% else %}
          <td class="text-center d-print-none">
                <a class="truncate_table_anchor ajax" href="{{ url('/sql') }}" data-post="{{ get_common(table_url_params|merge({
                  'sql_query': empty_table_sql_query,
                  'message_to_show': empty_table_message_to_show
                }), '') }}">
                  {{ may_have_rows ? get_icon('b_empty', 'Empty'|trans) : get_icon('bd_empty', 'Empty'|trans) }}
                </a>
          </td>
        {% endif %}
        <td class="text-center d-print-none">
            <a class="ajax drop_table_anchor
                {{- table_is_view or current_table['ENGINE'] == null ? ' view' }}" href="{{ url('/sql') }}" data-post="
                {{- get_common(table_url_params|merge({
                  'reload': 1,
                  'purge': 1,
                  'sql_query': drop_query,
                  'message_to_show': drop_message
                }), '') }}">
                {{ get_icon('b_drop', 'Drop'|trans) }}
            </a>
        </td>
    {% endif %}

    {% if current_table['TABLE_ROWS'] is defined
        and (current_table['ENGINE'] != null or table_is_view) %}
        {# Get the row count #}
        {% set row_count = format_number(current_table['TABLE_ROWS'], 0) %}

        {# Content to be appended into 'tbl_rows' cell.
        If row count is approximate, display it as an anchor to get real count. #}
        <td class="value tbl_rows text-monospace text-right"
            data-table="{{ current_table['TABLE_NAME'] }}">
            {% if approx_rows %}
                <a href="{{ url('/database/structure/real-row-