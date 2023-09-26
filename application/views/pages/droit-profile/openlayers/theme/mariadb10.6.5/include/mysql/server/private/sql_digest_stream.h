<?php
/* vim: set expandtab sw=4 ts=4 sts=4: */
/**
 * Displays form for password change
 *
 * @package PhpMyAdmin
 */
namespace PhpMyAdmin\Display;

use PhpMyAdmin\Message;
use PhpMyAdmin\Server\Privileges;
use PhpMyAdmin\Url;
use PhpMyAdmin\Util;

/**
 * Displays form for password change
 *
 * @package PhpMyAdmin
 */
class ChangePassword
{
    /**
      * Get HTML for the Change password dialog
      *
      * @param string $mode     where is the function being called?
      *                         values : 'change_pw' or 'edit_other'
      * @param string $username username
      * @param string $hostname hostname
      *
      * @return string html snippet
      */
    public static function getHtml($mode, $username, $hostname)
    {
        /**
         * autocomplete feature of IE kills the "onchange" event handler and it
         * must be replaced by the "onpropertychange" one in this case
         */
        $chg_evt_handler = 'onchange';

        $is_privileges = basename($_SERVER['SCRIPT_NAME']) === 'server_privileges.php';

        $html = '<form method="post" id="change_password_form" '
            . 'action="' . basename($GLOBALS['PMA_PHP_SELF']) . '" '
            . 'name="chgPassword" '
            . 'class="' . ($is_privileges ? 'submenu-item' : '') . '">';

        $html .= Url::getHiddenInputs();

        if (strpos($GLOBALS['PMA_PHP_SELF'], 'server_privileges') !== false) {
            $html .= '<input type="hidden" name="username" '
                . 'value="' . htmlspecialchars($username) . '" />'
                . '<