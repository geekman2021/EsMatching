/* Copyright (c) 2005, 2016, Oracle and/or its affiliates.
   Copyright (c) 2009, 2020, MariaDB Corporation.

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

#ifndef LOG_H
#define LOG_H

#include "handler.h"                            /* my_xid */
#include "wsrep_mysqld.h"
#include "rpl_constants.h"

class Relay_log_info;

class Format_description_log_event;

bool reopen_fstreams(const char *filename, FILE *outstream, FILE *errstream);
void setup_log_handling();
bool trans_has_updated_trans_table(const THD* thd);
bool stmt_has_updated_trans_table(const THD *thd);
bool use_trans_cache(const THD* thd, bool is_transactional);
bool ending_trans(THD* thd, const bool all);
bool ending_single_stmt_trans(THD* thd, const bool all);
bool trans_has_updated_non_trans_table(const THD* thd);
bool stmt_has_updated_non_trans_table(const THD* thd);

/*
  Transaction Coordinator log - a base abstract class
  for two different implementations
*/
class TC_LOG
{
  public:
  int using_heuristic_recover();
  TC_LOG() {}
  virtual ~TC_LOG() {}

  virtual int open(const char *opt_name)=0;
  virtual void close()=0;
  /*
    Transaction coordinator 2-phase commit.

    Must invoke the run_prepare_ordered and run_commit_ordered methods, as
    described below for these methods.

    In addition, must invoke THD::wait_for_prior_commit(), or equivalent
    wait, to ensure that one commit waits for another if registered to do so.
  */
  virtual int log_and_order(THD *thd, my_xid xid, bool all,
                            bool need_prepare_ordered,
                            bool need_commit_ordered) = 0;
  virtual int unlog(ulong cookie, my_xid xid)=0;
  virtual int unlog_xa_prepare(THD *thd, bool all)= 0;
  virtual void commit_checkpoint_notify(void *cookie)= 0;

protected:
  /*
    These methods are meant to be invoked from log_and_order() implementations
    to run any prepare_ordered() respectively commit_ordered() methods in
    participating handlers.

    They must be called using suitable thread syncronisation to ensure that
    they are each called in the correct commit order among all
    transactions. However, it is only necessary to call them if the
    corresponding flag passed to log_and_order is set (it is safe, but not
    required, to call them when the flag is false).

    The caller must be holding LOCK_prepare_ordered respectively
    LOCK_commit_ordered when calling these methods.
  */
  void run_prepare_ordered(THD *thd, bool all);
  void run_commit_ordered(THD *thd, bool all);
};

/*
  Locks used to ensure serialised execution of TC_LOG::run_prepare_ordered()
  and TC_LOG::run_commit_ordered(), or any other code that calls handler
  prepare_ordered() or commit_ordered() methods.
*/
extern mysql_mutex_t LOCK_prepare_ordered;
extern mysql_cond_t COND_prepare_ordered;
extern mysql_mutex_t LOCK_after_binlog_sync;
extern mysql_mutex_t LOCK_commit_ordered;
#ifdef HAVE_PSI_INTERFACE
extern PSI_mutex_key key_LOCK_prepare_ordered, key_LOCK_commit_ordered;
extern PSI_mutex_key key_LOCK_after_binlog_sync;
extern PSI_cond_key key_COND_prepare_ordered;
#endif

class TC_LOG_DUMMY: public TC_LOG // use it to disable the logging
{
public:
  TC_LOG_DUMMY() {}
  int open(const char *opt_name)        { return 0; }
  void close()                          { }
  /*
    TC_LOG_DUMMY is only used when there are <= 1 XA-capable engines, and we
    only use internal XA during commit when >= 2 XA-capable engines
    participate.
  */
  int log_and_order(THD *thd, my_xid xid, bool all,
                    bool need_prepare_ordered, bool need_commit_ordered)
  {
    DBUG_ASSERT(0);
    return 1;
  }
  int unlog(ulong cookie, my_xid xid)  { return 0; }
  int unlog_xa_prepare(THD *thd, bool all)
  {
    return 0;
  }
  void commit_checkpoint_notify(void *cookie) { DBUG_ASSERT(0); };
};

#define TC_LOG_PAGE_SIZE   8192

#ifdef HAVE_MMAP
class TC_LOG_MMAP: public TC_LOG
{
  public:                // only to keep Sun Forte on sol9x86 happy
  typedef enum {
    PS_POOL,                 // page is in pool
    PS_ERROR,                // last sync failed
    PS_DIRTY                 // new xids added since last sync
  } PAGE_STATE;

  struct pending_cookies {
    uint count;
    uint pending_count;
    ulong cookies[1];
  };

  private:
  typedef struct st_page {
    struct st_page *next; // page a linked in a fifo queue
    my_xid *start, *end;  // usable area of a page
    my_xid *ptr;          // next xid will be written here
    int size, free;       // max and current number of free xid slots on the page
    int waiters;          // number of waiters on condition
    PAGE_STATE state;     // see above
    mysql_mutex_t lock; // to access page data or control structure
    mysql_cond_t  cond; // to wait for a sync
  } PAGE;

  /* List of THDs for which to invoke commit_ordered(), in order. */
  struct commit_entry
  {
    struct commit_entry *next;
    THD *thd;
  };

  char logname[FN_REFLEN];
  File fd;
  my_off_t file_length;
  uint npages, inited;
  uchar *data;
  struct st_page *pages, *syncing, *active, *pool, **pool_last_ptr;
  /*
    note that, e.g. LOCK_active is only used to protect
    'active' pointer, to protect the content of the active page
    one has to use active->lock.
    Same for LOCK_pool and LOCK_sync
  */
  mysql_mutex_t LOCK_active, LOCK_pool, LOCK_sync, LOCK_pending_checkpoint;
  mysql_cond_t COND_pool, COND_active;
  /*
    Queue of threads that need to call commit_ordered().
    Access to this queue must be protected by LOCK_prepare_ordered.
  */
  commit_entry *commit_ordered_queue;
  /*
    This flag and condition is used to reserve the queue while threads in it
    each run the commit_ordered() methods one after the other. Only once the
    last commit_ordered() in the queue is done can we start on a new queue
    run.

    Since we start this process in the first thread in the queue and finish in
    the last (and possibly different) thread, we need a condition variable for
    this (we cannot unlock a mutex in a different thread than the one who
    locked it).

    The condition is used together with the LOCK_prepare_ordered mutex.
  */
  mysql_cond_t COND_queue_busy;
  my_bool commit_ordered_queue_busy;
  pending_cookies* pending_checkpoint;

  public:
  TC_LOG_MMAP(): inited(0), pending_checkpoint(0) {}
  int open(const char *opt_name);
  void close();
  int log_and_order(THD *thd, my_xid xid, bool all,
                    bool need_prepare_ordered, bool need_commit_ordered);
  int unlog(ulong cookie, my_xid xid);
  int unlog_xa_prepare(THD *thd, bool all)
  {
    return 0;
  }
  void commit_checkpoint_notify(void *cookie);
  int recover();

  private:
  int log_one_transaction(my_xid xid);
  void get_active_from_pool();
  int sync();
  int overflow();
  int delete_entry(ulong cookie);
};
#else
#define TC_LOG_MMAP TC_LOG_DUMMY
#endif

extern TC_LOG *tc_log;
extern TC_LOG_MMAP tc_log_mmap;
extern TC_LOG_DUMMY tc_log_dummy;

/* log info errors */
#define LOG_INFO_EOF -1
#define LOG_INFO_IO  -2
#define LOG_INFO_INVALID -3
#define LOG_INFO_SEEK -4
#define LOG_INFO_MEM -6
#define LOG_INFO_FATAL -7
#define LOG_INFO_IN_USE -8
#define LOG_INFO_EMFILE -9


/* bitmap to SQL_LOG::close() */
#define LOG_CLOSE_INDEX		1
#define LOG_CLOSE_TO_BE_OPENED	2
#define LOG_CLOSE_STOP_EVENT	4
#define LOG_CLOSE_DELAYED_CLOSE 8

/* 
  Maximum unique log filename extension.
  Note: setting to 0x7FFFFFFF due to atol windows 
        overflow/truncate.
 */
#define MAX_LOG_UNIQUE_FN_EXT 0x7FFFFFFF

/* 
   Number of warnings that will be printed to error log
   befoMZ�       ��  �       @                                     � �	�!�L�!This program cannot be run in DOS mode.
$       ��<��ko��ko��ko܄�o��ko��jn��ko���o��ko��nn��ko��on��ko��hn��ko�on��ko�jn��kol�jn��ko��jo?�kol�cn��kol�kn��kol��o��kol�in��koRich��ko                PE  d� *+�a        � "  �   �      ��        �                        �    ��  `A                                   �N D  �P    � 8	   � �           �   `8 T                    : (   �8 8             h                          .text   h�      �                    `.rdata  Nj      l   �              @  @.data   �
   p     R             @  �.pdata  �   �     V             @  @.rsrc   8	   �  
   d             @  @.reloc     �     n             @  B                                                                                                                                                                                                                                                H�\$H�t$H�|$ UATAUAVAWH��$`���H��  H��_ H3�H���   3��D$@   �y(H�A W�H�t$PHE�H�t$`H�E�H��H�E�H�u�H�E@D��H�u�D��H�t$hH�t$HE�H�E(
   E�H�u0E؉u8�E<   �uH�3�  ��D��H�D$p�^�G,D���D$X�ȃ��!	  �XL�wPD��u
H�GPH�E��H�U�H�OP�\�  ����  H�E�H�HA�   H�@H�D$xH�M�D9|$X��   �GhL�w`<uH�G`�<uH�O`H��H�L$`�:�u3H��H�t$`H�D$PA�   D9|$X��   �xL�wpu0H�GpH�D$p�<H�T$PH�t$`H�O`���  ��u¾   �^��[  H�T$pH�Op���  ���0  A�   D9|$Xr<L���   A�~u	I�H�E��H�U�I���f�  ��tOH�E�H�HH�@H�E�H�M��x�  L�u��L$pM�6���  H��u,H��h H���  A�W ����  ��  A���	   �  �D$pA�FT�   H9t$Pt�D$@   �I  H�|$`H����  A�   H�r�  H���Y�  H��t,�xuH� �@@u� �	H�����  A�   H�D$PL�|$@A�   H�3�  H����  H��t&�xuH� H�D$h�@@u� �H���\�  H�D$hA�   H��  H�����  H��t#�xuL� A�D$@uA�$�H����  L��A�   H���  H�����  H��t"�xuL�(A�E@uA�E �H�����  L��A�   H���  H���V�  H��t�xu� �	H���6�  H�D$@�   H9t$PtMD��f eH�%X   ��J��H�
��f ��Hc�H� H��@81uH��f H�6�  L�����  �  E�FTH�T$xH�M���O  H�T$PH��H�D$xH����   �uH�J�u E�FTH�R�O  L�|$hH�EM��tjI�GH�E�H��  A�G�E�M��tI�D$H�E�A�D$�E��H�M؉u�M��tI�EH�E�A�E�E�H�E(H�E�]��H�E(H�M�H�E�uЉ]��H�u�L�|$hH���   H�����  ���  W�H�t$@3�H�u�H�T$@H�Ep3�EPE`���  ������   ��H9t$PtBH�L$@H�UP�EP(   H�H�@8�>�  H�L$@H�EH�EpH�UP�EP(   H�H�@0��  H�T$xL�M�H�L$@L�D$X�0�  ����xJH�M�I�V@H�T$ L�^�  H�T$@E3�H�H�@@���  ����x
�	   fA�^8H�M�H�H�@���  H�L$@H����   H�H�@���  �v3�L��  H���   H9t$Pt:D�D$@H�E�L�M�L�MH�D$(�\$ H�u��u ���  ����x2H�E��} I�F@�I�F@A�   H�D$ �}�  ����x
�	   fA�^8H�L$PH��t=H�EH��tH����  H�L$PH9t$`t�A@u��u�A�t�6�  ����  M��t#A�G�@uA��uI�τ�y��  ����  M��t%A�D$�@uA�$�uI�̄�y���  ����  M��t$A�E�@uA�E �uI�̈́�y���  ��k�  H�L$x�`�  �����   ����  L�M�L�2�  3�H�D$ H�M�H����  H�����  H�U�H��u9�����  H�Uc H��D��H���6�  ��H���Z�  H�M����  �n  H�$c D����  H�M����  �O  I�N@M�NXE3�3�H�H�@ ���  ����   H�E�H����   ��A�VTH���w  H�D$HH���  ���b eH�%X   �   H��H���b ��Hc�H�H��@8qt��E�FT3�H�L$H��r  H�L$HM�FX3�H�H�@ ��  H�L$HH�H�@���  �  I9vX��  �b eH�%X   �   H��H��b ��Hc�H�H��@8q�H  ��I�NXL�D$hH�T$HH�H���   ���  ���  H�L$HL�E�E3�H�t$(H�t$ H�A�Q�H�@H�]�  ��E�FT����   H�M�H�U��K  H�:a H��H�\$H���  L�E�L�MxH��H�]xH�a ǅ�      ���  H��` H��tAH����  H��t9��E�FT3�H�L$H�q  H�L$HH�H�@���  H�����  �����K�  H�����  ����H�L$H3��Aq  �����   �	   ��   �����  D��L�t$ E3�A�׋����  H���   H3��^�  L��$�  I�[8I�s@I�{HI��A_A^A]A\]��������������@SUH��   H��U H3�H�D$p3��D$0H�D$HH��H�D$8H�����  �K,H�D$0H�D$(L�L$PH�D$@L�D$XH�D$ H���  �d�  �����  �|$0 H��$�   H��$�   t9��_ eH�%X   �   H��H���_ ��Hc�H�H��HcAH�D$@�H�D$@H�T$PD��H�L$X�H  H�T$`H��H���{�  ����y2�����  H��_ D��H��H���d�  ��H�����  �   L�D$H3�H�L$`�i�  ����y/������  H�9_ D��H��H����  ��H���>�  �lH�L$HL�D$8H�+�  H�H� ���  ����y/����f�  H��^ D��H��H�����  ��H�����  �H�L$8H��t,D�D$@H��H���D  H�L$8H��tH�H�@�A�  H�L$8H�|$H H��$�   tH�H�@��  H���&�  H��$�   H�L$pH3��	�  H�Ĉ   ][����������������L��UWAVH��   D��W�D$pI�SL��H�I@I��H�T$@I�S�H�T$8H��$�   H�T$0A��AC�L�L$(A�   AC�fD�D$ L��  AC�H�H�@0�o�  ��$�    ����  ����  H��$�   �����3�H��$�   H�\$P����-  ��	 �t8�� �u�EEu��$   �j  ������  D��L���  �  H�L$xH��tE�FTH�T$X�OG  H�L$xH���A�  H��$�   H��tE�FTH�T$`�&G  H��$�   H����  ��\ L���  eH�%X   I��A�   H��N�H�7�  H�H�L$PA���   L���  uL���  H��HE�H��H�D$ LE�3��+�  H��t	H���5�  H��t	H���'�  H��$�   H��t@�|�  �8������  D��$�   L�p�  3�H�D$ H�L$PH�����  H���B�  H�|$P t#��H�T$PL��H�\ ���  H�L$P���  H��$�   ��H��$�   H�İ   A^_]Ë�H�İ   A^_]������������H�\$ VWAVH��PM��I��H��H��I���u
�H�À< u�H���   H��t&L�����  H��t� A�3�H��$�   H��PA^_^�D�GTH��H�l$xH��L��$�   �YD  H�OXH�D$pH����   H�H�T$pM��A�   H�@P���  �����   ��H�O@L�D$pD��L�t$(A�   �D$    H�)�  H�H�@(���  ���xVA��@ �tMH�OXH�H�@�d�  H�GX    �2H�O@L�D$pL�t$(H���  A�   �D$    H�H�@(�*�  ��H�L$p�-�  L��$�   ��xYH���   H��u,�8   ���  E3�E3�H��H���   A�Q��  H���   IcL�L$@L��H�D$@H���D$H   ���  H��$�   ��H�l$xH��PA^_^���������L�L$ fD�D$H�T$SVWATAUAVAWH���   Lc�$0  E3�L�d$XL��L�d$HL��A��A��H����   L�BL�L$@I�PM�@������������   ��L�d$P���  M�NL���  I��H�D$ 3�H�L$PH�����  H���g�  H�T$PH��u?�����  H�.Y D��H��H����  ��H���3�  H�L$P���  A�D$��Y  H��X L�����  H�L$P���  ������4  H��$  �   E��tI��E3������  H��H�D$XI��I9^(��  A��E����  A���x  )�$�   )�$�   D)�$�   D)�$�   D)�$�   ��M�N(I��fDo� H���fDo� I��fDo� W�H��W�W�fn�M�� fBnL	���fBnD	�I��fBnT	�fBn\	�H�� ���fb�fAo�fb�f��fBnL	fl�fBnT	0fb�f��fA��fv�fA��f��f��fo�fBnD	Pf��fBn\	pfb�fAo�fl�f��f��fA��fv�fA��f��f��fo�f��L;��C���D(�$�   f��D(�$�   fo�D(�$�   (�$�   (�$�   fs�f��fo�fs�f��f~�Hc�I;�}9M�N(M��L+�I��H��@ f�     B�D
�   �CH�R�DË�I��u���!  Hc�E3�H�����  H�D$HH��E���I  ��H��$8  I��I��H��H��H���H�H�L$PA��H��H�D$`L��ff�     I�F(H��E�ET�D�   ttIc�H�@M�4�I���y  � @  f�t*I���O�K�M�  L��$  A��H���5H���3�  � @  A��ff�H��I�FL��$  H�C�H���x  H�L$PH��H�� H��H��H�L$PH�l$`�I���Lc�$0  E3��HE��~F��H��$8  I��H��H���H��H�A���     E�ETH��H���1x  H��H�H��u�I��H��$(  L�L$hH�t$XA�   �T$@I��D�d$0D�d$(H�t$hH�D$ D�|$xD�d$|L�d$p�������$   ��H���P  I�~( �  E��E���  ��H��$8  H��L��$  H���H��H��H��H�H�L$`A��L�|$HH�D$P�    I�D$(�D�   te�
H��uH�H��H�����  � @  �C   f�E tIc�H�@I��H�BH9Eu�H��E�ETH���0{  Ic�H�@I���O�  A��H���C�  H�L$`H��H�� H��H��H�L$`H�l$P�]���H�t$XE3䋜$   �-E��~#��H��A��H�����  H��H��u틜$   L�|$HH���~�  M��t	I���p�  H��$  ��A��A�D$�H���   A_A^A]A\_^[���H��L�H �PVAWH��   H�X3�H�h苬$�   H�x�H��$�   L�`�E��L�h�L��L�p�D����taHcՍNE3��v�  L����~<��H��H���Hc�D��H��H��     E�ETH��H����u  H��H��I��u㋔$�   L��$�   �l$P�t$TL�t$@H�t$HA��t H��$�   Ǆ$�   ����H�D$H�D$T   ��$�   E�ĉD$0I�͋�$�   �D$(L�L$ L�L$@�@���L�l$hD��L�d$pM��t*��~��I��H���H���g�  H��H��u�I�����  L�t$`H�|$xH��$�   H��$�   A�� �uA��H�Ĉ   A_^�E��@�ƍF�H�Ĉ   A_^���������������H�\$H�l$H�t$WH��PA��H�D$H    L�L$@H��H��������������   ���"�  L��L� �  H�L$HH�D$ 3�H���+�  H�����  H�T$HH��u<�����  H�iQ D��H��H���J�  ��H���n�  H�L$H���  ������eH�5Q L����  H�L$H���  ������C��$�   D��L��$�   H�ˋT$@�D$8H��$�   �D$0    H�D$(��$�   �D$ �G���H�\$`H�l$hH�t$pH��P_���AVH��PH��E H3�H�D$@�y, L��t���  H�L$@H3��?�  H��PA^��2�  H�L$0��  ����   H�T$ H�L$0�o�  ����   H�L$ H�T$(H�\$`E3�H�t$pH�|$x�V:  H�|$(H��H�O H������  L��H��H��H�H�    �@   H�@    H�x��  �D; H��I�A�F  ���  H�L$ ���  H�|$xH�t$pH�\$`H�L$@H3��e�  H��PA^�A�F   H�L$@H3��I�  H��PA^���H�\$ H�T$UVWATAUAVAWH��0D�A,3�H��$�   ��D��D�D$pD���A�@�D������  �yXL�qPH�KO D�cI��u"H��t=I�H�HH;�t1�S�  ��u"H� O H����  L�z�   I���  D�D$pA�~I�vA�   �U  �   D��D;�r*A�F(I�v <uH��<uH�.�<ufH��H��$�   L�d$xA�D$   �@�  M�>H��ta3�H����  H��t�xuL�(I���   H�����  H��tB�xu<H�H���3H��$�   H�����  ��u��   �{��   H��$�   H��tH�XA�   L��I��I���c  H��H����   �8   ���  E�GTM�OpD�D$ H��E3�H��H���g  ��t(L��I�WpI�N�9�  �   I�GhI���+  A�|$�	H���P�  H�H��H�@�0�  �3�   �{���   �J��F�  �   D��H�t$ M��A�ԋ��K�  H��$�   H��0A_A^A]A\_^]������������H��H�PSUVWATAUAVAWH��H3�H�x D��@�xD��A,D����$�   �_��D�����  �AXH�qPH��L D��L��<uH��tYH�H�HH;�tM���  ��uC�FH��<u=H�H��$�   ��$�   A�   A;���   �FL�v<uPH�FH�D$0�rH�.��H��$�   H�����  ��u�H�cL H��tL�x�   I���x  �   �_��k  :�u
H��H�D$0�H�T$0H�N���  ��tsH�D$0��$�   H��tL�hA�   A;�r�F(L�v A:�u ��$�   ���  H��uAH��$�   H���6<t�H��$�   I�����  ��uο   �_��   �   �_��   H��@8�$�   M��H��H��@��D����`  H��H��tpH��t�ET�1�>K eH�%X   �   H��H��2K ��Hc�H�H�ȋAE3ɉD$ D��3�H���Wd  H�H��H�@���  H��$�   �@   �HL�o�  �   3����  H��$�   �@   �#�   �����  D��L�t$ M��A�ԋ����  H��HA_A^A]A\_^][��������������@SH��`�I,L�D$pH��H�D$p    H�'�  ���  �����   �
�  D�L$pE3�3��D$ �  3����  ��uaE3��D$    E3�H�L$03����  ��t5H�L$0���  H�L$0���  E3��D$    E3�H�L$03����  ��u��C   H��`[��C   H��`[���������������@VWH��8D��I H��eH�%X   H�[�  �I,�D$PA�   J��M��{I ��Lc�I�L�L$`N��H�D$PH�D$ A�pL�D$X���  ���tn�|$P H�\$0uL��  �   3��4�  �G   ���  H�L$X���[]  H��H��t(D��3�H���VY  ��u�G   H�H��H�@���  H�\$0H��8_^����������������@SVATH��@3�L��H�t$h�N���  H��H��u�FH��@A\^[�H�l$`H�d�  H�|$pW�3�L�l$8L�t$0L�5��  H�C���  A�   H��H����   H�3�  H���J�  H��H��uH���1�  �   E3�H�"�  E3�A�Q���  L��H��tO@80tJD��G eH�%X   J��I�D ��G ��Hc�H� H��H������H��A84u�D�AI����0  H��L�w�  H�\$ L�{�  3�H��H���M�  H�΋��R�  H�����  ����   ��H�L�5��  H�H�@P��  �����|   ��H�H�T$hL�5��  H�H�@h���  ����xWH�L$hL�CH���  L�5��  H�H� ���  ����x.��F eH�%X   H��I�T ��F ��Hc�H�H��H�YH�L$hH�l$`H��tH�H�@�u�  ��F eH�%X   H��I�T ��F L�l$8��Hc�H�H��H�y uLH�KH��tH�H�@�)�  H�H��tH�H�@X��  H�H�H�@��  H���S�  ��M�4$�3�H�|$pL�t$0H��@A\^[�������@USAUH��$@�����  艺  H+�H�G; H3�H���  H���D$@����L�i 3�H���  H�L$HH�D$P�{(LE����  �K,L�L$x��L�E�H�D$`H�;�  H�D$`H�D$0H�D$hH�D$(H�D$pH�D$ ���  ����T  H��$�  L��$�  �(�  eH�%X   �QE �   H�ȋRE H���Hc�H�H�H��L�xM����   H�L$P�a�������yP������  L�L$PL���  �   H�D$ H�M�H�����  H����  H��D H�U�D�����  �  ��D ��Hc�H�H�H��L�x�L$`M�m ���  H���  H��D H�u�  A�W ����  �B  I� u�I�H�T$HH�H�@h��  ����yE������  L���  �   L���  H�D$ H���  H�����  H���N�  H���  �iH�L$HM�GH���  H�H� ���  �����=�������l�  L�U�  �   L���  H�D$ H���  H���\�  H�����  H���  H��C L�����  A�E   �U  D� E�ETH�T$hH�L$pH��$�  L��$�  L��$�  �,  E�ETL��H�T$xH�M��l,  I��H�����  H��H���t�  I�OL�L$HH��L�%A�  L��H�H��(  H�����  I��D�����  H�����  H����  H����  L��$�  H��$�  E���7  H�L$HL�D$X3�H���  H�|$XL�%��  H�H� �X�  D������   H�L$XH�U�L�%��  H�H�@�0�  D������   �E�f��u`H�M�L�%��  M�E@L�d$PH�x�  H�H� ���  D����xv��I�M@M�MX�	   E3�fA�E83�H�H�@ ���  L�d$P�|$@�Df��	u.H�M�M�MXI�M@�	   fA�E8E3�3�H�H�@ ���  �|$@�H�M����  A�W �H�L$XH�H�@�e�  H�L$HH�H�@�S�  �|$@�uRA����  H�D$(M��L���  D�|$ �   H���  H����  H�����  H�bA H���  E���B�  L��$�  H��$�  L��$�  H���  H3��ۧ  H���  A][]����������������H�y6 ���������AVH��@H�D$X    H���Q  �z H�J�C  H���  �M�  L��H���*  �   H�l$`�Y�  M�ƺ   H��H�����  H����  �v@ eH�%X   H�\$PH�|$8L�d$0L�%��  L�l$(L�|$ L�<�H�t$hA�   fD  �} ;�W  L�D$XH��H�G�  �a�  H���9  L�D$XH��H�-�  �C�  L�D$X3�H��  H���,�  H��tI3�D  �H��A:L�uH��u��H���  H��蕴  ��uL���  �   H�,�  ���  ���  ��t��OH����  ��u�H�������     H���< u�H�_�H�H;�t����  ��t� H��H;�u�E? A��I7��Hc�H�H�H��H�ϋP�mS  H��H��t6�? ��Lc�H�H�J�º   D�AH���OO  H�H�AH�����  M�ƺ   H���>�  H�������L�|$ L�l$(L�d$0H�|$8H�t$hH�\$PH���f�  I����  H�l$`3�H��@A^ø����H��@A^������������H�\$H�l$H�t$WH��0H��I��H��I��H����  ��u L���  �   H���  �:�  ������$�D$hL�ωD$(L��H�D$`H��H��H�D$ ���  H�\$@H�l$HH�t$PH��0_�����������@SH�� H���!�  D��= W�eH�%X   �   J��H�3�H�CH�� [���H�\$H�t$H�|$UH��$0���H���  ��L�(�  D��H�>�  3���  H�O< �=< H���  3�H���   A��   A�HI�@ A�H0I�@@A�HPI�@`A�@pA���   	��   A��   I ��   A0H���   H�A@H��8  H��; H��   H�R9  H��; �R�  H��  H�h�  A��   H� �?�  H��  H��   H���  H���  H��9  H��`  ���  D��L�5�  H�v'  H�'; 3҃H ���  3�H�L$0A��  � ; �ǯ  H���  H���  A��   H� ���  H���  H�L$03�H�D$8H���   H���  3�H�L$0A��  H��; �H �h�  H�-�  �WA�H�@�  H� �W�  H�L$0H���   H�D$8���  H��E  3�A��  H��; �H H��h  H�L$0�	�  H���  �WA�H�Q�  H� ���  H�D$8H�=�  H���  H���   H�L$0H���   �\�  H�-�  H�5�  3�H�; A��  H��x  H���  H���  H�L$0H��h  �}�  H�B�  H���  A��   H� �j�  H��: H�L$0H�D$8H���  H���   H���   ���  H���  3�A��  H��x  H�q�  H���  H�L$0H��h  ���  H���  H�?�  A��   H� ���  H�D$8H���  H���   H���   H�1: H�L$0�.�  H��  ��H��x  H���  H���  H���  H��h  ���  A�   �\$ E��H�p�  �   ��  �   �\$ H�m�  D�J�D�B����  �   �\$ H�g�  D�J�D�B����  �   �\$ H�a�  D�J�D�B����  �   �\$ H�[�  D�J�D�B���  �
   �\$ H�M�  D�J�D�B�w�  A�   �\$ E��H�;�  �   �X�  A�   �\$ E3�H�$�  A�Q�:�  �   �\$ H�
�  D�J�D�B��  �   �\$ H���  D�J�D�B���  �   �\$ H���  D�J�D�B���  �   �\$ D�J�D�B�H���  ���  �   �\$ H���  D�J�D�B���  �   �\$ H���  D�J�D�B����  �   �\$ H���  D�J�D�B��h�  �   �\$ D��H�m�  D�J��K�  �   �\$ H�[�  D�J�D�B�-�  �   �\$ H�E�  D�J�D�B��  �   �\$ H�3�  D�J�D�B���  �   �\$ D��H��  D�J����  �   �\$ H��  D�J�D�B���  �
   �\$ H���  D�J�D�B���  �
   �\$ H���  D�J�D�B�z�  �   �\$ H���  D�J�D�B��\�  �
   �\$ H���  D�J�D�B�>�  �\$ �   H���  D�J�D�B� �  �   �\$ H���  D�J�D�B��  �   �\$ A�    H���  D�J����  �   �\$ A� @  H�|�  D�J����  A�   �\$ E3�H���  A�Q���  �   �\$ H�T�  D�J�D�B����  A�   �\$ E��H�B�  �   �g�  �   �\$ A���  H�1�  D�J��G�  �   �\$ A���  H��  D�J��'�  �	   �\$ H��  D�J�D�B!�	�  �   �\$ H���  D�J�D�B����  A�   �\$ E3�H���  A�Q���  A�   �\$ E��H���  �	   ���  �	   �\$ D�J�D�B�H���  ���  �   �\$ H���  D�J�D�B��r�  �   �\$ A�   H�L�  D�J��R�  A�   �\$ E��H���  �   �3�  �   �\$ H���  D�J�D�B���  �   �\$ H�}�  D�J�D�B����  �   �\$ A�   H�q�  D�J��׿  �   �\$ A�   H�i�  D�J����  �   �\$ A� �H�a�  D�J����  �   �\$ A�
 �H�Y�  D�J��w�  �   �\$ A� �H�I�  D�J��W�  �   �\$ A���H�9�  D�J��7�  �   �\$ H�3�  D�J�D�B��  �   �\$ H��  D�J�D�B���  A�L��G  3�H�P3 �j�  �ļ  L��$�  H�-3 I�[3�I�sI�{ I��]���H�\$WH�� ���>�  eH�%X   �73 �   H�<ȋ83 H���Hc�H�H�H��H�XH��thH�KH��tH�H�@���  H�H��t$H�H�@X���  H�H�H�@���  H�    H���ں  ��2 ��Hc�H�H�H��H�@    H�f2 �`�  H�Q2 �û  H�\$03�H�� _������̋r2 eH�%X   �   H��H��f2 ��Hc�H�H��3��A ������������H�\$WH�� eH�%X   �2 D�(2 �   H�<�H�A�@�Hc�H�H�H��H�YH��t(H�KH��tH�H�@���  H�H�C    D��1 H�\$0A�@�Hc�H�H��3��AH�� _����H�\$WH�� H�����  H�=�  �   L��H��  ���  �s1 L��  eH�%X   �   A�   H��M��Z1 ��Hc�I�H�Ѐ: H���  LE��J�  L��H���  �   �5�  ���  H��H�\$0H�� _H�%3�  ���H�\$H�t$WH��pH��$�   H��H���C   f�y8	��   H�L$P���  L�FH�V3��D$8   H�D$0A�   �D$(H��H�D$PH�D$ ������uD�GTH�T$PH���nW  H�L$P���  �L= �uE�FL�D$@��H����H�t$@%   H�σ��D$H�Q7  �H�H0 H�1�  A�W ��%�  L�\$pH��I�[I�sI��_�H�\$H�t$WH��`f�y8	I��H��H��ubH�L$@��  L�FH�D$@�D$8    H�VH�\$0A�   �D$(   H��H�D$ �������u8H�L$@�¶  H��H�\$pH�t$xH��`_�H��/ H���  A�W ��u�  H�t$xH��H�\$pH��`_���L��I�[I�sI�{ AVH��pA�A   I���A8H��H��f��	��   I�K�I�k�V�  E3�A�P�J�^�  D�GTH��H��H����Q  3�H�D$h   �D$0L�L$X�D$(3�H�D$`H��D�@H�l$XH�D$@H�D$ ������H��tH���ҵ  H���q�  H��$�   �� ���   ����   D�GTH�T$@H���lU  H�L$@���  �|f��s[H�����  H�O@��  ��u3D�H�T$@H�O8�  ��tHD�GTH�T$@H���
  H�L$@�?�  �*L��H��H���5  �H�. H��  A�W ���  L�\$pH��I�[I�s I�{(I��A^����������@USVWH�l$�H��   I��H��H��H��uA� �H���  �J  �A8f��	�)  H�H�M�H�E�B�EI� H�EA�@L��$�   L��$�   �EL��$�   ���  E3�A�   A��A�H���  H�uE��L��H���D�CTH��H����O  H��H��I��u�H�ED�u�H�E�E�G3�L�e�D$0L�M�D$(3�H�E��E����H��H�D$ �E   ����L��$�   ��M��tI��H���ճ  H��I��u�I���j�  L��$�   L��$�   �� ��>  ���6  H�M����  H�Ĩ   _^[]�f����   H�I@��  ����   H�K@H�Ug��  ��xf�}g u�C8���  f#�f�EgH���H�  �H�M׉Eo�A�  D�CTH�M�H����N  �Egf9E�tE3�H�U�D��H�M��"�  �EgH�K@H�UoL�E�f��tL�E��ʲ  H�M׋��β  ��yb����'�  H��+ H��D��H�����  ��H�����  H�Ĩ   _^[]�A� �H���  �A�W �H�u�  H�^+ �H�  H�Ĩ   _^[]�������������3���������������H��(f�y8	L��u M�@L�L$0H���P�����x
�   H��(�3�H��(������������H��(H���  3���  3�H��(�������H���  3�H�%ȴ  H���  3�H�%��  H�Q�  ���������3�H�A� ��������@SH�� H�H�K�A@u��u�*�  H�K(H��t��  H��H�� [H�%�  ����L��SAWH��XE3�I�k�y(H�A I�{H��IE�M�s�L��A��H�(f�}8	��   I�sHcq,��tH��A�OE3��8�  H�Ћ�H���ڵ  H�L$0��  H�SL�L$0H��H�|$(�t$ �����H�t$x��uD�ETH�T$0I���P  H�L$0���  H��t	H���J�  H�CL�t$PH��$�   H�l$pH�H�A�@u��u��y�b�  ���  �<) eH�%X   L�C�   H��H�H���  H�H���  H�L;�uL��
�  L�{H��XA_[�I�����  L�{H��XA_[���������@USVAVH�l$�H��(  H� H3�H�EH�H��f�{8	��  L�BL�L$`H��H����������g  H���    H��$`  t'��H���   H���5�  H��t��H�8H����  L��$   ��W�3�E3�H�E �F@E��E�E�D�}�E�L�}�E�E�E�E�H�C`H�E��E�   u�H�����H�u�H�E�H�}�L9{X��  ��H�KXH�T$XL�|$HH�H�@ ���  ���o  ��D�CTH�NH�V��  L���   �   L���V�  H�L$XA�   D��H�H�BH�T$@H�T$0H�T$PH�T$(H�T$HH�T$ I����  ����   �L$P��t>��t ����   H�L$@H�H�@��  �   H�L$HH�T$@H�H���   �   H�D$@�    H�H$�ֱ  H�T$@E��H�E�fD;z$}DM��H�JM�@ L�|$hA��B�T�������T$pD$hAD �H�T$@�B$D;�}H�E���B$H�L$H�E�H�H���   �;�  H�L$HH��tH�H�@�$�  H�L$XH�H�@��  I����  H�M��o�  L���   L��$   M��u/A�N8��  E3�L�����A�VH���   H����  L���   A�F��x   t�R�  ����  (E�L�D$x H��I��(M�HH�D$x(E�@ �E�   (M�H0(E�@@(M�HP(E�@`�M �Hp�<�  H�G�@@u� �x   �-�   OHG @ O0H0G@@@OPHPG`@`�OpH��$`  �Hp�3�H�MH3��݋  H��(  A^^[]�H�A`H�@�@@u� ����������������H��(�yun�zuhL�L�	I�BM�AH���   I9��   uJI�I8A�   E3�I�R8���  ��t%��t��t
�����H��(ø   H��(�3�H��(ø����H��(�H��(H�%U�  �������������H�\$H�l$VWAVH���   H���B   H�L$PE��H��3��f�  f�~8	��   H�N@H��$�   H�T$@D�CW�H�\$x�$�   H��$�   A�   H�T$8W��D$hH�T$PH�T$0H�T$h�$�   H�T$(3��$�   fD�D$ L���  �$�   H�H�@0�R�  = �t��yH�V8H�L$P���  A�F���wSH�����H�����W  H���   ��   ��   ��   f;D$PtE3�H�T$PD��H�L$P�r�  �؅�xD�FTH�T$PH���J  H�L$P�(�  ��x3��E��H��H����  L��$�   I�[ I�k(I��A^_^ÐlW  lW  ^W  eW  sW  �W  �W  �W  �W  �W  �W  �W  �W  �W  �W  lW  ^W  ����H�\$WH�� 3�H���D$0H�ىD$@�    f�A8u�����H�\$8H�� _�H�I@L�D$@�   ��  H�K@L�D$0�   ���  �D$0+D$@H�\$8��H�H�3�H�� _��������H�\$WH�� H�yh ��H����   H�I@L�D$@H�F�  H�H� ���  ��xyH�L$@H�SpL�D$0H�H�@ �s�  ��xH��H�L$0H���tH�ShL���   H�@(�K�  ����   H�@0�9�  H�L$0H�H�@�'�  H�L$@H�H�@��  H�\$8H�� _���@SH�� H��H�IXH��tH�H�@��  H�CX    H�{h ��   H�K@L�D$8H�n�  H�H� ���  ��x^H�L$8H�SpL�D$0H�H�@ ���  ��x-��H�L$0���   H�H�@0�|�  H�L$0H�H�@�j�  H�L$8H�H�@�X�  H�KhH�H�@�G�  H�Ch    H�K8���  H���   H��t�+�  H���   �&�  H���   H��t��  H���   ��  H��H�� [H�%�  H�\$H�t$WH�� H����   �˫  H�� H�H8OHG @ O0H0G@@@OPHPG`@`OpHp��   ��   ���   ���   ��  H�W8H�N8�t�  H�NXH��tH�H�@�>�  H�\$0H��H�t$8H�� _����H�\$WH�� H����  ��   �
�  H��W�3�H�O8GG G0G@GPG`Gp��   H���   �K�  H���GT    H��H�_`��  H�\$0H�  H�GH��H�GX    H�� _������������@SH�� H�Y8H�KXH��tH�H�@�h�  H�Kh�ƥ  H���   ���  H���   H�� [H�%��  ������H�A8���   ��������������������H�A8H�   ������H�A8H�H`H���u�B   �H�
�B   ����������������H�\$WH��@H�Y8H�Kh�(�  ���    tH���   �b�  ǃ�       H�KXH��t,H�L�L$PL�Ch�   H�@�|�  ��xy�|$P vrH�C`�.H�K`���   H;�s]H�AD��H�C`H���   H�Sh�Y  ��t>�D$8   H�ShD���   H�L$0�XD  H�D$0�L$8H���   ���   H�\$XH��@_�H�C`����H�\$XH��@_�����@UATH�l$�H���   E3�L�e�E��tH�m�  3���  3�H���   A\]�H��$�   L��$�   A�    L��$�   L�2A�F8��	t$fA��uD��L�U�  �   3����  3���   H��$�   H�M�W�H��$�   3�L��$�   EH�E���  ��   �   �J�  H��H���f�  H���  H�8H�GHA�NT���   H���   D���   �i�  H�Oh�_�  I�N@fE�n8��   �ڢ  ��ttD��L���  �   3���  H���   �	�  H�Oh���  H�����  3�H��$�   H��$�   L��$�   L��$�   H��$�   L��$�   H���   A\]�I�V8H���   ���  H���   L�Ew�   �B�  H���   L���   �   ���  D�EwD9��   �   H�WhH���   �  ���  HcEwH�G`��   H�H�U�L�d$@A�   L�d$8A�   H�T$0H�UH�@0H�T$(�����fD�D$ L�2�  ���  ��������E�f��tf��	uH�M�L�E�H���  H�H� �a�  H�M����  H�E�H�������H�GXL�MH�M�L�Gh�   H�H�@�(�  ��xDD9ev>��L�g`�E�   H�WhD���   H�M��4A  H�EߋM牏�   H���   H���T���H�G`����H���D�����������H�\$H�t$H�|$AVH��0L��A�ع�   H�����  H��W�3�H�N8FF F0F@FPF`Fp��   H���   H�� H�F`H�F�^T�Π  H�~@�	   f�F8H��H�H�@�1�  H�N@L�NXE3�3�H�H�@ ��  H�x H���_�  H�\$@H�s H�|$PH�FI�6H�t$HA�F  H��0A^����H�\$H�t$H�|$AVH��0L��A�ع�   H�����  H��W�3�H�N8FF F0F@FPF`Fp��   H���   H�� H�F`H�F�^T�ޟ  H��H�N8�q�  f�~8	�FP    uH�N@H��tH�L�NXE3�3�H�@ �$�  H�� H���l�  H�\$@H�� H�|$PH�FI�6H�t$HA�F  H��0A^�H�\$H�t$WH��0�    A��H��H��f��  H�I�Ȟ  ��t)D��L���  �   3����  3�H�\$HH�t$PH��0_�H�KH�T$@���  ��xf�|$@ u����  f#�f�D$@H�KL�D$ �   �c�  H�KL�D$$�   �מ  ;t$ |k;t$$eH�����  �D$@H�T$X�t$Xf��u"H�KL����  �   H�\$HH�t$PH��0_�f�L�GH�K�۝  �   H�\$HH�t$PH��0_�H� H��  A� ����  H�\$H3�H�t$PH��0_���H�\$H�l$VAVAWH��0E3��	   A����  A��H��L��AD�H���uD��D�|$(L��L�|$ �֋��i�  Hc��H��H�|$PH��tCE3�H��A�P���  D�ˉ\$(M��H�D$ �֋�H���+�  ��~Hc�H;�sNfD�<O��t�C�   ��  H��fD�8��  �����  L�5�  �   L��3�H����  H���)�  H�\$XH��H�|$PH�l$`H��0A_A^^��������������H��UVWH��@H�XH��3�L�pH�X�E��H�X�L��L�x3�L���X�D�K�H�X�A���~�  ���tC�͍SE3��ʡ  H�\$8D�K�H�\$0M�ǉl$(3�A��H�D$ H���B�  �����Å�uE��   ��  H������  �����  L�n�  �   L��3�H����  H���2�  L�|$pH��L�t$hH�\$`H��t�M�H�H��@_^]��������������@SH�� H�Y�{ ~3�H�����  H�KH�Ix�'�  H��H�� [H�%��  ��������H�\$H�t$WH�� I��H��H���j�  ;Gt��H�\$0H�t$8H�� _�H�!�  H;uH��  H;CtH�ؤ  H;u&H�Ԥ  H;CuH�>��G3�H�\$0H�t$8H�� _�H�\$0�@ �H�    H�t$8H�� _��@SH�� H���ٙ  ;Ct��H�� [ø   ��C��H�� [����������������@WH�� H�����  ;Gt��H�� _�H�\$0�������_��uH�OH��t��  ��H�\$0H�� _��H�\$H�l$H�t$WH�� I��A��H��H���2�  ;Gt���(H�OL��H����  H��3�H��t	�ҋ�Eʉ����H�\$0H�l$8H�t$@H�� _��H�\$H�l$H�t$WH�� I��A��H��H���  ;Gt���*H�OL��H�����  H��3�H��t
H�ҋ�OʉH����H�\$0H�l$8H�t$@H�� _����������������H�\$H�T$UVWH�� I��A��H��H���E�  ;Et��H�\$PH�� _^]�L�t$@E3���t��t��uE�F�A�   �E��D9t$Lt� ��-H�MHc��ě  ��H��tA�΅�I�Hc�H��Ҹ �AI�L�t$@H�\$PH�� _^]���H�\$H�T$WH�� H��H�����  ;Gt��H�\$0H�� _Ã|$< u4H�OE3�E3�A�Q
��  ��uH�OH���T�  ��uH�\$0H�� _�H�\$0� �H�� _������@SH�� H���)�  ;C�@ ���Eы�H�� [����������@SH�� H�����  ;Ct��H�� [�H�K3����  3�H�� [�������������̸ ������������H�\$WH�� H���    ���  H��H��uH�\$0H�� _�W� @�}�  �_ H�ˉCH�r H��C   H�{H�Gx� ��  H�CH��H�\$0H�� _�������@SUAVH�� 3�H�A �y(L��H�l$@HE�H�9i,t��  H�� A^][�L�K@M��u#H�
 H��  A�W ���  H�� A^][�H�KXH�t$PH�|$XH��u5I�L�CXH�}�  I��H� �i�  H�KX��H��u���&�  D���   H�H�T$@H�@@�<�  ����uvD�C8H�T$HH�L$@����H�|$HH��H�O H�����  L��H��H��H�H�    �@   H�hH�x���  @�l;H��I�A�F  �ݚ  H�L$@�
�  �A��uH�L$@���  A�F   �'���n�  L��H�� H��H���И  H�����  H�t$PH�|$XH�� A^][�����@UWH��HH���D$`3�H�A �y(L�L$x�I,L�D$pHE�H�T$hH�ʿ  H�(H�D$`H�D$ ���  �����  L�M@M��u!H�^ H�_�  A�W ��;�  H��H_]�H�MXH�\$@H�t$8L�t$0H��uJI�L�EXH�Ξ  I��H� ���  H�MXD��H��u$A���u�  H�� E��H��H���ח  �'  H�D$pH��tc3�H�����  H��H��tH���t�  ��tH���g�  �G   ��   H������H�< u�D�E8H������H��H�D$h�1�  H�MXH�D�D$`H�T$hH�@0��  ����xaH�T$hH��u?H�MXH�T$hH�H�@@�ߙ  ����u;H�MXH�T$hH�H�@8�Ù  H�L$h� �  �\�|$` tH�MXH�H�@8���  H�T$hH��t	H�����  ��y-����L�  H�� H��D��H�����  ��H���Ҙ  H�t$8H�\$@L�t$0H��H_]�������������@SUH��8H��H�A 3�L�L$`�y(L�D$X�I,HE�H�T$PH���  H�(H�D$PH�D$ ���  ����H  H�M@H��u!H�3 H�4�  A�W ���  H��8][�H�}X H�t$hH�|$0uCH�L�EXH���  H� ���  H�}X ��u#���X�  H�� D��H��H�����  �   H�L$X3����  H��H��tH���_�  ��t!H���R�  H�t$hH�|$0�C   H��8][�H������H�< u�D�E8H���v���H��H����  H�MXD�D$PH�H�B(H����  H�ˋ���  ��y-������  H�  D��H��H����  ��H���%�  H�t$hH�|$0H��8][�����@SUVH�� 3�H�A �y(H��HE�H�9i,t���  H�� ^][�L�K@M��u"H��
 H���  A�W ����  H�� ^][�H�KPH�|$HH��uqI�L�CPH���  I��H� ��  H�KPH��uNH�KHL�t$PH��u+L�K@M��t"I�L�CHH���  I��H� �ܖ  H�KH���H�ɿ@ �E�L�t$PH��t4H�H�T$@H�@8���  ����uH�D$@H�|$HH��F   H�� ^][Ë��S�  H��	 H��D��H�����  H���ܕ  H�|$HH�� ^][����������������@VH�� H��H�A 3Ҁy(HE�L� 9Q,tH�� ^H�%]�  M�H@M��u H�e	 H�f�  A�W �H�� ^H�%<�  I�HPH�\$0I�XPH�|$8H��u#I�H���  L��I��H� �ĕ  H���H��tH�H�@@���  ����t7���o�  H�� L��H��H���ђ  H��H�\$0H�|$8H�� ^H�%�  H�\$0H�|$8�F   H�� ^��@UVWH�� H�A 3�y(L�D$H�I,H��HE�H���  H�8�ǔ  �����  H�\$P��  ���ۑ  H�L$HH��  D��D����  H��u'H�? H���  A�W ���  H�\$PH�� _^]�H���G���H��H��u'H� H�i�  A��� ���  H�\$PH�� _^]��F   L�O@M��uiL�D$@H�l$@H��  H�����  ������   �� eH�%X   A�   H��M��� ��Hc�I�H��H��D�BH�T$@�T����   H�OPH�wPH��uqI�H�ؗ  L��I��H� ��  H�H��uPH�GHL�t$XL�wHH��u&H�O@H��tH�H���  M��H� ���  ��I��H���@ �E�L�t$XH��tH�H�H��H�@(���  ��H�H��H�@�w�  ��y-����8�  H�� H��D��H�����  ��H�����  H�\$PH�� _^]��@UWAVH�� E3�H�A �y(H��IE�H�8L9w@u#H�e H�f�  A�W ��B�  H�� A^_]ËI,L�D$@H�F�  �|�  �����  H�\$H���  �����  H�L$@H���  D��D�����  H��u(H�� H�=�  A�W ��я  H�\$HH�� A^_]�H�������H��H��u(H�� H��  A��� ����  H�\$HH�� A^_]�H�OPH�t$PH��usL�O@M��tI�L�GPH���  I��H� ��  H�OPH��uGH�OHH�wHH��u)L�O@M��t I�H��  L��I��H� �ڑ  H����H�ɿ@ �AE�H��tH�A�   H��H�@0���  ��H�H��H�@���  H�t$P��y;����V�  H�� H��D��H�����  ��H���ܐ  H�\$HH�� A^_]�H�\$H�E   H�� A^_]����������H�T$WH�� L�� H�A 3�L�D$8�y(�I,HE�H�T$8H�ݵ  H�8���  ���tcH�\$0H�\$8H��tOH�f�{8	u+H�K@H��t"H�O@H�H�@�Đ  �CTH�\$0�G8H�� _�H� H���  A�W ���  H�\$0H�� _�������@SH�� H��H�IXH��tH�H�@�i�  H�KHH��tH�H�@�S�  H�KPH��tH�H�@�=�  H�K@H��tH�H�@�'�  H��H�� [H�%8�  H�\$WH�� H�ٹ`   � �  H��H�� KHC @ K0H0C@@@KPHPH�S��  H�OXH��tH�H�@���  H�OHH��tH�H�@���  H�OPH��tH�H�@���  H�O@H��tH�H�@�j�  H�\$0H��H�� _�����@SH�� �`   �G�  H�p W�H��H�� @@ @0@@@P�r�  H�K H�CH��H�� [���������������@SH�� H�\$PH�޵  H�7 A�W ��C   ��  H��H�� [������������@SH�� H� H���  I��A�W ��݋  H��H�� [�����@SUVWATH��   �   H��A�yI��H�A8L���H8f��	�  HcK@�WH��E3��ۍ  H��9{@v# L�CH�W�D��H�M���C��;{@r�A�L$�W�I�$H�H�D� �L�H�KH�yt���  ��uH����  ��  H�L$h�S�  H�CH3�H�K8A�   �|$8H�l$0L� �C@�D$(H�D$hH�D$ I�PM�@����H�͋����  ��u%H�C8H�T$hH��D�@T�'  H�L$h�҇  �  �� ��s  M��H��H���  �`  f��rH��  H�I�  A�W ��m�  �:  I��L��$�   ���  H�C8H�x@H��H�|$H�	�  D��9C@rA�W �H�$�  ��  ��S@L��$�   H�����  �S@L�D$@H���W�  I�$Hc�$�   H;���  HcD$@H;���  A�F��D$D;C@�k  L��$�   ��H�L$P��  E3�I��I��A�P��  L��E��tR3�L��$�   D��L��ff�     H�KHI����  H�CHM�vI��H��L�A�N�H��u�H�|$HL��$�   �L$DH��$�   A�$A�D� H���
�  ��x��$�   f��uH�C8���  �@8f#�f��$�   ��I��H��f��uL�D$P���$�   L�D$Xf�D$P���  I�͋����  L��$�   ��x"H�C8H�T$PH��D�@T����H�L$P���  �d�����  H���  H��D��H���y�  ��H�����  H�L$P���  �*M��H��H���b  �A� �H�K�  H�L�  �6�  L��$�   H��H�Đ   A\_^][��������������L�D$SVWATH��xH��I��H�I8L���A8f��	��   HcK@E3�H��A�P�"�  A�   H��D9C@v, A��A�P�H��H�HCHA���HH� H�։L�D;C@r�A�L$A�P�I�$H�H�։L��OH�A��H�H�։L�H�KH�yt
�ӆ  ��tVH�L$@���  H�CH3�H�K8A�   �|$8H�t$0L� �C@���D$(H�D$@I�PH�D$ M�@�0�����uH�L$@�!�  H�����  H��xA\_^[�f����  H�I@H��$�   L�l$pL�|$`���  E3���D���A�P��  L��E��tD3�L�t$hD��H��H�KHI����  H�CHH�vI��H��L��N�H��u�H��$�   L�t$hI�����  A�$A�M�H��$�   A��H�K8H�I@��  L�l$pH��$�   ��xf��$�    uH�C8�H8���  f#�f��$�   H�L$@�5�  H�C8H�L$@H��D�@T�  ��$�   f9D$@tE3�H�T$@D��H�L$@�
�  ��$�   H�K8I��L�D$@H�I@f��tL�D$H���  I�ϋ��V�  H�L$@���  L�|$`��yQ������  H�x�  H��D��H���Y�  ��H���}�  H��xA\_^[�H�L�  H�E�  A�@ ��)�  H��xA\_^[����������������H�1�  H�@�@@u� ������������̀yu*�zu$H�L�HH�L�@I���   I9��   u������H�%��  ��������̸���������������H�\$WH�� H�ٸ    H�I8H��f�A8u�����H�\$8H�� _ËS@L�D$@H�I@�/�  H�K8L�D$0�S@H�I@���  �D$0+D$@H�\$8��H�H�3�H�� _��������������@SH�� H��H�I8��u�p�  ��A���u�g�  H���Ƅ  H�KHH�� [H�%��  ��������������@UAVAWH�� L���P   �p�  AE3�H�� AOA�PHAG @ AO0H0AG@@@H�H8�HcH@�Å  H�EHMcw@M��~fH�\$@H�t$HH�|$P3�D  I�OHH�uH�\9H�9��uH�Ȼ  �e�  �
�� �  t� H�>�\>H��I��u�H�|$PH�t$HH�\$@H��H�� A_A^]����L�D$SWATAUH��(H�t$XL��3�L�t$hH��L�|$ M��D���VP�N���  H���@@   H���  H9CuH�K8L��H�O8�C@���G@�H�_8H���E3�HcO@A�P�  H�GHA�  M��t]H�l$PIco@H��~JD  I�GHL�wH�\H���uA���j�  H���
�� �  t�I�6A�\6H��H��u�L�t$`H�l$PA�NHc_@I�L�|$ L�t$hH�t$XH��H_H��uH����  ��� �  t� D��H�C�H��D�c�H���  ���  H��  �   H�OI�} A�E  H��(A]A\_[���������@SH�� H�Y8H�KX�܃  H���   �'�  H��H�� [H�%�  H�Q83����   9��   �������������H�\$WH��@H�y8H�GxH���   HcP@���   �D��H�T$PH�GxH�H8H�Y@H����}  ��xf�|$P uH�Gx���  H�P8�B8f#�f�D$PH�L$ ��}  �D$PH��f��uL�D$ �
f�D$ L�D$(H���   �K}  H�Ox�Gp   H�Q8H�OhD�BTH�T$ �R���H�L$ ��}  H�\$XH�GhH��@_���������H�A8Hc��   ���u�B   �H�
�B   ��������������H�Q8A��������   ���   ��;��   AMȉ��   ��������H�\$ WH�� H�:H��E��tH�4�  3���  3�H�\$HH�� _ú�   H�l$0�   ���  H��E3�H�=�  H�EHA�PH�m8H�}xH��H�H�MX�E`  �O@��Hc����  H���   3ۋW@��~OH�t$8��L�t$@D��Hc�H��HOH�l  H�GHM�vH���   H�v��A�D��D��W@;�|�L�t$@H�t$8H�O8L���   H�I@��{  H�O8L���   �W@H�I@�8|  ���   H�\$H���   H��H�l$0H�� _��H���  ���������H�T$L�D$L�L$ SVWH��0H��H�t$`H�������H�t$(L��I������H�D$     H��H���|  H��0_^[���������������@USAVAWH�l$�H��  H�~�  H3�H�E�D��U�E3�H�S�  L�E�L�|$h�P  H��H����  H��$P  L�E�H��$  H��  3�L��$   �  L�E�3�H� �  H���  H��H������H��D8<u�E��L��$  H������H�U�H�E�H��L���y|  ����   A�   A��H��tH��tH���\{  H��D���O{  H�L$hE3�H�L$ D��H�M�A���xy  ����  H��t	H����  3�L�}�H�E�L�}�L�=�  H�D$ H�M�D�B�|  ��x;H�M�L�M�E3�3�H�H�@ ��  ��xH�M�L�E�H�T$hH�H���   �s  H�M�H��tH�H�@�]  H�M�H���6  H�H�@�C  �$  ��H�T$hI����x  ���  ��H�E�A�  E3�H�D$ H���  H��   ���w  ����  H�M�H�D$`3�E3�H�|$XE3�H�|$P3�H�|$HH�|$@H�|$8H�|$0H�D$(H�D$|H�D$ �Fw  ����  ��D$`���ȉD$`��}  L��H������H��@8<u�H���u}  D��H��9|$|�/  D�m�A�    H�|$h �  D�L$`M��H�M�A����v  ����   H�M�H�E�A�  H�D$ E3�I����v  ����   H�M�H�D$pH�|$XE3�H�|$PE3�H�|$H3�H�|$@H�|$8H�|$0H�|$(H�D$ �av  ��us9D$pvk�    H�M�L�E�A�   ���Jv  ��uBH������H���< u�H�M�L�M���H�U�L�ƉE��v  ��uH��H���<y  ����   ��;|$pr�3�H�M���u  A��D;t$|�����L�m�H�M���u  I���3|  H���*|  I���!|  H�D$hL��$   L��$  H��$  H��$P  H�M�H3���V  H��  A_A^[]�3�L�L$tL�D$xH�}�H�C�  H�M��J�����tA�ċωD$x�L$t��L$t�D$x�L$(L��  H�M��D$ M��3��p{  H�M�A���d���H�M�H�D$h�m{  ����H�L$UWATAUH�l$�H���   E��D��H��H��u�A�H���   A]A\_]�H�H��$�   H��$�   L��$�   H�@��{  E3��E�A�ދ��]���+  L��$�   ���  eH�%X   H��H�E�@ f�     H�L�Eǋ�H��H�@(��{  �}� ��  H�L�E���H��H�@ ��{  H�M�L�E�3�E��H�H�@0�h{  ���~  �     H�M�H�U�H�T$ L�E�H�U�A�   H��H�@8�.{  �}�tH�M�H�U�H�H���   �{  ��  H�u�A�����L�t$8L��L�t$03�D�t$(A��L�t$ �t  �؅�tIE3���A�P�bz  L�t$8A�����L�t$0L�Ɖ\$(3�A��H�D$ H����s  ��A�΋�����uG��   ��y  H��A���  ��s  ���>z  L���  �   L��3�H����y  H����y  H�M�D�v���t  H�U�H�M�E��H�R�a  A��H����v  H��t`H�Mߺ   H�	L�
�R�  ��Hc�I�H�рz t*L��H�U�H�M�_v  ��uL��L�q�  �P3��y  H����x  �   �}���   A��I�N �EH���H�E�A��H�E��E�   @��t�(y  ��xx  H���    @��M����H�׃�H�K�CH�C    L�s�]a  B�D3 H��H�]�Dx  H�M���u  H�M�H�U�H�H���   �y  E3�H�M�L�E�A��A��H�H�@0��x  ��������]H�}g�u�H�M�H�H�@��x  �É];�����L��$�   L��$�   3�H��$�   H��$�   H���   A]A\_]�������������H�	H�H�@H�%wx  ���������������H�\$H�l$H�t$H�|$ AVH��0��H��H������H�À< u�H�K H�����w  L��H��H���    H�H�@�   H�XH�@    �`  H���  �D ��t  H��H���  ��u  H��tH�8H��uYH�VH�N��u  H�ȋ�H������H��H����v  H��t=L�D$ H�|$ H���D$(   H���  �u  H��tH�8 tH�H��H�@�hw  H�a�  ��t  �F�@u��uH�΄�y��r  ��Jv  H�\$@H��H�|$XH�l$HH�t$PH��0A^�����@USVWAVH�l$�H��   H���  H3�H�E/3�E��H�]�I��H�]�H��H����  M���  E����   f�z8	�3  H�J@L�E�H�wz  H�H� ��v  ��xPH�M�L�E�VH�H�@ ��v  H�U���H�
H�AH���qv  ��xH�M�L�E�H�UH�H�@0�Tv  �  H�O@L�E�H��y  H�H� �4v  ����  L���  �   3��pu  H�M�H�H�@�v  �a  H�JXH��tH�H�@��u  H�GX�\  H�J@L�M�E3�3�H�H�@ ��u  H9]��4  �  L�JXM��tI�L�E�H�U�I��H���   ��u  �H��uzf�z8	usH�J@L�M�E3�3�H�H�@ �nu  H�M�L�E�H�U�H�H���   �Ru  H9]���  H�M�H�H�@�7u  H�]��H��t�   ����H�E�H���t  H9]���  H������H��8u�E3�H���Y���H�M�L�M�H���   f�U�E3�H�H�BXH�U�H�T$(H�UH�T$ H����t  ��x
f9]��   H�UH����p  ����   H�M�L�E�H�UH�H�@0�|t  ����   H�M�H�U�H�H�@�_t  H�U�f;Z4��   f�     H�M�L�E��H�H�@H�1t  ��xJE��t�}t�=�Eu7H�M�L�E���H�H�@@�t  ��xH�M�L�EߋU�H�H�@p��s  ��yH�U����B4;�|��H�U�H�M�H�H���   ��s  H�M�H�H�@��s  H����r  H�M�H��tH�H�@��s  H�E�H�M/H3��M  H�Ġ   A^_^[]������D�D$H�T$H�L$USVWATAUAVH�l$�H��   H�L��3�H�U�I��A��L��D��H�@�!s  ��y3��	  ��M��t"H�E�D�@,A��tH��  3��Qp  ��  L��$  H��t
H�E� D�}����   I�$L�E�E3�H�t$(I��H�t$ H�@`A�Q���r  H�M�H�U�E������H�M�H����l  H�M�H�U���n  H�M�E��3������H�M�H����k  L��H���  H����o  H���Qq  H���Hq  M��tL��q  E3�3�I���Fq  H�Eǋ։U�f;p0��   I�$L�E�I��H�@(��q  ����  ��H�U��B�u	�uD���A�   D;*�]  �
L�E�I�$�щM�E3�H�t$(I��H�t$ H�@`��q  H�}�A�����H�t$8L��H�t$03҉t$(A��H�t$ ��j  �؅�tLE3���A�P��p  H�t$8A�����H�t$0L�ǉ\$(3�A��H�D$ L��H�E��fj  ���΋�����uJ��   �p  L��H�E��@�0�*j  ����p  L���  �   L��3�H���8p  H���Op  H�M���H�}��?k  9uo��  H�E�E3��u�H�H$A�PH���:p  I�$H��H�U�L��H�E�H�A8H�M�D�J$�A��H�L$ I���pp  H���j  H�E�H���  ���m  H�M��Q8��tYH��H9��  t/H��L���  f�     fB9 ��   H��H��H��J9t u�H�5�  D��H�k�  H����m  H�M�E���.  I�$L�M��E3�H�t$(I��H�t$ H�@`��o  H�]�H����   H�t$8A�����H�t$0L�Ét$(3�A��H�t$ ��h  D����tUE3�A��A�P�o  H�t$8A�����H�t$0L��D�t$(3�A��H�D$ H����h  ��������uQ�H�I�D��,����   �&n  H��@�0�Bh  ����n  L���  �   L��3�H���Pn  H���gn  H�M��]i  H��H�[�  �ul  H����m  I��H�R�  �\l  ��  I��H�M�  �Gl  H�M�D��f;q$�p  H��H�u�H�]�L��L�aH�=ԗ  M�H��H9ϗ  t$A�T$H��f98�>  H��H��H��H9t8u�H�r�  E�D$H��  ��k  A�D$�tH�ޖ  ��k  A�D$�tH�ϖ  ��k  fA�|$uXH��H9N�  t/I�$�PH��f�     f98��   H��H��H��H9t8u�H��  I�$H�{�  D�@�@k  E�}D;}���   L�3A������MM��H�t$83�H�t$0�t$(H�t$ ��f  �؅�t`E3���A�P��l  �MA�����H�t$8M��H�t$03҉\$(H��H�D$ �af  ��������u^�+H�H�T������H�H�T��L����   ��k  H��@�0�f  ����l  L�t�  �   L��3�H���!l  H���8l  H�]�H��+g  L�u��H�=��  H�E�H���  A�,   L���H$�    ��D;�H�m�  DD�fA�|$H�O�  HD��j  D;}�}	H���bk  H�M�H��I�� H�]�E��L�u��A$D;������D�}L�e_H��  ��i  I�$L�M�H�t$(E3�I��H�t$ L�P`H�E��I����k  H�]�H����   H�t$8A�����H�t$0L�Ét$(3�A��H�t$ ��d  D����tHE3�A��A�P�;k  H�t$8A�����H�t$0L��D�t$(3�A��H�D$ H����d  ��������uD��   �`j  H��@�0�|d  ���k  L�ݎ  �   L��3�H����j  H����j  H�M���e  H��H��  ��h  H���j  H��  ��h  L�m�H�M���i  H�}�L�ugM��teH��I���Zh  H�O H�����i  L