/* -*- C++ -*- */
/* Copyright (c) 2002, 2010, Oracle and/or its affiliates. All rights reserved.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; version 2 of the License.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1335  USA */

#ifndef _SP_RCONTEXT_H_
#define _SP_RCONTEXT_H_

#ifdef USE_PRAGMA_INTERFACE
#pragma interface			/* gcc class implementation */
#endif

#include "sql_class.h"                    // select_result_interceptor
#include "sp_pcontext.h"                  // sp_condition_value

///////////////////////////////////////////////////////////////////////////
// sp_rcontext declaration.
///////////////////////////////////////////////////////////////////////////

class sp_cursor;
class sp_lex_keeper;
class sp_instr_cpush;
class sp_instr_hpush_jump;
class Query_arena;
class sp_head;
class Item_cache;
class Virtual_tmp_table;


/*
  This class is a runtime context of a Stored Routine. It is use