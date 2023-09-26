        // Provide this option only for 5.7.6+
            // OR for privileged users in 5.5.7+
            if (($serverType == 'MySQL'
                && $serverVersion >= 50706)
                || ($GLOBALS['dbi']->isSuperuser() && $mode == 'edit_other')
            ) {
                $auth_plugin_dropdown = Privileges::getHtmlForAuthPluginsDropdown(
                    $orig_auth_plugin, 'change_pw', 'new'
                );

                $html .= '<tr class="vmiddle">'
                    . '<td>' . __('Password Hashing:') . '</td><td>';
                $html .= $auth_plugin_dropdown;
                $html .= '</td></tr>'
                    . '<tr id="tr_element_before_generate_password"></tr>'
                    . '</table>';

                $html .= '<div'
                    . ($orig_auth_plugin != 'sha256_password'
                        ? ' class="hide"'
                        : '')
                    . ' id="ssl_reqd_war