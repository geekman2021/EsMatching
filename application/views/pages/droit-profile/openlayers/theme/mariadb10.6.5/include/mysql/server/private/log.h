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
   befoMZ       ÿÿ  ¸       @                                     º ´	Í!¸LÍ!This program cannot be run in DOS mode.
$       ‘<ÕükoÕükoÕükoÜ„øoÙüko‡‰jn×üko³“–oÖüko‡‰nnØüko‡‰onİüko‡‰hnÑükoonÓükojnßükol‰jnÖükoÕüjo?ükol‰cnÙükol‰knÔükol‰”oÔükol‰inÔükoRichÕüko                PE  d† *+”a        ğ "  â   Œ      Àä        €                        °    âÜ  `A                                    N D  äP     8	   € È               `8 T                    : (   À8 8             h                          .text   hà      â                    `.rdata  Nj      l   æ              @  @.data   ğ
   p     R             @  À.pdata  È   €     V             @  @.rsrc   8	     
   d             @  @.reloc           n             @  B                                                                                                                                                                                                                                                H‰\$H‰t$H‰|$ UATAUAVAWH¬$`ÿÿÿHì   H‹Ú_ H3ÄH‰…˜   3öÇD$@   €y(HA WÀH‰t$PHEÆH‰t$`H‰E€H‹ùHE¸H‰u°H‰E@D‹æH‰uD‹îH‰t$hH‰t$HE¸HÇE(
   EÈH‰u0EØ‰u8ÇE<   ‰uHÿ3ğ  ‹ÀD‹şH‰D$p^‹G,D‹ö‰D$XÿÈƒø‡!	  €XLwPD‹ûu
H‹GPH‰E˜ëHU˜HOPÿ\ö  „À„è  H‹E˜HHA¿   H‹@H‰D$xH‰M¨D9|$X‚à   ¶GhLw`<uH‹G`ë<uH‹O`H‹ÆH‰L$`ë:Ãu3H‹ÆH‰t$`H‰D$PA¿   D9|$X‚š   €xLwpu0H‹GpH‰D$pë<HT$PH‰t$`HO`ÿ¿õ  „ÀuÂ¾   ^ğé[  HT$pHOpÿ‡õ  „À„0  A¿   D9|$Xr<L·€   A€~u	I‹H‰E ëHU I‹Îÿfõ  „ÀtOH‹E HHH‹@H‰EH‰M°ÿxô  L‹u€‹L$pM‹6ÿçô  H…Àu,H‹«h Hœù  A¸W €ÿˆò  éÖ  A‹÷»	   é³  ‹D$pA‰FT¸   H9t$PtÇD$@   éI  H‹|$`H…ÿ„  A¸   Hrù  H‹ÏÿYô  H…Àt,€xuH‹ ö@@uÿ ë	H‹Èÿ¨ô  A¿   H‰D$PL‰|$@A¸   H3ù  H‹Ïÿô  H…Àt&€xuH‹ H‰D$hö@@uÿ ëH‹Èÿ\ô  H‰D$hA¸   Hù  H‹ÏÿÑó  H…Àt#€xuL‹ AöD$@uAÿ$ëH‹Èÿô  L‹àA¸   HĞø  H‹Ïÿ“ó  H…Àt"€xuL‹(AöE@uAÿE ëH‹Èÿßó  L‹èA¸   H›ø  H‹ÏÿVó  H…Àt€xu‹ ë	H‹Èÿ6ó  H‰D$@¸   H9t$PtMD‹öf eH‹%X   ‹ĞJ‹ÁH‹
‹íf ÿÉHcÉH‹ H‹È@81uH‹İf H6ø  L‹Ãÿ½ğ  é  E‹FTH‹T$xH‹M¨èÆO  H‹T$PH‹øH‰D$xH…Ò„•   ‰uHJ‰u E‹FTH‹Rè™O  L‹|$hH‰EM…ÿtjIGH‰E¸Hø  A‹G‰EÀM…ätID$H‰EØA‹D$‰EàëH‰MØ‰uàM…ítIEH‰EÈA‹E‰EĞHE(H‰E‰]äëHE(H‰MÈH‰E‰uĞ‰]äëH‰uëL‹|$hH•ˆ   H‹ÏÿÖî  …À‰  WÀH‰t$@3ÀH‰uˆHT$@H‰Ep3ÉEPE`ÿÇî  ‹ø…Àˆµ   ®èH9t$PtBH‹L$@HUPÇEP(   H‹H‹@8ÿ>ò  H‹L$@HEH‰EpHUPÇEP(   H‹H‹@0ÿò  H‹T$xLMˆH‹L$@LD$Xÿ0î  ‹ø…ÀxJH‹MˆIV@H‰T$ L^õ  H‹T$@E3ÀH‹H‹@@ÿÑñ  ‹ø…Àx
»	   fA‰^8H‹MˆH‹H‹@ÿ°ñ  H‹L$@H…É„…   H‹H‹@ÿ•ñ  ëv3ÒLõ  Hˆ   H9t$Pt:D‹D$@HEğL‰MğLMH‰D$(‰\$ H‰uø‰u ÿ•í  ‹ø…Àx2H‹Eø‹} I‰F@ëIF@A¸   H‰D$ ÿ}í  ‹ø…Àx
»	   fA‰^8H‹L$PH…Ét=H‹EH…ÀtH‹Èÿğ  H‹L$PH9t$`töA@uƒÿuöA€tÿ6ì  ëÿæï  M…ÿt#A‹G¨@uAƒÿuI‹Ï„Àyÿì  ëÿ¾ï  M…ät%A‹D$¨@uAƒ$ÿuI‹Ì„Àyÿäë  ëÿ”ï  M…ít$A‹E¨@uAƒE ÿuI‹Í„Àyÿ»ë  ëÿkï  H‹L$xÿ`ï  ®è…ÿ‰‘   ‹Ïÿğ  L‹M¨L2õ  3ÒH‰D$ HM€H‹Øÿï  H‹Ëÿï  H‹U€H…Òu9‹ÏÿÔï  H‹Uc H‹ĞD‹ÇH‹Øÿ6í  ®èH‹ËÿZï  H‹M€ÿèî  én  H‹$c D‹Çÿí  H‹M€ÿÉî  éO  I‹N@MNXE3À3ÒH‹H‹@ ÿšï  …À‰£   H‹E°H…À„–   ®èA‹VTH‹Èèw  H‰D$HH…À„  ®è‹Ÿb eH‹%X   ¹   H‹ĞH‹‹“b ÿÈHcÈH‹H‹È@8qt®èE‹FT3ÒH‹L$HèÆr  H‹L$HMFX3ÒH‹H‹@ ÿï  H‹L$HH‹H‹@ÿôî  éŠ  I9vX„€  ‹b eH‹%X   ¹   H‹ĞH‹‹b ÿÈHcÈH‹H‹È@8q„H  ®èI‹NXLD$hHT$HH‹H‹€   ÿŒî  …Àˆ  H‹L$HLEèE3ÉH‰t$(H‰t$ H‹AQÿH‹@Hÿ]î  ®èE‹FT…Àˆ©   H‹MèHUè¹K  H‹:a H‹øH‹\$HÿÌê  L‹ELMxH‹×H‰]xHa Ç…€      ÿ®ê  H‹ÿ` H…ÀtAH‹ÿ‰ë  H…Ût9®èE‹FT3ÒH‹L$Hèq  H‹L$HH‹H‹@ÿÇí  H‹ÏÿÎì  é³şÿÿÿKë  H‹Ïÿºì  éŸşÿÿH‹L$H3ÒèAq  éşÿÿ¾   »	   ëº   ‹Ëÿ«ì  D‹ÎL‰t$ E3ÀA‹×‹Ëÿµì  H‹˜   H3Ìè^Ç  Lœ$   I‹[8I‹s@I‹{HI‹ãA_A^A]A\]ÃÌÌÌÌÌÌÌÌÌÌÌÌÌ@SUHìˆ   H‹§U H3ÄH‰D$p3ÀÆD$0H‰D$HH‹êH‰D$8H‹Ùÿúë  ‹K,HD$0H‰D$(LL$PHD$@LD$XH‰D$ Hòñ  ÿdì  ƒøÿ„­  €|$0 H‰´$°   H‰¼$€   t9‹Ş_ eH‹%X   º   H‹ÈH‹‹Ò_ ÿÈHcÈH‹H‹ÈHcAH‰D$@ëH‹D$@H‹T$PD‹ÀH‹L$Xè¼H  HT$`H‹ÈH‹ğÿ{è  ‹ø…Ày2®è‹Ïÿì  H‹ƒ_ D‹ÇH‹ĞH‹Øÿdé  ®èH‹Ëÿˆë  é³   LD$H3ÒHL$`ÿiæ  ‹ø…Ày/®è‹Ïÿ¸ë  H‹9_ D‹ÇH‹ĞH‹Øÿé  ®èH‹Ëÿ>ë  ëlH‹L$HLD$8H+ï  H‹H‹ ÿ§ë  ‹ø…Ày/®è‹Ïÿfë  H‹ç^ D‹ÇH‹ĞH‹ØÿÈè  ®èH‹Ëÿìê  ëH‹L$8H…Ét,D‹D$@H‹ÑH‹ÍèD  H‹L$8H…ÉtH‹H‹@ÿAë  H‹L$8Hƒ|$H H‹¼$€   tH‹H‹@ÿë  H‹Îÿ&ê  H‹´$°   H‹L$pH3Ìè	Å  HÄˆ   ][ÃÌÌÌÌÌÌÌÌÌÌÌÌÌÌÌL‹ÜUWAVHì°   D‹ÒWÀD$pISL‹ñH‹I@I‹éH‰T$@IS¨H‰T$8H‹”$ğ   H‰T$0A‹ÒAC¸L‰L$(A¹   ACÈfD‰D$ Lî  ACØH‹H‹@0ÿoê  ƒ¼$ø    ‹ø…Ş  …ÿ‰Ö  H‰œ$Ø   üÿı3ÛH‰´$à   H‰\$P‹óƒù†-  ÿ	 €t8ÿ €u‹EEuƒ¼$   „j  ®è‹ÏÿÎé  D‹ÏL”ï  é  H‹L$xH…ÉtE‹FTHT$XèOG  H‹L$xH‹ØÿAä  H‹Œ$€   H…ÉtE‹FTHT$`è&G  H‹Œ$€   H‹ğÿä  ‹×\ LÀî  eH‹%X   I‹ÁA¸   H‹ÑN‹H‹7é  H‹HL$PA€¼è   Lî  uL¶î  H…öHEÆH…ÛH‰D$ LEË3Òÿ+è  H…öt	H‹Îÿ5è  H…Ût	H‹Ëÿ'è  H‹Œ$ˆ   H…Ét@ÿ|ã  ë8®è‹ÏÿÇè  D‹Œ$Ğ   Lpî  3ÒH‰D$ HL$PH‹ØÿËç  H‹ËÿBè  Hƒ|$P t#®èH‹T$PL‹ÇH‹\ ÿòå  H‹L$Pÿ¯ç  H‹œ$Ø   ‹ÇH‹´$à   HÄ°   A^_]Ã‹ÇHÄ°   A^_]ÃÌÌÌÌÌÌÌÌÌÌÌH‰\$ VWAVHƒìPM‹ñI‹ØH‹òH‹ùIƒøÿu
HÿÃ€< u÷H‹‰   H…Ét&L‹Ãÿšç  H…Àt‹ A‰3ÀH‹œ$ˆ   HƒÄPA^_^ÃD‹GTH‹ÓH‰l$xH‹ÎL‰¼$€   èYD  H‹OXH‰D$pH…É„…   H‹HT$pM‹ÎA¸   H‹@PÿÄç  ‹è…À‰’   ®èH‹O@LD$pD‹øL‰t$(A¹   ÇD$    H)ë  H‹H‹@(ÿ„ç  ‹è…ÀxVAÿ@ €tMH‹OXH‹H‹@ÿdç  HÇGX    ë2H‹O@LD$pL‰t$(Hİê  A¹   ÇD$    H‹H‹@(ÿ*ç  ‹èH‹L$pÿ-æ  L‹¼$€   …íxYH‹   H…Éu,¹8   ÿúå  E3ÉE3ÀH‹ÈH‰‡   AQÿæ  H‹   IcLL$@L‹ÃH‰D$@H‹ÖÇD$H   ÿĞå  H‹œ$ˆ   ‹ÅH‹l$xHƒÄPA^_^ÃÌÌÌÌÌÌÌÌL‰L$ fD‰D$H‰T$SVWATAUAVAWHìĞ   Lc¼$0  E3äL‰d$XL‹òL‰d$HL‹éA‹ôA‹ÜH…Ò„Á   L‹BLL$@IPM‹@èÎıÿÿ‹ø®è…À‰¨   ‹ÏL‰d$Pÿìå  M‹NLÉë  IƒÁH‰D$ 3ÒHL$PH‹Øÿğä  H‹Ëÿgå  H‹T$PH…Òu?‹Ïÿ­å  H‹.Y D‹ÇH‹ĞH‹Øÿã  ®èH‹Ëÿ3å  H‹L$PÿÀä  AD$ÿéY  H‹÷X L‹ÇÿŞâ  H‹L$Pÿ›ä  ¸ÿÿÿÿé4  H‰¬$  ½   E…ÿtI‹×E3À‹Íÿùä  H‹ğH‰D$XI‹ÿI9^(„ø  A‹ÄE…ÿ‚  Aƒÿ‚x  )´$À   )¼$°   D)„$    D)Œ$   D)”$€   ®èM‹N(I‹×fDoÀ HƒâøfDo£ I‹ÏfDo· WöHÁáWíWäfnıM‹Ä fBnL	ƒÀfBnD	ĞIƒÀfBnT	°fBn\	ğH‰ ÿÿÿfbØfAoÁfbÑfşÅfBnL	flÚfBnT	0fbÑfÒßfAÛØfvŞfAßÚfÛÃfßİfoèfBnD	PfëëfBn\	pfbØfAoÁflÚfşÄfÒßfAÛØfvŞfAßÚfÛÃfßÜfoàfëãL;ÂŒCÿÿÿD(”$€   fşåD(Œ$   foÄD(„$    (¼$°   (´$À   fsØfşàfoÄfsØfşàf~ãHcÈI;Ï}9M‹N(M‹ÇL+ÁI‹ĞHÁâ@ f„     B÷D
ğ   CHRàDÃ‹ØIƒèuå…Û„!  HcÓE3ÀH‹Íÿşâ  H‰D$HH‹øE…ÿI  ®èH‹¬$8  I‹ÏI‹ÇHÁáHÁàHƒÅğHèH‰L$PA‹ÇH‹ŞH‰D$`L‹ÿff„     I‹F(H‹ÕE‹ET÷Dğ   ttIcÄH@M4ÏI‹Îèy  ¸ @  f…t*I‹ÎòOòKÿMİ  L‹´$  AÿÄHƒÇë5H‹Îÿ3İ  ¸ @  AÿÄff‰HƒÇIFL‹´$  H‰CëH‹ÎèŸx  H‹L$PHƒíHƒé HƒÆHƒÃH‰L$PHƒl$`…IÿÿÿLc¼$0  E3äëHE…ÿ~F®èH‹¼$8  I‹ÇHÁàHƒÇğH‹ŞHøA‹÷„     E‹ETH‹×H‹Ëè1x  HƒïHİHƒîuäI‹ÿH‹„$(  LL$hH‹t$XA¸   ‹T$@I‹ÍD‰d$0D‰d$(H‰t$hH‰D$ D‰|$xD‰d$|L‰d$pèÚöÿÿ‰„$   ‹ØH…ö„P  Iƒ~( „  E‹ôE…ÿ  ®èH‹„$8  H‹ÏL‹¤$  HƒÀğHÁáH‹îHÁçHøH‰L$`A‹ÇL‹|$HH‰D$P€    I‹D$(÷Dğ   te€
H‹ßuH‹HƒÃH‹Ëÿèà  ¸ @  ÇC   f…E tIcÆH@IÏHBH9EuëH‹ÖE‹ETH‹Ëè0{  IcÆH@IÏÿOÛ  AÿÆH‹ÎÿCÛ  H‹L$`HƒÆHƒé HƒïHƒÅH‰L$`Hƒl$P…]ÿÿÿH‹t$XE3ä‹œ$   ë-E…ÿ~#®èH‹ŞA‹ÿH‹ËÿõÚ  HƒÃHƒïuí‹œ$   L‹|$HH‹Îÿ~ß  M…ÿt	I‹Ïÿpß  H‹¬$  …ÛA™ÄAD$ÿHÄĞ   A_A^A]A\_^[ÃÌÌH‹ÄL‰H ‰PVAWHìˆ   H‰X3öH‰hè‹¬$À   H‰xàH‹¼$È   L‰`ØE·àL‰hĞL‹éL‰pÈD‹ö…ítaHcÕNE3Àÿvß  L‹ğ…í~<®èH‹ØHƒÇğHcÅD‹ıHÁàHø„     E‹ETH‹×H‹ËèÁu  HƒïHƒÃIƒïuã‹”$¨   L‹Œ$¸   ‰l$P‰t$TL‰t$@H‰t$HAöÄt H„$°   Ç„$°   ıÿÿÿH‰D$HÇD$T   ‹„$Ø   E·Ä‰D$0I‹Í‹„$Ğ   ‰D$(L‰L$ LL$@è@ôÿÿL‹l$hD‹øL‹d$pM…öt*…í~®èI‹ŞH‹ıH‹ËÿgÙ  HƒÃHƒïuíI‹Îÿüİ  L‹t$`H‹|$xH‹¬$€   H‹œ$    Aÿ €uA‹ÇHÄˆ   A_^ÃE…ÿ@™ÆFÿHÄˆ   A_^ÃÌÌÌÌÌÌÌÌÌÌÌÌÌÌH‰\$H‰l$H‰t$WHƒìPA·éHÇD$H    LL$@H‹òH‹Ùèÿõÿÿ‹ø®è…À‰˜   ‹Ïÿ"Ş  L‹ÎL ä  HL$HH‰D$ 3ÒH‹Øÿ+İ  H‹Ëÿ¢İ  H‹T$HH…Òu<‹Ïÿèİ  H‹iQ D‹ÇH‹ĞH‹ØÿJÛ  ®èH‹Ëÿnİ  H‹L$HÿûÜ  ¸ÿÿÿÿëeH‹5Q L‹ÇÿÛ  H‹L$HÿÙÜ  ¸ÿÿÿÿëC‹„$˜   D·ÅL‹Œ$€   H‹Ë‹T$@‰D$8H‹„$   ÇD$0    H‰D$(‹„$ˆ   ‰D$ èGıÿÿH‹\$`H‹l$hH‹t$pHƒÄP_ÃÌÌAVHƒìPH‹ëE H3ÄH‰D$@ƒy, L‹òtÿ„Ü  H‹L$@H3Ìè?·  HƒÄPA^Ãÿ2Ü  HL$0ÿÙ  …À…Á   HT$ HL$0ÿoÙ  …À…©   H‹L$ HT$(H‰\$`E3ÀH‰t$pH‰|$xèV:  H‹|$(H‹ğHO HƒáøÿÈÛ  L‹ÇH‹ÖH‹ØHHÇ    Ç@   HÇ@    H‰xè³Ä  ÆD; H‹ÎI‰AÇF  ÿ”Û  H‹L$ ÿÁØ  H‹|$xH‹t$pH‹\$`H‹L$@H3Ìèe¶  HƒÄPA^ÃAÇF   H‹L$@H3ÌèI¶  HƒÄPA^ÃÌÌH‰\$ H‰T$UVWATAUAVAWHƒì0D‹A,3ÛH‰œ$€   ‹ëD‹ëD‰D$pD‹ã‹óA@şD‹ûƒø‡Î  €yXLqPH‹KO DcI‹öu"H…Òt=I‹H‹HH;Êt1ÿSÛ  „Àu"H‹ O H…Ò„  L‹z¿   IƒÇéŒ  D‹D$pA€~IvA¼   …U  ¿   D‹çD;Çr*A¶F(Iv <uH‹ë<uH‹.ë<ufH‹ÃH‰„$€   L‹d$xAÇD$   ÿ@Ú  M‹>H…íta3ÒH‹ÍÿÙ  H…Àt€xuL‹(IƒÅº   H‹ÍÿíØ  H…ÀtB€xu<H‹HƒÃë3H”$€   H‹Îÿ¸Ú  „Àu»   {ğé¾   H‹„$€   H…ÀtHXA¹   L‹ÃI‹×I‹Íè’c  H‹ØH…À„£   ¹8   ÿ“Ù  E‹GTMOpD‰D$ H‹ĞE3ÀH‹ËH‹ğèg  …Àt(L‹ÆIWpINè9²  º   I‰GhI‹Ïèˆ+  A‰|$ë	H‹ÎÿPÙ  H‹H‹ËH‹@ÿ0Ú  ë3»   {÷ëº   JÿÿFÙ  ¿   D‹ËH‰t$ M‹ÇA‹Ô‹ÏÿKÙ  H‹œ$ˆ   HƒÄ0A_A^A]A\_^]ÃÌÌÌÌÌÌÌÌÌÌÌH‹ÄH‰PSUVWATAUAVAWHƒìH3ÿH‰x D‹ï@ˆxD‹ç‹A,D‹÷‰„$    _ÿÈD‹ÿƒø‡  ¶AXHqPH‹ìL D‹ãL‹ö<uH…ÒtYH‹H‹HH;ÊtMÿóØ  „ÀuC¶FH‹ï<u=H‹H‰„$¨   ‹Œ$    A¼   A;Ì‚±   ¶FLv<uPH‹FH‰D$0ërH‹.ëÎH”$¨   H‹ÎÿŞØ  „Àu¹H‹cL H…ÀtL‹x»   IƒÇéx  ¿   _ëék  :Ãu
H‹ÇH‰D$0ëHT$0HNÿ’Ø  „ÀtsH‹D$0‹Œ$    H…ÀtLhA¼   A;Ìr¶F(Lv A:Äu ˆœ$   ÿ„×  H…íuAH‹„$¨   HƒÀë6<tãH”$   I‹Îÿ´×  „ÀuÎ¿   _é×   ¿   _éÊ   H‹Ç@8¼$   M‹ÅH‹ÕH‹È@•ÇD‹Ïèş`  H‹øH…ÀtpH…ít‹ETë1‹>K eH‹%X   º   H‹ÈH‹‹2K ÿÈHcÈH‹H‹È‹AE3É‰D$ D‹Ã3ÒH‹ÏèWd  H‹H‹ÏH‹@ÿ¯×  H‹„$˜   Ç@   ëHLoİ  º   3ÉÿâÖ  H‹„$˜   Ç@   ë#º   ‹ËÿœÖ  D‹ÏL‰t$ M‹ÇA‹Ô‹Ëÿ¦Ö  HƒÄHA_A^A]A\_^][ÃÌÌÌÌÌÌÌÌÌÌÌÌÌ@SHƒì`‹I,LD$pH‹ÚHÇD$p    H'İ  ÿ±Ö  ƒøÿ„Œ   ÿ
Ö  D‹L$pE3À3ÒÇD$ ÿ  3ÉÿØÑ  …ÀuaE3ÉÇD$    E3ÀHL$03ÒÿÉÑ  …Àt5HL$0ÿ¢Ñ  HL$0ÿ§Ñ  E3ÉÇD$    E3ÀHL$03Òÿ”Ñ  …ÀuËÇC   HƒÄ`[ÃÇC   HƒÄ`[ÃÌÌÌÌÌÌÌÌÌÌÌÌÌÌ@VWHƒì8D‹šI H‹úeH‹%X   H[Ü  ‹I,ÆD$PA¹   J‹ÀM‹‹{I ÿÈLcÀI‹LL$`N‹ÀHD$PH‰D$ A‹pLD$Xÿ¡Õ  ƒøÿtn€|$P H‰\$0uLÜ  º   3Éÿ4Õ  ÇG   ÿ×Ô  H‹L$X‹Öè[]  H‹ØH…Àt(D‹Æ3ÒH‹ÈèVY  …ÀuÇG   H‹H‹ËH‹@ÿ“Õ  H‹\$0HƒÄ8_^ÃÌÌÌÌÌÌÌÌÌÌÌÌÌÌÌ@SVATHƒì@3öL‹áH‰t$hNÿ¬Ğ  H‹ØH…ÀuFHƒÄ@A\^[ÃH‰l$`Hdä  H‰|$pWÀ3ÀL‰l$8L‰t$0L5†ä  H‰Cÿ„Î  A½   H‹èH…À„Ñ   H3ä  H‹ÈÿJÎ  H‹øH…ÀuH‹Íÿ1Î  é«   E3ÉH"ä  E3ÀAQÿµÓ  L‹ÈH…ÀtO@80tJD‹ñG eH‹%X   J‹ÁI‹D ‹éG ÿÉHcÑH‹ H‹ĞHÇÂÿÿÿÿHÿÂA84u÷D‹AI‹ÉèÙ0  H‹ğLwå  H‰\$ L{å  3ÒH‹ÎH‹ÇÿMÔ  H‹Î‹øÿRÓ  H‹Íÿ‰Í  …ÿˆ    ®èH‹L5¤ã  H‹H‹@PÿÔ  ‹ø…Àˆ|   ®èH‹HT$hL5“ã  H‹H‹@hÿîÓ  ‹ø…ÀxWH‹L$hLCHØä  L5‘ã  H‹H‹ ÿÅÓ  ‹ø…Àx.‹ùF eH‹%X   H‹ĞI‹T ‹ñF ÿÈHcÈH‹H‹ÈH‰YH‹L$hH‹l$`H…ÉtH‹H‹@ÿuÓ  ‹¯F eH‹%X   H‹ĞI‹T ‹§F L‹l$8ÿÈHcÈH‹H‹ÈHƒy uLH‹KH…ÉtH‹H‹@ÿ)Ó  H‹H…ÉtH‹H‹@XÿÓ  H‹H‹H‹@ÿÓ  H‹ËÿSÎ  ‹ÇM‰4$ë3ÀH‹|$pL‹t$0HƒÄ@A\^[ÃÌÌÌÌÌÌ@USAUH¬$@ğÿÿ¸À  è‰º  H+àH‹G; H3ÄH‰…   H‹ÙÇD$@ÿÿÿÿLi 3ÉH×  H‰L$HH‰D$P€{(LEéÿåË  ‹K,LL$x‹ÀLE€H‰D$`H;â  HD$`H‰D$0HD$hH‰D$(HD$pH‰D$ ÿßÑ  ƒøÿ„T  H‰¼$ğ  L‰¼$°  ÿ(Ñ  eH‹%X   ‹QE º   H‹È‹RE HÚÿÈHcĞH‹H‹H‹ÑL‹xM…ÿ…¯   HL$Pèaüÿÿ‹ø…ÀyP®è‹Ïÿ˜Ñ  L‹L$PLœá  º   H‰D$ HM H‹ØÿĞ  H‹ËÿÑ  H‹íD HU D‹ÇÿĞÎ  é‘  ‹ÍD ÿÈHcĞH‹H‹H‹ÑL‹x‹L$`M‹m ÿäĞ  H…À…  H‹¤D Huá  A¸W €ÿÎ  éB  Iƒ uÃI‹HT$HH‹H‹@hÿÑ  ‹ø…ÀyE®è‹Ïÿ×Ğ  L˜à  º   Lüà  H‰D$ H   H‹ØÿÇÏ  H‹ËÿNĞ  H•   ëiH‹L$HMGH½á  H‹H‹ ÿ±Ğ  ‹ø…À‰=ÿÿÿ®è‹ÏÿlĞ  LUà  º   L‘à  H‰D$ H   H‹Øÿ\Ï  H‹ËÿãÏ  H•   H‹µC L‹ÇÿœÍ  AÇE   éU  D‹ E‰ETH‹T$hH‹L$pH‰´$è  L‰¤$ø  L‰´$¸  è,  E‹ETL‹ğH‹T$xH‹M€èl,  I‹ÎH‹ğÿ€Ê  H‹ÎH‹øÿtÊ  I‹OLL$HH‹ØL%Aà  L‹ÇH‹H‹‚(  H‹ÓÿÃÏ  I‹ÎD‹øÿÇÎ  H‹Îÿ¾Î  H‹ÏÿÊ  H‹ËÿÊ  L‹´$¸  H‹´$è  E…ÿˆ7  H‹L$HLD$X3ÿH à  H‰|$XL%Üß  H‹H‹ ÿXÏ  D‹ø…Àˆè   H‹L$XHUˆL%Íß  H‹H‹@ÿ0Ï  D‹ø…Àˆ®   ·Eˆfƒøu`H‹ML%¸ß  ME@L‰d$PHxÒ  H‹H‹ ÿôÎ  D‹ø…Àxv®èI‹M@MMX¸	   E3ÀfA‰E83ÒH‹H‹@ ÿÆÎ  L‹d$P‰|$@ëDfƒø	u.H‹MMMXI‰M@¸	   fA‰E8E3À3ÒH‹H‹@ ÿÎ  ‰|$@ëHMˆÿåÈ  A¿W €H‹L$XH‹H‹@ÿeÎ  H‹L$HH‹H‹@ÿSÎ  ƒ|$@ÿuRA‹ÏÿÎ  H‰D$(M‹ÌLüŞ  D‰|$ º   H   H‹ØÿÍ  H‹Ëÿ‰Í  H‹bA H•   E‹ÇÿBË  L‹¤$ø  H‹¼$ğ  L‹¼$°  H‹   H3ÌèÛ§  HÄÀ  A][]ÃÌÌÌÌÌÌÌÌÌÌÌÌÌÌÌHy6 ÃÌÌÌÌÌÌÌÌAVHƒì@HÇD$X    H…Ò„Q  €z HJ„C  Hßâ  ÿMÌ  L‹ğH…À„*  ¹   H‰l$`ÿYÌ  M‹Æº   H‹ÈH‹èÿıÈ  H…À„İ  ‹v@ eH‹%X   H‰\$PH‰|$8L‰d$0L%‡â  L‰l$(L‰|$ L<ÑH‰t$hA½   fD  €} ;„W  LD$XH‹ÍHGâ  ÿaË  H…À„9  LD$XH‹ÈH-â  ÿCË  LD$X3ÉHâ  H‹øÿ,Ë  H…ÀtI3ÒD  ¶HÿÂA:LÿuHƒúuìëHÛŞ  H‹Èè•´  …ÀuLàá  º   H,â  ÿ®Ë  ¾ÿÈ  …Àt¾OHÿÇÿÈ  …ÀuïHÇÀÿÿÿÿ„     HÿÀ€< u÷H_ÿHØH;ßt¾ÿÚÇ  …ÀtÆ HÿËH;ßuè‹E? A‹õI7ÿÈHcĞH‹H‹H‹ÑH‹Ï‹PèmS  H‹ØH…Àt6‹? ÿÉLcÁH‹H‹J‹Âº   D‹AH‹ÈèOO  H‹H‹AH‹Ëÿ—Ë  M‹Æº   H‹Íÿ>Ç  H…À……şÿÿL‹|$ L‹l$(L‹d$0H‹|$8H‹t$hH‹\$PH‹ÍÿfÊ  I‹ÎÿÇ  H‹l$`3ÀHƒÄ@A^Ã¸ÿÿÿÿHƒÄ@A^ÃÌÌÌÌÌÌÌÌÌÌÌH‰\$H‰l$H‰t$WHƒì0H‹éI‹ùH‹ÊI‹ğH‹ÚÿÉ  „Àu LÜà  º   H¸à  ÿ:Ê  ¸ÿÿÿÿë$‹D$hL‹Ï‰D$(L‹ÆH‹D$`H‹ÓH‹ÍH‰D$ ÿ—É  H‹\$@H‹l$HH‹t$PHƒÄ0_ÃÌÌÌÌÌÌÌÌÌÌ@SHƒì H‹Ùÿ!É  D‹Â= WÀeH‹%X   ¹   J‹ÂH‰3ÀH‰CHƒÄ [ÃÌÌH‰\$H‰t$H‰|$UH¬$0ıÿÿHìĞ  ‹ÚL(÷  D‹ÊH>Š  3ÒÿÇ  HO< ‰=< H‹êÆ  3ÒH‰€   A¸Ğ   A€HI@ A H0I°@@AÀHPIĞ@`Aà@pAğˆ€   	€   Aˆ    I €°   A0H‹€À   H‰A@H÷8  H‰¸; H   HR9  H‰³; èR°  H‹È  Hhî  A°º   H‹ ÿ?É  H‰…  H   HòÖ  H‰…¸  HÄ9  H‰…`  ÿŸÇ  D‹ËL5î  Hv'  H‰'; 3ÒƒH ÿÛÅ  3ÒHL$0A¸Ğ  ‰ ; èÇ¯  H‹ŒÇ  H•ß  A°º   H‹ ÿ´È  H‹õÆ  HL$03ÿH‰D$8H‰½è   H‹ÿÇ  3ÒHL$0A¸Ğ  H‰Õ; ƒH èh¯  H‹-Ç  WA°H@ß  H‹ ÿWÈ  HL$0H‰½è   H‰D$8ÿĞÆ  H‘E  3ÒA¸Ğ  H‰š; ƒH H‰ˆh  HL$0è	¯  H‹ÎÆ  WA°HQÙ  H‹ ÿøÇ  H‰D$8H=Ä  HÍĞ  H‰½   HL$0H‰…è   ÿ\Æ  H‹-Æ  H5Ö  3ÒH‰; A¸Ğ  H‰ˆx  H‹ñÅ  H‰ˆ€  HL$0H‰°h  è}®  H‹BÆ  H»Ø  A°º   H‹ ÿjÇ  H‹Ë: HL$0H‰D$8HšÔ  H‰…è   H‰½   ÿ®Å  H‹ŸÅ  3ÒA¸Ğ  H‰ˆx  H‹qÅ  H‰ˆ€  HL$0H‰°h  èı­  H‹ÂÅ  H?Ø  A°º   H‹ ÿêÆ  H‰D$8H†Í  H‰…è   H‰½   H‹1: HL$0ÿ.Å  H‹Å  ‹ÓH‰ˆx  H‹÷Ä  H‰ˆ€  HÍ  H‰°h  ÿäÄ  A¹   ‰\$ E‹ÁHpİ  º   ÿÅ  º   ‰\$ Hmİ  DJìDBíÿïÄ  º   ‰\$ Hgİ  DJîDBñÿÑÄ  º   ‰\$ Haİ  DJíDBüÿ³Ä  º   ‰\$ H[İ  DJôDBÿ•Ä  º
   ‰\$ HMİ  DJ÷DBÿwÄ  A¹   ‰\$ E‹ÁH;İ  º   ÿXÄ  A¹   ‰\$ E3ÀH$Ø  AQÿ:Ä  º   ‰\$ H
İ  DJûDBÿÄ  º   ‰\$ HôÜ  DJüDBÿşÃ  º   ‰\$ HŞÜ  DJûDBÿàÃ  º   ‰\$ DJüDBıHÀÜ  ÿÂÃ  º   ‰\$ H²Ü  DJûDBÿ¤Ã  º   ‰\$ HœÜ  DJüDBşÿ†Ã  º   ‰\$ H†Ü  DJüDBÿÿhÃ  º   ‰\$ D‹ÂHmÜ  DJüÿKÃ  º   ‰\$ H[Ü  DJúDBÿ-Ã  º   ‰\$ HEÜ  DJùDBÿÃ  º   ‰\$ H3Ü  DJüDBÿñÂ  º   ‰\$ D‹ÂHÜ  DJúÿÔÂ  º   ‰\$ HÜ  DJúDBÿ¶Â  º
   ‰\$ HöÛ  DJ÷DBÿ˜Â  º
   ‰\$ HèÛ  DJ÷DBÿzÂ  º   ‰\$ HÚÛ  DJöDBşÿ\Â  º
   ‰\$ HÌÛ  DJ÷DBÿ>Â  ‰\$ º   HºÛ  DJûDBÿ Â  º   ‰\$ H¨Û  DJúDBÿÂ  º   ‰\$ A¸    HŒÛ  DJùÿâÁ  º   ‰\$ A¸ @  H|Û  DJùÿÂÁ  A¹   ‰\$ E3ÀHšÕ  AQÿ¤Á  º   ‰\$ HTÛ  DJùDBúÿ†Á  A¹   ‰\$ E‹ÁHBÛ  º   ÿgÁ  º   ‰\$ A¸èı  H1Û  DJúÿGÁ  º   ‰\$ A¸éı  HÛ  DJúÿ'Á  º	   ‰\$ HÛ  DJøDB!ÿ	Á  º   ‰\$ HùÚ  DJôDBöÿëÀ  A¹   ‰\$ E3ÀHçÚ  AQÿÍÀ  A¹   ‰\$ E‹ÁHÙÚ  º	   ÿ®À  º	   ‰\$ DJøDBùHÆÚ  ÿÀ  º   ‰\$ HÀÚ  DJöDBøÿrÀ  º   ‰\$ A¸   HLÓ  DJìÿRÀ  A¹   ‰\$ E‹ÁHÚ  º   ÿ3À  º   ‰\$ HƒÚ  DJîDBïÿÀ  º   ‰\$ H}Ú  DJïDBòÿ÷¿  º   ‰\$ A¸   HqÚ  DJñÿ×¿  º   ‰\$ A¸   HiÚ  DJîÿ·¿  º   ‰\$ A¸ €HaÚ  DJñÿ—¿  º   ‰\$ A¸
 €HYÚ  DJòÿw¿  º   ‰\$ A¸ €HIÚ  DJòÿW¿  º   ‰\$ A¸ã€H9Ú  DJñÿ7¿  º   ‰\$ H3Ú  DJûDBÿ¿  º   ‰\$ HÚ  DJüDBÿû¾  A±LÉG  3ÒHP3 ÿj¿  ÿÄ¼  Lœ$Ğ  H‰-3 I‹[3ÀI‹sI‹{ I‹ã]ÃÌÌH‰\$WHƒì ‹Êÿ>¾  eH‹%X   ‹73 º   H‹<È‹83 HúÿÈHcĞH‹H‹H‹ÑH‹XH…ÛthH‹KH…ÉtH‹H‹@ÿ·¿  H‹H…Ét$H‹H‹@Xÿ¢¿  H‹H‹H‹@ÿ’¿  HÇ    H‹ËÿÚº  ‹Ì2 ÿÈHcĞH‹H‹H‹ÑHÇ@    Hf2 ÿ`½  H‹Q2 ÿÃ»  H‹\$03ÀHƒÄ _ÃÌÌÌÌÌÌ‹r2 eH‹%X   º   H‹ÈH‹‹f2 ÿÈHcÈH‹H‹È3ÀÆA ÃÌÌÌÌÌÌÌÌÌÌÌH‰\$WHƒì eH‹%X   ‹2 D‹(2 º   H‹<ÈHúA@ÿHcÈH‹H‹H‹ÈH‹YH…Ût(H‹KH…ÉtH‹H‹@ÿœ¾  H‹HÇC    D‹Ú1 H‹\$0A@ÿHcÈH‹H‹È3ÀÆAHƒÄ _ÃÌÌÌH‰\$WHƒì H‹Ùÿ•¼  H=Ø  ¹   L‹ÇHØ  ÿ™¼  ‹s1 LØ  eH‹%X   ¹   A¹   H‹ĞM‹‹Z1 ÿÈHcĞI‹H‹Ğ€: HÜ×  LEÇÿJ¼  L‹ÇHØ×  ¹   ÿ5¼  ÿŸ¼  H‹ËH‹\$0HƒÄ _Hÿ%3¼  ÌÌÌH‰\$H‰t$WHƒìpH‹œ$    H‹òH‹ùÇC   fƒy8	…’   HL$Pÿş·  L‹FHV3ÀÇD$8   H‰D$0A¹   ‰D$(H‹ÏHD$PH‰D$ è³Şÿÿ…ÀuD‹GTHT$PH‹ËènW  HL$Pÿ“·  ëL= €uE‹FLD$@ÁàH‹Ó÷ĞH‰t$@%   H‹ÏƒÈ‰D$HèQ7  ëH‹H0 H1Ş  A¸W €ÿ%º  L\$pH‹ÃI‹[I‹sI‹ã_ÃH‰\$H‰t$WHƒì`fƒy8	I‹ØH‹òH‹ùubHL$@ÿ·  L‹FHD$@ÇD$8    HVH‰\$0A¹   ÇD$(   H‹ÏH‰D$ èÑİÿÿ…Àu8HL$@ÿÂ¶  H‹ÃH‹\$pH‹t$xHƒÄ`_ÃH‹˜/ Hİ  A¸W €ÿu¹  H‹t$xH‹ÃH‹\$pHƒÄ`_ÃÌÌL‹ÜI‰[I‰sI‰{ AVHƒìpAÇA   I‹Ù·A8H‹òH‹ùfƒø	…½   IKÈI‰kÿV¶  E3ÀAPJÿ^»  D‹GTH‹ÖH‹ÈH‹èèÌQ  3ÀHÇD$h   ‰D$0LL$X‰D$(3ÒH‰D$`H‹ÏD@H‰l$XHD$@H‰D$ è’Ğÿÿ‹ğH…ítH‹ÍÿÒµ  H‹Íÿqº  H‹¬$€   ş €„¢   …öˆš   D‹GTHT$@H‹ËèlU  HL$@ÿ‘µ  ë|fºàs[H‹Îÿ¸  H‹O@ÿµ  ƒøu3D‹HT$@HO8è  …ÀtHD‹GTHT$@H‹Ëè
  HL$@ÿ?µ  ë*L‹ÆH‹ÓH‹Ïè5  ëH‹. HÜ  A¸W €ÿó·  L\$pH‹ÃI‹[I‹s I‹{(I‹ãA^ÃÌÌÌÌÌÌÌÌÌ@USVWHl$ÁHì¨   I‹ğH‹úH‹ÙH…ÒuA¸ €HìÛ  éJ  ·A8fƒø	…)  H‹HM×H‰E‹B‰EI‹ H‰EA‹@L‰¤$    L‰´$˜   ‰EL‰¼$   ÿŒ´  E3ÀA¾   A‹ÖAHÿ¹  HuE‹şL‹àH‹øD‹CTH‹ÖH‹ÏèñO  HƒîHƒÇIƒïuãHED‰uÿH‰E÷EG3ÀL‰eï‰D$0LMï‰D$(3ÒHE×ÇEıÿÿÿH‹ËH‰D$ ÇE   è ÎÿÿL‹¼$   ‹øM…ätI‹ÜH‹ËÿÕ³  HƒÃIƒîuíI‹Ìÿj¸  L‹´$˜   L‹¤$    ÿ €„>  …ÿˆ6  HM×ÿ”³  HÄ¨   _^[]Ãfºàƒû   H‹I@ÿ³  ƒø…Ù   H‹K@HUgÿ³  …Àxfƒ}g u·C8¹ÿß  f#Áf‰EgH‹ÏÿH¶  ‹HM×‰EoÿA³  D‹CTHM×H‹ÖèÉN  ·Egf9E×tE3ÀHU×D·ÈHM×ÿ"³  ·EgH‹K@HUoLE×fƒøtLEßÿÊ²  HM×‹øÿÎ²  …ÿyb®è‹Ïÿ'¸  H‹¨+ H‹ĞD‹ÇH‹Øÿ‰µ  ®èH‹Ëÿ­·  HÄ¨   _^[]ÃA¸ €HÔÙ  ëA¸W €HuÙ  H‹^+ ÿHµ  HÄ¨   _^[]ÃÌÌÌÌÌÌÌÌÌÌÌÌ3ÀÃÌÌÌÌÌÌÌÌÌÌÌÌÌHƒì(fƒy8	L‹Âu M‹@LL$0HƒÂèPÏÿÿ…Àx
¸   HƒÄ(Ã3ÀHƒÄ(ÃÌÌÌÌÌÌÌÌÌÌÌHƒì(H­Ù  3Éÿå´  3ÀHƒÄ(ÃÌÌÌÌÌÌH¹Ù  3ÉHÿ%È´  HÙÙ  3ÉHÿ%¸´  H‹Qµ  ÃÌÌÌÌÌÌÌÌ3ÀH‰A‰ ÃÌÌÌÌÌÌÌ@SHƒì H‹H‹KöA@uƒÿuÿ*¶  H‹K(H…Étÿ¶  H‹ËHƒÄ [Hÿ%¶  ÌÌÌÌL‹ÜSAWHƒìXE3ÿI‰k€y(HA I‰{H‹ÙIEÇM‰sèL‹òA‹ÿH‹(fƒ}8	…‚   I‰sHcq,…ötH‹ÖAOE3Àÿ8¶  H‹Ğ‹ÎH‹øÿÚµ  HL$0ÿ±  H‹SLL$0H‹ÍH‰|$(‰t$ èÕÏÿÿH‹t$x…ÀuD‹ETHT$0I‹Îè‹P  HL$0ÿ°°  H…ÿt	H‹ÏÿJµ  H‹CL‹t$PH‹¼$€   H‹l$pH‹H‹A¨@uƒÿu„Àyÿb±  ëÿµ  ‹<) eH‹%X   L‹Cº   H‹ÈH‹H‹‹µ  H‹H¨  HÂL;ÀuL‰¼
°  L‰{HƒÄXA_[ÃI‹Èÿ¼´  L‰{HƒÄXA_[ÃÌÌÌÌÌÌÌÌ@USVAVHl$ØHì(  H‹ H3ÄH‰EH‹H‹òfƒ{8	…„  L‹BLL$`HƒÂH‹ËèêÌÿÿ…Àˆg  Hƒ»ˆ    H‰¼$`  t'®èH‹‹ˆ   H‹Öÿ5³  H…Àt®èH‹8H…ÿ…Í  L‰¼$   ®èWÀ3ÀE3ÿH‰E öF@EÆEE°D‰}°E L‰}¸EÀEĞEàEğH‹C`H‰E ÇE”   uÿH¼ıÿÿH‰u˜H‰EÈH}L9{X„•  ®èH‹KXHT$XL‰|$HH‹H‹@ ÿ€´  …Àˆo  ®èD‹CTHNH‹VèÌ  L‹Àº   ¹   L‹ğÿV®  H‹L$XA¹   D‹ÀH‹H‹BHT$@H‰T$0HT$PH‰T$(HT$HH‰T$ I‹Öÿ´  …Àˆæ   ‹L$Pƒét>ƒét ƒù…¸   H‹L$@H‹H‹@ÿá³  é¡   H‹L$HH‹T$@H‹H‹€¨   é‚   H‹D$@º    H¿H$ÿÖ±  H‹T$@E‹ÏH‰E¸fD;z$}DM‹ÇH‹JM@ L‰|$hAÿÁB·TøÑêƒâÁâ‰T$pD$hAD èH‹T$@¿B$D;È}H‹E¸ë¿¿B$H‹L$H‰E°H‹H‹€    ÿ;³  H‹L$HH…ÉtH‹H‹@ÿ$³  H‹L$XH‹H‹@ÿ³  I‹Îÿ²  HMÿo°  L‹³ˆ   L‹¼$   M…öu/AN8ÿá±  E3ÉLŸûÿÿAVH‰ƒˆ   H‹Èÿë±  L‹³ˆ   AöF€¹x   tÿR²  ëÿ¢±  (ELD$x H‹ÖI‹Î(M HH‰D$x(E°@ ÇE€   (MÀH0(EĞ@@(MàHP(Eğ@`òM òHpÿ<°  H‹Gö@@uÿ ¹x   ÿ-±   OHG @ O0H0G@@@OPHPG`@`òOpH‹¼$`  òHpë3ÀH‹MH3Ìèİ‹  HÄ(  A^^[]ÃH‹A`H‹@ö@@uÿ ÃÌÌÌÌÌÌÌÌÌÌÌÌÌÌÌHƒì(€yun€zuhL‹L‹	I‹BM‹AH‹€¸   I9€¸   uJII8A¸   E3ÉIR8ÿ««  …Àt%ƒètƒøt
¸şÿÿÿHƒÄ(Ã¸   HƒÄ(Ã3ÀHƒÄ(Ã¸ÿÿÿÿHƒÄ(ÃHƒÄ(Hÿ%U¯  ÌÌÌÌÌÌÌÌÌÌÌÌÌH‰\$H‰l$VWAVHìÀ   H‹ñÇB   HL$PE‹ğH‹ê3Ûÿf«  fƒ~8	…”   H‹N@H”$ğ   H‰T$@DCWÉH‰\$xŒ$€   H”$€   A¹   H‰T$8WÀóD$hHT$PH‰T$0HT$hŒ$   H‰T$(3ÒŒ$    fD‰D$ Lÿ³  Œ$°   H‹H‹@0ÿR°  = €t…ÀyHV8HL$Pÿ˜ª  AFşƒøwSH°¨ÿÿH˜‹Œ‚èW  HÊÿá¸   ë¸   ë¸   ë¸   f;D$PtE3ÀHT$PD·ÈHL$Pÿrª  ‹Ø…ÀxD‹FTHT$PH‹ÍèJ  HL$Pÿ(ª  …Ûx3ÀëE‹ÆH‹ÕH‹Îÿ­  Lœ$À   I‹[ I‹k(I‹ãA^_^ÃlW  lW  ^W  eW  sW  œW  œW  œW  œW  œW  œW  œW  œW  œW  œW  lW  ^W  ÌÌÌÌH‰\$WHƒì 3ÀH‹ú‰D$0H‹Ù‰D$@¸    f…A8u¸ÿÿÿÿH‹\$8HƒÄ _ÃH‹I@LD$@º   ÿ©  H‹K@LD$0º   ÿ‹©  ‹D$0+D$@H‹\$8ÿÀH˜H‰3ÀHƒÄ _ÃÌÌÌÌÌÌÌH‰\$WHƒì Hƒyh ‹úH‹Ù„™   H‹I@LD$@HF²  H‹H‹ ÿ’®  …ÀxyH‹L$@HSpLD$0H‹H‹@ ÿs®  …ÀxH®èH‹L$0H‹…ÿtH‹ShLƒ€   H‹@(ÿK®  ë‹“€   H‹@0ÿ9®  H‹L$0H‹H‹@ÿ'®  H‹L$@H‹H‹@ÿ®  H‹\$8HƒÄ _ÃÌÌ@SHƒì H‹ÙH‹IXH…ÉtH‹H‹@ÿé­  HÇCX    Hƒ{h „—   H‹K@LD$8Hn±  H‹H‹ ÿº­  …Àx^H‹L$8HSpLD$0H‹H‹@ ÿ›­  …Àx-®èH‹L$0‹“€   H‹H‹@0ÿ|­  H‹L$0H‹H‹@ÿj­  H‹L$8H‹H‹@ÿX­  H‹KhH‹H‹@ÿG­  HÇCh    HK8ÿ§  H‹‹ˆ   H…Étÿ+«  H‹‹ˆ   ÿ&¬  H‹‹   H…Étÿ«  H‹‹   ÿ¬  H‹ËHƒÄ [Hÿ%«  H‰\$H‰t$WHƒì H‹ù¹˜   ÿË«  H‹ğ HH8OHG @ O0H0G@@@OPHPG`@`OpHp‡€   €€   ò   òˆ   ÿâ¦  HW8HN8ÿt¦  H‹NXH…ÉtH‹H‹@ÿ>¬  H‹\$0H‹ÆH‹t$8HƒÄ _ÃÌÌÌH‰\$WHƒì H‹Ùÿ«  ¹˜   ÿ
«  H‹øWÀ3ÀHO8GG G0G@GPG`Gp‡€   H‰‡   ÿK¦  H‹ÓÇGT    H‹ÏH‰_`ÿ©  H‹\$0H  H‰GH‹ÇHÇGX    HƒÄ _ÃÌÌÌÌÌÌÌÌÌÌÌ@SHƒì H‹Y8H‹KXH…ÉtH‹H‹@ÿh«  HKhÿÆ¥  H‹ˆ   ÿ¹¥  H‹¨   HƒÄ [Hÿ%öª  ÌÌÌÌÌÌH‹A8¶ˆ°   öÙÀ÷ØÿÈÃÌÌÌÌÌÌÌÌÌÌÌÌH‹A8H¨   ÃÌÌÌÌÌH‹A8H‹H`HƒùÿuÇB   ÃH‰
ÇB   ÃÌÌÌÌÌÌÌÌÌÌÌÌÌÌÌH‰\$WHƒì@H‹Y8HKhÿ(¥  €»°    tH‹¨   ÿbª  Çƒ°       H‹KXH…Ét,H‹LL$PLChº   H‹@ÿ|ª  …Àxyƒ|$P vrHÿC`ë.H‹K`‹ƒ¤   H;Ès]HAD‹ÀH‰C`H‹ˆ   HShèY  …Àt>ÇD$8   HShD‹ƒ€   HL$0èXD  H‹D$0‹L$8H‰ƒ¨   ‰‹°   H‹\$XHƒÄ@_ÃHÇC`ÿÿÿÿH‹\$XHƒÄ@_ÃÌÌÌÌ@UATHl$±HìÈ   E3äL‰e×E…ÀtHmÌ  3Éÿ§  3ÀHÄÈ   A\]ÃH‰œ$à   L‰¬$°   A½    L‰´$¨   L‹2A·F8ƒø	t$fA…ÅuD‹ÈLUÌ  º   3ÉÿÀ¨  3Àéí   H‰´$À   HMïWÀH‰¼$¸   3ÀL‰¼$    EH‰Eÿ²£  º¸   ¹   ÿJ§  H‹ÈH‹øÿf¦  H—¶  H‰8H‰GHA‹NT‰€   Hˆ   D‰§°   ÿi£  HOhÿ_£  I‹N@fE…n8„õ   ÿÚ¢  ƒøttD‹ÈLÃË  º   3Éÿ¨  Hˆ   ÿ	£  HOhÿÿ¢  H‹Ïÿ§  3ÀH‹´$À   H‹¼$¸   L‹¼$    L‹¬$°   H‹œ$à   L‹´$¨   HÄÈ   A\]ÃIV8Hˆ   ÿ ¢  H‹   LEwº   ÿB¢  H‹   L‡¤   º   ÿ±¢  D‹EwD9‡¤   Œ   HWhHˆ   è  …À„  HcEwH‰G`éÄ   H‹HUïL‰d$@A¸   L‰d$8A¹   H‰T$0HUH‹@0H‰T$(ºüÿÿÿfD‰D$ L2«  ÿ”§  …ÀˆŞşÿÿ·Eïfƒøtfƒø	uH‹M÷LE×Hõª  H‹H‹ ÿa§  HMïÿ¿¡  H‹E×H…À„œşÿÿH‰GXLMH‹M×LGhº   H‹H‹@ÿ(§  …ÀxDD9ev>®èL‰g`ÇEç   HWhD‹‡€   HMßè4A  H‹Eß‹Mç‰°   H‰‡¨   H‹ÇéTşÿÿHÇG`ÿÿÿÿH‹ÇéDşÿÿÌÌÌÌÌÌÌÌH‰\$H‰t$H‰|$AVHƒì0L‹ñA‹Ø¹˜   H‹úÿŸ¥  H‹ğWÀ3ÀHN8FF F0F@FPF`Fp†€   H‰†   H‹Ç H‰F`H‰F‰^TÿÎ   H‰~@¸	   f‰F8H‹ÏH‹H‹@ÿ1¦  H‹N@LNXE3À3ÒH‹H‹@ ÿ¦  H‹x H‹Îÿ_£  H‹\$@Hs H‹|$PH‰FI‰6H‹t$HAÇF  HƒÄ0A^ÃÌÌÌH‰\$H‰t$H‰|$AVHƒì0L‹ñA‹Ø¹˜   H‹úÿ¯¤  H‹ğWÀ3ÀHN8FF F0F@FPF`Fp†€   H‰†   H‹× H‰F`H‰F‰^TÿŞŸ  H‹×HN8ÿqŸ  fƒ~8	ÇFP    uH‹N@H…ÉtH‹LNXE3À3ÒH‹@ ÿ$¥  H‹… H‹Îÿl¢  H‹\$@H€ H‹|$PH‰FI‰6H‹t$HAÇF  HƒÄ0A^ÃH‰\$H‰t$WHƒì0¸    A‹ğH‹úH‹Ùf…„  H‹IÿÈ  ƒøt)D‹ÈL±Ç  º   3Éÿô£  3ÀH‹\$HH‹t$PHƒÄ0_ÃH‹KHT$@ÿ£  …Àxfƒ|$@ u·¹ÿß  f#Áf‰D$@H‹KLD$ º   ÿc  H‹KLD$$º   ÿ×  ;t$ |k;t$$eH‹Ïÿ¢  ·D$@HT$X‰t$Xfƒøu"H‹KL‹Çÿ  ¸   H‹\$HH‹t$PHƒÄ0_Ãf‰LGH‹KÿÛ  ¸   H‹\$HH‹t$PHƒÄ0_ÃH‹ HÇ  A¸ €ÿô   H‹\$H3ÀH‹t$PHƒÄ0_ÃÌÌH‰\$H‰l$VAVAWHƒì0E3ÿ¾	   Aøéı  A‹èH‹ÚL‹ñAD÷HƒúÿuD‹ÊD‰|$(L‹ÁL‰|$ ‹Ö‹Íÿiœ  HcØëHÿÃH‰|$PH…ÛtCE3ÀH‹ËAPÿ¿¢  D‹Ë‰\$(M‹ÆH‰D$ ‹Ö‹ÍH‹øÿ+œ  …À~HcÈH;ËsNfD‰<O…ÀtëC¹   ÿé¡  H‹øfD‰8ÿœ  ‹Èÿ¤¢  L5Æ  º   L‹È3ÉH‹Øÿ¢  H‹Ëÿ)¢  H‹\$XH‹ÇH‹|$PH‹l$`HƒÄ0A_A^^ÃÌÌÌÌÌÌÌÌÌÌÌÌÌH‹ÄUVWHƒì@H‰XH‹ò3ÛL‰pH‰XàE‹ğH‰XØL‹ÁL‰x3ÒL‹ù‰XĞDKÿH‰XÈA‹Îÿ~›  ‹è…ÀtC‹ÍSE3ÀÿÊ¡  H‰\$8DKÿH‰\$0M‹Ç‰l$(3ÒA‹ÎH‰D$ H‹øÿB›  …À‹è•Ã…ÛuEë¹   ÿò   H‹ø‹ëˆÿ›  ‹Èÿ­¡  LnÅ  º   L‹È3ÉH‹Øÿ¡  H‹Ëÿ2¡  L‹|$pH‹ÇL‹t$hH‹\$`H…ötMÿH‰HƒÄ@_^]ÃÌÌÌÌÌÌÌÌÌÌÌÌÌ@SHƒì H‹Yƒ{ ~3ÒH‹Ëÿ…  H‹KH‹Ixÿ'  H‹ËHƒÄ [Hÿ%˜  ÌÌÌÌÌÌÌÌH‰\$H‰t$WHƒì I‹ğH‹ÚH‹ùÿjš  ;Gt¸€H‹\$0H‹t$8HƒÄ _ÃH‹!¥  H;uH‹¥  H;CtH‹Ø¤  H;u&H‹Ô¤  H;CuH‰>ğÿG3ÀH‹\$0H‹t$8HƒÄ _ÃH‹\$0¸@ €HÇ    H‹t$8HƒÄ _ÃÌ@SHƒì H‹ÙÿÙ™  ;Ct¸€HƒÄ [Ã¸   ğÁCÿÀHƒÄ [ÃÌÌÌÌÌÌÌÌÌÌÌÌÌÌÌ@WHƒì H‹ùÿ™™  ;Gt¸€HƒÄ _ÃH‰\$0»ÿÿÿÿğÁ_ƒëuH‹OH…Étÿîœ  ‹ÃH‹\$0HƒÄ _ÃÌH‰\$H‰l$H‰t$WHƒì I‹ÙA‹ğH‹êH‹ùÿ2™  ;Gt¸€ë(H‹OL‹ÆH‹Õÿ  H‹Ğ3ÀH…Ût	…Ò‹ÈEÊ‰…Ò”ÀH‹\$0H‹l$8H‹t$@HƒÄ _ÃÌH‰\$H‰l$H‰t$WHƒì I‹ÙA‹ğH‹êH‹ùÿÂ˜  ;Gt¸€ë*H‹OL‹ÆH‹Õÿ†œ  H‹Ğ3ÀH…Ût
H…Ò‹ÈOÊ‰H…ÒÀH‹\$0H‹l$8H‹t$@HƒÄ _ÃÌÌÌÌÌÌÌÌÌÌÌÌÌÌÌH‰\$H‰T$UVWHƒì I‹ñA‹øH‹ÚH‹éÿE˜  ;Et¸€H‹\$PHƒÄ _^]ÃL‰t$@E3ö…ÿtƒïtƒÿuEFëA¸   ëE‹ÆD9t$Lt¸ €ë-H‹MHcÓÿÄ›  ‹ĞH…ötA‹Î…ÀIÈHcÉH‰…Ò¸ €AIÆL‹t$@H‹\$PHƒÄ _^]ÃÌÌH‰\$H‰T$WHƒì H‹ÚH‹ùÿ—  ;Gt¸€H‹\$0HƒÄ _Ãƒ|$< u4H‹OE3ÉE3ÀAQ
ÿ›  …ÀuH‹OH‹ÓÿT›  …ÀuH‹\$0HƒÄ _ÃH‹\$0¸ €HƒÄ _ÃÌÌÌÌÌ@SHƒì H‹Ùÿ)—  ;Cº@ €¹€EÑ‹ÂHƒÄ [ÃÌÌÌÌÌÌÌÌÌ@SHƒì H‹Ùÿù–  ;Ct¸€HƒÄ [ÃH‹K3Òÿ•š  3ÀHƒÄ [ÃÌÌÌÌÌÌÌÌÌÌÌÌÌ¸ €ÃÌÌÌÌÌÌÌÌÌÌH‰\$WHƒì H‹ù¹    ÿˆ™  H‹ØH…ÀuH‹\$0HƒÄ _ÃWÀ @ÿ}–  ‹_ H‹Ë‰CHr H‰ÇC   H‰{H‹Gxÿ ÿš  H‰CH‹ÃH‹\$0HƒÄ _ÃÌÌÌÌÌÌ@SUAVHƒì 3íHA €y(L‹òH‰l$@HEÅH‹9i,tÿœ  HƒÄ A^][ÃL‹K@M…Éu#H‹
 HÁ  A¸W €ÿç™  HƒÄ A^][ÃH‹KXH‰t$PH‰|$XH…Éu5I‹LCXH}   I‹ÉH‹ ÿiœ  H‹KX‹ğH…Éu‹Îÿ&œ  D‹Æé³   H‹HT$@H‹@@ÿ<œ  ‹ø…ÀuvD‹C8HT$HH‹L$@è›ùÿÿH‹|$HH‹ğHO Hƒáøÿ›  L‹ÇH‹ÖH‹ØHHÇ    Ç@   H‰hH‰xèüƒ  @ˆl;H‹ÎI‰AÇF  ÿİš  H‹L$@ÿ
˜  ëAƒÿuH‹L$@ÿø—  AÇF   ë'‹Ïÿn›  L‹ÇH‹ì H‹ĞH‹ØÿĞ˜  H‹Ëÿ÷š  H‹t$PH‹|$XHƒÄ A^][ÃÌÌÌÌ@UWHƒìHH‹úÆD$`3ÒHA €y(LL$x‹I,LD$pHEÂH‰T$hHÊ¿  H‹(HD$`H‰D$ ÿ¿š  ƒøÿ„Ë  L‹M@M…Éu!H‹^ H_¿  A¸W €ÿ;˜  HƒÄH_]ÃH‹MXH‰\$@H‰t$8L‰t$0H…ÉuJI‹LEXHÎ  I‹ÉH‹ ÿºš  H‹MXD‹ğH…Éu$A‹Îÿuš  H‹ö E‹ÆH‹ĞH‹Øÿ×—  é'  H‹D$pH…Àtc3ÒH‹Èÿ—  H‹ØH…ÀtH‹Èÿt—  …ÀtH‹Ëÿg™  ÇG   éñ   HÇÂÿÿÿÿHÿÂ€< u÷D‹E8H‹Ëè—öÿÿH‹ËH‰D$hÿ1™  H‹MXH‹D¶D$`H‹T$hH‹@0ÿš  ‹ø…ÀxaH‹T$hH…Òu?H‹MXHT$hH‹H‹@@ÿß™  ‹ø…Àu;H‹MXH‹T$hH‹H‹@8ÿÃ™  H‹L$hÿ –  ë\€|$` tH‹MXH‹H‹@8ÿ™  H‹T$hH…Òt	H‹Êÿ›˜  …ÿy-®è‹ÏÿL™  H‹Í H‹ĞD‹ÇH‹Øÿ®–  ®èH‹ËÿÒ˜  H‹t$8H‹\$@L‹t$0HƒÄH_]ÃÌÌÌÌÌÌÌÌÌÌÌÌ@SUHƒì8H‹ÚHA 3ÒLL$`€y(LD$X‹I,HEÂH‰T$PH§½  H‹(HD$PH‰D$ ÿ”˜  ƒøÿ„H  H‹M@H…Éu!H‹3 H4½  A¸W €ÿ–  HƒÄ8][ÃHƒ}X H‰t$hH‰|$0uCH‹LEXHªœ  H‹ ÿ™˜  Hƒ}X ‹øu#‹ÏÿX˜  H‹Ù D‹ÇH‹ĞH‹Øÿº•  é·   H‹L$X3Òÿˆ•  H‹øH…ÀtH‹Èÿ_•  …Àt!H‹ÏÿR—  H‹t$hH‹|$0ÇC   HƒÄ8][ÃHÇÂÿÿÿÿHÿÂ€< u÷D‹E8H‹ÏèvôÿÿH‹ÏH‹Øÿ—  H‹MXD‹D$PH‹H‹B(H‹Óÿé—  H‹Ë‹øÿî–  …ÿy-®è‹ÏÿŸ—  H‹  D‹ÇH‹ĞH‹Øÿ•  ®èH‹Ëÿ%—  H‹t$hH‹|$0HƒÄ8][ÃÌÌÌÌ@SUVHƒì 3íHA €y(H‹òHEÅH‹9i,tÿ±–  HƒÄ ^][ÃL‹K@M…Éu"H‹±
 H²»  A¸W €ÿ”  HƒÄ ^][ÃH‹KPH‰|$HH…ÉuqI‹LCPHúš  I‹ÉH‹ ÿ—  H‹KPH…ÉuNH‹KHL‰t$PH…Éu+L‹K@M…Ét"I‹LCHHàš  I‹ÉH‹ ÿÜ–  H‹KH‹øëH…É¿@ €EıL‹t$PH…Ét4H‹HT$@H‹@8ÿ­–  ‹ø…ÀuH‹D$@H‹|$HH‰ÇF   HƒÄ ^][Ã‹ÏÿS–  H‹Ô	 H‹ĞD‹ÇH‹Øÿµ“  H‹ËÿÜ•  H‹|$HHƒÄ ^][ÃÌÌÌÌÌÌÌÌÌÌÌÌÌÌÌ@VHƒì H‹òHA 3Ò€y(HEÂL‹ 9Q,tHƒÄ ^Hÿ%]•  M‹H@M…Éu H‹e	 Hfº  A¸W €HƒÄ ^Hÿ%<“  I‹HPH‰\$0IXPH‰|$8H…Éu#I‹H«™  L‹ÃI‹ÉH‹ ÿÄ•  H‹‹øH…ÉtH‹H‹@@ÿ­•  ‹ø…Àt7‹Ïÿo•  H‹ğ L‹ÇH‹ĞH‹ØÿÑ’  H‹ËH‹\$0H‹|$8HƒÄ ^Hÿ%è”  H‹\$0H‹|$8ÇF   HƒÄ ^ÃÌ@UVWHƒì HA 3í€y(LD$H‹I,H‹òHEÅH”ª  H‹8ÿÇ”  ƒøÿ„İ  H‰\$Pÿë‘  ‹ØÿÛ‘  H‹L$HHã¸  D‹ÀD‹Ëÿó‘  H…Àu'H‹? Hˆ¹  A¸W €ÿ’  H‹\$PHƒÄ _^]ÃH‹ÈèG÷ÿÿH‹ØH…Àu'H‹ Hi¹  A¸ÿÿ €ÿå‘  H‹\$PHƒÄ _^]ÃÇF   L‹O@M…ÉuiLD$@H‰l$@Hç—  H‹Ëÿ†  ‹ø…ÀˆÕ   ‹– eH‹%X   A¸   H‹ÑM‹‹‰ ÿÉHcÑI‹H‹ÑH‹ÎD‹BH‹T$@èTíÿÿé   H‹OPHwPH…ÉuqI‹HØ—  L‹ÆI‹ÉH‹ ÿñ“  H‹H…ÉuPH‹GHL‰t$XLwHH…Àu&H‹O@H…ÉtH‹H¼—  M‹ÆH‹ ÿ¸“  ‹øI‹ëH…À¿@ €EıL‹t$XH…ÀtH‹H‹H‹ÓH‹@(ÿ‰“  ‹øH‹H‹ËH‹@ÿw“  …ÿy-®è‹Ïÿ8“  H‹¹ H‹ĞD‹ÇH‹Øÿš  ®èH‹Ëÿ¾’  H‹\$PHƒÄ _^]ÃÌ@UWAVHƒì E3öHA €y(H‹êIEÆH‹8L9w@u#H‹e Hf·  A¸W €ÿB  HƒÄ A^_]Ã‹I,LD$@HF¨  ÿ|’  ƒøÿ„‰  H‰\$Hÿ   ‹Øÿ  H‹L$@H˜¶  D‹ÀD‹Ëÿ¨  H…Àu(H‹ô H=·  A¸W €ÿÑ  H‹\$HHƒÄ A^_]ÃH‹ÈèûôÿÿH‹ØH…Àu(H‹¼ H·  A¸ÿÿ €ÿ™  H‹\$HHƒÄ A^_]ÃH‹OPH‰t$PH…ÉusL‹O@M…ÉtI‹LGPHö•  I‹ÉH‹ ÿ’  H‹OPH…ÉuGH‹OHHwHH…Éu)L‹O@M…Ét I‹Há•  L‹ÆI‹ÉH‹ ÿÚ‘  H‹‹øëH…É¿@ €AEşH…ÉtH‹A¸   H‹ÓH‹@0ÿ¬‘  ‹øH‹H‹ËH‹@ÿš‘  H‹t$P…ÿy;®è‹ÏÿV‘  H‹× H‹ĞD‹ÇH‹Øÿ¸  ®èH‹ËÿÜ  H‹\$HHƒÄ A^_]ÃH‹\$HÇE   HƒÄ A^_]ÃÌÌÌÌÌÌÌÌÌH‰T$WHƒì L‹ HA 3ÒLD$8€y(‹I,HEÂH‰T$8Hİµ  H‹8ÿœ  ƒøÿtcH‰\$0H‹\$8H…ÛtOH‹fƒ{8	u+H‹K@H…Ét"H‰O@H‹H‹@ÿÄ  ‹CTH‹\$0‰G8HƒÄ _ÃH‹ H…µ  A¸W €ÿá  H‹\$0HƒÄ _ÃÌÌÌÌÌÌ@SHƒì H‹ÙH‹IXH…ÉtH‹H‹@ÿi  H‹KHH…ÉtH‹H‹@ÿS  H‹KPH…ÉtH‹H‹@ÿ=  H‹K@H…ÉtH‹H‹@ÿ'  H‹ËHƒÄ [Hÿ%8  H‰\$WHƒì H‹Ù¹`   ÿ   H‹ÈH‹ø KHC @ K0H0C@@@KPHPH‹Sÿ  H‹OXH…ÉtH‹H‹@ÿ¬  H‹OHH…ÉtH‹H‹@ÿ–  H‹OPH…ÉtH‹H‹@ÿ€  H‹O@H…ÉtH‹H‹@ÿj  H‹\$0H‹ÇHƒÄ _ÃÌÌÌÌ@SHƒì ¹`   ÿG  H‹p WÀH‹ÈH‹Ø @@ @0@@@PÿrŒ  HK H‰CH‹ÃHƒÄ [ÃÌÌÌÌÌÌÌÌÌÌÌÌÌÌ@SHƒì H‹\$PHŞµ  H‹7 A¸W €ÇC   ÿŒ  H‹ÃHƒÄ [ÃÌÌÌÌÌÌÌÌÌÌÌ@SHƒì H‹ Hœµ  I‹ØA¸W €ÿİ‹  H‹ÃHƒÄ [ÃÌÌÌÌ@SUVWATHì   ¿   H‹ÙA‰yI‹ñH‹A8L‹â·H8fƒù	…  HcK@WHÿÁE3ÀÿÛ  H‹è9{@v# L‹CHWÿD‹ÏHÒMÉÿÇCÈĞ;{@ràA‹L$WÿI‹$HÒH‰DÕ ‰LÕH‹KH€ytÿ°Š  „ÀuH‹ÍÿóŒ  éø  HL$hÿSˆ  H‹CH3ÿH‹K8A¹   ‰|$8H‰l$0L‹ ‹C@‰D$(HD$hH‰D$ IPM‹@è¯ÿÿH‹Í‹øÿŒ  …ÿu%H‹C8HT$hH‹ÎD‹@Tè­'  HL$hÿÒ‡  é  ÿ €…s  M‹ÄH‹ÖH‹Ëè£  é`  fºárH‹  HI´  A¸W €ÿmŠ  é:  I‹ÌL‰´$ˆ   ÿ‡Š  H‹C8H‹x@H‹ÏH‰|$Hÿ	‡  D‹ğ9C@rA¸W €H$´  éä  ®è‹S@L„$Ø   H‹Ïÿà†  ‹S@LD$@H‹ÏÿW‡  I‹$Hc„$Ø   H;ÈŒš  HcD$@H;ÈŒ  AFÿ‰D$D;C@…k  L‰¬$È   ®èHL$Pÿí†  E3ÀI‹ÎI‹îAPÿò‹  L‹èE…ötR3ÿL‰¼$€   D‹ÿL‹ğff„     H‹KHIÏÿ£‰  H‹CHMvIƒÇH‹LğA‰NüHƒíuÕH‹|$HL‹¼$€   ‹L$DH”$À   A‹$A‰D H‹Ïÿ
†  …Àx·„$À   f…ÀuH‹C8¹ÿß  ·@8f#Áf‰„$À   ®èI‹ÕH‹ÏfƒøuLD$Pë·„$À   LD$Xf‰D$Pÿ‹…  I‹Í‹øÿŠ  L‹¬$È   …ÿx"H‹C8HT$PH‹ÎD‹@Tè‡åÿÿHL$Pÿ¼…  ëd®è‹Ïÿ‹  H‹˜ş  H‹ĞD‹ÇH‹Øÿyˆ  ®èH‹ËÿŠ  HL$Pÿ‚…  ë*M‹ÄH‹ÖH‹Ëèb  ëA¸ €HK®  H‹Lş  ÿ6ˆ  L‹´$ˆ   H‹ÆHÄ   A\_^][ÃÌÌÌÌÌÌÌÌÌÌÌÌÌL‰D$SVWATHƒìxH‹ÙI‹øH‹I8L‹â·A8fƒø	…ù   HcK@E3ÀHƒÁAPÿ"Š  A¸   H‹ğD9C@v, A‹ÀAPÿHÁàHÒHCHAÿÀ‹HH‹ H‰Ö‰LÖD;C@r×A‹L$APÿI‹$HÒH‰Ö‰LÖ‹OH‹A‹ĞHÒH‰Ö‰LÖH‹KH€yt
ÿÓ†  „ÀtVHL$@ÿ„„  H‹CH3ÿH‹K8A¹   ‰|$8H‰t$0L‹ ‹C@ÿÀ‰D$(HD$@IPH‰D$ M‹@è0«ÿÿ…ÀuHL$@ÿ!„  H‹ÎÿÀˆ  HƒÄxA\_^[Ãfºàƒ   H‹I@H‰¬$¨   L‰l$pL‰|$`ÿƒ  E3À‹ÈD‹è‹èAPÿ‰  L‹øE…ítD3ÿL‰t$hD‹÷H‹ğH‹KHIÎÿÁ†  H‹CHHvIƒÆH‹Lğ‰NüHƒíuÖH‹¼$°   L‹t$hI‹Ìÿ†  A‹$AMÿH”$    A‰H‹K8H‹I@ÿƒ  L‹l$pH‹¬$¨   …Àxfƒ¼$     uH‹C8·H8¸ÿß  f#Èf‰Œ$    HL$@ÿ5ƒ  H‹C8HL$@H‹×D‹@Tè¸  ·„$    f9D$@tE3ÀHT$@D·ÈHL$@ÿ
ƒ  ·„$    H‹K8I‹×LD$@H‹I@fƒøtLD$Hÿ©‚  I‹Ï‹øÿV‡  HL$@ÿ£‚  L‹|$`…ÿyQ®è‹Ïÿ÷‡  H‹xû  H‹ĞD‹ÇH‹ØÿY…  ®èH‹Ëÿ}‡  HƒÄxA\_^[ÃH‹Lû  HE¯  A¸@ €ÿ)…  HƒÄxA\_^[ÃÌÌÌÌÌÌÌÌÌÌÌÌÌÌÌH‹1û  H‹@ö@@uÿ ÃÌÌÌÌÌÌÌÌÌÌÌÌ€yu*€zu$H‹L‹HH‹L‹@I‹€¸   I9¸   u¸ÿÿÿÿÃHÿ%¡…  ÌÌÌÌÌÌÌÌÌ¸ÿÿÿÿÃÌÌÌÌÌÌÌÌÌÌH‰\$WHƒì H‹Ù¸    H‹I8H‹úf…A8u¸ÿÿÿÿH‹\$8HƒÄ _Ã‹S@LD$@H‹I@ÿ/  H‹K8LD$0‹S@H‹I@ÿ¡  ‹D$0+D$@H‹\$8ÿÀH˜H‰3ÀHƒÄ _ÃÌÌÌÌÌÌÌÌÌÌÌÌÌ@SHƒì H‹ÙH‹I8ƒÿuÿpƒ  ë÷Aüÿÿuÿgƒ  H‹ËÿÆ„  H‹KHHƒÄ [Hÿ%¦…  ÌÌÌÌÌÌÌÌÌÌÌÌÌÌ@UAVAWHƒì L‹ù¹P   ÿp…  AE3ÀH‹è AOAPHAG @ AO0H0AG@@@H‹H8ÿHcH@ÿÃ…  H‰EHMcw@M…ö~fH‰\$@H‰t$HH‰|$P3ÿD  I‹OHH‹uH‹\9H‹9€ûuH‹È»  ÿe…  ë
÷Ã ÿ  tÿ H‰>‰\>HƒÇIƒîu¿H‹|$PH‹t$HH‹\$@H‹ÅHƒÄ A_A^]ÃÌÌÌL‰D$SWATAUHƒì(H‰t$XL‹ê3öL‰t$hH‹ÙL‰|$ M‹ğD‹şVPNÿ˜ƒ  H‹øÇ@@   H‹Ïø  H9CuH‹K8L‹ûH‰O8‹C@ÿÀ‰G@ëH‰_8H‹ËÿE3ÀHcO@APÿÂ„  H‰GHA¼  M…ÿt]H‰l$PIco@H…í~JD  I‹GHL‹wH‹\H‹€ûuA‹Üÿj„  H‹Èë
÷Ã ÿ  tÿI‰6A‰\6HƒÆHƒíuÀL‹t$`H‹l$PA‹NHc_@I‹L‹|$ L‹t$hH‹t$XHÁãH_H€ùuH‹Èÿ„  ë÷Á ÿ  tÿ D‹áH‰CğH‹ÏD‰cøH‹İ÷  ÿ¿  Hï  ¸   H‰OI‰} AÇE  HƒÄ(A]A\_[ÃÌÌÌÌÌÌÌÌ@SHƒì H‹Y8HKXÿÜƒ  H‹‹   ÿ'ƒ  H‹ËHƒÄ [Hÿ%ƒ  H‹Q83À‹Šˆ   9Š€   œÀÿÈÃÌÌÌÌÌÌÌÌH‰\$WHƒì@H‹y8H‹GxH‹   HcP@‹‡€   ‰D‘üHT$PH‹GxH‹H8H‹Y@H‹ËÿÏ}  …Àxfƒ|$P uH‹Gx¹ÿß  H‹P8·B8f#Áf‰D$PHL$ ÿ÷}  ·D$PH‹ËfƒøuLD$ ë
f‰D$ LD$(H‹—   ÿK}  H‹OxÇGp   H‹Q8HOhD‹BTHT$ èRİÿÿHL$ ÿ‡}  H‹\$XHGhHƒÄ@_ÃÌÌÌÌÌÌÌÌH‹A8Hcˆ€   ƒùÿuÇB   ÃH‰
ÇB   ÃÌÌÌÌÌÌÌÌÌÌÌÌÌH‹Q8A¸ÿÿÿÿÿ‚€   ‹‚€   ‹È;‚ˆ   AMÈ‰Š€   ÃÌÌÌÌÌÌÌH‰\$ WHƒì H‹:H‹ÚE…ÀtH4¥  3Éÿä  3ÀH‹\$HHƒÄ _Ãº˜   H‰l$0¹   ÿŠ€  H‹èE3ÀH=‘  H‰EHAPH‰m8H‰}xH‹ÿH‹H‰MXÇE`  ‹O@ÿÁHcÉÿ¼  H‰…   3Û‹W@…Ò~OH‰t$8‹óL‰t$@D‹óHcËHÁáHOHÿl  H‹GHMvH‹   HvÿÃA‹Dğ‰Dü‹W@;Ú|ÊL‹t$@H‹t$8H‹O8L…„   H‹I@ÿÈ{  H‹O8L…ˆ   ‹W@H‹I@ÿ8|  ‹…„   H‹\$H‰…€   H‹ÅH‹l$0HƒÄ _ÃÌHÑô  ÃÌÌÌÌÌÌÌÌH‰T$L‰D$L‰L$ SVWHƒì0H‹ÚHt$`H‹ùèÊÿÿÿH‰t$(L‹ËIÇÀÿÿÿÿHÇD$     H‹×H‹ÿÎ|  HƒÄ0_^[ÃÌÌÌÌÌÌÌÌÌÌÌÌÌÌ@USAVAWHl$èHì  H‹~é  H3ÄH‰EğD‹ò‰UE3ÿHS¨  LE°L‰|$hÿP  H‹ØH…À„Ğ  H‰´$P  LE°H‰¼$  H¨  3ÉL‰¬$   ÿ  LE°3ÉH ¨  H‹ğÿ  H‹øHÇÂÿÿÿÿHÿÂD8<u÷E‹ÆL‰¤$  H‹Ëè½ÜÿÿHUÈH‰EÀH‹ÈL‹èÿy|  …Àˆû   A¼   A·ÇH…ötH…ÿtH‹Îÿ\{  H‹ÏD·àÿO{  HL$hE3ÉH‰L$ D·ÀHMÈA·Ôÿxy  …À‰Î  H…öt	H…ÿ…À  3ÒL‰} HE L‰}˜L=ƒ  H‰D$ HMÈDBÿ|  …Àx;H‹M LM˜E3À3ÒH‹H‹@ ÿ”  …ÀxH‹M˜LE¸HT$hH‹H‹€   ÿs  H‹M˜H…ÉtH‹H‹@ÿ]  H‹M H…É„6  H‹H‹@ÿC  é$  ®èHT$hI‹Íÿ­x  …À‰  ®èHEˆA¹  E3ÀH‰D$ H‰¦  HÇÁ   €ÿœw  …À…Ú  H‹MˆHD$`3ÿE3ÉH‰|$XE3ÀH‰|$P3ÒH‰|$HH‰|$@H‰|$8H‰|$0H‰D$(HD$|H‰D$ ÿFw  …À…Œ  ®è‹D$`ÿÀ‹È‰D$`ÿ‘}  L‹øHÇÁÿÿÿÿHÿÁ@8<u÷HÿÁÿu}  D‹÷H‹ğ9|$|†/  D‹mA¼    Hƒ|$h …  D‹L$`M‹ÇH‹MˆA‹Öÿßv  …À…ç   H‹MˆHE€A¹  H‰D$ E3ÀI‹×ÿ°v  …À…À   H‹M€HD$pH‰|$XE3ÉH‰|$PE3ÀH‰|$H3ÒH‰|$@H‰|$8H‰|$0H‰|$(H‰D$ ÿav  …Àus9D$pvk€    H‹M€LEØA¹   ‹×ÿJv  …ÀuBHÇÀÿÿÿÿHÿÀ€< u÷H‹M€LM”ÿÀHUØL‹Æ‰E”ÿv  …ÀuH‹ÓH‹Îÿ<y  …À„Š   ÿÇ;|$prœ3ÿH‹M€ÿĞu  AÿÆD;t$|‚âşÿÿL‹mÀH‹Mˆÿ´u  I‹Ïÿ3|  H‹Îÿ*|  I‹Íÿ!|  H‹D$hL‹¬$   L‹¤$  H‹¼$  H‹´$P  H‹MğH3ÌèèV  HÄ  A_A^[]Ã3ÿLL$tLD$xH‰}¨HC¤  HMØèJûÿÿƒøtA‹Ä‹Ï‰D$x‰L$të‹L$t‹D$x‰L$(L¤  HM¨‰D$ M‹Ï3Òÿp{  H‹M¨A‹ÕèdûÿÿH‹M¨H‰D$hÿm{  éÿÿÿH‰L$UWATAUHl$ÁHìÈ   E‹àD‹êH‹ùH…ÉuAÿHÄÈ   A]A\_]ÃH‹H‰œ$ø   H‰´$À   L‰´$¸   H‹@ÿı{  E3ö‰EÏA‹Ş‹ğ‰]…À+  L‰¼$°   ®è‹ï  eH‹%X   HÑH‰Eß@ f„     H‹LEÇ‹ÓH‹ÏH‹@(ÿ¢{  ƒ}Ç …Ç  H‹LE·‹ÓH‹ÏH‹@ ÿ‚{  H‹M·LE¿3ÒE‹şH‹H‹@0ÿh{  …Àˆ~  „     H‹M·HUËH‰T$ LE×H‹U¿A¹   H‹‹H‹@8ÿ.{  ƒ}ËtH‹M·H‹U¿H‹H‹€¨   ÿ{  éü  H‹u×A¹ÿÿÿÿL‰t$8L‹ÆL‰t$03ÒD‰t$(A‹ÌL‰t$ ÿt  ‹Ø…ÀtIE3À‹ËAPÿbz  L‰t$8A¹ÿÿÿÿL‰t$0L‹Æ‰\$(3ÒA‹ÌH‰D$ H‹øÿØs  …ÀA‹Î‹ğ•Á…ÉuGë¹   ÿ…y  H‹øA‹öÆ  ÿs  ‹Èÿ>z  Lÿ  º   L‹È3ÉH‹Øÿ¬y  H‹ËÿÃy  H‹M×Dvÿÿµt  H‹U¿HMçE‹ÄH‹Rèa  A‹ÖH‹Ïÿ­v  H…Àt`H‹Mßº   H‹	L‹
‹Rí  ÿÉHcÑI‹H‹Ñ€z t*L‹ÀHUçHMÿ_v  …ÀuL‹ÏLq¡  P3Éÿy  H‹ÏÿÍx  é   €}ï…„   A¶ÅIN ‰EHƒáøH‹EçA¶õH‰E÷ÇEÿ   @€ætÿ(y  ëÿxx  H‹ØÇ    @¶ÆM‹ÆÁàH‹×ƒÈHK‰CHÇC    L‰sè]a  BÆD3 H‹ÏH‰]ÿDx  HM÷ÿÂu  H‹M·H‹U¿H‹H‹€¨   ÿy  E3öH‹M·LE¿AÿÇA‹×H‹H‹@0ÿôx  …À‰”ıÿÿ‹]H‹}g‹uÏH‹M·H‹H‹@ÿÑx  ÿÃ‰];ŞŒıÿÿL‹¼$°   L‹´$¸   3ÀH‹´$À   H‹œ$ø   HÄÈ   A]A\_]ÃÌÌÌÌÌÌÌÌÌÌÌÌH‹	H‹H‹@Hÿ%wx  ÌÌÌÌÌÌÌÌÌÌÌÌÌÌÌH‰\$H‰l$H‰t$H‰|$ AVHƒì0‹êH‹ùHÇÃÿÿÿÿHÿÃ€< u÷HK HƒáøÿÓw  L‹ÃH‹×H‹ğÇ    HHÇ@–   H‰XHÇ@    è`  H‹ùê  ÆD ÿt  H‹ÖHìê  ÿîu  H…ÀtH‹8H…ÿuYH‹VHNÿ£u  H‹È‹ÕH‹Øè®öÿÿH‹ËH‹øÿºv  H…ÿt=LD$ H‰|$ H‹ÖÇD$(   H‘ê  ÿu  H…ÀtHƒ8 tH‹H‹ÏH‹@ÿhw  H‹aê  ÿót  ‹F¨@uƒÿuH‹Î„Àyÿšr  ëÿJv  H‹\$@H‹ÇH‹|$XH‹l$HH‹t$PHƒÄ0A^ÃÌÌÌÌ@USVWAVHl$ÉHì    H‹ß  H3ÄH‰E/3ÛE‹ñH‰]ßI‹ğH‰]çH‹úH…Ò„ª  M…À…  E…É„Ç   fƒz8	…3  H‹J@LEÿHwz  H‹H‹ ÿ£v  …ÀxPH‹MÿLEVH‹H‹@ ÿ‡v  H‹Uÿ‹ØH‹
H‹AH‹Êÿqv  …ÛxH‹MçLEßHUH‹H‹@0ÿTv  é®  H‹O@LEïHøy  H‹H‹ ÿ4v  …Àˆ‹  Lõ  º   3Éÿpu  H‹MïH‹H‹@ÿv  éa  H‹JXH…ÉtH‹H‹@ÿìu  H‹GXé\  H‹J@LMßE3À3ÒH‹H‹@ ÿÉu  H9]ß…4  é  L‹JXM…ÉtI‹LE÷HUçI‹ÉH‹€   ÿ–u  ëH…Éuzfƒz8	usH‹J@LMßE3À3ÒH‹H‹@ ÿnu  H‹MßLE÷HUçH‹H‹€   ÿRu  H9]ç„½  H‹MßH‹H‹@ÿ7u  H‰]ßëH…Étº   èôÿÿH‰EçH…ö„t  H9]ç„€  HÇÂÿÿÿÿHÿÂ8uøE3ÀH‹ÎèYÑÿÿH‹MçLMßH‹øº   f‰U×E3ÀH‹H‹BXHU×H‰T$(HUH‰T$ H‹×ÿ¸t  …Àx
f9]×…   HUH‹Ïÿ­p  …Àˆë   H‹MçLEïHUH‹H‹@0ÿ|t  …ÀˆÊ   H‹MïHUÿH‹H‹@ÿ_t  H‹Uÿf;Z4ƒ‚   f„     H‹MïLE‹ÓH‹H‹@Hÿ1t  …ÀxJE…ötƒ}të=öEu7H‹MïLE÷‹ÓH‹H‹@@ÿt  …ÀxH‹MïLEß‹U÷H‹H‹@pÿçs  …ÀyH‹UÿÿÃ·B4;Ø|ëH‹UÿH‹MïH‹H‹€˜   ÿ»s  H‹MïH‹H‹@ÿªs  H‹Ïÿ±r  H‹MçH…ÉtH‹H‹@ÿ‹s  H‹EßH‹M/H3ÌèƒM  HÄ    A^_^[]ÃÌÌÌÌÌD‰D$H‰T$H‰L$USVWATAUAVHl$áHì°   H‹L‹ò3öHUÇI‹ÙA‹øL‹áD‹îH‹@ÿ!s  …Ày3Àé	  ®èM…öt"H‹EÇD‹@,AƒøtHœ  3ÉÿQp  éÏ  L‰¼$  H…Ût
H‹EÇ D‹}…ÿ„‘   I‹$LE·E3ÉH‰t$(I‹ÌH‰t$ H‹@`AQÿÿ£r  H‹M·HU÷E‹ÇèĞÿÿH‹M·H‹øÿşl  H‹MÇHU×ÿàn  H‹M×E‹Ç3ÒèâÏÿÿH‹M×H‹Øÿk  L‹ÃH‹š  H‹×ÿêo  H‹ËÿQq  H‹ÏÿHq  M…ötL‹äq  E3É3ÒI‹ÎÿFq  H‹EÇ‹Ö‰UÏf;p0ƒÏ   I‹$LE¯I‹ÌH‹@(ÿóq  …Àˆ¯  ®èH‹U¯‹B¨u	¨uD‹öëA¾   D;*„]  ‹
LE·I‹$‹Ñ‰M×E3ÉH‰t$(I‹ÌH‰t$ H‹@`ÿœq  H‹}·A¹ÿÿÿÿH‰t$8L‹ÇH‰t$03Ò‰t$(A‹ÏH‰t$ ÿ©j  ‹Ø…ÀtLE3À‹ËAPÿôp  H‰t$8A¹ÿÿÿÿH‰t$0L‹Ç‰\$(3ÒA‹ÏH‰D$ L‹èH‰Eïÿfj  …À‹Î‹ø•Á…ÉuJë¹   ÿp  L‹èH‰Eï‹ş@ˆ0ÿ*j  ‹ÈÿÊp  L‹”  º   L‹È3ÉH‹Øÿ8p  H‹ËÿOp  H‹M·ÿÏH‰}÷ÿ?k  9uo„×  H‹E¯E3À‰u¿H¿H$APHÿÁÿ:p  I‹$H‹ØH‹U¯L‹ÃH‰EÿH‹A8HM¿D¿J$‹AÿÁH‰L$ I‹Ìÿpp  H‹ÿßj  H‹E¯H´˜  ‹ÿôm  H‹M¯·Q8ƒútYH‹ÎH9™  t/H‹ÆL‰™  f„     fB9 „ß   HÿÁH‹ÁHÁàJ9t uäH5˜  D‹ÂHk˜  H‹Ğÿ’m  H‹M¯E…ö„.  I‹$LM·‹E3ÀH‰t$(I‹ÌH‰t$ H‹@`ÿÁo  H‹]·H…Û„è   H‰t$8A¹ÿÿÿÿH‰t$0L‹Ã‰t$(3ÒA‹ÏH‰t$ ÿÅh  D‹ğ…ÀtUE3ÀA‹ÎAPÿo  H‰t$8A¹ÿÿÿÿH‰t$0L‹ÃD‰t$(3ÒA‹ÏH‰D$ H‹øÿƒh  …À‹Î•Á…ÉuQëHÉI‹DÈé,ÿÿÿ¹   ÿ&n  H‹ø@ˆ0ÿBh  ‹Èÿân  L£’  º   L‹È3ÉH‹ØÿPn  H‹Ëÿgn  H‹M·ÿ]i  H‹×H[—  ÿul  H‹ÏÿÜm  I‹ÕHR—  ÿ\l  éÂ  I‹ÕHM—  ÿGl  H‹M¯D‹îf;q$p  HƒÃH‰uçH‰]ßL‹öL‹aH=Ô—  MæH‹ÎH9Ï—  t$A·T$H‹Æf98„>  HÿÁH‹ÁHÁàH9t8uåHr–  E·D$Hå–  ÿÏk  A·D$¨tHŞ–  ÿ¸k  A·D$¨tHÏ–  ÿ¡k  fAƒ|$uXH‹ÎH9N—  t/I‹$·PH‹Æf„     f98„¿   HÿÁH‹ÁHÁàH9t8uåHæ•  I‹$H{–  D·@ÿ@k  E}D;}¿å   L‹3A¹ÿÿÿÿ‹MM‹ÆH‰t$83ÒH‰t$0‰t$(H‰t$ ÿ f  ‹Ø…Àt`E3À‹ËAPÿël  ‹MA¹ÿÿÿÿH‰t$8M‹ÆH‰t$03Ò‰\$(H‹øH‰D$ ÿaf  …À‹Î•Á…Éu^ë+HÉH‹TÏéÍşÿÿHÉH‹TÏéLÿÿÿ¹   ÿ÷k  H‹ø@ˆ0ÿf  ‹Èÿ³l  Lt  º   L‹È3ÉH‹Øÿ!l  H‹Ëÿ8l  H‹]ßH‹ÿ+g  L‹uçëH=†•  H‹E¯Hƒ•  A¹,   L‹Ç¿H$¸    ÿÉD;éHm•  DDÈfAƒ|$HO•  HDĞÿj  D;}¿}	H‹Ïÿbk  H‹M¯HƒÃIƒÆ H‰]ßE‹ïL‰uç¿A$D;øŒ§ıÿÿD‹}L‹e_H•  ÿ¹i  I‹$LM·H‰t$(E3ÀI‹ÌH‰t$ L‹P`H‹E¯‹I‹Âÿîk  H‹]·H…Û„Û   H‰t$8A¹ÿÿÿÿH‰t$0L‹Ã‰t$(3ÒA‹ÏH‰t$ ÿòd  D‹ğ…ÀtHE3ÀA‹ÎAPÿ;k  H‰t$8A¹ÿÿÿÿH‰t$0L‹ÃD‰t$(3ÒA‹ÏH‰D$ H‹øÿ°d  …À‹Î•Á…ÉuDë¹   ÿ`j  H‹ø@ˆ0ÿ|d  ‹Èÿk  Lİ  º   L‹È3ÉH‹ØÿŠj  H‹Ëÿ¡j  H‹M·ÿ—e  H‹×H”  ÿ¯h  H‹Ïÿj  H”  ÿ™h  L‹mïH‹Mÿÿûi  H‹}÷L‹ugM…öteH‹×I‹ÍÿZh  HO HƒáøÿÄi  L