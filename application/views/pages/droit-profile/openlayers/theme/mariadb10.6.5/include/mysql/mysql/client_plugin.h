/* Copyright (C) 2010 - 2012 Sergei Golubchik and Monty Program Ab
                 2014 MariaDB Corporation AB

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.
   
   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.
   
   You should have received a copy of the GNU Library General Public
   License along with this library; if not see <http://www.gnu.org/licenses>
   or write to the Free Software Foundation, Inc., 
   51 Franklin St., Fifth Floor, Boston, MA 02110, USA */

/**
  @file

  MySQL Client Plugin API

  This file defines the API for plugins that work on the client side
*/
#ifndef MYSQL_CLIENT_PLUGIN_INCLUDED
#define MYSQL_CLIENT_PLUGIN_INCLUDED

#ifndef MYSQL_ABI_CHECK
#include <stdarg.h>
#include <stdlib.h>
#endif


#ifndef PLUGINDIR
#define PLUGINDIR "lib/plugin"
#endif

#define plugin_declarations_sym "_mysql_client_plugin_declaration_"

/* known plugin types */
#define MYSQL_CLIENT_PLUGIN_RESERVED         0 
#define MYSQL_CLIENT_PLUGIN_RESERVED2        1
#define MYSQL_CLIENT_AUTHENTICATION_PLUGIN   2 /* authentication   */

#define MYSQL_CLIENT_AUTHENTICATION_PLUGIN_INTERFACE_VERSION  0x0100
#define MYSQL_CLIENT_MAX_PLUGINS             3

/* Connector/C specific plugin types */
#define MARIADB_CLIENT_REMOTEIO_PLUGIN       100 /* communication IO */
#define MARIADB_CLIENT_PVIO_PLUGIN           101
#define MARIADB_CLIENT_TRACE_PLUGIN          102
#define MARIADB_CLIENT_CONNECTION_PLUGIN     103

#define MARIADB_CLIENT_REMOTEIO_PLUGIN_INTERFACE_VERSION 0x0100
#define MARIADB_CLIENT_PVIO_PLUGIN_INTERFACE_VERSION 0x0100
#define MARIADB_CLIENT_TRACE_PLUGIN_INTERFACE_VERSION 0x0100
#define MARIADB_CLIENT_CONNECTION_PLUGIN_INTERFACE_VERSION 0x0100

#define MARIADB_CLIENT_MAX_PLUGINS             4

#define mysql_declare_client_plugin(X)          \
     struct st_mysql_client_plugin_ ## X        \
        _mysql_client_plugin_declaration_ = {   \
          MYSQL_CLIENT_ ## X ## _PLUGIN,        \
          MYSQL_CLIENT_ ## X ## _PLUGIN_INTERFACE_VERSION,
#define mysql_end_client_plugin             }

/* generic plugin header structure */
#ifndef MYSQL_CLIENT_PLUGIN_HEADER
#define MYSQL_CLIENT_PLUGIN_HEADER                      \
  int type;                                             \
  unsigned int interface_version;                       \
  const char *name;                                     \
  const char *author;                                   \
  const char *desc;                                     \
  unsigned int version[3];                              \
  const char *license;                                  \
  void *mysql_api;                                      \
  int (*init)(char *, size_t, int, va_list);            \
  int (*deinit)(void);                                  \
  int (*options)(const char *option, const void *);
struct st_mysql_client_plugin
{
  MYSQL_CLIENT_PLUGIN_HEADER
};
#endif

struct st_mysql;

/********* connection handler plugin specific declarations **********/

typedef struct st_ma_connection_plugin
{
  MYSQL_CLIENT_PLUGIN_HEADER
  /* functions */
  MYSQL *(*connect)(MYSQL *mysql, const char *host,
                    const char *user, const char *passwd,
		                const char *db, unsigned int port,
                    const char *unix_socket, unsigned long clientflag);
  void (*close)(MYSQL *mysql);
  int (*set_optionsv)(MYSQL *mysql, unsigned int option, ...);
  int (*set_connection)(MYSQL *mysql,enum enum_server_command command,
                        const char *arg,
                        size_t length, my_bool skipp_check, void *opt_arg);
  my_bool (*reconnect)(MYSQL *mysql);
  int (*reset)(MYSQL *mysql);
} MARIADB_CONNECTION_PLUGIN;

#define MARIAD<?php
/**
 * Contains functions used by browse foreigners
 */

declare(strict_types=1);

namespace PhpMyAdmin;

use function asort;
use function ceil;
use function floor;
use function htmlspecialchars;
use function is_array;
use function mb_strlen;
use function mb_substr;

/**
 * PhpMyAdmin\BrowseForeigners class
 */
class BrowseForeigners
{
    /** @var int */
    private $limitChars;
    /** @var int */
    private $maxRows;
    /** @var int */
    private $repeatCells;
    /** @var bool */
    private $showAll;
    /** @var string */
    private $themeImage;

    /** @var Template */
    public $template;

    /**
     * @param Template $template Template object
     */
    public function __construct(Template $template)
    {
        global $cfg, $PMA_Theme;

        $this->template = $template;

        $this->limitChars = (int) $cfg['LimitChars'];
        $this->maxRows = (int) $cfg['MaxRows'];
        $this->repeatCells = (int) $cfg['RepeatCells'];
        $this->showAll = (bool) $cfg['ShowAll'];
        $this->themeImage = $PMA_Theme->getImgPath();
    }

    /**
     * Function to get html for one relational key
     *
     * @param int    $horizontal_count   the current horizontal count
     * @param string $header             table header
     * @param array  $keys               all the keys
     * @param int    $indexByKeyname     index by keyname
     * @param array  $descriptions       descriptions
     * @param int    $indexByDescription index by description
     * @param string $current_value      current value on the edit form
     *
     * @return array the generated html
     */
    private function getHtmlForOneKey(
        int $horizontal_count,
        string $header,
        array $keys,
        int $indexByKeyname,
        array $descriptions,
        int $indexByDescription,
        string $current_value
    ): array {
        $horizontal_count++;
        $output = '';

        // whether the key name corresponds to the selected value in the form
        $rightKeynameIsSelected = false;
        $leftKeynameIsSelected = false;

        if ($this->repeatCells > 0 && $horizontal_count > $this->repeatCells) {
            $output .= $header;
            $horizontal_count = 0;
        }

        // key names and descriptions for the left section,
        // sorted by key names
        $leftKeyname = $keys[$indexByKeyname];
        [
            $leftDescription,
            $leftDescriptionTitle,
        ] = $this->getDescriptionAndTitle($descriptions[$indexByKeyname]);

        // key names and descriptions for the right section,
        // sorted by descriptions
        $rightKeyname = $keys[$indexByDescription];
        [
            $rightDescription,
            $rightDescriptionTitle,
        ] = $this->getDescriptionAndTitle($descriptions[$indexByDescription]);

        $indexByDescription++;

        if (! empty($current_value)) {
            $rightKeynameIsSelected = $rightKeyname == $current_value;
            $leftKeynameIsSelected = $leftKeyname == $current_value;
        }

        $output .= '<tr class="noclick">';

        $output .= $this->template->render('table/browse_foreigners/column_element', [
            'keyname' => $leftKeyname,
            'description' => $leftDescription,
            'title' => $leftDescriptionTitle,
            'is_selected' => $leftKeynameIsSelected,
            'nowrap' => true,
        ]);
        $output .= $this->template->render('table/browse_foreigners/column_element', [
            'keyname' => $leftKeyname,
            'description' => $leftDescription,
            'title' => $leftDescriptionTitle,
            'is_selected' => $leftKeynameIsSelected,
            'nowrap' => false,
        ]);

        $output .= '<td width="20%">'
            . '<img src="' . $this->themeImage . 'spacer.png" alt=""'
            . ' width="1" height="1"></td>';

        $output .= $this->template->render('table/browse_foreigners/column_element', [
            'keyname' => $rightKeyname,
            'description' => $rightDescription,
            'title' => $rightDescriptionTitle,
            'is_selected' => $rightKeynameIsSelected,
            'nowrap' => false,
        ]);
        $output .= $this->template->render('table/browse_foreigners/column_element', [
            'keyname' => $rightKeyname,
            'description' => $rightDescription,
            'title' => $rightDescriptionTitle,
            'is_selected' => $rightKeynameIsSelected,
            'nowrap' => true,
        ]);

  