	/*
   Copyright (c) 2002, 2021, Oracle and/or its affiliates.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License, version 2.0,
   as published by the Free Software Foundation.

   This program is also distributed with certain software (including
   but not limited to OpenSSL) that is licensed under separate terms,
   as designated in a particular file or component or in included license
   documentation.  The authors of MySQL hereby grant you an additional
   permission to link the program and your derivative works with the
   separately licensed software that they have included with MySQL.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License, version 2.0, for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301  USA */

#ifndef _my_getopt_h
#define _my_getopt_h

#include "my_sys.h"                             /* loglevel */

C_MODE_START

#define GET_NO_ARG     1
#define GET_BOOL       2
#define GET_INT        3
#define GET_UINT       4
#define GET_LONG       5
#define GET_ULONG      6
#define GET_LL         7
#define GET_ULL        8
#define GET_STR        9
#define GET_STR_ALLOC 10
#define GET_DISABLED  11
#define GET_ENUM      12
#define GET_SET       13
#define GET_DOUBLE    14
#define GET_FLAGSET   15
#define GET_PASSWORD  16

#if SIZEOF_INT == 4
#define GET_INT32 GET_INT
#define GET_UINT32 GET_UINT
#elif SIZEOF_LONG == 4
#define GET_INT32 GET_LONG
#define GET_UINT32 GET_ULONG
#else
#error Neither int or long is of 4 bytes width
#endif

#define GET_ASK_ADDR	 128
#define GET_TYPE_MASK	 127

/**
  Enumeration of the my_option::arg_type attributes.
  It shou