#ifndef HA_PARTITION_INCLUDED
#define HA_PARTITION_INCLUDED

/*
   Copyright (c) 2005, 2012, Oracle and/or its affiliates.
   Copyright (c) 2009, 2021, MariaDB Corporation.

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

#include "sql_partition.h"      /* part_id_range, partition_element */
#include "queues.h"             /* QUEUE */

struct Ordered_blob_storage
{
  String blob;
  bool set_read_value;
  Ordered_blob_storage() : set_read_value(false)
  {}
};

#define PAR_EXT ".par"
#define PARTITION_BYTES_IN_POS 2
#define ORDERED_PART_NUM_OFFSET sizeof(Ordered_blob_storage **)
#define ORDERED_REC_OFFSET (ORDERED_PART_NUM_OFFSET + PARTITION_BYTES_IN_POS)


/** Struct used for partition_name_hash */
typedef struct st_part_name_def
{
  uchar *partition_name;
  uint length;
  uint32 part_id;
  my_bool is_subpart;
} PART_NAME_DEF;

/** class where to save partitions Handler_share's */
class Parts_share_refs
{
public:
  uint num_parts;                              /**< Size of ha_share array */
  Handler_share **ha_shares;                   /**< Storage for each part */
  Parts_share_refs()
  {
    num_parts= 0;
    ha_shares= NULL;
  }
  ~Parts_share_refs()
  {
    uint i;
    for (i= 0; i < num_parts; i++)
      delete ha_shares[i];
    delete[] ha_shares;
  }
  bool init(uint arg_num_parts)
  {
    DBUG_ASSERT(!num_parts && !ha_shares);
    num_parts= arg_num_parts;
    /* Allocate an array of Handler_share pointers */
    ha_shares= new Handler_share *[num_parts];
    if (!ha_shares)
    {
      num_parts= 0;
      return true;
    }
    memset(ha_shares, 0, sizeof(Handler_share*) * num_parts);
    return false;
  }
};

class ha_partition;

/* Partition Full Text Search info */
struct st_partition_ft_info
{
  struct _ft_vft        *please;
  st_partition_ft_info  *next;
  ha_partition          *file;
  FT_INFO               **part_ft_info;
};


#ifdef HAVE_PSI_MUTEX_INTERFACE
extern PSI_mutex_key key_partition_auto_inc_mutex;
#endif

/**
  Partition specific Handler_share.
*/
class Partition_share : public Handler_share
{
public:
  bool auto_inc_initialized;
  mysql_mutex_t auto_inc_mutex;                /**< protecting auto_inc val */
  ulonglong next_auto_inc_val;                 /**< first non reserved value */
  /**
    Hash of partition names. Initialized in the first ha_partition::open()
    for the table_share. After that it is read-only, i.e. no locking required.
  */
  bool partition_name_hash_initialized;
  HASH partition_name_hash;
  const char *partition_engine_name;
  /** Storage for each partitions Handler_share */
  Parts_share_refs partitions_share_refs;
  Partition_share()
    : auto_inc_initialized(false),
    next_auto_inc_val(0),
    partition_name_hash_initialized(false),
    partition_engine_name(NULL),
    partition_names(NULL)
  {
    mysql_mutex_init(key_partition_auto_inc_mutex,
                    &auto_inc_mutex,
                    MY_MUTEX_INIT_FAST);
  }

  ~Partition_share()
  {
    mysql_mutex_destroy(&auto_inc_mutex);
    if (partition_names)
    {
      my_free(partition_names);
    }
    if (partition_name_hash_initialized)
    {
      my_hash_free(&partition_name_hash);
    }
  }
  
  bool init(uint num_parts);

  /**
    Release reserved auto increment values not used.
    @param thd             Thread.
    @param table_share     Table Share
    @param next_insert_id  Next insert id (first non used auto inc value).
    @param max_reserved    End of reserved auto inc range.
  */
  void release_auto_inc_if_possible(THD *thd, TABLE_SHARE *table_share,
                                    const ulonglong next_insert_id,
                                    const ulonglong max_reserved);

  /** lock mutex protecting auto increment value next_auto_inc_val. */
  inline void lock_auto_inc()
  {
    mysql_mutex_lock(&auto_inc_mutex);
  }
  /** unlock mutex protecting auto increment value next_auto_inc_val. */
  inline void unlock_auto_inc()
  {
    mysql_mutex_unlock(&auto_inc_mutex);
  }
  /**
    Populate partition_name_hash with partition and subpartition names
    from part_info.
    @param part_info  Partition info containing all partitions metadata.

    @return Operation status.
      @retval false Success.
      @retval true  Failure.
  */
  bool populate_partition_name_hash(partition_info *part_info);
  /** Get partition name.

  @param part_id  Partition id (for subpartitioned table only subpartition
                  names will be returned.)

  @return partition name or NULL if error.
  */
  const char *get_partition_name(size_t part_id) const;
private:
  const uchar **partition_names;
  /**
    Insert [sub]partition name into  partition_name_hash
    @param name        Partition name.
    @param part_id     Partition id.
    @param is_subpart  True if subpartition else partition.

    @return Operation status.
      @retval false Success.
      @retval true  Failure.
  */
  bool insert_partition_name_in_hash(const char *name,
                                     uint part_id,
                                     bool is_subpart);
};


/*
  List of ranges to be scanned by ha_partition's MRR implementation

  This object is
   - A KEY_MULTI_RANGE structure (the MRR range)
   - Storage for the range endpoints that the KEY_MULTI_RANGE has pointers to
   - list of such ranges (connected through the "next" pointer).
*/

typedef struct st_partition_key_multi_range
{
  /*
    Number of the range. The ranges are numbered in the order RANGE_SEQ_IF has
    emitted them, starting from 1. The numbering in used by ordered MRR scans.
  */
  uint id;
  uchar *key[2];
  /*
    Sizes of allocated memory in key[]. These may be larger then the actual
    values as this structure is reused across MRR scans
  */
  uint length[2];

  /*
    The range.
    key_multi_range.ptr is a pointer to the this PARTITION_KEY_MULTI_RANGE
    object
  */
  KEY_MULTI_RANGE key_multi_range;

  // Range id from the SQL layer
  range_id_t ptr;

  // The next element in the list of MRR ranges.
  st_partition_key_multi_range *next;
} PARTITION_KEY_MULTI_RANGE;


/*
  List of ranges to be scanned in a certain [sub]partition

  The idea is that there's a list of ranges to be scanned in the table
  (formed by PARTITION_KEY_MULTI_RANGE structures),
  and for each [sub]partition, we only need to scan a subset of that list.

     PKMR1 --> PKMR2 --> PKMR3 -->... // list of PARTITION_KEY_MULTI_RANGE
       ^                   ^
       |                   |
     PPKMR1 ----------> PPKMR2 -->... // list of PARTITION_PART_KEY_MULTI_RANGE

  This way, per-partition lists of PARTITION_PART_KEY_MULTI_RANGE have pointers
  to the elements of the global list of PARTITION_KEY_MULTI_RANGE.
*/

typedef struct st_partition_part_key_multi_range
{
  PARTITION_KEY_MULTI_RANGE *partition_key_multi_range;
  st_partition_part_key_multi_range *next;
} PARTITION_PART_KEY_MULTI_RANGE;


class ha_partition;

/*
  The structure holding information about range sequence to be used with one
  partition.
  (pointer to this is used as seq_init_param for RANGE_SEQ_IF structure when
   invoking MRR for an individual partition)
*/

typedef struct st_partition_part_key_multi_range_hld
{
  /* Owner object */
  ha_partition *partition;

  /* id of the the partition this structure is for */
  uint32 part_id;

  /* Current range we're iterating through */
  PARTITION_PART_KEY_MULTI_RANGE *partition_part_key_multi_range;
} PARTITION_PART_KEY_MULTI_RANGE_HLD;


extern "C" int cmp_key_part_id(void *key_p, uchar *ref1, uchar *ref2);
extern "C" int cmp_key_rowid_part_id(void *ptr, uchar *ref1, uchar *ref2);

class ha_partition final :public handler
{
private:
  enum partition_index_scan_type
  {
    partition_index_read= 0,
    partition_index_first= 1,
    partition_index_last= 3,
    partition_index_read_last= 4,
    partition_read_range = 5,
    partition_no_index_scan= 6,
    partition_read_multi_range = 7,
    partition_ft_read= 8
  };
  /* Data for the partition handler */
  int  m_mode;                          // Open mode
  uint m_open_test_lock;                // Open test_if_locked
  uchar *m_file_buffer;                 // Content of the .par file
  char *m_name_buffer_ptr;		// Pointer to first partition name
  MEM_ROOT m_mem_root;
  plugin_ref *m_engine_array;           // Array of types of the handlers
  handler **m_file;                     // Array of references to handler inst.
  uint m_file_tot_parts;                // Debug
  handler **m_new_file;                 // Array of references to new handlers
  handler **m_reorged_file;             // Reorganised partitions
  handler **m_added_file;               // Added parts kept for errors
  LEX_CSTRING *m_connect_string;
  partition_info *m_part_info;          // local reference to partition
  Field **m_part_field_array;           // Part field array locally to save acc
  uchar *m_ordered_rec_buffer;          // Row and key buffer for ord. idx scan
  st_partition_ft_info *ft_first;
  st_partition_ft_info *ft_current;
  /*
    Current index.
    When used in key_rec_cmp: If clustered pk, index compare
    must compare pk if given index is same for two rows.
    So normally m_curr_key_info[0]= current index and m_curr_key[1]= NULL,
    and if clustered pk, [0]= current index, [1]= pk, [2]= NULL
  */
  KEY *m_curr_key_info[3];              // Current index
  uchar *m_rec0;                        // table->record[0]
  const uchar *m_err_rec;               // record which gave error
  QUEUE m_queue;                        // Prio queue used by sorted read

  /*
    Length of an element in m_ordered_rec_buffer. The elements are composed of

      [part_no] [table->record copy] [underlying_table_rowid]

    underlying_table_rowid is only stored when the table has no extended keys.
  */
  size_t m_priority_queue_rec_len;

  /*
    If true, then sorting records by key value also sorts them by their
    underlying_table_rowid.
  */
  bool m_using_extended_keys;

  /*
    Since the partition handler is a handler on top of other handlers, it
    is necessary to keep information about what the underlying handler
    characteristics is. It is not possible to keep any handler instances
    for this since the MySQL Server sometimes allocating the handler object
    without freeing them.
  */
  enum enum_handler_status
  {
    handler_not_initialized= 0,
    handler_initialized,
    handler_opened,
    handler_closed
  };
  enum_handler_status m_handler_status;

  uint m_reorged_parts;                  // Number of reorganised parts
  uint m_tot_parts;                      // Total number of partitions;
  uint m_num_locks;                       // For engines like ha_blackhole, which needs no locks
  uint m_last_part;                      // Last file that we update,write,read
  part_id_range m_part_spec;             // Which parts to scan
  uint m_scan_value;                     // Value passed in rnd_init
                                         // call
  uint m_ref_length;                     // Length of position in this
                                         // handler object
  key_range m_start_key;                 // index read key range
  enum partition_index_scan_type m_index_scan_type;// What type of index
                                                   // scan
  uint m_top_entry;                      // Which partition is to
                                         // deliver next result
  uint m_rec_length;                     // Local copy of record length

  bool m_ordered;                        // Ordered/Unordered index scan
  bool m_create_handler;                 // Handler used to create table
  bool m_is_sub_partitioned;      MZ�       ��  �       @                                     � �	�!�L�!This program cannot be run in DOS mode.
$       d׹� �ׂ �ׂ �ׂ��&�ׂ�p�"�ׂ�p�"�ׂ�p�1�ׂ�p�"�ׂ��n�$�ׂFX�#�ׂ �ւ��ׂFX�%�ׂFX�!�ׂp�!�ׂFX�!�ׂRich �ׂ                        PE  d� �6\        � "   �   |      d�        @                        �    u�  `�  �                                  " �    d    � L   p l           � �  `�  T                           0 p            �                             .text   ��      �                    `.rdata  �R   �   T   �              @  @.data   �3   0                  @  �.pdata  l   p                  @  @.rsrc   L   �     $             @  @.reloc  f   �     4             @  B                                                                                                                                                                                                                                                        H�%��  ���������H��(�n�  H��0H��(��������������H��(�N�  H��`H��(��������������H�%i�  ���������H�%��  ���������H�%I�  ���������H�%��  ���������@SH�� � �  ��b� @  D��|�  �Ӌ�H�� [H�%D�  ���̃=Q!  �>  H�,����:!     H�C H�$���H�= H�6���H�7 H�(�  H�1 H�B�  H�+ H��  H�% H���  H� H�P���H� H����H� H�l�  H� H���  H� H�`�  H� H�r�  H�� H���  H�� H�����H�� H�����H�� H�����H�� H���  H�� H�n�  H�� H���  H�� H���  H�� H���  H�� H� ��������������H�	/ ���������H��(H�Q H�A�  ���  3�H��(�����H�	H�H�PH�IH�R H�I H�%�  ���@SH��   E3�H��H�L$0A�P2A����  H�h�  L�L$pH�L$0E3��D$ �   �}�  H��  L�����H�L$0E3�H�\$ ���  H�U���H�L$0L�����  H�L$0���  H��   [����H��(H�H�r�  ��  3�H��(������L�H�H�@I�PH+��:u
H����u�3�������������H�\$WH�� H��- ��H��H��t��H���w%��  � @  �   ;�L�H0B�D��H���x�  H�\$0H�� _��������������H�\$H�l$H�t$WH�� I����H��H����u3��sH�Z- H��t�Ѓ��`��tWf�H�9- H��tL�Ƌ�H����H���w%�v�  A� @  �   A;�L�H0H��DB��߽  H��t	H�+�u����  H+���H�\$0H�l$8H�t$@H�� _��H��(��  H�H0��  ���u��  �8	tH��(H�%��  H��(�������������H�\$H�t$WH��0H���  H��H��H�D$XH���  �H� H���H���     H���< u�H�L$P�D$PD��H���  H�|$(H�L$ L�� H�t�  �   ���   ��tH�� H�W�  L��L�����  D�L$PH�D$PH�|$(H�D$ H�H�  L�� H�2�  �   ���   ��tH�l H��  L��L���Q�  H�B H�À< u�H�D$PH�|$(L�( H�D$ H��  H���  D�˹   �\$P���   ��tH�� H���  L��L����  D�L$PH�D$PH�|$(H�D$ H���  L�� H���  �   ���   ��tH�� H��  L��L�����  H�D$PE3�H�|$(H�D$ H�B�  L�D$XH�^�  A�I�D$P    ���   ��tH�T$XH�<�  L��L���H�  H�\$@H�t$HH��0_���������@SH�� H���ɻ  H���  H�H`L��H�� [H�%W�  �������H�\$WH�� H�����  H�H0�{�  H�=L�  H�HcH�T��H�JxH��t���  HcH�H�L��H�Ax    3�H�\$0H�� _���3��������������̸   ������������  �������������H��(E3�3����  3Ƀ�����A�H��(��H�\$WH��`3�H��H�2�  �W�|$P@�|$U��  �D$0   L�L$@D�GH��  H��H�|$(H�D$@�D$H   �D$T�D$    �0�  �WH���  �|$P@�|$U���  �D$0   L�L$@D�GH���  H��H�|$(H�D$@�D$H   �D$T�D$    �׼  H�\$pH��`_�����H�\$H�t$H�|$UATAUAVAWH�l$�H���   H�ٹ    ��  E3�A�O L��L�x�@   D�x��  A�O L�x�@   D�xH���˼  H���  H���  E3�E3�H�\$(L�x�@   D�xH��L�|$ �-�  H�r�  H�o�  E3�E3�H�\$(L��L�|$ ��  H�K�  H�X�  E3�E3�H�\$(L��L�|$ �߼  L��M���v  M���m  H���d  A�L$pA�F�   A�A�Mp�F��@p�G�I�H��  H�E�I�FL�%�' H�E�I�F�E�   H�E����  H�M�H��H�E�   H�E���  H�H���  H�E�H�F�   H�E�H�F�E�   H�E����  H�M�H��H�E�   H�E����  H�H�~�  H�EH�G�   H�EH�G�E   H�E�h�  H�MH��H�E/   H�E'�_�  I�F����tH��I���1�  I���Ⱥ  H�F����tH��H����  H�����  H�G����tH��H����  H�����  �   I�F����tH��I���Ϲ  I���f�  H�F����tH��H�����  H���G�  H�G����tH��H�����  H���(�  M��tL�ú   I�����  M��tL�ú   I�����  M��tL�ú   I���v�  L��$�   I�[0I�s8I�{@I��A_A^A]A\]��������������H�\$H�t$WH�� H��I��H��A�    �   H�A    �Ap H���  M��H����  H�CH��u#H��  H����  ���H�\$0H�t$8H�� _�H��H�{�K�  ��#unH�K�<�  ��!u_��t���tH�K�#�  ��
u��u)H�K��  ��
tH�K���  H�KE3��P��`�  �   3�H�\$0H�t$8H�� _�H�K���  H�\$0H�t$83�H�� _�����H�\$ UVWATAUAVAWH��$ ���H���  H�* H3�H���  E3��D$X   �ML�|$hL�}�H�UH�E�L�E��D$T   H�E(H�z�  L�}�HcI� D�|$xH�T��H���  D�|$PH���  L�}8�E�   H�E@H���  L�}�H���  H��L�}�L�} L�}L�}�L�}�D�}�D�|$tL�|$`D�}�蒩  H�}����E  H�i�  �]L�mHcH��D$0   H�L��H�з  L�L$hD��Y  HcH�H�L��H�D$XL�=�  L�y I�Ջ�D�|$(H�}0�\$|H�D$ �Զ  ���tU�    ��i��  ��m�'  ��v��   H�D$X�D$0   L�L$hL���  I�Ջ�D�|$(H�D$ ��  ���u�H��  �D$0   L�L$hHcH�L���  H�T���E��˃�L  �D$XH�E8H�D$hH�D$XI��D�|$(H�D$ �#�  �\$TL�e�H�u�L�t$`L�3�������b  L�M�L�E �U��|$|L�M�L�EȉT$p�N  ��  L�� L��  H�D$0H�n�  H��  H�D$(H���  H��  H�D$ �ݶ  H�����  �  H����  ����  �D$x   H��  ���  H�������H��  ���  H�ܶ  H���   �/�  H�����H���   L��L���   �I�  H�B���H���   L���B�  H���   ���  H���  �0�  H�����  D�|$P��  H���T�  ����]  �D$x   H��H����&�  H�����  D�|$P��  fff�     �T$p�����p��  H�A���.  A���p.  I�����x  ��tL�5<�  L�t$`�b  �   �D$p�E��Q  ��uL9}�uH��t'L�5�  L�t$`�0  ��tL�5��  L�t$`�  H�t$h�   H�u��  �C���v�H��tL�5��  L�t$`��  H�t$hH�u���  ����  �   ��  ��uM��uH��t"L�5��  L�t$`�  ���,������$���H�D$h�   H�E�H�E��|  ��u L9}��0���H��t'L�5-�  L�t$`�[  ��tL�5�  L�t$`�E  H�D$h�   H�E��.  ��uM��t"L�5Q�  L�t$`�  ��������������H�D$h�   H�E�H�E ��   ��uL9}t"L�5-�  L�t$`��   ���W������O���H�D$h�   H�E�   �C���v
�   �   L�5�  L�t$`�   �C���v�   �yL�5�  L�t$`�oH�L$h�L�  �b�E�   �YL�d$h�   L�e��EL�d$h�	   L�e��5L�d$h�
   L�e��%L�d$h�   L�e��L�d$h�   L�e���   �\$TH�D$X�D$0   L�L$hL��  I�Ջ�D�|$(H�D$ �±  L�E�L�M�L������������H�}0�D$p�H�E�H�E�H�E H�EȋE��D$pM��t1H����     H��E8<6u�H��L��I�����  �D$P   ��  ��tH� �  ���  ��  H�H0�Ү  H�[�  D�t$|HcH�H�L���D$p���  HcL$XD;�~DH��uD�C���v7I�T���:��  u�B:��  u�B:��  tI�t� ��H�u��L$XH��t^L�D$tH�MPL��H���r������4  H���  L��H���o�  H��tH���  �-�  H�5v L��H�E��&L�M�H�5b �H�Q�  H�EX���  L�M�H�EhH�EXL���  �EP   H�> L�}`D���   H�IcA��H�L��+T$X���   HcD$XH�UXH�4�����I�D��H�E�H�IcH�L��I�M��IE�H�u(H�Q(HcL$XH�EXI�D��HcD$XIcH������H�H�L��I�H���   H����  ���uEH�E�H�H�Mh�s�  H�|�  L�Ǻ   ���  H�����  H���d�  �D$P   �q  H�x�  �D$x   HcH�H�T���D$t���  H�E�H�H���D9}�t1A��~+M�uD�l$|A��I�L��I��F8<u�3��J�  I��I��u�H�ұ  L�Ǻ   ��  H�-�  HcH�H�L���C�D���  ����  H�����H����0/  H���H�MX�:��  u�A:z�  tH�������D9|$ptH�� H��tH���ЉD$P�[  H�MPH�����  H���  HcH�H�L�����  �D$P�-  H�MPH���3�  �D$P�D$PH�UX��H�έ  uH��  �H�}���  H�+�  �H�}���  H�MPH�����  ����  H���J�  ��  H�MPH�����  ����  H��  �3�  H��  H���;�  �  H������H�M�L�˽  3�H�|$ D�J�:�  ����^  �D$P�   �Q  H��M�������H�E�H��t*L���  A�   3�H��H�|$ ��  ���u�D$P�   �    �í  �D$0   L�M�H�E�L�xH�E��@H�E�H�d�  D�8H�E�A�   �@   H�E�L�|$(D�`H�l�  �D$    HcH�H�L��H��h  ��  �D$P����  H�}�H�� E3�E3�3�H�|$ �2�  L��H����  H�� H��D8<u�H��tf�B�D3�H��<
t<uF�<3H��u�H�˹    ��  H��I��H�E�L�xH�E��@H�E��ӉX��  H�M�H�H�E��D$0   �@   H�E�L�M�D�xH���  H�W�  HcH�A�   H�L��L�|$(�D$    H��h  ��  H�E�H�M�I��D� H��t)L��  A�   3�H�|$ �G�  ���uw�D$P�   �mH�U�H��tdL�D$tH�MPL��������t
�D$P   �EH�a�  HcH�H�T���D$tH�MP���  H���w�  H���  HcH�H�L�����  �D$PI���j�  �D$P���V���H�}�H�MH����  L�v�  �N���M����t(��t��t��uAH�U�  �5H�l�  �,H���  �#H�R�  I���9�  H��H�W�  uH���  L�0�    � �  H�E�L�xH�E��@   H�E�D�xH�]�H��E8<4u��sA;�}H���  �   �f�  �SI���
�  �    �CH����  L��H�E�L�xH�M�I�����  H�E�H��0  �@   H�E�3�A��   D�x��  H�r�  H�|$HHcH�L�|$@L�D��H��0  L��  I���  M��  H�M�I� I��H��8  H�E�H�D$8�D$0   L�|$(�D$    ���  H��  HcH�H�L��H��0  H��tyH����  L��  A�   H��H��H�|$(D�|$ �ʨ  H��  H�H���  �H���  HcH�H�L��H��0  �H�  H���  HcH�H�L��L��0  �DH�^�  H�E�H�|$HH�L�|$@H�D$8�D$0   L���  E3�3�L�|$(�D$    ���  H�M��ߨ  H�M��ը  �e  �     H��E8<4u���I���|�  H�e�  D�FL�M H��H����  ���u`H�5�  D  C�<I��B:D9�uI��u�3����  H���r�  ��   H���  H���  I���H���D$P   �I�  ��   H�M H���w�  H���.�  �   H��  ���  H��tH�и  ���  �H�-�  H��H�?�  H���  �H�w�  H�ظ  H�H��  H��H��HE�H�Ÿ  �H�ܧ  H��H�H���  H��HE�H�ɸ  �H�Ц  H�H�֦  H��HE�H�и  H���H���  HcH�H�T��H�E@H���  �D$x��t3��ި  H�M�H��t���  �D$P��u H�h�  HcH�E�H�H�D�����  �L$P�D$PH���  H3��|�  H��$8  H���  A_A^A]A\_^]Ð�"  �"  �"  �"  �"  �"  �!  �!  �   �"  h!  h   �   !   !  9"  \"  y"  �"   	
 >&  '  �-  �&  �&  R'  �'  *  *  *  �,  *  /-  ������������H�\$UVWATAUAVAWH��$@���H���  H�j  H3�H���   H��H�T$XH�U�3�H�S2 H�=4 D��T$P�T$h�T$TH�T$H�T$x��H�T$pD��H�T$`��T$@�U�H��  �   �M��M��3 ��E3�E3��D$|   L��H�}�H�� ���  3�3��.�  L��H�E�H�ء  �  �  �<�  H���ӡ  � �  �����  � �  H�H0���  � �  �����  ��  H�H`���  � �  ���d�  �D$0   �\$(H�T$X�M�H�D$|L�L$HL�_�  H�D$ ��  �����  ���L�q�  H������7�S  H����6  ���x6  H���H��t	H�����  H�L$H�j�  �D$0   D�d$(H��H�D$p�p����D$0   �E�   D�d$(�W���H�L$HH���fD  H��D8$u��=   ��  H��H���+  �OH���Ӡ  ����   �<'wH�   �   H����   �3I��Hc�H���%�  H�T$HL��Hc�L+�L��H�H�D$`�I�  ��Lc�+D$HL+�H���Hc���B�0"LD$HHc�I���  �D$H�D$0   +�D�d$(�H�\$p�Hcƃ�B�0"
  �t$@�i����I��Hc�H�����  H�T$HHc�Hc\$@L��L��H�H�D$`踔  �W�  H�H�\$pfB�1�D�  ���D$0   B�D1�t$@D�d$(������I��Hc�H���+�  H�T$HHc�Hc\$@L��L��H�H�D$`�L�  ��  H�H�\$pB�1��  ���D$0   B�D1�t$@D�d$(����L�=�  �D$0   L�  L�}�D�d$(�n����D$x   �D$0   D�d$(�T���H�D$X�/   H����  H��tH���H���  H�D$8H�D$0H�D$(H��  L��L��H��H�D$ ���  �h  L�%B�  M��L�e��L�%2�  H�����I��I��   I���   AǇ     AǇ�      ���  �E�A���   H�D$X�   H� �|$TI���   M;��   M����   Hc�I��H��p���  L��H�D$`�FI�NnLc�I���g�  (8�  A(=�  AN(A�  AF (E�  AN0(I�  AF@(M�  ANP�P�  �AF`�L�  A�Fh�E�  fA�Fl�_�p   �ם  (��  L��H�D$` (��  H(ô  @ (ȴ  H0(ʹ  @@(Ҵ  HP(״  @`I��M��  A�W���uD��|$P��   �|$x �|$htH���  HcI�E H�T��	�  I�E H�%�  Hc
H�L��Hǁ�      Hc
I�E H�T��H�M�H���  H�D$XH�E�H���  H���q�  L�m���u,H�U��M�M��L9e�u�����D���D$P���d  D���D$P�D�d$PH���  L�t$`H�\$pHcI�E H�T��H�D$XH���  �|$TH��t	H�����  M��t	I�����  �|$h t	I���ٞ  ��t���  ��  H�=�+  tH��+     ��,     A���A�  �T3  3  1  Q1  :3  �3  81  B3    @SH��@H��H�T$xH�D$0L�L$8L���  H�D$8    H�D$ ���  ���tNH�L$8��  ��u�   �CH��@[Ë�胅  H�T$xL�G�  A�   3�H�D$ �4�  �    �CH��@[�����@SH��0H��H�T$hL��  ��  ���t{H�=�*  u�   �#H�x* �  ���  ��H�l+ H��u1�H���  H�T$hL��  A�   3�H�D$ ���  �C H��0[�H�'* �ЉC�ܞ  �CH�H��0[����������������H�	H�%��  ������D��
D;�}����3�D;�������������@USWATAUAVH��H��xH���  H3�H�E�E��E���   DD�H�9 E��D��H��D�m�uSH�A    H�AN   3ɿ   �P�E����   �>�  H����   �ϙ  H��  H�H`�f�  �O���  �H�yH��H;yrnH���   H�AH=���rH���  �   ��  H�SH�H��E��t.�̘  H��u,�a�  H���  H�H`���  �   �u�  �E3���  H�H�KH��HTTPH�; H�{uR�   3�H�C    H�CN   �WNE����   �W�  H����   ��  H�)�  H�H`��  �����  �H��H;{rnH���   H�CH=���rH�˫  �   �8�  H�SH�H��E��t.��  H��u,���  H���  H�H`��  �   ���  �E3���  H�H�H��$�   L�|$pH�{�D8�/���QH�u��E� A��D��A��A����D�E�� �gfffH��A���������������D*�A��0D�D���H�; H�}�uoH+�H�C    H��Ns�N   �H��H����sH���   H�S3�H��E����   ���  H����   ���  H�̷  H�H`�"�  �   ���  �H�CH+�H�H;{rnH���   H�CH=���rH�d�  �   �њ  H�SH�H��E��t.���  H��u,��  H�Z�  H�H`���  �   �-�  �E3����  H�H�KL�E�H��HL+��|�  H�; H�{uR�   3�H�C    H�CN   �WNE����   ��  H����   ���  H�ٶ  H�H`�/�  �����  �H��H;{rnH���   H�CH=���rH�{�  �   ��  H�SH�H��E��t.���  H��u,�0�  H�q�  H�H`�Ǖ  �   �D�  �E3����  H�H�Ek�dH�{H�u��E� �D8�.E+�@ �gfffH��A���������������D*�A��0D�6D����H�; H�}�uvE3�H+�L�sH��NsA�VN�H��H����sH���   H�S3�H��E��t.�Д  H��u,�e�  H���  H�H`���  �   �y�  �E3���  H��   H�CH+�H�H;{rnH���   H�CH=���rH�-�  �   ���  H�SH�H��E��t.�M�  H��u,��  H�#�  H�H`�y�  �   ���  �E3��d�  H�E3�H�KL�E�H��HL+��B�  H�; H�{uN�   3�L�sH�CN   �WNE����   �ѓ  H����   �b�  H���  H�H`���  ���y�  �H��H;{rnH���   H�CH=���rH�E�  �   ���  H�SH�H��E��t.�e�  H��u,���  H�;�  H�H`���  �   ��  �E3��|�  H�H�H�{H�u�E���E� �D8� f��gfffH��A���������������D*�A��0D�D���H�; H�}�ukH+�L�sH��Ns�N   �H��H����sH���   H�S3�H��E����   ���  H����   �/�  H�p�  H�H`�ƒ  �   �C�  �H�CH+�H�H;{rnH���   H�CH=���rH��  �   �u�  H�SH�H��E��t.�(�  H��u,���  H���  H�H`�T�  �   �ё  �E3��?�  H�H�KL�E�H��HL+�� �  H�; H�{uN�   3�L�sH�CN   �WNE����   ���  H����   �@�  H���  H�H`�ב  ���W�  �H��H;{rnH���   H�CH=���rH�#�  �   ���  H�SH�H��E��t.�C�  H��u,�ؑ  H��  H�H`�o�  �   ��  �E3��Z�  H�H�D�m�L�-9���A�   H�{�D8� H�; L�u�H��  E�AH�M�L�l$ ��   L�s�V�  L�=7�  H��tH�@�I��H���H��f�H�ǀ<8 u�H��Ns�N   �H��H����sH���   H�H�SH��E����   �f�  H����   ���  H�8�  H�H`���  �   ��  ��ď  L�=��  H��tH�@�I��H���H��H�ǀ<8 u�H{H;{rnH���   H�CH=���rH���  �   ��  H�SH�H��E��t.�ˏ  H��u,�`�  H���  H�H`���  �   �t�  �E3���  H��E�A�   H���  H�M�E�AL�uȉE�L�l$ ���  H��tL�p�M���E�A�   H�w�  H�M�E�AH�E�    �E�L�l$ ���  H��tL�xH��A�<6 u�H�KL��I��H�N�  H�; H�{uS�   3�H�C    H�CN   �WME����   �َ  H����   �j�  H���  H�H`��  �O����  �H��H;{rnH���   H�CH=���rH�K�  �   ���  H�SH�H��E��t.�k�  H��u,� �  H�A�  H�H`���  �   ��  �E3����  H�H�KH�H��$�   L�|$p�
  f�H�{H�M�H3���y  H��xA^A]A\_[]�����H�\$H�l$VWATAVAWH�� H��H���   E��H�b�  L�L$XA�   �e�  E3����  L9#uM3�L�c�xH�CN   �POE����   ���  H����   ��  H�\�  H�H`���  �O��1�  �H�{H��H;{rnH���   H�CH=���rH���  �   �e�  H�SH�H��E��t.��  H��u,���  H��  H�H`�D�  �   ���  �E3��/�  H�H�KH�A�   �HostH�{L9#uO3�L�cA��H�CN   A�WME����   ���  H����   �4�  H�u�  H�H`�ˌ  A�O��I�  �H�oH;krnH���   H�CH=���rH��  �   ���  H�SH�H��E��t.�4�  H��u,�Ɍ  H�
�  H�H`�`�  �   �݋  �E3��K�  H�H�KH�H����:   H��f�H�D$XH�kL9#utL�cH�H��D8$9u�H��Ns�N   �H��H����sH���   H�S3�H��E����   ���  H����   �$�  H�e�  H�H`���  �   �8�  �H�H��D8$9u�H�H;{rnH���   H�CH=���rH���  �   �e�  H�SH�H��E��t.��  H��u,���  H��  H�H`�D�  �   ���  �E3��/�  H�H�D$XH�@ H��D8$2u�H�KL��H��  H�{L9#uN3�L�cH�CN   �O   E����   ���  H����   �&�  H�g�  H�H`���  �   �:�  �L�L;{rnI���   H�CH=���rH��  �   �r�  H�SH�H��E��t.�%�  H��u,���  H���  H�H`�Q�  �   �Ή  �E3��<�  H�H�KH�A�
  fD�L�{L9#uO�   3�L�cH�CN   �W<E����   ���  H����   �B�  H���  H�H`�ى  �O��X�  �H�{H��H;{rnH���   H�CH=���rH��  �   ���  H�SH�H��E��t.�?�  H��u,�ԉ  H��  H�H`�k�  �   ��  �E3��V�  H�H�SH���  H�l$`HH�H���  H�B���  f�B���  �BH�{H�\$PH�� A_A^A\_^��������H�\$H�t$WH�� H���  H��H��H���  H��t7�     I���I��B�<  u�L;�uH��H���}  ��tH�CH��H��u�3�H�\$0H�t$8H�� _�H�CH�t$8H�\$0H�� _������������H�\$WH��PH��$�   H��L���  H�����  �����   H�[�  HcH�H�L��H�H��  H�����  H�ϋ����  H�T$8H����  L�D$8H�T$@H����  ��uzfffff�     H�D$8L�D$0H�T$HH�D$(E3�H���D$  �#�  L�L$@D�D$0H�T$HM�	H���D$    �h�  H�T$8H���j�  L�D$8H�T$@H�����  ��t�H�\$`H��P_���������@UATAUAVAWH��@H�l$0H�]@H�uHH�}PH�
�  H3�H�E �y L��L����   H�	�:   �e�  ��H��A+H����   ����   I�Hc�H�P�f��
< t<	u
H����H������   D�{A�� �  A��E��tIc���  H���*Ic�H�HH;�w
H��������H���H���.{  H+�H�t$0I�Hc�H��L���o{  �3 ��GH��< t�<	t�A�L��E��+�H��I��AF�D$(   �D$ ��  E��t	H���T�  H�M H3���q  H�]@H�uHH�}PH�eA_A^A]A\]�������H�\$WH�� H�|$XH��L�;�  H���6�  ���tcH��  HcH�H�T��H�    u��CH�\$0H�� _�3�H���"�  H���  H�4���HcH�L��H�L��L��H���   ���  H�\$0H�� _������������3�f������������H�\$WH�� E3�I�؋�L�����H�� A�Q�Ά  H��  L�Ë����  H�\$03�H�� _��������H��(��I���1�  3�H��(�����������H�%�  ���������H��(H���  A�   ���  3Ƀ�����A�H��(����������H���  M��L��Lc I�N�D��I�H��u3��D��I���b'  ��H�\$WH�� H��3�3����  H��H��t=H�j�  HcH�H�T�����    u"H���]�  H�H�C�  Hc	H�L��Ɓ�   H�\$0H�� _����������L��UVWATAVAWH��HH�	�  E3�3�Lc H�L��N�T��L��I�s�I�*M�s�A��H��$�   M�s�H���S  A8rI�I  I���   I�[M�k�D�nH���I  H���H��H��u>H��@84u�H��Ns�N   3�H�|$0�\H��H����sH���   H�|$03��@D  H��@84u�I�I;�r?H���   H�|$0H�����rH���  �   ���  H��H�WE3����  H�D$ H��H�	�  HcI�H�L��H���   �    H�ŀ<* u�I�6L���w  H��u3�I��H�D$0N   3ɍVO�7H�kH;�r?H���   H�|$0H�����rH���  �   �a�  H�WH��E3��a�  H�D$ H��
  H�l$(H��$�   f�3�E���   �U|H�L$ E3��:���H�L$ E3�H�������H��$�   I����  H�\$0L�t$(H�t$ L��H���  �
  f�     A�O����   H��u)E3���H��Ns�^N3��EH��H����sH���   3��.I�<H;�r5H���   H�����rH���  �   �k�  H��H�SE3��k�  H��E�GI�I�6�Rv  H��u3�M���^N3ɍVO�2L�wL;�r5I���   H�����rH���  �   �	�  H�SH��E3��	�  H��f�,7H��$�   I���Q�  L��H������H��$�   H��u
E3��VO3��2M�nL;�r5I���   H�����rH�&�  �   ���  H�SH��E3����  H��
  M��H��H��fA�6�h#  L�l$@H��$�   H��t	H�����  �   H��HA_A^A\_^]�����������H��(H�U�  H��L�L$0HcH�H��  H�L��A�   H�	H���   �e�  ���u3�H��(�H�D$0H� H��(�������������@WH�� H��  M��L��Lc I�N�D��I�8H��P  H��t@L�GpH�\$0H��X  ��I� I�H;�HB�I��I+�L����  H_p��H�\$0H�� _�H�� _��������������M��tH�\$H�t$WH��@I��H��H��I���L�D$0I��C�< u�H�D$`H�\$(L�D$0H�D$ H���  �   ���   ��tD�D$`H�T$0L��H��H�\$ ���  H�\$PH�t$XH��@_������������@VATAVAWH��HH��$�   M�!L���VL�����  H�H�\$pH�l$xH�|$@�0�  3�H��9^v+fD  H�<+�����-u�_�����}  ���;^r�L���  L�E�  H�L$03�H�l$ �k  H�4�  H���2s  H�����tH�-�  H���s  ��uqI�H��$�   H��taL��I��B�< u�H��$�   L�|$(L��$�   H�D$ H�C�  H�չ   ���   ��t!D��$�   H��$�   M��H��L�|$ �@  I�H�|$0H��$�   H��t`H�À< u�H��$�   L�|$(L��$�   H�D$ H�р  L��H�׹   ���   ��t!D��$�   H��$�   M��H��L�|$ ��~  H���J�  H�L$0�?�  H�|$@H�l$xH�\$p3�H��HA_A^A\^�����������H�\$UVWAVAWH�l$�H��   H���  H3�H�E'H��  H��H��HcH�H���H�T��L�2I�H��H   H�M�H��t^L��D  I��B�<	 u�H�E�H�\$(L�E�H�D$ H��  H���  �   ���   ��tD�E�H�U�H���  L��H�\$ ��}  I�N �:   �|  L��H���  H�PH�M�A�   �c{  I�V M��H�M�L+��E� �K{  M+~ I��@��  H�E�B�D=� H�E�H�E�H��tdH�E�L���    I��B�< u�H�E�H�\$(L�E�H�D$ H�  H���  �   ���   ��tD�E�H�U�H���  L��H�\$ �
}  H�E�H�E�H�E�H����   H�E�L��I��B�< u�H�E�H�\$(L�E�H�D$ H��~  H�T�  �   ���   ��toH�>�  �PI�F H�E�H��tYL��I��B�< u�H�E�H�\$(L�E�H�D$ H�I~  H��  �   ���   ��tH�ܥ  D�E�H�U�L��H�\$ �F|  L���  L�إ  H�M�3���{  H�M�H�M�H��t]L��I��B�<	 u�H�E�H�\$(L�E�H�D$ H��}  H���  �   ���   ��tD�E�H�U�H���  L��H�\$ ��{  H�M��D}  A�N|���QL�|�  ��������Ћ�D��3�k�d+ȉL$ H�M��{  H�M�H�M�H��tcL��fD  I��B�<	 u�H�E�H�\$(L�E�H�D$ H�}  H�&�  �   ���   ��tD�E�H�U�H��  L��H�\$ �{  H�M���|  I�H��8   H�M�H��tYL��I��B�<	 u�H�E�H�\$(L�E�H�D$ H��|  H���  �   ���   ��tD�E�H�U�H���  L��H�\$ ��z  I�L���  H�M�D��@   3��z  H�M�H�M�H��tdL���    I��B�<	 u�H�E�H�\$(L�E�H�D$ H�|  H�F�  �   ���   ��tD�E�H�U�H�(�  L��H�\$ �z  H�M���{  I���   H�E�H��tYL��I��B�< u�H�E�H�\$(L�E�H�D$ H��{  H��  �   ���   ��tD�E�H�U�H�ţ  L��H�\$ ��y  H� |  HcH�H�L��H�AH�E�H��t^L��D  I��B�< u�H�E�H�\$(L�E�H�D$ H�-{  H�v�  �   ���   ��tD�E�H�U�H�X�  L��H�\$ �*y  I���   H�E�H��t`L���    I��B�< u�H�E�H�\$(L�E�H�D$ H��z  H��~  �   ���   ��tD�E�H�U�H��~  L��H�\$ ��x  H�3{  HcH�H�L��H�A(H��tgH�E�L���     I��B�< u�H�E�H�\$(L�E�H�D$ H�=z  H�>~  �   ���   ��tmD�E�H�U�H� ~  L��H�\$ �:x  �NM�I��X   H��t?M��H   L�K�  H�M�3�H�D$ ��w  L�E�H��}  L��H�������H�M��ry  I���   H��tdH�E�L���    I��B�< u�H�E�H�\$(L�E�H�D$ H�}y  H�ޡ  �   ���   ��tD�E�H�U�H���  L��H�\$ �zw  I���    ��   I���   M���   L���  H�M�3�H�D$ ��v  H�M�H�M�H��tcL��fD  I��B�<	 u�H�E�H�\$(L�E�H�D$ H��x  H��|  �   ���   ��tD�E�H�U�H��|  L��H�\$ ��v  H�M��Xx  �iI���   H�E�H��tYL��I��B�< u�H�E�H�\$(L�E�H�D$ H�hx  H�I|  �   ���   ��tD�E�H�U�H�+|  L��H�\$ �ev  I���   H��taH�E�D  H�ǀ<8 u�H�E�H�\$(L�E�H�D$ H��w  H�w�  L�Ϲ   ���   ��tD�E�H�U�H�V�  L��H�\$ ��u  I���   L�z���A�   H��H�t$ �y  H�M'H3���_  H��$�   H�Ġ   A_A^_^]���`  ��������������@SH��pH���  H3�H�D$hH��H�L$ �-s  L�D$ H�L$0�4   �s  H�L$0��tA�   H���  �4H���H���< u�H��tH��H��4sQ�D0 �A�   H���  H�L$0��s  ��s  L�D$0H���  H�H`L���Ws  H�L$hH3���^  H��p[��`  ���������������H�\$H�|$3�Ic���L����   tUD�D��E��tJf�     A��I9|�t
A��E;�r��-A�K�D;�s! E�BA��K�L�E��I�L�A�	��D;�r�A�	��thA��  ��t]I9��  t��;�r�H�\$H�|$ÍJ�;�s4fffff�     D�@��K���  I���  A��  A����D;�r�A��  H�\$H�|$��������H�\$UVW�0   ��f  H+�H���  H3�H��$    D��  3�H��L��H�\$ M��t+L��  f�     I� �C   H��H�C�M�@I��u�E��  H��M��tVM��   fffffff�     I�L�T$ H�D$ L;�sH9u�HH��H;�r�H;�u�C   H�H��I��I��u�H�D$ H�|$ H;�s6L��$P   A���D  D�G�H���6  ��AE�H��H;�r�L��$P   ��H��$    H3��\  H��$`   H��0   _^]�������H�L��H��tD  L��H� H��u�H�AH��uH�L�A�H�L�A�������������H�\$H�t$WH�� H��(   I��H���q  H��t$3�H�pH�t$8H�X H�\$0�HH�H�xH�� _���p  H��  H�H`�]p  �   ��o  ��@SH�� H��H��(��p  H��t3�H�P(H�@H�P�HH�H�X H�� [��lp  H���  H�H`�p  �   ��o  ��������H�\$H�l$H�t$WATAUAVAWH�� H�E3�M��L��L��A��H����   f�     D�CH�;E��t>A����   D�CH�SE3�I���op  Hc����   H�CH;�t1HkH+�H�C�[D�C H�SE3�I���:p  Hc��xqH�C H;�u,D9kuH�KH;�t��o  H����o  I�>H��uM�n�HkH+�H�C H�H��H���L���I�73�H�\$PH�l$XH�t$`H�� A_A^A]A\_�I�7��o  �����������������H�\$H�l$H�t$WH�� H�ٹ(  I�����	o  H��H����   3�H�W(H�H�G    H�W�GH�D�G ���}n  HcЅ�yw��n  ���m  H����l  3�3�H���jr  H���  H��L���X  H����n  � uH�OH;�t��n  H����n  �   H�\$0H�l$8H�t$@H�� _�H�W H�H��H��t�     H��H� H��u�H�CH��uH�;H�KH��H�8H�H�K3����m  H�5�  H�H`��m  �   �m  ����������������H�\$H�t$WH��@H��t  I��I��H�D$hH�D$0H�Kp  LcH�@2�N�D��D��I���   t%A���  ��t��t��t��0t���   u@�H���   L�]�  3�H�D$(D�L$ L�I H�L$P�
n  H�|$P ��   H��t)L�6�  H�L$hL��3���m  H�|$h u
H�L$P�   @��teH��o  L��  3�HcH�L�L��H�L$0A���  �D$(I���  M���  H�D$ ��m  H�D$0H��uH�L$P�jo  H��tUH�L$h�HH�D$0L�L$hL�D$PH���  H��H�D$ �C   H�L$P�0o  H��tH�L$h� o  @��tH�L$0�o  H�\$XH�t$`H��@_���������H�T$L�D$L�L$ H��8L��L�L$PH�L$ 3�H�D$     �Om  H�L$ H��t&H��n  L���   M��tH�T$HA��H�L$ ��n  H��8�����������L�L$ D�D$SUVATAUH��PL��$�   L��$�   L��L�D$ ���L�D$0�VM��3�3��kl  ��u�H��PA]A\^][�H��$�   H�|$0L�t$HL�|$@H9��  L��$�   �H��tH���Rk  3�H�E3��A�P��k  H������7  L�7A���tx��tHc�A�    �pk  �  �   ��j  H��H���{  A�A�$�A�F�CA�F�CA�F�CA�F�CA�F�CA�F�C��j  f�CA�   �G�   ��j  H��H����  A�A�$�A�F�CA�F�CA�F�C��j  f�CA�   Lc�L��$�   ���  A�   I��Ǆ$�      �D$    �tj  E�H��I���uj  �����   ��j  ��=&'  t@=@'  t9I���]j  ���H��H�? ��������tHcκ����Ij  ��t�Oj  ��H��t	H����i  H�|$0H��t	H����j  ����   ��x	Hc���i  M��tE3�3ҋ���j  I�E ����   �Ki  H���  H�H`��h  �   �_h  �H��$�   �3�A9,$�Q���Hc�M��H���fi  ���Q������t	���)����K�;i  ��A�$������h  H��  H�H`�hh  �   ��g  ̋�L�t$HH��$�   L�|$@H��PA]A\^][�������������H�\$WH�� 3�H��L������yH�yH�yH�yH�y H�y(H�y0H�y8H�y@H�yHH�yPH��XA�3�� l  H���   A�E3�3��l  H���   H���   H���   H���   H�\$03�H�� _���@SH�� H��H�IH��t�h  H�KH��t��g  H�K(H��t��g  H�K8H��t��g  H�KHH��t��g  H�KX��j  H���   ��j  H���   H��tH�� [H�%�g  H�� [��������@SUVWATAUAVH��pH���  H3�H�D$`H��H�Q I��H��I���   �i  E3�H��E��E��D�l$ H���v  L��H��H��L��$�   �-g  H�<3L��H�|$(L9m vH�E�8/t�\H��H�UH�M H�H;�v@ �H��<.tH;�w���D$    L�E H����f  H�M H� �9/u�\H��H��H;�w�H} H��D�'H;��0  ff�     H�T$0H���2e  ��t@M��tI���pf  �\H;�vH�ˀ;\u�L��H��L+�A���^h  D�+L��H;�w��   � @  f�D$6��   �{�\L�=k�  t�\H��L9-\�  tWf�I�H���H��D8,:u�L�GH����e  H�T$0H����d  ��u� �  f�D$6uI��M9/u��H�M9/tD9l$ tM��t	I����e  H����e  ��   L�|$(M��t+L�u@M��tK�&�     �x�\H�@�u� /I��u�L�e8H�M�`e  ��I��A+��bg  H�u(H�EH��H+�I+�H�]0H�E H��tI�͐H�E�<\u�/A��A��H;M r�H�D$0H���   H�D$8H��   H�D$@H��  H�D$HH��  H�D$PH��  H�D$XH��   L��$�   H�L$`H3���O  H��pA^A]A\_^][�����������H�\$ ATAVAWH�� L��L��H�    A��I��I����f  L��H����  ��H��H�t$HH�|$P�e  I��Hc�I��9\u�/�H����u�I��L;��?  A�>/u3I��@ �;/uH��H;�r�I�~L��H��H��L+��d  H��H+�H�H;���   H�l$@fD  H��H;�s�;/tH��H;�r�H��H+�H��uN�?.uI�.uCI;�v$H��I;�t�    H�π?/t
I;�u�?/uH��H;�s3fD  �;/u(H��H;�r��H��u5�?.u0H;�s��;/uH��H;�r�L��H��H��L+��Ic  H��H+�H��:H;�s2H��} /uH��H;�r�H�{L��H��H��L+��c  H��H+�H��H��H;�����H�l$@H�|$P� I+�I�4$H�t$HM�7H�\$XH�� A_A^A\��������@SH��0H�YM��L��H�T$XH�L$@����H�D$@H���   H�D$XH���   3�H��0[��H�\$WH�� H�YH��A��H��I���pd  H���   H���   H�\$03�H�� _������H�\$WH�� H�Y�AL��A��I��I���Cx�)d  H���   H���   H�\$03�H�� _���������������H�\$H�t$WH�� H�YI��H���ChtH�KX��a  �ch�H�sXH�t$8H�{`H�\$03�H�� _����������@VH��@H�qH��A��H����c  H�D$PH��u�   H��@^ËV`H�NXH�\$XH�|$`�{e  �V`H�NXH����d  D�F`�D$0   H���   L�L$PA��H��H�D$(    H���D$    ��c  D�F`�D$0   H��  L�L$PA��H��H�D$(    �D$    �Vc  H���}c  H���tc  �FhH�|$`H�\$XtH�NX��`  �fh�3�H��@^�������@SH�� H�Y�ChtH�KX��`  �ch�3�H�CXH�� [�������H�\$H�t$WH�� H�YI��H��H��P   u!H�I�3`  H��tdH��P  HǃX      H��X  H��P  H��M_  H��tYH��X  L��H��H�H��P  ��_  H�X  H�\$0H�t$83�H�� _���_  H��  H�H`�C_  �   ��^  ���_  H��  H�H`� _  �   ��^  �����@SH�� �Q�AH�Yk�d�H�Kx�S|H�L��P   H��H   �����H���   H���   H�Hǃh      H��H��`  H;�v.�H�Ȁ8.tH;�w�KP3�H�� [�H��H+�H��`  H��h  �KP3�H�� [��������H�t$ W�0@  � S  H+�H�&�  H3�H��$ @  �APH��H��t
�   ��   H�IH�T$ E3�A��?  ��^  ��y,�_  =3'  u3���   E3�3ҋ���_  H�����   uH���  ��_  H�����   H��$P@  Hc�H�O0L�D$ L��H�H��(  H;�t)�D$ ��xH���  <uH�c�  �M_  H�����@H�X t3H�O`�   D����_  H��H��t�L�G`H�WXH����]  �OhH�_X�GP��H��$P@  H��$ @  H3���H  H��$X@  H��0@  _�������������H�\$ UVWAVAWH��@H�ɺ  H3�H�D$0I��H��H��I��D  H�OH��E3�H+�D��H���]  Hcȅ�yL��]  =3'  u4H�GH�L$ �   A�'  �D$(   H�D$ ��^  ��~	�D$*����_  H���H+�H���H��H�L$0H3���G  H��$�   H��@A_A^_^]���������@SH�� HcAxH��H���  H��L�L$0H��  H��A|H���   ���   H�A�H�C(H�A�H�C H�A�H�C���   �C3�D�@H�ChH�C`H�CX�0`  ��uH�D$0H�H�K8H�� [�����������L��I�[WH��@Ic�3�H��T$pH�AH�D$xM��L�II�C�H�ىQI�{�M�K(M�CI��I�{�I�{�|$p�u]  �T$pH�L$P��]  H�L$PH�C HcD$pH�C(�^^  f�C0 �{4f�{F�cP��ch�H�KxH�{XH�{`H�{p��������u�H�\$XH��@_Ã��  �ǃ�  ����H�\$X3�H��@_�������������@SH�� H��H��x�������  ��x�.Z  ǃ�  ����H�K�[  H�K �[  ���  tEH���  H��t9H�|$0fD  �{ H�;uH�KH;�t��Z  H����Z  H��H��u�H�|$0H�� [�������������@SUVWATAUAVAWH��   H��  H3�H��$�   3�I��L�L$hE��H��H�T$HH�L$`D�D$pH�Կ��D�KD�C/H���  H�L$pH�\$xH�D$ �Y  H��tH�p�H�5ރ  �   L��L�5o�  H�J�H�BI�H��H;�HB�H��H�E��E;�~H���}L��I;�u�L���H�M�t���Y  ���X  H����W  H�l$8L���  I�$I�\$H���   H���   ���  �\$0L�D$XE3�H�\$(H�D$P�D$    �[  L��H�D$@H���8\H�Iu�H�i3H�M(�FY  H��H���c  H�H(L�_u  E��H��L�|$(�XH�HH�H�@H�h H�t$ ��Z  H�OH���H��8u�H�G H�H��H��t�     H��H� H��u�I�D$H��uI�<$�H�8I�L$A�(   A����X  H����  H��r  �@   H�H�HH�@[  I�L$H��uI�$�H�I��I�D$�gX  H���A  H�'r  �@   H�H�HH�@   I�L$H��uI�$�H�I�D$H���D  H��A8u�H��� H��8u�H�D$XH��H�H��4���H�l$@H��H����  H�P H�HL��M��H�l$ ��Y  H�GH���f�H��8u�H�O H�H��H��tf�     H��H� H��u�I�D$H��uI�<$�H�8I�L$I���tW  H���+  H�Tq  H���@   H�HI��H�H�@   �����H�|$H�W|3�H�L$pD�HE��H�\$pH�D$xH��$�   趼��H9\$p��  H�L$pA�   H���H���H�|$pH��u>�WO3�L��wN�%V  H��H����   ��V  H��v  H�H`�JV  �N���U  �L�t$xH��$�   M�~(L;�rXI���   H�����rH��i  �   ��Y  H�VH����U  H��H��u!�EV  H��v  H�H`��U  �O�[U  �H�ۂ  I�>H�؂  I�D>H�Ԃ  I�D>H�Ђ  I�D>H�̂  I�D> H����   �WO3�L���wN�5U  H��H��tP�n�H���  L��I�?H���  I�D?I�$L��$�   H����    �Q����   ����   H���   �wU  H��u  H�H`�U  �   ��T  �I�oH;�r�H���   H�����rH�Zh  �   ��X  H�VH����T  H��H���L����U  H�Qu  H�H`��T  �O�&T  �H�A�H�A H�	L�H���D�����$�   I��������� I��I��I��H��������D*�A��0E�L��H��u�L��$�   M+�H��uZH��I��Ns�rN�I��I����sI���   H�V3���S  H��H����   �OT  H��t  H�H`��S  �O�eS  �L�L;�rXI���   H�����rH�5g  �   ��W  H�VH���]S  H��H��u!��S  H�0t  H�H`��S  �O�S  �L��$�   H�/I��M+��_H  �   H��uA�UM3�L��D���uL��R  H��H����   ��S  H��s  H�H`�$S  �M���R  �M�~L;�rXI���   H�����rH�rf  �   ��V  H�VH����R  H��H��u!�,S  H�ms  H�H`��R  �O�BR  ̸
  fA�>H��u;�WO3�L���TR  H��H����   ��R  H�#s  H�H`�yR  �O��Q  �I�oH;�r]H���   H�����rH��e  �   �4V  H�VH����Q  H��H��u!��R  H��r  H�H`�R  �O��Q  ̸
  L��H��H��fA�?�6���H��H��uVH��t	H���`R  H�l$@H�D$PH��t	H���HR  H����T  ���H��$�   H3��L=  H�Ĩ   A_A^A]A\_^][�H��H� H��tH��H� H��u�I�$H�I9\$uI�L$H�|$PH�t$HL�L$hI�$L��~  H��A��H��LE������L�L$`LcFA��  ��t��M9��  t��;�r�;�u��   s��M���  A��  M;�(   vM��(   H��t	H���bQ  H�L$@��S  3������"Q  H�cq  H�H`��P  �   �6P  ���P  H�@q  H�H`��P  �   �P  ���P  H�q  H�H`�sP  �   ��O  ���P  H��p  H�H`�PP  �   ��O  �����@SH���  H� �  H3�H��$�  M��L�D$(L���   H�T$ H���H��A�<  u�H;��   tA��  ������   H�T  3҉T$@HcI�H�L��H�A(H�T$X��$�   H�D$HH�S  H�T$PHcI�H�T��H��$�   H���  H�D$0H��$�   H���  H���iD  H�\$(��uH�L$@H���tR  H��R  L��E3�HcH�H�T��H�D$0H���  H�YS  HcH�H�T��H�L$ ���   ����3�H��$�  H3��:  H���  [����@SWAVH��   H���  H3�H�D$PH��H�L$(H���   I��L��H�\$ H���	  H���H���< u�H;��   tL��A��  H���k����  H����  H���   �T���.t�� uL��A��  I��H���3�����  H����  3���N  ����  H��$�   3�L�|$`I���  I���  I��h  I��`  A���  A���  3�H�l$0H�D$8H�D$@�����A�V|H��L�=?{  D�MH�L$0A��   LE��߳��H9l$0uL��E3���  I���E�������%  H�L$0A�   I��H�t$xL�d$p�P���H�\$0H��uB�SO3�H���{ND�c�)M  H��H����   ��M  H��m  H�H`�NM  �O���L  �H�t$8H�|$@L�fL;�rYI��$�   H�����rH��`  �   ��P  H�WH����L  H��H��u!�KM  H��m  H�H`��L  �K�aL  �H�Iz  H��Gz  �D�@z  f�DH���H��ueL��H��A8,7u�H��Ns�N   �H��H����sH���   H�W3��.L  H��H����   ��L  H��l  H�H`�SL  �K��K  �H��A8,7u�I�H;�rXH���   H�����rH��_  �   �P  H�WH����K  H��H��u!�SL  H��l  H�H`��K  �K�iK  �I���I��C8,u�I�I����@  H�Hy  A�   I���)K  ����   H��uB�PO3�H���xND�x�GK  H��H����   ��K  H�l  H�H`�lK  �O���J  �L�~L;�rXI���   H�����rH��^  �   �'O  H�WH����J  H��H��u!�tK  H��k  H�H`�K  �K��J  �H��x  H���x  �D��x  f�D�wx  �DI��L�l$hA�   H��uA�SO3�H��E���{N�fJ  H��H����   ��J  H�5k  H�H`��J  �O��
J  �L�~L;�rXI���   H�����rH��]  �   �FN  H�WH���J  H��H��u!��J  H��j  H�H`�*J  �K��I  ̸
  f�H��uB�SO3�L���{ND�c��I  H��H����   �CJ  H��j  H�H`��I  �O��YI  �M�gL;�rYI��$�   H�����rH�']  �   ��M  H�WH���OI  H��H��u!��I  H�"j  H�H`�xI  �K��H  �H��v  @�l$OI���������I�H��v  I�DMc��  L�|$Offff�     I��I��I��H��������D*�A��0E�L��H��u�H�t$OI+�H��uZL��H��Ns�zN�H��H����sH���   H�W3���H  H��H����   �I  H�Si  H�H`��H  �K�(H  �I�H;�rXH���   H�����rH��[  �   �eL  H�WH��� H  H��H��u!��H  H��h  H�H`�IH  �K��G  �L�D$OI�I��M+��%=  H��uA�SO3�H��M���{N��G  H��H����   �XH  H��h  H�H`��G  �O��nG  �L�~L;�rXI���   H�����rH�=[  �   ��K  H�WH���eG  H��H��u!��G  H�8h  H�H`��G  �K�G  �A�
  fD�$H��u;�SO3�L���G  H��H����   ��G  H��g  H�H`�CG  �K��F  �M�oL;�rXI���   H�����rH��Z  �   ��J  H�WH����F  H��H��u!�KG  H��g  H�H`��F  �K�aF  �M��H��H��fE�$����H��H��u[H��t	H���/G  L�L$ E3���  I���*������L�l$hL�d$pH�t$xH��$�   L�|$`H�L$PH3��2  H�Ā   A^_[�H��H� H��t�    H��H� H��u�I���  H��u	I���  �H�L�L$ I���  E3���   I������L�L$(McFA��  ��t��M9��  t��;�r�;�u��   s��M���  A��  M;�(   vM��(   3�� ���L��A��  I��H���s����#�����������������H�\$H�L$WH�� H��I  H��I��HcI� H�T��H��H������H���   L�L$0H��r  A�   ��I  ��uH�L$0H��H�	��F  H�BI  HcH�H�L��ǁ�   �   H���,I  ���u�H�\$8H�� _�H��G  H�\$8HcH�H�L��3�Ɓ�   H�� _�������������@UWAVH���  H�l$0H���  H���  H�j�  H3�H���  �$E3�I��H��L�ED�uD�u �  H+�H�L$0��  H�MD�1�oE  D�u0H��X   H�E8H��G  L�uHD���   L�u@HcH�H�T��H���   H���  H�E H���   H���  H�U���8  H�u��uT�   H�E0L�E�KH��D��L�uH�D$ �)E  ��u&H�EH��t"�xuD90tA��H�M�]��F  ��]��]H��F  H�}HcH�L�D��H�E I���  D87tH��H���}E  D8u t	H����F  ��H���  H3���.  H���  H���  H���  A^_]����H�\$H�l$H�t$H�|$ AVH��0H�/G  E3�I��LcI� H��N�L��H��A��I�H��h  u(H��`  �f;K  u�A:K  u	H9��   u�   L9�X   u��uB�������u8H��F  E3�3�HcH��D$    H�L��L�1D�OH��p   ��C  �  L9�X   tL��H��������t[����   L��H��H���������tL��A��  H��H����������+  H�7F  HcH�H�T�����   0  uD���   3���F  D�OH��p   E3�3��D$    ��B  H�H��E  Hc
H�L��L�1Hc
H�H�L��L��@  ��   L9�X   t{H�E  H�B`H�\�  H��E  HcH�H�L��H�����D���   3�H�B`�F  H��D  H�"�  H�kE  H�H`H�Hc
H�L��Ɓ�   Hc
H�H�L��L��@  L��H��H��������tD�OH��p   E3�3��D$    �B  H�E  HcH�H�L��L�1H�\$@H�l$HH�t$PH�|$X3�H��0A^��@SH�� H��H�	H�I��A  H�E3�LcXH� A��D�@E��t?�L9\�t	��A;�r��.A�H�;�s#�     D�BJ�L�H�L��HA����D;�r��H��  ��tU�     N9��  t
A��D;�r��9�J�D;�s+@ E�AA��J���  E��H���  ��  ��D;�r���  H�����H�H�� [H�%\@  ������������H��H�XH�h H�HVWATAVAWH��P3�I����:[I��M��H��H�x�D��H�x�@�  ��   H�J��=  H��H��u����   �]   H����?  H��$�   H��tV@�8H��$�   H��H��$�   ���:��   H�HH��$�   A�
   �|>  ��$�   ��=��  ��   H��$�   �R?  H��l  L��H�H`��>  H�΃���Z?  M��t	I���L?  I���v	I����?  ��L�\$PI�[@I�kHI��A_A^A\_^Ä�tY�H����<  H��H�������:   H����>  H��$�   H���h���@�8H��$�   H��H��$�   ����H��$�   H���<���H��$�   H�D$@L���  H�\$0H�D$(H���  H��$�   H��H�D$ ����Lc�I���uBH�L$@D��$�   H�9k  H��L��H��HE�H��k  H�D$ �����H�L$@��@  �����H���L�����H�5�  L;�A�L�%��  IG�3҉=��  H���  ��$�   L�%��  ���     H�5��  ���  ��A  H���fD  H��A8<u���I����?  L��H���J���H���  H���  H��t>H���H��@8<+u���H����?  H������H�m�  H�n�  �4�     �"���H�=P�  H�=Q�  ��     ������H�\$H�l$H�t$WH��0H��H��H�T$X3�H��I��H�\$X������yIL�L$XL�G H�kj  H������H�L$X��?  D�OH��p   E3�3��D$    �j=  ����   ��u>�xuL��A��  H��H��������hD�GH�N�   �����L��H��H��������D�NLcG��t@ ��L9D�t��;�r�;�u��   s
��L�D��FL;�(   vL��(   3�H�\$@H�l$HH�t$PH��0_�����H�\$H�t$WH��0���  H��H����   ���  ��xjH���  H�9 u]L�D$H������t1D�KH��p   E3�3��D$    �[<  ���H�\$@H�t$PH��0_�H�|$H u���  ��:  ǃ�  ����H�SL�D$HH���  ������t=3'  u�H���   u'���   }D�KH��p   E3�3��D$    ��;  H�\$@H�t$P3�H��0_��������H�\$UVWH��0H�YH�1Hc�A��H��H9�N  Hc�h   H�D$P    H�ȉD$X��:  H��H���  H�L�D$XH���9;  L��3ҋ�H���Y;  ����   ��  ��:  H����   �L$XH�t$(L�ωL$ D��H��H��H�D$P�B������tJH��p   L�D$PA�   ���D$(   H�D$     ��:  H�T$PL��H�������3�H�\$`H��0_^]�H��g  H�������H���:  H���b:  3�H�\$`H��0_^]���9  H�Z  H�H`�\9  �   ��8  ���9  H��Y  H�H`�99  �   ��8  �H��p   L�D$P��:  ��u0��   tH�T$PL��H��H��W@��tH�T$PL��H��H��W3�H�\$`H��0_^]���������H��H�HSWH��X3�H��9�  �  H�hH�pL�p�L�5����H�5��  H�-�����H���  H���  A�  H�D$p   ��8  H���  H���  A�  ��8  ���  H�D$pL���  H�r�  ��E3�H�D$ ��9  ��~'H�T$0H�B�  H�|$0H�t$8H�l$@L�t$H������t��8  ='  u'9:�  �T�����H��$�   H�l$xL�t$PH��X_[�E3�3ҋ���9  H�d  H��L��H�������H����:  ���뷋�H��X_[����      ������@SUVWATAVAWH���  H�7�  H3�H��$�  E3�I��H�D$@�D$0   L�2L  L�L$HD�d$(L���L�d$H�D$@   A��H�D$ A��E����9  �����   ��St��tuH�\$H�H�|$HH�D$@�D$0   L�L$HL��K  I�֋�D�d$(H�D$ �m9  ���u�H����   H�T$`H����5  ��t$��6  H��d  L��H�H`��6  �   �  �D$f� �  f#�� @  f;�t$��6  H��d  L��H�H`�P6  �   �O  H��$�   L��H����:  H��t6H��$�   �,H��$�   L�ź  �%7  H�nd  H��$�   H��HD�HcD$@;�~M�<�M��L��H��H�l$ ��������C���H�79  H�L$PD��  �5  L�D$PH�L$`�4   ��4  ��tH��a  H�L$`A�   �6  L�D$`H�|F  H��c  L��H�\$ ��5  H������   ��4  H���y���H���  ��8  H�U�  �6  H���  H��t��5  H�~�  H��t��5  H�|�  H��t�y5  3�H��$�  H3��   H���  A_A^A\_^][������L�L$ H�T$UVWATAUAVAWH��P�A�iD�iD�aI��L��3ɉ�$�   I��D��H�L$(D��H�L$0H�L$8D��H�L$ ��H��$�   D��H��u(��6uH��Q  H��tI����3�H��PA_A^A]A\_^]�H�\$H��)u
M��L�D$(�N��+u
M��L�D$0�?��uL�D$8�0��u
M��L�D$ �!��uI��H��$�   ���w�  ���sM��I��L�L��$�   I;��J  H��a���;��-A� u����$�   = @ ��  �E���4��  H�����  H�Oa��H���@����  @��
��  A�&I�F����H�CP  H��tI���Ѕ��S  L��$�   @��Hu!�   H��$�   L�L$ L�T$(L�\$0�  A�&��  @��TuA�&�A��   �n  @��E��   A�&��   A�FD�e��M  A�&I�F����H��O  H��tI���Ѕ���   L��$�   @��
�k���@���a���@��H��   �   �M����   ��  �   ��  �	   ��  �G�<wc@�ǽ
   f��0fA�F�  @��.u
�   �  @��0|6@��90A�N��f��f�@��f�f��0fȸ�  fA�Nf;��p  A�FH+�H���  �G�<	w�@�ǽ   f��0fA�F�B  @�� u
�   �2  @��0|�@��9�A�N��f��f�@��f�f��0fȸ�  fA�Nf;�w���  �G�<	w@�ǽ   f��0fA�F��  @�� �]�����  �G�<	w5A�N��f��f�@��f�f��0fȸ�  fA�Nf;��!����  @��
�(  @��t@�� �����   �h  @��u
�   �X  @��
��$�   H��^���N  �(   �D  @���,  @��
�"  A�&I�F����H��M  H��tI���Ѕ������L��$�   �G�<�z���@��A�F A�   �������   H�^��H������  H���A�F�   � ���A�F �   �����E�f�   �����A�F�   �����A�F
�   �����A�F�   ����A�F�   ����A�F�   ����A�F�   ����A�F�   �z���A�F�   �k���A�F�   �\���A�F�   �M���A�F�   �>���@���p���A�NH��Ȱ: @�� u(��tA�Ā< tA�FH��$�   �   A���  ����  A��@:<��  ��uNA��u@��H��  H��$�   A�FA���e  A����  @��P��  H��$�   A�F	A���<  ����   A��uU@��OuH��$�   A�FA���  @��EuH��$�   A�FA����  @��-�  H��$�   A�FA����  A��u@��A��   H��$�   A�FA���  A����   @��A��   H��$�   A�FA���  A��uh����   @��RuH��$�   A�FA���`  ����   @��UuH��$�   A�FA���<  ��ug@��AuaH��$�   A�FA���  A��u ��uA@��Su;H��$�   A�FA����  A��u ��u@��PuH��$�   A�FA����  A�FA��H��$�   �  @�� ��  @��/t%@��*t@�� @��a@���#���L���   �  L��H��H��$�   �   �n  @�� ,a<�^  @��:u
�   �N  @��.u
�   �>  @��0@��	������   �&  �   �  @�� ,a<�  @��/t@��,~
@��:��  @�� t@��/�c���@��:�l����   ��  M��t*H�QI  H��tL��I��I��M+��Ѕ��@���L��$�   3�D���h������G�<	��  @�� t�@��/����H��H��$�   �   �f  H��Y��@�ǀ�p�   �M  @��
��  @���_  @�� ��   @��#tY@��?�����H��t'H��H  H��tL��I��L+��Ѕ������L��$�   L�L$ L�T$(L�\$03���H��$�   �h��   H��t'H�?H  H��tL��I��L+��Ѕ��A���L��$�   L�L$ L�T$(L�\$03���H��$�   �h�   M��t"H� H  H��tL��I��I��M+��Ѕ������H��$�   3�D��H��t!H��G  H��tL��I��L+��Ѕ������3ɽ   H��H��$�   L�L$ L�T$(L�\$0L��$�   H�LX����$�   H��I;���  ��$�   ����M��t"H�YG  H��tL��I��I��M+��Ѕ��H���H��$�   3�D��H��t!H�G  H��tL��I��L+��Ѕ�����3�A�F  	 �'   �L���M��t"H��F  H��tL��I��I��M+��Ѕ������H��$�   3�D��H��t!H��F  H��tL��I��L+��Ѕ������3�A�F  	 �(   �����H�NW��@�ǀ�p�   tL��H�\$ �   �����@��
ts@��t)@�� �����@��#t@��?�E��������   ����M��t*H�%F  H��tL��I��I��M+��Ѕ�����L��$�   3�A�F  	 �i'D������M��t*H��E  H��tL��I��I��M+��Ѕ������L��$�   3�A�F  	 �i(D���s���H�jV��@�ǀ�p�   ����@��
�b  @����   @�� th@��#t@��?�k��������M��t*H�ME  H��tL��I��I��M+��Ѕ��D���L��$�   H��$�   L�T$(L�\$03�D��H�D$ �h����M��t"H�E  H��tL��I��I��M+��Ѕ������H�T$ 3�D��H��t!H��D  H��tL��I��L+��Ѕ������3�H��$�   L��H�L$ �   � ���M��t"H��D  H��tL��I��I��M+��Ѕ������H�T$ 3�D��H��t!H�XD  H��tL��I��L+��Ѕ��R���3�H��$�   L��H�L$ A�F  	 �'   ����M��t"H�D  H��tL��I��I��M+��Ѕ�����H�T$ 3�D��H��t!H��C  H��tL��I��L+��Ѕ������3�H��$�   L��H�L$ A�F  	 �(   ����H�lT��@�ǀ�p�   tH�\$8�   �����@��
�����@���B���@�� ����@��#�����@��?�Z���H�\$8�   ����H�T��@�ǀ�p�   �����@��
�  @����   @�� t@��#�����@��?�����v���M��t"H��B  H��tL��I��I��M+��Ѕ������H�L$83�D��H��t"H��B  H��tH��L��L+�I���Ѕ������H��$�   H�|$8�   �����M��t"H�B  H��tL��I��I��M+��Ѕ��n���H�L$83�D��H��t"H�VB  H��tH��L��L+�I���Ѕ��=���H��$�   3�A�F  	 �i'H�L$8�r���M��t"H�B  H��tL��I��I��M+��Ѕ������H�L$83�D��H��t"H��A  H��tH��L��L+�I���Ѕ������H��$�   H�|$8A�F  	 �(   �����@�� ����@��H������   ������    ������!   ������"   ������#   ������G�<�Q���@�ǽ$   f��0fA�F����@��.������%   �����G�<	����@�ǽ&   f��0fA�F�t���@��u
�'   �d���@��
�(����(   �P���@��
������(   �<���@��u
�-   �,���@��
��  H�nQ��@����p�  �������3���L�Ӎo)H�\$(D���ct>��t.��t��H�/Q��tD�������A�   �����A�   ����A�   ����A�   ����H��P��@����p�  ���'  A�E�����   H�����  H��P��H���3�A�Ā�oH��$�   D��   DD��H���3�A�Ā�nH��$�   D��   DD��'�����$�   A��H�iP����nt��tH��$�   A�   �    DE�� ���H��$�   A�   �����A��H�(P��A��
w+A:�  u!H��$�   ��$�   A��	�����E������3�D��H��$�   ����A��H��O��A��w�A:�0 u�H��$�   ��$�   A���m���E�l$��c���A��H��O��A��w�A:�H u�H��$�   ��$�   A���0���E�l$��&���A��H�aO��A���`���A:�X �R���H��$�   ��$�   A�������E�l$������A��H�O��A������A:�p ����H��$�   ��$�   A�������E�l$����H��$�   @�� ��$�   H��N���|���3�D���r���@��:uZM��t1H��=  H��t%L��I��I��M+��Ѕ������L��$�   H�{N��H��$�   L�L$ L�\$03�D��H�D$(�h*����@��u\�,   M��t1H��=  H��t%L��I��I��M+��Ѕ��g���L��$�   H�N��H��$�   L�L$ L�\$03�D��H�D$(����@��
�(���M��t1H�'=  H��t%L��I��I��M+��Ѕ�����L��$�   H��M��H��$�   L�L$ L�\$03�D��H�D$(�h(�E���@�� �4���3�@��L�ۀ� H�\$0�+   D��@��uPH��<  H��tE3�H��I���Ѕ������L��$�   3�H��$�   L�L$ L�T$(L��H�L$0D��,   �����@��
uMH�[<  H��tE3�H��I���Ѕ��5���L��$�   3�H��$�   L�L$ L�T$(L��H�L$0�(   �r���A�̓�	t}��tT��t2H��$�   ��$�   ��H��L��t
3�D���L���A�@3�D���>���3���cH��$�   D��   DD������G�<	�����H��$�   @�ǃ�0H�I�F�������$�   H�2L����kuH��$�   A�   �����3���cH��$�   D��   DD�����D��A�� @��uKM��t"H�2;  H��tL��I��I��M+��Ѕ��	���H��$�   L�L$ L�T$(3�D��H�D$0�h,�H���@��
��   M��t"H��:  H��tL��I��I��M+��Ѕ������L��$�   3�D��H�D$0H��$�   L�L$ L�T$(A�ͽ(   ����  ����  ��$�   ��H�%K�������A������A���L  Ic�H�K�����4�  H��J��H���@�� ������G�<	����@�ǃ�0Hc�I�FH��H�JH��$�   I�F�X���A��H��J��A��w-E:�x u#��$�   L��$�   A���;���E�l$
�1���L��$�   3�D������A��H�ZJ��A��
w�E:�� uԋ�$�   L��$�   A��	�����E�l$�����A��H�J��A��w�E:�� u���$�   L��$�   A�������E�l$����L��$�   @�� ��$�   H��I�������3�D���{���3��+   D���V���A��U���A��L���A����t7H��8  H��tI���Ѕ������L��$�   A��   �   D��_���3ɋ���$�   �uA�~uA�FH��8  H��tI���Ѕ�t���g���A��L��$�   A8~��  A������ u3�t�.   �ZI�FH��t~�5   �H��tI���W  ��u�h6�2H�/8  H��tI���Ѕ������A�L��$�   �   �   D�H��$�   L�L$ L�T$(L�\$0H�~H���4���I�~I��H+�H;�HB�H������H��7  H��tL��H��I����L��$�   H��H�I)~�2���H��7  H��tI���Ѕ��Y���A�H��$�   �   �   D�����I��H+������H�F7  H��tL��H��I����L��$�   H��H������H��G��@����p�  ��������H���/   I�F�B���H��G��@��u
�0   �2���@����p�  ���u"@��;t
@�� �����H��$�   �1   �����I�NH��H��$�   H��H�I�N�����@����$�   H�G��������0   �����I�~ ��$�   H��F��uA� �(   �����2   ����I�~I��H+�H;�HB�H��t%H�6  H��tL��H��I����L��$�   H��H�I�F�3   H;�D�H+�I�F�����4   �)����.   ����H��5  H�������I���Ѕ������H+�H���  ��$�   M��t"H�5  H��tL��I��I��M+��Ѕ��^���H�T$0H��tH�[5  H��tL��I��L+��Ѕ��5���H�L$8H��t"H�"5  H��tH��L��L+�I���Ѕ��	���H�T$ H��tH��4  H��tL��I��L+��Ѕ������H��$�   H��tH��4  H��tL��I��L+��Ѕ������M��t"H��4  H��tL��I��I��M+��Ѕ������H��$�   A�nE�nE�fA�~H�\$HH��PA_A^A]A\_^]�f���  '�  `�  5�  ��  ��  ɟ  ӟ  �  M�  k�  ��  �  E�  S�  w�  Ģ  �  ?�  ��  e�  ��  �  G�  ��  ��  ��  �  ��  ��  í  ͭ  ׭  �  �  �  9�  ]�  q�  �  o�  �  ��  a�  =�  k�  ��  ո  $�  z�  ��  ��  �  �  �  ��  ��  �  �  ��  ��  ��  .�  =�  L�  [�  j�  ��  y�  ��  ��  ��  D�  e�  ��  ί  �  X�  ��  ڰ  �  �  �  �  ��  @�  @�  @�  @�  @�  @�  @�  @�  ��  �  ��  @�  M�  ��  ٵ  �  �  �  f�y vf�y v�������������������������H��(��ti��tY��tI��t9��tH�AF  H��(��6  H�F  H�8�  D���g  H�(�  H��(�H��E  H��(�H��E  H��(�H��E  H��(�H�pE  H��(����H��(H�=̤   u
�   H��(�H��H�\$ H���  H��A�  �"  ���   H���H���< u�H��H���  �~  H�\$ 3Ʌ��   Dʋ�H��(��%�  �%r  �%  �%�  �%�  ������ff�     H;)r  uH��f����u��H���M   �@SH�� H���  �   ��  �g  H���e  �=҂   u
�   �L  �	 �H�� [�I  ���H�L$H��8�   ��  ��t�   �)H��}  �(  H�D$8H��~  H�D$8H��H�6~  H��~  H� }  H�D$@H�~  ��|  	 ���|     ��|     �   Hk� H��|  H�   �   Hk� H�q  H�L �   Hk�H�q  H�L H��  �����H��8����H��(�   �   H��(�̉L$H��(�   ��  ��t�D$0���)H��|  �:  H�D$(H��}  H�D$(H��H�N}  H��}  H�|  ��{  	 ���{     �|     �   Hk� H��{  �T$0H�H�  �6���H��(��H��(�MZ  f90@��t3��8Hc_@��H�@��H��8PE  u�  f9Hu�3Ƀ��   v	9��   ����  �   ��  H�����  H��  H�9�  H�:�  ��  �H�{  �̀  ��:  �  �=�o   uH��  �{  �=�o  �u	����q  3�H��(���H��8H�1  ��  ���  D�w�  �i�  H�b�  L�O�  H�@�  H�1�  H�D$ �>  �$�  ��y
�   ��  H��8��@WH�� eH�%0   H�H3�3��H�L�  tH;�u�   ���>�  ��u
�H�  �?�)�  ��u+��     H�  H��  �  ��t��   ��   ��     ��  ��uH��  H��  �t  �ȡ     ��u	3�H���  H�=ˡ   t"H�¡  �  ��tE3�A�P3�H���  ��H�0  H��  H�L�  H�  �  �mm����~  �=   u����  �=�~   u��  ��~  �-��~  �=�~   u	����  ̃=�~   u��  ��~  H�� _�H��(�c  H��(�~����%�  �%�  �%�  �%�  �%�  H��(H��8csm�u�xu�H ����l��v�� @�t3�H��(��K  �H��(H������@  3�H��(���%>  �%0  3���������������LcA<E3�L��L�A�@E�XH��I�E��t�PL;�r
�H�L;�rA��H��(E;�r�3��������������H�\$WH�� H��H�=�<��H���4   ��t"H+�H��H������H��t�@$���Ѓ��3�H�\$0H�� _����H���MZ  f9t3��HcH<H�3��9PE  u�  f9Q�����H�\$ UH��H�� H�l  H�e H�2��-�+  H;�uoH�M�6  H�EH�E��  ��H1E�,  H�M ��H1E�$  �E H�� H�MH3E H3EH3�H�������  H#�H�3��-�+  H;�HD�H��k  H�\$HH��H��k  H�� ]�@SH�� H�=�   u6�   �J�  H��H����  H�˞  H���  H��u�C�H�# 3�H�� [���@SH�� H��H���  �N  H�D$8H���uH����  �~�   �   �H�n�  �   H�D$8H�T�  �  H�D$@H����
  H��L�D$@H�T$8��   H��H�L$8��
  H�!�  H�L$@��
  H��  �   �   H��H�� [�H��(�G���H�������H��(��H�\$WH�� H�c>  H�=\>  �H�H��t��H��H;�r�H�\$0H�� _�H�\$WH�� H�;>  H�=4>  �H�H��t��H��H;�r�H�\$0H�� _��%�
  �%�
  �%�
  �%�
  �%�
  �%�
  �%z
  �%d
  �%�	  ��H��(M�A8H��I���   �   H��(����@SH�� E�H��L��A���A� L��tA�@McP��L�Hc�L#�Ic�J�H�C�HHK�At�A���H�L�L3�I��H�� [�����������ff�     H��L�$L�\$M3�L�T$L+�MB�eL�%   M;�sfA�� �M�� ���A� M;�u�L�$L�\$H����%D	  �%F	  �%H	  �%�  �%D	  ����@UH�� H��H�H�ы�����H�� ]����@UH�� H��H�3Ɂ8  �����H�� ]��@UH�� H��   �����H�� ]��                                                                                                     d!     �      �      �      !     $!     �!     z!     �      N!     4!             �!     �!     �!     �!     �!     �!     �      �      �      �      �      `      L      @      4                        �     �     �     �     �     �     �     �     j     L     4               �     �     �     �     �     �     �     �     �     �     |     r     j     ^     T     J     B     8     .     $                    �     �     �     �     �     �     �     �     �     �     �     �     �     �     z     l     b     X     N     F     >     4     ,     �!             	      �      �      �      �      �      �      �      �      �      �      �o      �        *     T     f     �     �     �     �     �     �          .     H     h     �     �     �     �     �     �           �     �     �     �     �     �     n     T     .          �     �     �     �     �     �     t     b     D     4                �     �     �     �     �     |     l     N     4     &          �     �     �     �     �     �     �     r     V     D     2          
     �     �     �     �     �     �     v     V     @     4     &          �     �     �     �     �     �     t     ^     L     2                �     �     �     �     �     �     �     x     f     P     >     ,          �     �     �     �     �     �     �     f     R     D     2     &                �     �     �     �          �                     �� @                   �� @   |� @   �� @               �6\       I   � ��      �6\    
      � ��      �6\          � ��      �;@    <@   _SERVER title   interactive     process-begin   no-chdir        php-ini define  process-end     profile-info    process-file    file    help    info    syntax-check    modules no-php-ini      no-header       process-code    hide-args   run syntax-highlight        syntax-highlighting server      docroot strip   usage   version zend-extension  rf      rfunction   rc  rclass  re      rextension  rz  rzendextension  ri      rextinfo    ini %s
     PHP_SELF        SCRIPT_NAME     SCRIPT_FILENAME PATH_TRANSLATED DOCUMENT_ROOT   0       report_zend_debug   1   display_errors  cli     Command Line Interface  extension_filename  dl  cli_set_process_title   cli_get_process_title   php             Usage: %s [options] [-f] <file> [--] [args...]
   %s [options] -r <code> [--] [args...]
   %s [options] [-B <begin_code>] -R <code> [-E <end_code>] [--] [args...]
   %s [options] [-B <begin_code>] -F <file> [-E <end_code>] [--] [args...]
   %s [options] -S <addr>:<port> [-t docroot]
   %s [options] -- [args...]
   %s [options] -a

  -a               Run interactively
  -c <path>|<file> Look for php.ini file in this directory
  -n               No php.ini file will be used
  -d foo[=bar]     Def