/* Copyright 2008-2021 Codership Oy <http://www.codership.com>
   Copyright (c) 2020, 2021, MariaDB

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; version 2 of the License.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1335 USA */

#ifndef WSREP_MYSQLD_H
#define WSREP_MYSQLD_H

#include <wsrep.h>

#ifdef WITH_WSREP
extern bool WSREP_ON_;
extern bool WSREP_PROVIDER_EXISTS_;

#include <mysql/plugin.h>
#include "mysql/service_wsrep.h"

#include <my_global.h>
#include <my_pthread.h>
#include "log.h"
#include "mysqld.h"

typedef struct st_mysql_show_var SHOW_VAR;
#include <sql_priv.h>
#include "mdl.h"
#include "sql_table.h"
#include "wsrep_mysqld_c.h"

#include "wsrep/provider.hpp"
#include "wsrep/streaming_context.hpp"
#include "wsrep_api.h"
#include <vector>
#include <map>
#include "wsrep_server_state.h"

#define WSREP_UNDEFINED_TRX_ID ULONGLONG_MAX

class set_var;
class THD;

enum wsrep_consistency_check_mode {
    NO_CONSISTENCY_CHECK,
    CONSISTENCY_CHECK_DECLARED,
    CONSISTENCY_CHECK_RUNNING,
};

// Global wsrep parameters

// MySQL wsrep options
extern const char* wsrep_provider;
extern const char* wsrep_provider_options;
extern const char* wsrep_cluster_name;
extern const char* wsrep_cluster_address;
extern const char* wsrep_node_name;
extern const char* wsrep_node_address;
extern const char* wsrep_node_incoming_address;
extern const char* wsrep_data_home_dir;
extern const char* wsrep_dbug_option;
extern long        wsrep_slave_threads;
extern int         wsrep_slave_count_change;
extern ulong       wsrep_debug;
extern my_bool     wsrep_convert_LOCK_to_trx;
extern ulong       wsrep_retry_autocommit;
extern my_bool     wsrep_auto_increment_control;
extern my_bool     wsrep_drupal_282555_workaround;
extern my_bool     wsrep_incremental_data_collection;
extern const char* wsrep_start_position;
extern ulong       wsrep_max_ws_size;
extern ulong       wsrep_max_ws_rows;
extern const char* wsrep_notify_cmd;
extern my_bool     wsrep_certify_nonPK;
extern long int    wsrep_protocol_version;
extern ulong       wsrep_forced_binlog_format;
extern my_bool     wsrep_desync;
extern ulong       wsrep_reject_queries;
extern my_bool     wsrep_recovery;
extern my_bool     wsrep_replicate_myisam;
extern my_bool     wsrep_log_conflicts;
extern ulong       wsrep_mysql_replication_bundle;
extern my_bool     wsrep_load_data_splitting;
extern my_bool     wsrep_restart_slave;
extern my_bool     wsrep_restart_slave_activated;
extern my_bool     wsrep_slave_FK_checks;
extern my_bool     wsrep_slave_UK_checks;
extern ulong       wsrep_trx_fragment_unit;
extern ulong       wsrep_SR_store_type;
extern uint        wsrep_ignore_apply_errors;
extern ulong       wsrep_running_threads;
extern ulong       wsrep_running_applier_threads;
extern ulong       wsrep_running_rollbacker_threads;
extern bool        wsrep_new_cluster;
extern bool        wsrep_gtid_mode;
extern uint        wsrep_gtid_domain_id;
extern ulonglong   wsrep_mode;
extern my_bool     wsrep_strict_ddl;

enum enum_wsrep_reject_types {
  WSREP_REJECT_NONE,    /* nothing rejected */
  WSREP_REJECT_ALL,     /* reject all queries, with UNKNOWN_COMMAND error */
  WSREP_REJECT_ALL_KILL /* kill existing connections and reject all queries*/
};

enum enum_wsrep_OSU_method {
    WSREP_OSU_TOI,
    WSREP_OSU_RSU,
    WSREP_OSU_NONE
};

enum enum_wsrep_sync_wait {
    WSREP_SYNC_WAIT_NONE= 0x0,
    // select, begin
    WSREP_SYNC_WAIT_BEFORE_READ= 0x1,
    WSREP_SYNC_WAIT_BEFORE_UPDATE_DELETE= 0x2,
    WSREP_SYNC_WAIT_BEFORE_INSERT_REPLACE= 0x4,
    WSREP_SYNC_WAIT_BEFORE_SHOW= 0x8,
    WSREP_SYNC_WAIT_MAX= 0xF
};

enum enum_wsrep_ignore_apply_error {
    WSREP_IGNORE_ERRORS_NONE= 0x0,
    WSREP_IGNORE_ERRORS_ON_RECONCILING_DDL= 0x1,
    WSREP_IGNORE_ERRORS_ON_RECONCILING_DML= 0x2,
    WSREP_IGNORE_ERRORS_ON_DDL= 0x4,
    WSREP_IGNORE_ERRORS_MAX= 0x7
};

/* wsrep_mode features */
enum enum_wsrep_mode {
  WSREP_MODE_STRICT_REPLICATION= (1ULL << 0),
  WSREP_MODE_BINLOG_ROW_FORMAT_ONLY= (1ULL << 1),
  WSREP_MODE_REQUIRED_PRIMARY_KEY= (1ULL << 2),
  WSREP_MODE_REPLICATE_MYISAM= (1ULL << 3),
  WSREP_MODE_REPLICATE_ARIA= (1ULL << 4),
  WSREP_MODE_DISALLOW_LOCAL_GTID= (1ULL << 5)
};

// Streaming Replication
#define WSREP_FRAG_BYTES      0
#define WSREP_FRAG_ROWS       1
#define WSREP_FRAG_STATEMENTS 2

#define WSREP_SR_STORE_NONE   0
#define WSREP_SR_STORE_TABLE  1

extern const char *wsrep_fragment_units[];
extern const char *wsrep_SR_store_types[];

// MySQL status variables
extern my_bool     wsrep_connected;
extern my_bool     wsrep_ready;
extern const char* wsrep_cluster_state_uuid;
extern long long   wsrep_cluster_conf_id;
extern const char* wsrep_cluster_status;
extern long        wsrep_cluster_size;
extern long        wsrep_local_index;
extern long long   wsrep_local_bf_aborts;
extern const char* wsrep_provider_name;
extern const char* wsrep_provider_version;
extern const char* wsrep_provider_vendor;
extern char*       wsrep_provider_capabilities;
extern char*       wsrep_cluster_capabilities;

int  wsrep_show_status(THD *thd, SHOW_VAR *var, void *buff,
                       system_status_var *status_var, enum_var_type scope);
int  wsrep_show_ready(THD *thd, SHOW_VAR *var, char *buff);
void wsrep_free_status(THD *thd);
void wsrep_update_cluster_state_uuid(const char* str);

/* Filters out --wsrep-new-cluster oprtion from argv[]
 * should be called in the very beginning of main() */
void wsrep_filter_new_cluster (int* argc, char* argv[]);

int  wsrep_init();
void wsrep_deinit(bool free_options);

/* Initialize wsrep thread LOCKs and CONDs */
void wsrep_thr_init();
/* Destroy wsrep thread LOCKs and CONDs */
void wsrep_thr_deinit();

void wsrep_recover();
bool wsrep_before_SE(); // initialize wsrep before storage
                        // engines (true) or after (false)
/* wsrep initialization sequence at startup
 * @param before wsrep_before_SE() value */
void wsrep_init_startup(bool before);

/* Recover streaming transactions from fragment storage */
void wsrep_recover_sr_from_storage(THD *);

// Other wsrep global variables
extern my_bool     wsrep_inited; // whether wsrep is initialized ?
extern bool        wsrep_service_started;

extern "C" void wsrep_fire_rollbacker(THD *thd);
extern "C" uint32 wsrep_thd_wsrep_rand(THD *thd);
extern "C" time_t wsrep_thd_query_start(THD *thd);
extern void wsrep_close_client_connections(my_bool wait_to_end,
                                           THD *except_caller_thd= NULL);
extern "C" query_id_t wsrep_thd_wsrep_last_query_id(THD *thd);
extern "C" void wsrep_thd_set_wsrep_last_query_id(THD *thd, query_id_t id);

extern int  wsrep_wait_committing_connections_close(int wait_time);
extern void wsrep_close_applier(THD *thd);
extern void wsrep_wait_appliers_close(THD *thd);
extern void wsrep_close_applier_threads(int count);


/* new defines */
extern void wsrep_stop_replication(THD *thd);
extern bool wsrep_start_replication(const char *wsrep_cluster_address);
extern void wsrep_shutdown_replication();
extern bool wsrep_check_mode (enum_wsrep_mode mask);
extern bool wsrep_check_mode_after_open_table (THD *thd, const handlerton *hton,
                                               TABLE_LIST *tables);
extern bool wsrep_check_mode_before_cmd_execute (THD *thd);
extern bool wsrep_must_sync_wait (THD* thd, uint mask= WSREP_SYNC_WAIT_BEFORE_READ);
extern bool wsrep_sync_wait (THD* thd, uint mask= WSREP_SYNC_WAIT_BEFORE_READ);
extern enum wsrep::provider::status
wsrep_sync_wait_upto (THD* thd, wsrep_gtid_t* upto, int timeout);
extern void wsrep_last_committed_id (wsrep_gtid_t* gtid);
extern int  wsrep_check_opts();
extern void wsrep_prepend_PATH (const char* path);
extern bool wsrep_append_fk_parent_table(THD* thd, TABLE_LIST* table, wsrep::key_array* keys);
extern bool wsrep_reload_ssl();

/* Other global variables */
extern wsrep_seqno_t wsrep_locked_seqno;
#define WSREP_ON unlikely(WSREP_ON_)

/* use xxxxxx_NNULL macros when thd pointer is guaranteed to be non-null to
 * avoid compiler warnings (GCC 6 and later) */

#define WSREP_NNULL(thd) \
  (WSREP_PROVIDER_EXISTS_ && thd->variables.wsrep_on)

#define WSREP(thd) \
  (thd && WSREP_NNULL(thd))

#define WSREP_CLIENT_NNULL(thd) \
  (WSREP_NNULL(thd) && thd->wsrep_client_thread)

#define WSREP_CLIENT(thd) \
    (WSREP(thd) && thd->wsrep_client_thread)

#define WSREP_EMULATE_BINLOG_NNULL(thd) \
  (WSREP_NNULL(thd) && wsrep_emulate_bin_log)

#define WSREP_EMULATE_BINLOG(thd) \
  (WSREP(thd) && wsrep_emulate_bin_log)

#define WSREP_BINLOG_FORMAT(my_format)                         \
   ((wsrep_forced_binlog_format != BINLOG_FORMAT_UNSPEC) ?     \
   wsrep_forced_binlog_format : my_format)

/* A wrapper function for MySQL log functions. The call will prefix
   the log message with WSREP and forward the result buffer to fun. */
void WSREP_LOG(void (*fun)(const char* fmt, ...), const char* fmt, ...);

#define WSREP_DEBUG(...)                                                \
    if (wsrep_debug)     sql_print_information( "WSREP: " __VA_ARGS__)
#define WSREP_INFO(...)  sql_print_information( "WSREP: " __VA_ARGS__)
#define WSREP_WARN(...)  sql_print_warning(     "WSREP: " __VA_ARGS__)
#define WSREP_ERROR(...) sql_print_error(       "WSREP: " __VA_ARGS__)
#define WSREP_UNKNOWN(fmt, ...) WSREP_ERROR("UNKNOWN: " fmt, ##__VA_ARGS__)

#define WSREP_LOG_CONFLICT_THD(thd, role)                               \
  WSREP_INFO(                                                \
            "%s: \n "                                            \
            "  THD: %lu, mode: %s, state: %s, conflict: %s, seqno: %lld\n " \
            "  SQL: %s",                                                \
            role,                                                       \
            thd_get_thread_id(thd),                                     \
            wsrep_thd_client_mode_str(thd),                             \
            wsrep_thd_client_state_str(thd),                            \
            wsrep_thd_transaction_state_str(thd),                       \
            wsrep_thd_trx_seqno(thd),                                   \
            wsrep_thd_query(thd)                                        \
            );

#define WSREP_LOG_CONFLICT(bf_thd, victim_thd, bf_abort)                \
  if (wsrep_debug || wsrep_log_conflicts)                               \
  {                                                                     \
    WSREP_INFO("cluster conflict due to %s for threads:",               \
               (bf_abort) ? "high priority abort" : "certification failure" \
              );                                                        \
    if (bf_thd)     WSREP_LOG_CONFLICT_THD(bf_thd, "Winning thread");   \
    if (victim_thd) WSREP_LOG_CONFLICT_THD(victim_thd, "Victim thread"); \
    WSREP_INFO("context: %s:%d", __FILE__, __LINE__); \
  }

#define WSREP_PROVIDER_EXISTS (WSREP_PROVIDER_EXISTS_)

static inline bool wsrep_cluster_address_exists()
{
  if (mysqld_server_started)
    mysql_mutex_assert_owner(&LOCK_global_system_variables);
  return wsrep_cluster_address && wsrep_cluster_address[0];
}

extern my_bool wsrep_ready_get();
extern void wsrep_ready_wait();

class Ha_trx_info;
struct THD_TRANS;

extern mysql_mutex_t LOCK_wsrep_ready;
extern mysql_cond_t  COND_wsrep_ready;
extern mysql_mutex_t LOCK_wsrep_sst;
extern mysql_cond_t  COND_wsrep_sst;
extern mysql_mutex_t LOCK_wsrep_sst_init;
extern mysql_cond_t  COND_wsrep_sst_init;
extern int wsrep_replaying;
extern mysql_mutex_t LOCK_wsrep_replaying;
extern mysql_cond_t  COND_wsrep_replaying;
extern mysql_mutex_t LOCK_wsrep_slave_threads;
extern mysql_cond_t  COND_wsrep_slave_threads;
extern mysql_mutex_t LOCK_wsrep_gtid_wait_upto;
extern mysql_mutex_t LOCK_wsrep_cluster_co<?php

/*
 * This file is part of Twig.
 *
 * (c) Fabien Potencier
 *
 * For the full copyright and license information, please view the LICENSE
 * file that was distributed with this source code.
 */

namespace Twig\Node\Expression;

use Twig\Compiler;
use Twig\Node\Expression\Binary\AndBinary;
use Twig\Node\Expression\Test\DefinedTest;
use Twig\Node\Expression\Test\NullTest;
use Twig\Node\Expression\Unary\NotUnary;
use Twig\Node\Node;

class NullCoalesceExpression extends ConditionalExpression
{
    public function __construct(Node $left, Node $right, int $lineno)
    {
        $test = new DefinedTest(clone $left, 'defined', new Node(), $left->getTemplateLine());
        // for "block()", we don't need the null test as the return value is always a string
        if (!$left instanceof BlockReferenceExpression) {
            $test = new AndBinary(
                $test,
                new NotUnary(new NullTest($left, 'null', new Node(), $left->getTemplateLine()), $left->getTemplateLine()),
                $left->getTemplateLine()
            );
        }

        parent::__construct($test, $left, $right, $lineno);
    }

    public function compile(Compiler $compiler)
    {
        /*
         * This optimizes only one case. PHP 7 also supports more complex expressions
         * that can return null. So, for instance, if log is defined, log("foo") ?? "..." works,
         * but log($a["foo"]) ?? "..." does not if $a["foo"] is not defined. More advanced
         * cases might be implemented as an optimizer node visitor, but has not been done
         * as benefits are probably not worth the added complexity.
         */
        if ($this->getNode('expr2') instanceof NameExpression) {
            $this->getNode('expr2')->setAttribute('always_defined', true);
            $compiler
                ->raw('((')
                ->subcompile($this->getNode('expr2'))
                ->raw(') ?? (')
                ->subcompile($this->getNode('expr3'))
                ->raw('))')
            ;
        } else {
            parent::compile($compiler);
        }
    }
}

class_alias('Twig\Node\Expression\NullCoalesceExpression', 'Twig_Node_Expression_NullCoalesce');
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                              <?php

/*
 * This file is part of Twig.
 *
 * (c) Fabien Potencier
 * (c) Armin Ronacher
 *
 * For the full copyright and license information, please view the LICENSE
 * file that was distributed with this source code.
 */

namespace Twig\Node\Expression;

use Twig\Compiler;

/**
 * Represents a parent node.
 *
 * @author Fabien Potencier <fabien@symfony.com>
 */
class ParentExpression extends AbstractExpression
{
    public function __construct(string $name, int $lineno, string $tag = null)
    {
        parent::__construct([], ['output' => false, 'name' => $name], $lineno, $tag);
    }

    public function compile(Compiler $compiler)
    {
        if ($this->getAttribute('output')) {
            $compiler
                ->addDebugInfo($this)
                ->write('$this->displayParentBlock(')
                ->string($this->getAttribute('name'))
                ->raw(", \$context, \$blocks);\n")
            ;
        } else {
            $compiler
                ->raw('$this->renderParentBlock(')
                ->string($this->getAttribute('name'))
                ->raw(', $context, $blocks)')
            ;
        }
    }
}

class_alias('Twig\Node\Expression\ParentExpression', 'Twig_Node_Expression_Parent');
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                            <?php

/*
 * This file is part of Twig.
 *
 * (c) Fabien Potencier
 *
 * For the full copyright and license information, please view the LICENSE
 * file that was distributed with this source code.
 */

namespace Twig\Node\Expression;

use Twig\Compiler;

class TempNameExpression extends AbstractExpression
{
    public function __construct(string $name, int $lineno)
    {
        parent::__construct([], ['name' => $name], $lineno);
    }

    public function compile(Compiler $compiler)
    {
        $compiler
            ->raw('$_')
            ->raw($this->getAttribute('name'))
            ->raw('_')
        ;
    }
}

class_alias('Twig\Node\Expression\TempNameExpression', 'Twig_Node_Expression_TempName');
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                              