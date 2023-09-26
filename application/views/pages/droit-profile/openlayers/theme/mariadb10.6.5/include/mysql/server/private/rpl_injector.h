/* Copyright (c) 2006, 2010, Oracle and/or its affiliates. All rights reserved.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; version 2 of the License.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1335  USA */

#ifndef INJECTOR_H
#define INJECTOR_H

/* Pull in 'byte', 'my_off_t', and 'uint32' */
#include <my_bitmap.h>

#include "rpl_constants.h"
#include "table.h"                              /* TABLE */

/* Forward declarations */
class handler;
class MYSQL_BIN_LOG;
struct TABLE;


/*
  Injector to inject rows into the MySQL server.
  
  The injector class is used to notify the MySQL server of new rows that have
  appeared outside of MySQL control.
 
  The original purpose of this is to allow clusters---which handle replication
  inside the cluster through other means---to insert new rows into binary log.
  Note, however, that the injector should be used whenever rows are altered in
  any manner that is outside of MySQL server visibility and which therefore
  are not seen by the MySQL server.
 */
class injector 
{
public:

  /*
    Get an instance of the injector.

    DESCRIPTION
      The injector is a Singleton, so this static function return the
      available instance of the injector.

    RETURN VALUE
      A pointer to the available injector object.
  */
  static injector *instance();

  /*
    Delete the singleton instance (if allocated). Used during server shutdown.
  */
  static void free_instance();

    /*
      A transaction where rows can be added.

      DESCRIPTION
        The transaction class satisfy the **CopyConstructible** and
        **Assignable** requirements.  Note that the transaction is *not*
        default constructible.
     */
    class transaction {
      friend class injector;
    public:
      /* Convenience definitions */
      typedef uchar* record_type;
      typedef uint32 server_id_type;

      /*
        Table reference.

        RESPONSIBILITY

          The class contains constructors to handle several forms of
          references to tables.  The constructors can implicitly be used to
          construct references from, e.g., strings containing table names.

        EXAMPLE

          The class is intended to be used *by value*.  Please, do not try to
          construct objects of this type using 'new'; instead construct an
          object, possibly a temporary object.  For example:

            injector::transaction::table tbl(share->table, true);
            MY_BITMAP cols;
            my_bitmap_init(&cols, NULL, (i + 7) / 8, false);
            inj->write_row(::server_id, tbl, &cols, row_data);

          or

            MY_BITMAP cols;
            my_bitmap_init(&cols, NULL, (i + 7) / 8, false);
            inj->write_row(::server_id, 
                           injector::transaction::table(share->table, true), 
                           &cols, row_data);

          This will work, be more efficient, and have greater chance of
          inlining, not run the risk of losing pointers.

        COLLABORATION

          injector::transaction
            Provide a flexible interface to the representation of tables.

      */
      class table 
      {
      public:
        table(TABLE *table, bool is_transactional_arg)
            : m_table(table), m_is_transactional(is_transactional_arg)
        { 
        }

        char const *db_name() const { return m_table->s->db.str; }
        char const *table_name() const { return m_table->s->table_name.str; }
        TABLE *get_table() const { return m_table; }
        bool is_transactional() const { return m_is_transactional; }

      private:
        TABLE *m_table;
        bool m_is_transactional;
      };

      /*
        Binlog position as a structure.
      */
      class binlog_pos {
        friend class transaction;
      public:
        char const *file_name() const { return m_file_name; }
        my_off_t file_pos() const { return m_file_pos; }

      private:
        char const *m_file_name;
        my_off_t m_file_pos;
      };

      transaction() : m_thd(NULL) { }
      transaction(transaction const&);
      ~transaction();

      /* Clear transaction, i.e., make calls to 'good()' return false. */
      void clear() { m_thd= NULL; }

      /* Is the transaction in a good state? */
      bool good() const { return m_thd != NULL; }

      /* Default assignment operator: standard implementation */
      transaction& operator=(transaction t) {
        swap(t);
        return *this;
      }
      
      /*

        DESCRIPTION

          Register table for use within the transaction.  All tables
          that are going to be used need to be registered before being
          used below.  The member function will fail with an error if
          use_table() is called after any *_row() function has been
          called for the transaction.

        RETURN VALUE

          0         All OK
          >0        Failure

       */
#ifdef TO_BE_DELETED
      int use_table(server_id_type sid, table tbl);
#endif
      /*
        Commit a transaction.

        This member function will clean up after a sequence of *_row calls by,
        for example, releasing resource and unlocking files.
      */
      int commit();

      /*
        Get the position for the start of the transaction.

        Returns the position in the binary log of the first event in this
        transaction. If no event is yet written, the position where the event
        *will* be written is returned. This position is known, since a
        new_transaction() will lock the binary log and prevent any other
        writes to the binary log.
      */
      binlog_pos start_pos() const;

    private:
      /* Only the injector may construct these object */
      transaction(MYSQL_BIN_LOG *, THD *);

      void swap(transaction& o) {
        /* std::swap(m_start_pos, o.m_start_pos); */
        {
          binlog_pos const tmp= m_start_pos;
          m_start_pos= o.m_start_pos;
          o.m_start_pos= tmp;
        }

        /* std::swap(m_thd, o.m_thd); */
        {
          THD* const tmp= m_thd;
          m_thd= o.m_thd;
          o.m_thd= tmp;
        }
        {
          enum_state const tmp= m_state;
          m_state= o.m_state;
          o.m_state= tmp;
        }
      }

      enum enum_state
      {
        START_STATE,                            /* Start state */
        TABLE_STATE,      /* At least one table has been registered */
        ROW_STATE,          /* At least one row has been registered */
        STATE_COUNT               /* State count and sink state */
      } m_state;

      /*
        Check and update the state.

        PARAMETER(S)

          target_state
              The state we are moving to: TABLE_STATE if we are
              writing a table and ROW_STATE if we are writing a row.

        DESCRIPTION

          The internal state will be updated to the target state if
          and only if it is a legal move.  The only legal moves are:

              START_STATE -> START_STATE
              START_STATE -> TABLE_STATE
              TABLE_STATE -> TABLE_STATE
              TABLE_STATE -> ROW_STATE

          That is:
          - It is not possible to write any row before having written at
            least one table
          - It is not possible to write a table after at least one row
            has been written

        RETURN VALUE

           0    All OK
          -1    Incorrect call sequence
       */
      int check_state(enum_state const target_state)
      {
#ifndef DBUG_OFF
        static char const *state_name[] = {
          "START_STATE", "TABLE_STATE", "ROW_STATE", "STATE_COUNT"
        };

        DBUG_div.olControlZoom {
    position: absolute;
    top: 8px;
    left: 8px;
    background: rgba(255,255,255,0.4);
    border-radius: 4px;
    padding: 2px;
}
* {
    -webkit-tap-highlight-color: rgba(0, 0, 0, 0);
}
div.olControlZoom a {
    display: block;
    margin: 1px;
    padding: 0;
    color: white;
    font-size: 28px;
    font-family: sans-serif;
    font-weight: bold;
    text-decoration: none;
    text-align: center;
    height: 32px;
    width: 32px;
    line-height: 28px;
    text-shadow: 0 0 3px rgba(0,0,0,0.8);
    background: #130085; /* fallback for IE - IE6 requires background shorthand*/
    background: rgba(0, 60, 136, 0.5);
    filter: alpha(opacity=80);
}
a.olControlZoomIn {
    border-radius: 4px 4px 0 0;
}
a.olControlZoomOut {
    border-radius: 0 0 4px 4px;
}
div.olControlZoom a:hover {
    background: #130085; /* fallback for IE */
    background: rgba(0, 60, 136, 0.7);
    filter: alpha(opacity=100);
}
@media only screen and (max-width: 600px) {
    div.olControlZoom a:hover {
        background: rgba(0, 60, 136, 0.5);
    }
}
div.olMapViewport {
    -ms-touch-action: none;
}
.olLayerGrid .olTileImage {
    -webkit-transition: opacity 0.2s linear;
    -moz-transition: opacity 0.2s linear;
    -o-transition: opacity 0.2s linear;
    transition: opacity 0.2s linear;
}
/* Turn on GPU support where available */
.olTileImage {
    -webkit-transform: translateZ(0);
    -moz-transform: translateZ(0);
    -o-transform: 