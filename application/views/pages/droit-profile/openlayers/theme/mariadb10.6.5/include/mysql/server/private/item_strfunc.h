#ifndef ITEM_STRFUNC_INCLUDED
#define ITEM_STRFUNC_INCLUDED

/*
   Copyright (c) 2000, 2011, Oracle and/or its affiliates.
   Copyright (c) 2009, 2019, MariaDB

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; version 2 of the License.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1335  USA */


/* This file defines all string functions */

#ifdef USE_PRAGMA_INTERFACE
#pragma interface			/* gcc class implementation */
#endif

extern size_t username_char_length;

class Item_str_func :public Item_func
{
protected:
  /**
     Sets the result value of the function an empty string, using the current
     character set. No memory is allocated.
     @retval A pointer to the str_value member.
   */
  virtual String *make_empty_result()
  {
    /*
      Reset string length to an empty string. We don't use str_value.set() as
      we don't want to free and potentially have to reallocate the buffer
      for each call.
    */
    if (!str_value.is_alloced())
      str_value.set("", 0, collation.collation); /* Avoid null ptrs */
    else
    {
      str_value.length(0);                      /* Reuse allocated area */
      str_value.set_charset(collation.collation);
    }
    return &str_value; 
  }
public:
  Item_str_func(THD *thd): Item_func(thd) { decimals=NOT_FIXED_DEC; }
  Item_str_func(THD *thd, Item *a): Item_func(thd, a) {decimals=NOT_FIXED_DEC; }
  Item_str_func(THD *thd, Item *a, Item *b):
    Item_func(thd, a, b) { decimals=NOT_FIXED_DEC; }
  Item_str_func(THD *thd, Item *a, Item *b, Item *c):
    Item_func(thd, a, b, c) { decimals=NOT_FIXED_DEC; }
  Item_str_func(THD *thd, Item *a, Item *b, Item *c, Item *d):
    Item_func(thd, a, b, c, d) { decimals=NOT_FIXED_DEC; }
  Item_str_func(THD *thd, Item *a, Item *b, Item *c, Item *d, Item* e):
    Item_func(thd, a, b, c, d, e) { decimals=NOT_FIXED_DEC; }
  Item_str_func(THD *thd, List<Item> &list):
    Item_func(thd, list) { decimals=NOT_FIXED_DEC; }
  longlong val_int() override;
  double val_real() override;
  my_decimal *val_decimal(my_decimal *) override;
  bool get_date(THD *thd, MYSQL_TIME *ltime, date_mode_t fuzzydate) override
  { return get_date_from_string(thd, ltime, fuzzydate); }
  const Type_handler *type_handler() const override
  { return string_type_handler(); }
  void left_right_max_length();
  bool fix_fields(THD *thd, Item **ref) override;
};



/*
  Functions that return values with ASCII repertoire
*/
class Item_str_ascii_func :public Item_str_func
{
  String ascii_buf;
public:
  Item_str_ascii_func(THD *thd): Item_str_func(thd) {}
  Item_str_ascii_func(THD *thd, Item *a): Item_str_func(thd, a) {}
  Item_str_ascii_func(THD *thd, Item *a, Item *b): Item_str_func(thd, a, b) {}
  Item_str_ascii_func(THD *thd, Item *a, Item *b, Item *c):
    Item_str_func(thd, a, b, c) {}
  String *val_str(String *str) override
  {
    return val_str_from_val_str_ascii(str, &ascii_buf);
  }
  String *val_str_ascii(String *) override= 0;
};


/**
  Functions that return a checksum or a hash of the argument,
  or somehow else encode or decode the argument,
  returning an ASCII-repertoire string.
*/
class Item_str_ascii_checksum_func: public Item_str_ascii_func
{
public:
  Item_str_ascii_checksum_func(THD *thd, Item *a)
   :Item_str_ascii_func(thd, a) { }
  Item_str_ascii_checksum_func(THD *thd, Item *a, Item *b)
   :Item_str_ascii_func(thd, a, b) { }
  bool eq(const Item *item, bool binary_cmp) const
  {
    // Always use binary argument comparison: MD5('x') != MD5('X')
    return Item_func::eq(item, true);
  }
};


/**
  Functions that return a checksum or a hash of the argument,
  or somehow else encode or decode the argument,
  returning a binary string.
*/
class Item_str_binary_checksum_func: public Item_str_func
{
public:
  Item_str_binary_checksum_func(THD *thd, Item *a)
   :Item_str_func(thd, a) { }
  Item_str_binary_checksum_func(THD *thd, Item *a, Item *b)
   :Item_str_func(thd, a, b) { }
  bool eq(const Item *item, bool binary_cmp) const
  {
    /*
      Always use binary argument comparison:
        FROM_BASE64('test') != FROM_BASE64('TEST')
    */
    return Item_func::eq(item, true);
  }
};


class Item_func_md5 :public Item_str_ascii_checksum_func
{
public:
  Item_func_md5(THD *thd, Item *a): Item_str_ascii_checksum_func(thd, a) {}
  String *val_str_ascii(String *) override;
  bool fix_length_and_dec() override
  {
    fix_length_and_charset(32, default_charset());
    return FALSE;
  }
  LEX_CSTRING func_name_cstring() const override
  {
    static LEX_CSTRING name= {STRING_WITH_LEN("md5") };
    return name;
  }
  Item *get_copy(THD *thd) override
  { return get_item_copy<Item_func_md5>(thd, this); }
};


class Item_func_sha :public Item_str_ascii_checksum_func
{
public:
  Item_func_sha(THD *thd, Item *a): Item_str_ascii_checksum_func(thd, a) {}
  String *val_str_ascii(String *) override;    
  bool fix_length_and_dec() override;
  LEX_CSTRING func_name_cstring() const override
  {
    static LEX_CSTRING name= {STRING_WITH_LEN("sha") };
    return name;
  }
  Item *get_copy(THD *thd) override
  { return get_item_copy<Item_func_sha>(thd, this); }
};

class Item_func_sha2 :public Item_str_ascii_checksum_func
{
public:
  Item_func_sha2(THD *thd, Item *a, Item *b)
   :Item_str_ascii_checksum_func(thd, a, b) {}
  String *val_str_ascii(String *) override;
  bool fix_length_and_dec() override;
  LEX_CSTRING func_name_cstring() const override
  {
    static LEX_CSTRING name= {STRING_WITH_LEN("sha2") };
    return name;
  }
  Item *get_copy(THD *thd) override
  { return get_item_copy<Item_func_sha2>(thd, this); }
};

class Item_func_to_base64 :public Item_str_ascii_checksum_func
{
  String tmp_value;
public:
  Item_func_to_base64(THD *thd, Item *a)
   :Item_str_ascii_checksum_func(thd, a) {}
  String *val_str_ascii(String *) override;
  bool fix_length_and_dec() override;
  LEX_CSTRING func_name_cstring() const override
  {
    static LEX_CSTRING name= {STRING_WITH_LEN("to_base64") };
    return name;
  }
  Item *get_copy(THD *thd) override
  { return get_item_copy<Item_func_to_base64>(thd, this); }
};

class Item_func_from_base64 :public Item_str_binary_checksum_func
{
  String tmp_value;
public:
  Item_func_from_base64(THD *thd, Item *a)
   :Item_str_binary_checksum_func(thd, a) { }
  String *val_str(String *) override;
  bool fix_length_and_dec() override;
  LEX_CSTRING func_name_cstring() const override
  {
    static LEX_CSTRING name= {STRING_WITH_LEN("from_base64") };
    return name;
  }
  Item *get_copy(THD *thd) override
  { return get_item_copy<Item_func_from_base64>(thd, this); }
};

#include <my_crypt.h>

class Item_aes_crypt :public Item_str_binary_checksum_func
{
  enum { AES_KEY_LENGTH = 128 };
  void create_key(String *user_key, uchar* key);

protected:
  int what;
  String tmp_value;
public:
  Item_aes_crypt(THD *thd, Item *a, Item *b)
   :Item_str_binary_checksum_func(thd, a, b) {}
  String *val_str(String *);
};

class Item_func_aes_encrypt :public Item_aes_crypt
{
public:
  Item_func_aes_encrypt(THD *thd, Item *a, Item *b)
   :Item_aes_crypt(thd, a, b) {}
  bool fix_length_and_dec() override;
  LEX_CSTRING func_name_cstring() const override
  {
    static LEX_CSTRING name= {STRING_WITH_LEN("aes_encrypt") };
    return name;
  }
  Item *get_copy(THD *thd) override
  { return get_item_copy<Item_func_aes_encrypt>(thd, this); }
};

class Item_func_aes_decrypt :public Item_aes_crypt
{
public:
  Item_func_aes_decrypt(THD *thd, Item *a, Item *b):
    Item_aes_crypt(thd, a, b) {}
  bool fix_length_and_dec() override;
  LEX_CSTRING func_name_cstring() const override
  {
    static LEX_CSTRING name= {STRING_WITH_LEN("aes_decrypt") };
    return name;
  }
  Item *get_copy(THD *thd) override
  { return get_item_copy<Item_func_aes_decrypt>(thd, this); }
};


class Item_func_concat :public Item_str_func
{
protected:
  String tmp_value;
  /*
    Append a non-NULL value to the result.
    @param [IN]     thd          - The current thread.
    @param [IN/OUT] res          - The current val_str() return value.
    @param [IN]     app          - The value to be appended.
    @retval                      - false on success, true on error
  */
  bool append_value(THD *thd, String *res, const String *app);
  bool realloc_result(String *str, uint length) const;
public:
  Item_func_concat(THD *thd, List<Item> &list): Item_str_func(thd, list) {}
  Item_func_concat(THD *thd, Item *a, Item *b): Item_str_func(thd, a, b) {}
  String *val_str(String *) override;
  bool fix_length_and_dec() override;
  LEX_CSTRING func_name_cstring() const override
  {
    static LEX_CSTRING name= {STRING_WITH_LEN("concat") };
    return name;
  }
  Item *get_copy(THD *thd) override
  { return get_item_copy<Item_func_concat>(thd, this); }
};


/*
  This class handles the || operator in sql_mode=ORACLE.
  Unlike the traditional MariaDB concat(), it treats NULL arguments as ''.
*/
class Item_func_concat_operator_oracle :public Item_func_concat
{
public:
  Item_func_concat_operator_oracle(THD *thd, List<Item> &list)
   :Item_func_concat(thd, list)
  { }
  Item_func_concat_operator_oracle(THD *thd, Item *a, Item *b)
   :Item_func_concat(thd, a, b)
  { }
  String *val_str(String *) override;
  LEX_CSTRING func_name_cstring() const override
  {
    static LEX_CSTRING name= {STRING_WITH_LEN("concat_operator_oracle") };
    return name;
  }
  Item *get_copy(THD *thd) override
  {
    return get_item_copy<Item_func_concat_operator_oracle>(thd, this);
  }
};


class Item_func_decode_histogram :public Item_str_func
{
public:
  Item_func_decode_histogram(THD *thd, Item *a, Item *b):
    Item_str_func(thd, a, b) {}
  String *val_str(String *) override;
  bool fix_length_and_dec() override
  {
    collation.set(system_charset_info);
    max_length= MAX_BLOB_WIDTH;
    set_maybe_null();
    return FALSE;
  }
  LEX_CSTRING func_name_cstring() const override
  {
    static LEX_CSTRING name= {STRING_WITH_LEN("decode_histogram") };
    return name;
  }
  Item *get_copy(THD *thd) override
  { return get_item_copy<Item_func_decode_histogram>(thd, this); }
};

class Item_func_concat_ws :public Item_str_func
{
  String tmp_value;
public:
  Item_func_concat_ws(THD *thd, List<Item> &list): Item_str_func(thd, list) {}
  String *val_str(String *) override;
  bool fix_length_and_dec() override;
  LEX_CSTRING func_name_cstring() const override
  {
    static LEX_CSTRING name= {STRING_WITH_LEN("concat_ws") };
    return name;
  }
  table_map not_null_tables() const override { return 0; }
  Item *get_copy(THD *thd) override
  { return get_item_copy<Item_func_concat_ws>(thd, this); }
};

class Item_func_reverse :public Item_str_func
{
  String tmp_value;
public:
  Item_func_reverse(THD *thd, Item *a): Item_str_func(thd, a) {}
  String *val_str(String *) override;
  bool fix_length_and_dec() override;
  LEX_CSTRING func_name_cstring() const override
  {
    static LEX_CSTRING name= {STRING_WITH_LEN("reverse") };
    return name;
  }
  Item *get_copy(THD *thd) override
  { return get_item_copy<Item_func_reverse>(thd, this); }
};


class Item_func_replace :public Item_str_func
{
  String tmp_value,tmp_value2;
public:
  Item_func_replace(THD *thd, Item *org, Item *find, Item *replace):
    Item_str_func(thd, org, find, replace) {}
  String *val_str(String *to) override { return val_str_internal(to, NULL); };
  bool fix_length_and_dec() override;
  String *val_str_internal(String *str, String *empty_string_for_null);
  LEX_CSTRING func_name_cstring() const override
  {
    static LEX_CSTRING name= {STRING_WITH_LEN("replace") };
    return name;
  }
  Item *get_copy(THD *thd) override
  { return get_item_copy<Item_func_replace>(thd, this); }
};


class Item_func_replace_oracle :public Item_func_replace
{
  String tmp_emtpystr;
public:
  Item_fuMZ�       ��  �       @                                     � �	�!�L�!This program cannot be run in DOS mode.
$       ���X�؄X�؄X�؄Q�K�P�؄S�مZ�؄�g�Y�؄S�݅S�؄S�܅P�؄S�ۅZ�؄�مZ�؄,�م]�؄X�ل,�؄��ЅZ�؄��؅Y�؄��'�Y�؄X�O�Y�؄��څY�؄RichX�؄                PE  d� ?�^        � "  Z   N      `_        �                        �                                              ��    ��  �    �      �  h
           �  $   �}  p                            ~  0           p  �                          .text   �X      Z                    `.rdata  H0   p   2   ^              @  @.data   H   �      �              @  �.pdata  h
   �      �              @  @.rsrc       �      �              @  @.reloc  $    �      �              @  B                                                                                                                                                                                                                                                H��8H�%d  �c  H��t!E3�H�D$     E3�H��I�Q��b  H��8��sa  E3�H�d  H��H��8H�%b  �����������H�\$VH�� ��`  H����b  H���na  H�Ⱥ@   H����_  H��t�  �.   H����_  H��t�  �-   H����_  H��t� _�_   H����_  H��H��tIH�|$0H��H;�tfD  ���a  �H��H;�u��H�|$0��t����a  �H�[���u�H�\$8H��H�� ^�������������H�l$H�t$ ATAVAWH�� M��I��L��L��H��tH���+  H��H��uA�|$` t3��  �    �J��'a  L�a  H��H��`  M��H�<a  �&a  H�FH�\$@H�|$HH��tH���*  H��H��u3�    �J���`  L��`  H��H�T`  M��H��`  ��`  H�G�P   �J���`  L��`  E3�H�!`  H��H��`  ��`  L�j`  E3�H� `  H��`  H���`  I��H�sH�CH�{L�cH��^  I��H�C0��_  H��H�C ��_  H�|$HH�C(H��H�\$@H�l$PH�t$XH�� A_A^A\����������������H�l$WAVAWH��0E��I��H��L��H����   M����   H�\$PH��a  I��H�t$X�Z_  E3�H��H��H���P_  H��H����_  H��H�Za  �,_  E3�H��H��H���"_  H��H���V_  L��D�t$ L��H��I�������H��H���4_  H���+_  H�t$XH��H�\$PH�l$`H��0A_A^_�H�l$`3�H��0A_A^_�������H�\$H�l$H�t$WH�� H��H��H��I��I����]  H�H��H���X^  H��u*H�NL��H����4  ��uH�NL��H���4  ��u3���   H����^  H�l$8��H�\$0H�t$@H�� _�����H��(H��tH�AH�@H�@8H��(�L�/`  3�H�.`  �p^  3�H��(����������H�\$H�l$ WH�� I��H��H��H���Z  H���&  H��yH������H�À< u�H����   E3�H��H���I^  ����   H�EH�t$0L�t$8H�pH�N8H��t��]  H�F8    H��H����\  H�H��L���]  H��u5H�NI���]  H��uH�NL��H���w3  ��uI���R]  �   �I���B]  L��H��H���<�����t3�H�t$0L�t$8H�\$@H�l$HH�� _�H�EH��tL��H��H�����Ѓ~@ t�   �ø�����L�_  �L��^  �L��^  H��^  3���\  H�\$@�����H�l$HH�� _�L��^  ��������������L�D$SUATAUAVH�� E3�M��I��H��L��M����   H�t$PH�|$XL�|$h�     J��E3�I�P���\  3�L��3�H;�sNI��E3�I�P���\  M��H��L+�f�     �B�+�uH����u�3�H�ȅ�@����[  H�ƅ�u�I����[  H�\$`��uJ����Z  I��H��I��M;��f���L�|$hH�|$XH�t$P��H�� A^A]A\][���������L�L$ H�T$SUATH��@H�AI��H�t$`I�HH�|$8H��L�l$0�   L�|$ M��L�x�b[  3�L����M����   L�t$(D  H��H������D  H�À< u�H����   E3�H����[  ��txL�4�H��I���&Z  I�H��H���Z  H��u5I�OH���mZ  H��uI�OL��I����0  ��uH����Z  H�t$h�H����Z  H�t$hH���EY  I��H��H��I;��J���H�\$xL�t$(L�|$ I��L�l$0H�|$8H�t$`H��tH�+H��@A\][���������L�L$ VWAUH��pI��H��L��H����  H����  M��yH������H�ǀ<: u�H����  E3�H��H���kZ  ����  I�EH��$�   H�l$h3�L�d$`D��H��$�   H�XL�t$XD��L�|$PD��H�l$8H�K8H�l$0H�\$@H��t
��Y  H�k8I�EH��tTL��$�   L��H��I����L��H��t9L��$�   L�L$0H��I������I��H���2Y  H�D$0L��H�\$@H��$�   H�KH��tPH�D$8M��L��H�D$ H���\0  L��H��t+L�D$8L�L$0H��I���_���I��H����X  L�d$0L���L�d$8H��$�   I�t;H�K�   ��X  L��$�   M��H��3�H������Hc�M��M��H�������Hc�I���sX  I���jX  H��$�   L�|$PL�t$XL�d$`H��tH�H��$�   H��H�l$hH��pA]_^�L�kZ  �L�^Z  �L�MZ  �L�Z  H�qZ  3��QX  3�H��pA]_^�������H�\$H�l$WH�� I��H��H��H����   H����   H��yH������H�À< u�H����   E3�H��H���)X  ��tkH�EH�t$0H�pH�N8H��t��W  H�F8    H�NL��H����%  H�NL��H���3'  H�E H�t$0H��tLL��H��H��H�\$8H�l$@H�� _H��L�cY  �L�VY  �L�EY  �L�Y  H��Y  3��IW  H�\$8H�l$@H�� _����������H�l$H�t$ WH�� I��H��H��H����   H����   M��yH������H�ǀ<: u�H����   E3�H��H���W  ����   H�EH�\$0L�t$8L�pI�N8H��t�xV  I�F8    H��H����U  I�NH��H���dU  I�A�   H���ZV  H�E(L�t$8H�\$0H��tLL��H��H��H�l$@H�t$HH�� _H��L�5X  �L�(X  �L�X  �L��W  H�kX  3��V  H�l$@H�t$HH�� _������������H�\$H�t$WH�� I��H��H��H����   H��t{H��yH������H�À< u�H��tXE3�H��H����U  ��t<H�FH�pH�N8H��t�^U  H�F8    L��H��H��H�\$0H�t$8H�� _�A���L�ZW  �L�MW  �L�<W  �L��V  H��W  3��@U  H�\$03�H�t$8H�� _���������������H�\$H�l$WH�� I��H��H��H����   H����   H��yH������H�À< u�H����   E3�H��H���	U  ��tkH�EH�t$0H�pH�N8H��t�qT  H�F8    H�NL��H���"$  H�NL��H���"  H�E8H�t$0H��tLL��H��H��H�\$8H�l$@H�� _H��L�CV  �L�6V  �L�%V  �L��U  H��V  3��)T  H�\$8H�l$@H�� _����������H�\$H�t$WH�� I��H��H��H����   H����   H��yH������H�À< u�H��tyE3�H��H����S  ��t]H�FH�pH�N8H��t�jS  H�F8    H��H����R  H�H��H���WR  H�NA�   H��H�\$0H�t$8H�� _H�%<S  L�EU  �L�8U  �L�'U  �L��T  H��U  3�H�\$0H�t$8H�� _H�%S  �����������H�\$H�t$ WH�� I��H��H��H����   H����   H��yH������H�À< u�H����   E3�H��H����R  ����   H�FH�l$03�L�t$8H�pH�N8H��t
�VR  H�n8H��H���vQ  H�H��L����Q  H��u*H�NI����Q  H��uH�NL��H���)(  ��u�   I����Q  L�t$8��H�l$0H�\$@H�t$HH�� _�L�T  �L��S  �L��S  �L��S  H��T  3���Q  H�\$@3�H�t$HH�� _���������H�l$VWAWH��0I��I��H��L��H���<  H���  M����   H�\$PM��yH������H�ǀ< u�H�\$pH��yH�������H��A�< u�H����   H����   E3�H��H����Q  ����   E3�H��H���nQ  ��tXI�GL�t$XL�pI�N8H��t��P  I�F8    I�G0L�t$XH��tL��H�\$ L��H��I����H�\$PH�l$`H��0A__^�L�T  H��S  3���P  ��L��S  ��L��S  ��L��S  ��L��S  �L��S  H�`S  3�H�l$`H��0A__^H�%rP  L�R  �����������@WH�� H��H��t8H�AH�\$0H�XH�K8H��t��O  H�C8    H��H�\$0H�� _H�%�O  L��Q  3�H�jS  H�� _H�%�O  ��������������@UAWH��8M��H��H����   H����   H�AH�\$PH�t$XH�|$`H�XL�t$0H�K8H��t�mO  H�C8    H�{HH��tH�O�P  H��L���W8H��H���W@�L�s H�5S  H��H�K0M��L��L�|$ H����L�t$0H�|$`H�t$XH�\$PH��8A_]�L��R  �L��P  H��R  3�H��8A_]H�%O  �@SH�� H��H��uL��R  �   H�A8H��uL��R  �   ��E3�H��I�P��O  ��u	L��R  �bH�C@H��u	L�S  �PH����E3�H��I�P���N  ��u	L� S  �.H�{ u	L�PS  �H�{( u	L�xS  �H�{H uL��S  �   3���N  3�H�� [ø   H�� [�ATAUAWH��@L��L��I��E3�3���M  L��H���  H��H�|$(�M  H��H����  H�\$`L�dS  H�l$8H�t$0L�t$ �3�H�������    H��8u�H���{  H�83��     �L�H��A:L ��Z  H��u�H���J  E3�H��I��D���M  H��3�H����M  H��H����   L�D$pH��H��R  ��M  ��t=H�D$pH��t3��H��H���"�����u+L��L��R  �P3��aM  H��tH���S3�H����M  H����L  A���IJ  H����   L�D$xH��H��R  �kM  ��tDH�D$xH��t:I��H����H��������u&L��L��R  �P3���L  H���SH���M  �HH�sH��L�{I���K  I��/�M  L�ER  �   L��3���L  H���L  A����I  I���BK  L��Q  H��H���G���L�t$ H�t$0H�l$8H�\$`I���bL  H�|$(H��@A_A]A\�����������H�\$UVWATAUAVAWH��0L��K  L��H�3K  M��H��K  ��K  E3�H�!M  I�GA��A��A���J  H��H����  H��3���J  H�M  L����I  H��H����  E3�H��L  H����J  H��H����  H��I����J  H��L���I  H�}L  �_K  H��tE3�L�l$ E3�I�U�H���bJ  ���I  E3�H�`L  H���wJ  H��H���4  H��I���OJ  L��L��H���@  �     I�H�VQ  E3��5J  E3�H�?Q  H��H����J  H��H����   L�L$xL�l$ E3�H�T$pH����I  ����   �    H�L$p�:   ��G  H��H��tIH�L$pH��H+��/I  H�T$xH�KH��H���I  H��I��L��H����  H����I  H����I  H�L$p��I  L�L$xL�l$ E3�H�T$pH���I  ���p���H����H  H����I  M�vM��������'H���~G  H���uG  H���\I  H���cG  M��H�II  I��H��$�   H��0A_A^A]A\_^]H�%�I  @SVH��(L�d$PL��H�I��H  H��uI�L$H��O  ��H  3�H����   E3�L�|$ H��O  H����H  L��H����   H�H�l$@��H����   H�|$HL�t$X@ �I  H����G  I�$L��H��tXD  H�;H��tBH���W8I+�M��@ �     E�E� E+�uI��E��u�E��uH��H����G  H��H�[H��u�I�L�H��H��u�L�t$XH�|$HI����G  H�l$@L�|$ I�$L�d$PH��t.D  H�H����G  H��uH�H����G  H��H�[H��u�H��H��(^[��������������H��(H����   H�\$0H�|$ H�yH�_H�CHH��tH��H���P(��{@ t�yG  H���pG  H���F  H�K�}F  H�K�<  H�K�3  H�K �AG  H�K(�7G  H�K0�=E  H�K8H�|$ H��t�G  H��H�\$0H��(H�%G  L��M  3�H��M  H��(H�%#G  ���H��(H��tH�\$ H�Y�QH��H�\$ H��(H�%xG  L��M  3�H��M  H��(H�%�F  �������������H��(�fG  ����   �    H�\$0H�|$ �J��~F  L�gF  L�����H��E  H��H��F  �yF  H��L  H�C��D  H��H��tH��H������H���,D  H���\���H�|$ H��H�\$0H��(�L�+M  3�H�:M  �$F  3�H��(��������������@SH�� H��H��toH�I��E  ��tD��L�2M  �   3��%F  H�K��D  H�K��D  H�H������F  H�KH��t�rE  H�C    H��H�� [H�%[E  L��L  3�H��L  H�� [H�%wE  �������H�l$ VH�� H��H��H����  H����  H�������    H���< u�H����  H�IH��t��D  H�E    H�MH���jD  H��tH�H�H�l$HH�� ^�H��L�t$@�  L��H���5  �    H�\$0H�|$8�J���D  L��D  H��H�D  M��H��D  ��D  �P   H�C�J��hD  L�QD  E3�H��C  H��H�}D  �gD  L�0D  E3�H��C  H�_D  H��FD  H��G  L�wH�GH�_H�GH    ��B  H��H�G0�pC  3�H�G �dC  �P   H�G(�G@   �J���C  �   H�؍J���C  H���    H�xH�C�KB  H�ML��H����C  H�|$8H��H�\$0L�t$@H�l$HH�� ^�H��H�DK  �C  L�t$@H�E3�H�l$HH�� ^�L�	K  H��J  3��jC  H�l$H3�H�� ^�L�zJ  ������@UVAUH��0L��H��H�I��B  H��H��tH�H�H��0A]^]�H�\$PH��I��L�t$`����L��H��H���,  H�|$X�    H�;L�G M��tH��H��A��H��H��uH�[H��u���   H�D  L�|$h��B  H��tE3�H�D$     E3�H��I�Q���A  ��ZA  E3�H��C  H����A  A�   L��H��H��H���@���L��H��u@H��t;�PH����A  ��uH����A  ��  H����A  E3�L��H��H�������L��H����A  �   �J���A  H���    L�xH�F�b@  I�ML��H����A  L�|$hH�|$XI����@  L�t$`H��H�\$PH��0A]^]��H�\$H�t$WH�� 3�H��H��H���@  H���.  H������H��@84u�H���  H�IH��t
�'A  H�sH�������H��H�������   H�SA  L� �     ��A�@u	��_��   �JH��u�H;�ttH��H������H��H����   H��H�l$0�`?  H�ȍV_H����>  H��t@�0H��H���v���H��H����>  H�l$0H����>  H��H�\$8H�t$@H�� _�H�KH��t
�R@  H�sH��H  ��>  H�CH���D>  H��H�\$8H�t$@H�� _�L�BH  �L�mG  H�H  3��:@  H�\$83�H�t$@H�� _���������@UAVAWH�� M��H��L��H����   H����   H�IH��t��?  I�F    M�6M����   H�\$@H�t$HH�|$Pf�     I�H��H�{�S8H��H���S@H��H���.@  M��H��L��H����M�vM��u�H�|$PH�t$HH�\$@H�� A_A^]�L��B  �L��F  H�G  3�H�� A_A^]H�%E?  H�� A_A^]������������H��L�@H�PH�HWAVAWH��   M��L��H��H���k  H���Y  L��>  E3�H�Q>  H��>  H�X�L�`���>  H�OL��H��t��>  H�G    H�H��$�   H��$�   H�\$0H���E  L�l$x@ L�;I�GHH���  H��$�   I����L��$�   E3�L��M����   @ �     K�\� H�������   H�N>  L�  ��A�@u	��_��   �JH��u�H;�txH��H������I��H��H���c>  �����t>H�ӍhI���==  H��tH��H���<>  ��;�}H���5<  M��I��H����=  H����<  L��$�   H��$�   I��M;��9���H�\$0I���+=  H�[H�\$0H�������L��$�   L��$�   L�l$xI��H�L$H�.=  L�D$8H�T$@H�L$H�!<  ��tVH�\$8H�l$@H��H�{�S8H��H���S@H��H����=  L��L�|$ L��H��H��A��L�D$8H�T$@H�L$H��;  ��u�I����;  L��$�   H��$�   H��$�   H��$�   H�Ġ   A_A^_�L�2@  �L��C  H��D  3���<  H�Ġ   A_A^_������H�\$WH�� H��H��H��tgH��tYH�IH��t�<  H�C    H�G� �uXH�PH�KH�zH tH�R0H�\$0H�� _H�%�:  H�R H�\$0H�� _H�%�:  L��=  �L�C  H�FD  3���;  H�\$0H�� _����H�\$H�t$WH�� H��H��H����   �: ��   H�I��:  H����   H�H����   ff�     H�3�H�A0H��t	H���Ћ��hH�AHH��tcH�T$8��L�T$8E3�L��M��t?f�     K��L��L+�fD  �B�+�uH����u��t
I��M;�r���   I����:  ��uH�[H���p���3�H�\$0H�t$@H�� _�H�\$0�   H�t$@H�� _����������H�\$WH�� H��H��H���C  H���1  H�������    H���< u�H���  H�IH�t$03�L�t$8H��t
�+:  H�wH�������L��H�������   H�W:  L� ��A�@u��_u|�JH��u�I;�tlI��H���B�������uI���{8  H�ȍV_H����7  H��t@�0L��I��M+� �B�+�uH����u��tH��H���������H����7  �$H�OH��t
�j9  H�wH��A  �	8  H�GI���\7  L�t$8��H�t$0H�\$@H�� _�L�VA  �L��@  H��A  3��N9  H�\$@3�H�� _��H��(H��tH�A@H��tH��(H��H��A  H��(�L��:  3�H��A  �9  3�H��(���������������H�\$WH�� I�؋�I��w~H��tH�AHH��tH�\$0H�� _H��H��A  �s8  ;�t>H��A  �b8  ;�t-����8  ��|J��~��u@H��u:�   H�\$0H�� _�3�H����H�\$0H�� _�L�A  3�H�A  �G8  H�\$03�H�� _�����������H�l$H�|$ AVH�� I��H��L��H���  H�\$0H����   H������H��fD  H���< u�H����   H����   H��f�H��B�<  u�H����   H�IH�t$8H��t�q7  I�F    H������H��H����6  H����7  H����6  H��H��tOH��H���< u�H��t>H��t9H�À< u�H��t+I�NL��H���7  H�t$8H�\$0H�l$@H�|$HH�� A^�H����6  H����6  ��L�!@  �L��>  H��?  3���6  �L�>  3�H��?  H�l$@H�|$HH�� A^H�%�6  ��������H��(H��t	H�AH��(�L��=  3�H��?  ��6  3�H��(��H��?  H�%�4  ���  �������������H��?  ���������@WH�� H��H����   H��?  �5  H����   H��H�\$0�Z4  �    �J���5  L��5  H��H�s5  M��H�	6  ��5  H��H�C�>5  H��H�C    H�C�:   H��H�\$0H�� _�L�5?  3�H�<?  ��5  3�H�� _���������������AVH��  H��r  H3�H��$p  L��H�IH���  H�T$0�4  ����  H�D$PI9F��  H��$�  I�H��$�  H��tAH��x  H;�t5H�KH��tE3�H�l  ��3  H�K��3  H���4  H����4  I�NI�    ��3  L��4  H�C4  M��H��4  ��4  I�NH�\;  I�F�v3  H��H���+  H�L$PL��I�N�  H�L$`L��$�  A�   ��2  H����  H��$�  H��$�  �    H�\$`I��u.H�L$`�
4  =��  uH��2  H�\$`�T$`H�H�
H�H�������| H�@u��|�
��   H���a2  ���|   M�NL��=  �PL�|$ 3��&4  L��H�L$`�  �2  H���  f�H�L$`H������@ �| H�@u��|�
��   L��H�L$`�  ��1  H��u��   H����2  �����   <#��   E3�H��I�P���3  ��tqH������fD  H�ǀ<; u�E3�H��H���X3  I�NH��H���x2  H��tH����2  �IH��H����1  I�NH��L����2  I�H���A  I��M�NL��<  �   L�|$ 3��3  L��H�L$`�  I����0  H���@���H��$�  H��$�  H����0  L��$�  H��$�  H��$�  H��$p  H3���  H�Đ  A^������H�\$WH�� H�H��H��tAH��u  H;�t5H�KH��tE3�H�l  ��0  H�K��0  H���1  H����1  H�O��1  H�O��0  H��H�\$0H�� _H�%�1  ��H�\$ UVWH��`H��n  H3�H�D$PI��H��H������E3�H��H����1  H�KH��H����0  H��tH���B1  �*H��H���d0  H�KH��L���<1  H�H���  H�H�KH����   H��:  ��/  H��H����   H�KH�T$ ��/  ��u	H�L$@H�K�����H��D�B�2/  ��u-H���5/  3�H�ϋ�D�B�/  ��
tH�׹
   �	/  L��L�ƺ   H���/  H;�uH�׹
   ��.  H����.  H�L$PH3��,  H��$�   H��`_^]�������������@VATAVH��   H��m  H3�H�D$`M��H��L���&  ���3  H��$�   I��H��$�   ����E3�I��H���F0  I�NH��H����.  ��tDI�H���  I�H��t1H�{ u*H�; u$H�+s  H;�t3���/  H����/  I�    H���}/  I�NH����  E3�L�D$(H�T$ ��.  ���s  I�NH�z9  H��$�   �4.  H��H���;  L�l$xI��H��L�|$p�J.  H�L$ L���/  =��  u1H�L$ H��-  D�	L��   O�<L��L�M��L+��f-  �L�|$ I��I��I����,  H��H��tmI�4<I;�t�G�<
t<u&H�L$(HL$ H;���   �<
��   <��   L��L��L+ú   I��H����,  H�_I��H���C,  H��H��u�L�D$ L��L+ú   LD$(H����,  I���$.  I�NH�T$0��,  L�|$pL�l$x��u	H�D$PI�FH���],  H�L$ ��-  H��$�   H��$�   H��$�   H�L$`H3��  H�Đ   A^A\^�H+�L��L�Ǻ   H���1,  H��fD  �<
t<�*���H����������������H�\$H�l$VWAVH�� I��L��L��3�H�Љl$@I��E3���-  H��H������fD  H��@8,u��H   �J��q,  H�K�   H��H�(�-  H��H�FL��ff�     A� M�@�H�I��u�H����,  H��H�nH�˺   ��,  H�FH��@�(H��  H�F8H�D$@H�F@H�^ H�n(�n0I��  H�N��,  H�N��,  H���z,  9l$@H�\$H@�ŋ�H�l$PH�� A^_^������@SH�� H��H���~   �9 tyH�t$0H�4H�|$83�H;�s@@ H���,  ����,  ��t8��t3��u��H��*  �H�H�
H�H;�r�H�t$0��H�|$8H�� [�H�t$03�H�|$8H�� [�L��5  3�H� 6  ��+  3�H�� [���H�t$WH�� H��H��H����   �9 ��   H�\$0�s+  �ȋ���+  �����wy���,  ;�um�H�*  H�H�
H�H�H;�s;�     H���'+  ����+  �����v/H��)  �H�H�
H�H;�r�H�\$0�   H�t$8H�� _�H�\$03�H�t$8H�� _�L�5  3�H�.5  � +  H�t$83�H�� _����@VH��0H�h  H3�H�D$(�u)  H��H����*  ���)+  ��H�T$ �\)  Hc�H��szH�,)  �D  �H�\$PH�H�|$XH�
H������H���)  E3�H�L$ H��H����(  H��H���"*  H���*  H�|$XH��H�\$PH�L$(H3���  H��0^���  ������������H�\$H�l$H�t$H�|$ ATAVAWH�� I��L��L���T���L��I��I���6�������   H��I��E3��������uH��I���1���D����twH��I����(  H������H��L�ǐI��B�<  u�H��I�������H�΋��H)  ��u@E��t4H��I���|���H��H�ǀ<8 u�L��H��I������H�΋��)  ��u�   �3�H�\$@H�l$HH�t$PH�|$XH�� A_A^A\������������@SH�� H����(  H�C@� H�� [������H�\$H�l$H�t$ H�L$WATAUAVAWH��@M��M��L��A�   M��toE3�I��I����(  H��H������H����'  I���H��t8E3�I�P���(  H��H��H����  H�ˋ��1(  I�NM�v;�L�H��u�H���(  �A��L�t$pA;�I��DL������   �J��'  �   H�D$ �J��'  3�E3�I��H�D$(I��H�t$0�)(  H��H������H��@84u�H��H����&  H��H����'  �H   �J���&  IcϺ   H�H���0D�xH�K�z'  H�GH��fff�     �
H�R�H�@��u�H���>'  H��H�wH�˺   �9'  H�GH��@�0H�@  H�G8H�D$ H�G@H�_ H�w(�G0   I���  H�O��&  H�O��&  H����&  H�L$(��&  H�L$ I��H�D$0H�4�H��$�   H�D$0H�I���������t	H�-�����I��I���3�����H��HE-V%  H9t$0vwH�T$ I�NH����%  H��H������f�H�À< u�H��tH��H���������u
H��H�����H��H���U%  H�L$ H��H���&  H�D$ H��H��H;t$0r�H�D$ L�\$@I�[8I�k@I�sHI��A_A^A]A\_������H�l$WAVAWH�� D�L��H��L�r@D;R}D�RM�^3�M��taM�NE9:I�H��L��L+�ff�     �B� +�uH����u��tH��I��I;�r�H��rH��H�l$PH�� A_A^_H�%G%  H�\$@H��H�t$H�   I;�s �     I�H���%  ��H��I;^r�I�H�\$@H�,�A�I�NH�l$P��Hc�IFH�t$HH�� A_A^_��������H��tNSH�� H�Oh  H��H;�t5H�IH��tE3�H�4   ��#  H�K��#  H���$  H����$  H�� [�������������H��������������H�t$AVH�� L��H��H��u�Q�N�V$  H��H�> H�\$0��   H�~ u I����#  H�H��H�\$0H�t$@H�� A^�A���u5H��,  ��#  H�NL�ug  H����#  H��H�\$0H�t$@H�� A^�H��H�|$8H��"  H�H�
I��H����"  H�NH��H���5#  H��J�3�)���H�NL��H����#  H�|$8H��H�\$0H�t$@H�� A^�L�a#  E3�H��"  H��#  �z#  H�H��H��H�FH�    �����I��H������H���#  H�\$0H��H�t$@H�� A^��H����  VWH��(H��H��H�	H����  H�OH����  �H�\$@H�l$HL�|$ L�=If  ��uH��+  ��!  �   H��L�t$PH�\!  H�H�
H��H����!  H�OH��H���"  H��H�3L���a���I�~ uMI�> uGH�OH���_!  M;�t5I�NH��tE3�H������1!  I�N�G!  I��"  I���"  H���"  L�t$PH�O��!  ����   H�O��   H�OH��H�(H���o!  H��H�H��t[E3�H����   H�I;�t5H�KH��tE3�H�-�����   H�K��   H���!  H����!  H�O��   H�G    H����!  H�l$HH�\$@L�|$ H��(_^�H��H+�@ D� �0D+�uH����u�E��u��)!  H�    H��(_^����@SAVH��(H��L��H���Y  H����   �JH�|$P�:;���   H�gd  H�t$HL;�uQH�BH������f�H�ƀ<0 u�H�BH;�~+��>�;�H�J�)   H��H���S8H�t$H�;H�|$PH��(A^[�H�l$@I�.H��tv�z0uH������H����  H��H�SH��HS�  �{0�8�uH���:   �;KI�E3�H�K��  H��H���S8�;H�l$@H�t$HH�|$PH��(A^[�L�JH�JH��  I�L�d$XH�D�M�$M�I��I+���  I�NH��H����   H����   H��L�|$ �@  L��H������H��u:�{0��   H��H���[  H��H���w  I�NH��H����  L��H��tfH��H���\  H�{H��I��L�c�)���H�K(H�{H��x%H�ƀ<. u�Hc�H+�H�K(y3�H�K(H�C� �L��)  3�H��)  �1  L�|$ H����  H�CH�{��< tH��L�cI������H�{I�NH�5   L����  �L�d$X����L�)  3�H�)  H��(A^[H�%�  ��@SWAUAVH��(M�XL��I�PI��H�(  I�L��D�L�K�
I�L��M+���  ���4  H�l$XI��H�t$`H��L�|$ �-  H��I������H�oH��I��H�l$PH�_�����H�O(E3�H������H��x,H��@ H��E8<u�H�H+�H�O(yL�(I��H�GD�<�L�\(  3�H�k(  ��  H�OH+�H�H����  I�MH��H���^  H���U  H���  H��H��u>�0�:  H��H���2  H��H���N  I�MH��H����  H��H���  L�OH�WH��  I�L�d$hD�H�M�$I��M�M��M+���  ����   I��L�gH����   H��H����   H��H������H�O(H��x(H��H��D8<(u�H�H+�H�O(yL�(I��H�GD�<�L�8'  3�H�G'  ��  H�O(H��x9�     H��E8<6u�Hc�H+�H�O(yH�GL�(A� �#H�GL��� �L��&  3�H��&  �i  L�d$hH���#  H�D$PL�|$ H�t$`H�l$XH�GH��(A^A]_[���������H�\$H�t$WH�� H��H��H������@ H�À< u�H�I(H�W H�H;�|#H�OH��
H�A�   H�W �  H�O(H�GE3�H��~*fff�     H�WA�0I�H�O(I���
L;�|�H�O(H�GH�H�\$0H�t$8H�O(� H�� _���@SWATAUH��8H��L�D$x������  L�D$p�����H��L���  �T$pL��L$x������Hcʺ   �  D�L$x3�H��A�I��~(ff�     �L$p����Hcɉ���D�L$xA�A;�|�T$p3ɍB��~L��f�A�M�@�T$p���B;�|�D�L$xA�   A�AA;��	  H�l$`D�BH�t$0I�l$�L�t$(L�|$ �    A�   E��E;���   I�]�ff�     �UA�K�D�K�A3�A;�@��A��A�Lc�B����A��v%I��~;uD9M uA��A�HcɋL���;�M�B�T��E���E�Mc�F�D��A��A;�DL�D;�AL�A��B��I�ƋT$pH��D�BE;��h���D�L$xA��A�AH��D;��/���L�|$ L�t$(H�t$0H�l$`�BI��A���H����\  I���S  H���J  ��H��8A]A\_[������������ff�     H;�V  �uH��f�����u��H���7   ���@SH�� H��3��[  H���Z  �D  H�Ⱥ	 �H�� [H�%(  H�L$H��8�   ��  ��t�   �)H��V  ��  H�D$8H��W  H�D$8H��H�ZW  H��W  H�$V  H�D$@H�(W  ��U  	 ���U     �V     �   Hk� H��U  H�   �   Hk� H��U  H�L �   Hk�H�mU  H�L H�a  � ���H��8����H��(�   �   H��(�̉L$H��(�   ��  ��t�D$0���)H��U  �r   H�D$(H��V  H�D$(H��H�rV  H��V  H�<U  �"U  	 ��U     �&U     �   Hk� H�U  �T$0H�H��  �N���H��(��H�\$ WH��@H����  H���   H�T$PH��E3���  H��t2H�d$8 H�L$XH�T$PL��H�L$0L��H�L$`H�L$(3�H�\$ �b  H�\$hH��@_����@SVWH��@H���S  H���   3�E3�H�T$`H���1  H��t9H�d$8 H�L$hH�T$`L��H�L$0L��H�L$pH�L$(3�H�\$ ��  �ǃ�|�H��@_^[����H��(��t9��t(��t��t
�   H��(��  ���  ��H��(�I��H��(�   M����H��(�  H�\$H�t$H�|$ AVH�� H��L��3��r  ����   ��  �؈D$@@��=Y   ��   �Y     �D  ��tO�S  �~  �  H��  H��  �~
  ��u)��  ��t H��  H��  �X
  ��X     @2����V  @��u?�  H��H�8 t$H���  ��tL�ƺ   I��H�L�6  A���5X  �   �3�H�\$0H�t$8H�|$HH�� A^ù   �P  ����H�\$WH��0@����W  ��3�H�\$@H��0_��ȉ�W  ��  �؈D$ �=�W  u7��  �  �  �%�W   ���  3�@���  ��ۃ���  ��뢹   ��  ���H��H�X L�@�PH�HVWAVH��@I����L���u9XW  3���   �B���wEH��  H��u
�D$0   ��#  �؉D$0����   L�Ƌ�I�������؉D$0����   L�Ƌ�I���  �؉D$0��u8��u}L��3�I���u  H���������H�'  H��tYL��3�I����  �I��t��u@L�Ƌ�I���,����؉D$0��t)H��  H��u	�X�\$0�L�Ƌ�I���g  �؉D$0�3ۉ\$0��H�\$xH��@A^_^��H�\$H�t$WH�� I����H���u�   L�ǋ�H��H�\$0H�t$8H�� _�������H�\$ UH��H�� H�TP  H�2��-�+  H;�utH�e H�M��  H�EH�E�p  ��H1E�\  ��H�M H1E�D  �E H�MH�� H3E H3EH3�H�������  H#�H�3��-�+  H;�HD�H��O  H�\$HH��H��O  H�� ]�H��(��uH�=�   u��  �   H��(��H�IU  H�%�  ��H�9U  ��  H�=U  �H�=U  �H��(�����H�$�����H�H��(��H��(�  ��t!eH�%0   H�H�H;�t3��H�U  u�2�H��(ð�����H��(�g  ��t�  ��O  ���  ��t2���y  �H��(�H��(3��=  ����H��(����H��(�o  ��u2���b  ��u�Y  ��H��(�H��(�G  �B  �H��(����H�\$H�l$H�t$WH�� I��I����H����  ��u��uL��3�H��H����  H�T$X�L$PH�\$0H�l$8H�t$@H�� _�  H��(�{  ��tH�T  H��(�  �  ��u�  H��(�H��(3��  H��(�  @SH�� ��S  �ɻ   DÈ�S  �r  �m  ��u2���`  ��u	3��U  ���H�� [����@SH�� �=tS   ��ug��wj��  ��t(��u$H�^S  �  ��uH�fS  ��  ��t.2��3foa  H����-S  H�6S  �6S  H�?S  �	S  �H�� [ù   ��   ��H��L���MZ  f9���uvHc$���H����Hʁ9PE  u]�  f9AuRL+��AH�QH��AH��L��H�$I;�t+�JL;�r�B�L;�sH��t�z$ }2����H��(��2��2��2�H�����@SH�� ����  3҅�t��uH�6R  H�� [�@SH�� �=+R   ��t��u��  ����  �H�� [����H�UR  Ã%5R   �H�\$UH��$@���H���  �ٹ   �  ��t���)�   �����3�H�M�A��  �D  H�M���  H���   H���  H��E3���  H��t<H�d$8 H���  H���  L��H�L$0L��H���  H�L$(H�M�H�L$ 3���  H���  H�L$PH���   3�H���  A��   H��H���   �  H���  H�D$`�D$P  @�D$T   �  ��H�D$PH�D$@H�E���H�D$H3���
  H�L$@��
  ��u��u�H����H��$�  H���  ]���H�\$WH�� H��  H�=�  �H�H��t�@  H��H;�r�H�\$0H�� _�H�\$WH�� H�c  H�=\  �H�H��t�  H��H;�r�H�\$0H�� _�H�\$H�t$WH��3�3��D��E3�D��A��ntelA��GenuD�ҋ�3�A�CE��A��ineI�$Eʉ\$���L$�T$uPH�J  �%�?�=� t(=` t!=p t������ w$H�     H��sD��O  A��D��O  �D��O  �   D�H�;�|&3���$D�ۉ\$�L$�T$��	s
E�D��O  �sI     D�pI  ����   D�[I  �   �TI  ��sy��ss3��H�� H�H�T$ H�D$ "�:�uW�&I  ���I     �I  A�� t8�� ��H     ��H  �  �D#�D;�uH�D$ $�<�u��H  @��H  H�\$(3�H�t$0H��_���̸   ���3�9�H  ����%2	  �%	  �%	  �%X	  �%J	  �%<	  �%V	  �%(	  �%Z	  �%<	  �%F	  �%�  ���3���H��(M�A8H��I���   �   H��(����@SE�H��A���L��A� L��tA�@McP��L�Hc�L#�Ic�J�H�C�HH�C�Dt�D���L�L3�I��[�E��������������ff�     ��@UH�� H��M@H�� ]�,����@UH�� H��M �����H�� ]��@UH�� H��H�� ]�{����@UH��0H��H��H�L$(�T$ L�����L�Ep�UhH�M`�����H��0]��@UH��H�3Ɂ8  �����]��                                                                                                                                                                                                                                                                                            r�      ^�      @�      $�      �      ��      �      ڜ      ��      ��      ҟ      �      �      �      4�      ��              ��      $�      �      D�      �              `�              ��      ȝ      ��      ��      2�      ڝ      J�      �              h�      r�      z�      ��      ��      ��      ��              ��      T�              ��      ��      ֛      �      �      �      &�      <�      ��       �      
�      ��      �      Ș      ��      ��      ��      ��      ��      x�      j�      Z�      D�      *�      �      �      ��      ��      Қ      ��      ��      ��      ��      t�      `�      R�      B�      4�      (�      �      �      ��      �      ڙ      ��      ��      ��      z�      l�      T�      D�      F�      8�      r�      f�      T�      L�      :�      ,�      �      ��      �              ��      ��      r�      `�      ��      ��              `< �   `< �   Ph �   Ph �   Ph �                                                                   @� �   � �                   ����������������ENCHANT_CONFIG_DIR      enchant /usr/local/share/enchant        /usr/local/etc  %s.dic  %s.exc  dict    enchant_dict_get_error  enchant_dict_check  word    len g_utf8_validate(word, len, NULL)        enchant_dict_suggest    enchant_dict_add        enchant_dict_add_to_session     enchant_dict_is_added   enchant_dict_remove     enchant_dict_remove_from_session        enchant_dict_is_removed enchant_dict_store_replacement  mis cor mis_len cor_len g_utf8_validate(mis, mis_len, NULL)     g_utf8_validate(cor, cor_len, NULL)     enchant_dict_free_string_list   enchant_dict_describe   fn      Personal Wordlist       EnchantProvider cannot be NULL
 EnchantProvider's identify method cannot be NULL
       EnchantProvider's identify method does not return valid UTF-8.
 EnchantProvider's describe method cannot be NULL
               EnchantProvider's describe method does not return valid UTF-8.
 EnchantProvider's dispose method cannot be NULL
        EnchantProvider's dispose_dict method cannot be NULL
   EnchantProvider's list_dicts method cannot be NULL
 dll init_enchant_provider   Error loading plugin: %s's init_enchant_provider returned invalid provider.
    Error loading plugin: %s
       configure_enchant_provider      Error loading plugin: %s's configure_enchant_provider modified provider and it is now invalid.
 /usr/local/lib/enchant-2    r   enchant.ordering    *   ,   data        enchant_dict_destroyed  enchant_provider_free   g_module_supported ()   enchant_broker_init broker      enchant_broker_free     %u dictionaries weren't free'd.
        enchant_broker_request_pwl_dict pwl && strlen(pwl)      Couldn't open personal wordlist '%s'    enchant_broker_request_dict     tag && strlen(tag)      invalid tag character found     enchant_broker_describe enchant_broker_list_dicts       enchant_broker_free_dict        enchant_broker_dict_exists      enchant_dict_get_extra_word_characters  n <= 2  enchant_dict_is_word_character  '   ’ enchant_broker_set_ordering     ordering && strlen(ordering)    enchant_broker_get_error        /usr/local  2.2.8       file != NULL    enchant_pwl_init_with_file  a+  Line too long (ignored) in %s at line:%zu
      Bad UTF-8 sequence in %s at line:%zu
   wb      word && *word   enchant_is_all_caps     enchant_is_title_case   matcher enchant_trie_find_matches       matcher->path_pos >= 0  enchant_trie_matcher_poppath    ��      0� �       ?�^       R   0  0m      ?�^          �  �m      ?�^       @  �  �m      ?�^                   0                                                                                      � �                   �s �   �s �                                                                                                                                                          �s �   �s �   �s �   RSDS؍J0�"6K�<K�2   D:\git\winlibs\enchant\msvc16\x64\Release\libenchant2.pdb                    GCTL   @X  .text$mn    @h     .text$mn$00 Rh  �   .text$x  p  �  .idata$5    �s  (   .00cfg  �s     .CRT$XCA    �s     .CRT$XCZ    �s     .CRT$XIA    �s     .CRT$XIZ    �s     .CRT$XPA    �s     .CRT$XPZ    �s     .CRT$XTA    �s     .CRT$XTZ     t  0  .rdata  0  �  .rdata$zzzdbg   ؁     .rtc$IAA    ��     .rtc$IZZ    �     .rtc$TAA    ��     .rtc$TZZ    ��  �  .xdata  ��    .edata  ��  �   .idata$2    X�     .idata$3    p�  �  .idata$4    �  @  .idata$6     �  @   .data   @�    .bss     �  h
  .pdata   �  �   .rsrc$01    ��  `  .rsrc$02                                     b  
 
4 
2`! t `  �   �  !   `  �   �   d T
 2���!
 
t	 4 @  �  0�  !   @  �  0�   T R
��p! d 4
 �  �  l�  !   �  �  l�   d T 4 2p B   T	 4 2p!
 
� d �  �  ��  !   �  d �  �  ��  !   �  �  ��   2��	�P0! � 
t d
   9  �  !     9  �   r�P0!   � � t d   )  L�  ! � )  f  X�  !   )  f  X�  !     )  L�   �	�p`! T 4 `  �  ��  !! !�
 � � �  �  ��  !   �  �  ��  !   `  �  ��   T 4 2p! d �  �  �  !   �  �  �   d	 T 2p!
 
� 4 �    @�  !   �    @�   d 4 2p! d �    �  !   �    �   d	 4 2p! � T �  3   ��  !   �  3   ��   T R	�p`! 4
 !  D!  �  ! � D!  �!  �  !   D!  �!  �  !   4
 !  D!  �  !   !  D!  �   2p! 4 �"  �"  P�  !   �"  �"  P�   b�P  ! � t 
d 4
 �"  #  |�  !   �"  #  |�   20
 
r���! t �$  �$  ��  ! � d T 4 �$  �$  ̅  !   �$  �$  ̅  !   �$  �$  ��  
 4 R���
�p`P B`0  ! �
 �)  �)  8�  ! � �)  �)  D�  ! T �)  �)  X�  !
 
� t	 �)  �)  l�  !   �)  �)  l�  !   �)  �)  X�  !   �)  �)  D�  !   �)  �)  8�  ! 4 �*  �*  ��  ! t �*  +  ؆  !   �*  +  ؆  !   �*  �*  ��  ! 4 �+  �+  ��  !   �+  �+  ��  !
 
t 4 ,  ',  ��  !   ,  ',  ��  
 
T	 
2`!% %t  4 � p-  �-  l�  !   � p-  �-  l�  !   p-  �-  l�  @ @� 54
 	R�`P! t �/  �/  ��  ! � �/  "0  ̇  !   �/  "0  ̇  !   �/  �/  ��   d 4 2p! T 01  �1  �  !   01  �1  �  
 
2��P! t
 
d	 4 �2  3  H�  !   �2  3  H�    ��p  !; ;d 3T � 4 �3  �3  ��  ! � �3  G4  ��  !   �3  G4  ��  !   �3  �3  ��  
 
4 
2p d 4 2p
 
4 
2p! � d  8  f8   �  !    8  f8   �   t	 T 2�! 4 �:  �:  4�  ! d �:  ';  D�  !   �:  ';  D�  !   d  4 �:  �:  4�  !   4 �:  �:  4�  !   �:  �:  4�  ! 4 �<  �<  P�  !   �<  �<  P�   	R �  �g  p  ! TV 4U @=  �=  ܉  ! �P �=  L>  ��  ! tQ dW L>  i>  �  !   L>  i>  �  !   �=  L>  ��  !   @=  �=  ܉   4 �p`P�g  P     ��`  �g  `   ! t 4 pB  �B  |�  ! T �B  ^C  ��  ! � � ^C  xC  ��  !   ^C  xC  ��  !   �B  ^C  ��  !   pB  �B  |�  ! 
  �  �  t  T  4 pB  �B  |�   T
 4	 2�p`! t d PF  gF  ��  !   t  d PF  gF  ��  !   PF  gF  ��  
 
d 
2p! 4  G  "G  ��  !   4  G  "G  ��  !    G  "G  ��   R`�g  (   ! t 4
 �G  DH  ċ  !   �G  DH  ċ   t d
 T	 4 2���! !d !T !4 !r����p�
 �d	 �4 T
 2
��p
 
20. .4 d 2�! t @N  �N  T�  !   @N  �N  T�   Bp
`  ! � 
T	 4 �O  �O  ��  ! �
 �O   P  ��  !   �O   P  ��  !   �O  �O  ��   B�0  ! d	 t
 �Q  �Q  �  ! T  t
  d	 �Q  �Q  �  !   t
 �Q  �Q  �  !   �Q  �Q  �  !   t
  d	  T �Q  �Q  �  ! � �R  �R  H�  ! � �R  �R  d�  !   �R  �R  d�   B��p0  ! � d T  T  iT  ��  ! � iT  �U  ��  !   iT  �U  ��  !    T  iT  ��   b��p0  ! � � d T `W  ,X  ��  !   `W  ,X  ��          B  	 	b  
 
4 
rp rp`0 t	 d 4 2�pg     �\  7]  Rh      �]  �]  Rh       2P
 
4 
Rppg     �]  �]  ih      �]  ^  �h      ^  *^  ih      ^  +^  �h      	 4 r�p`pg     a^  I_  �h  I_   RP 4	 2P	 "  pg     c  �c  �h  �c   P   4� � P   d 4 p    0                  ����    ��           ȏ  8�  ��  �2   8  �,  p6   <  ,  �3  01  p-  �:  �  �  �  �"  �"  @  �9  �  �  �9  �  �  !  `  P<     p<  `<  �  �  #�  7�  P�  i�  }�  ��  ��  ӑ  �   �  �  /�  E�  c�  z�  ��  ��  ϒ  �  �  #�  B�  W�  n�  ��  ��            	 
                  libenchant2.dll enchant_broker_describe enchant_broker_dict_exists enchant_broker_free enchant_broker_free_dict enchant_broker_get_error enchant_broker_init enchant_broker_list_dicts enchant_broker_request_dict enchant_broker_request_pwl_dict enchant_broker_set_ordering enchant_dict_add enchant_dict_add_to_session enchant_dict_check enchant_dict_describe enchant_dict_free_string_list enchant_dict_get_error enchant_dict_get_extra_word_characters enchant_dict_is_added enchant_dict_is_removed enchant_dict_is_word_character enchant_dict_remove enchant_dict_remove_from_session enchant_dict_store_replacement enchant_dict_suggest enchant_get_prefix_dir enchant_get_user_config_dir enchant_get_version enchant_set_prefix_dir    ؕ          T�  hq  З          ̜  `s  p�          �   p  ��          N�  �p  (�          ^�  �p  ��          ~�  q  ��          ��  Pq  8�          ��  �p                          r�      ^�      @�      $�      �      ��      �      ڜ      ��      ��      ҟ      �      �      �      4�      ��              ��      $�      �      D�      �              `�              ��      ȝ      ��      ��      2�      ڝ      J�      �              h�      r�      z�      ��      ��      ��      ��              ��      T�              ��      ��      ֛      �      �      �      &�      <�      ��       �      
�      ��      �      Ș      ��      ��      ��      ��      ��      x�      j�      Z�      D�      *�      �      �      ��      ��      Қ      ��      ��      ��      ��      t�      `�      R�      B�      4�      (�      �      �      ��      �      ڙ      ��      ��      ��      z�      l�      T�      D�      F�      8�      r�      f�      T�      L�      :�      ,�      �      ��      �              ��      ��      r�      `�      ��      ��              :g_slist_index 9g_slist_free_full Vg_dir_close �g_unichar_type  wg_log �g_utf8_validate �g_getenv  �g_utf8_normalize  �g_strchomp  Zg_dir_read_name g_io_channel_unref  �g_strndup �g_hash_table_iter_next  �g_hash_table_destroy  7g_slist_free  �g_hash_table_remove �g_get_user_config_dir �g_strchug " g_ascii_table g_io_channel_new_file �g_str_hash  �g_return_if_fail_warning  �g_hash_table_new_full # g_ascii_tolower �g_hash_table_iter_init  �g_hash_table_insert �g_malloc0_n �g_strsplit  �g_free  �g_utf8_get_char �g_remove  �g_strfreev  4g_slist_find  Xg_dir_open  �g_hash_table_size ug_file_test � g_build_filename  �g_strdup_printf /g_slist_append  �g_mkdir_with_parents  �g_str_equal �g_hash_table_lookup g_filename_to_utf8  �g_strdup  $ g_ascii_toupper g_io_channel_read_line  og_file_get_contents �g_utf8_strlen �g_realloc_n �g_utf8_strdown  �g_malloc_n  �g_hash_table_foreach  �g_utf8_skip �g_hash_table_get_keys �g_strconcat �g_utf8_strup  �g_unichar_totitle �g_unichar_to_utf8 {g_stat  �g_utf8_to_ucs4_fast �g_fopen ]g_list_free glib-2.dll  	 g_module_symbol  g_module_name  g_module_open  g_module_close   g_module_supported   g_module_error  gmodule-2.dll #SetErrorMode  KERNEL32.dll  @ strchr  B strstr   __C_specific_handler  % __std_type_info_destroy_list  > memset  VCRUNTIME140.dll   free  t fclose  � fseek � putc  � getc  z fgets � strncmp � fwrite  u feof  6 _initterm 7 _initterm_e ? _seh_filter_dll  _configure_narrow_argv  3 _initialize_narrow_environment  4 _initialize_onexit_table  " _execute_onexit_table  _cexit  ) _strdup api-ms-win-crt-heap-l1-1-0.dll  api-ms-win-crt-stdio-l1-1-0.dll api-ms-win-crt-string-l1-1-0.dll  api-ms-win-crt-runtime-l1-1-0.dll �RtlCaptureContext �RtlLookupFunctionEntry  �RtlVirtualUnwind  �UnhandledExceptionFilter  {SetUnhandledExceptionFilter GetCurrentProcess �TerminateProcess  �IsProcessorFeaturePresent PQueryPerformanceCounter GetCurrentProcessId "GetCurrentThreadId  �GetSystemTimeAsFileTime "DisableThreadLibraryCalls lInitializeSListHead �IsDebuggerPresent                                                                                                                                                                                                                                                                                                                                                                                                                                                         �] �f���2��-�+  ����          /        �                                                                                                                                                                                                                                                                                                                                                                                                                                                                                        U  ��  `  �   �  �    �    4   �  @  �  0�  �  �  D�  �  �  \�  �  �  l�  �  �  |�  �  �  ��  �  <  ��  @  w  ��  �  �  ��  �  �  Ђ  �  �  �  �     �    9  �  9  �   �  �    <�    )  L�  )  f  X�  f  0  x�  0  L  ��  L  X  ��  `  �  ��  �  �  ��  �  B  Ѓ  B  ^  �  ^  �  ��  �  �  �  �  K  �  K  �  0�  �    @�    y  P�  y  �  h�  �  �  x�  �    �    k  ��  k  �  ��  �  �  x�  �  3   ��  3   �   ��  �   !  Ԅ  !  D!  �  D!  �!  �  �!  �!  �  �!  "  �  "  A"  ,�  A"  w"  @�  �"  �"  P�  �"  �"  X�  �"  �"  l�  �"  #  |�  #  �#  ��  �#  �#  ��  �#  �$  ��  �$  �$  ��  �$  �$  ̅  �$  �&  ��  �&  �&   �  �&  �&  �  �&  �)   �  �)  �)  8�  �)  �)  D�  �)  �)  X�  �)  �)  l�  �)  �*  ��  �*  �*  ��  �*  �*  ��  �*  �*  ��  �*  �*  Ȇ  �*  �*  ��  �*  +  ؆  +  �+  �  �+  �+   �  �+  �+  �  �+  �+  ��  �+  �+   �  �+  ,  4�  ,  ',  ��  ',  �,  D�  �,  �,  \�  �,  i-  ��  p-  �-  l�  �-  :/  x�  :/  `/  ��  `/  �/  ��  �/  �/  ��  �/  "0  ̇  "0  1  ��  1  1  �  1  /1  �  01  �1  �  �1  G2  $�  G2  �2  8�  �2  3  H�  3  r3  T�  r3  �3  p�  �3  �3  ��  �3  G4  ��  G4  �5  ��  �5  ?6  Ĉ  ?6  k6  Ԉ  p6  7  �  7  8  ��   8  f8   �  f8  s9  �  s9  �9  $�  �9  �9  ��  �9  �:  �  �:  �:  4�  �:  ';  D�  ';  �;  X�  �;  �;  l�  �;  �;  |�  �;  �;  ��  �;  <  ��   <  O<  ��  �<  �<  P�  �<  =  ��  =  2=  ̉  @=  �=  ܉  �=  L>  ��  L>  i>  �  i>  P@  �  P@  a@  4�  a@  q@  D�  q@  �@  T�  �@  A  �  A  dB  d�  pB  �B  |�  �B  ^C  ��  ^C  xC  ��  xC  �D  ��  �D  �D  ؊  �D  �D  �  �D  �D  ��  �D  $E  �  0E  KF  ,�  PF  gF  ��  gF  �F  @�  �F  �F  X�  �F  �F  p�   G  "G  ��  "G  �G  ��  �G  �G  ��  �G  �G  ��  �G  DH  ċ  DH  �H  ԋ  �H  �H  �  �H  �I  ��  �I  �I  ��  �I  �L  �  �L  �M  4�  �M  $N  L�  @N  �N  T�  �N  @O  d�  @O  �O  x�  �O  �O  ��  �O   P  ��   P  �P  ��  �P  OQ  Č  OQ  �Q  Ԍ  �Q  �Q  �  �Q  $R  ��  $R  �R  �  �R  �R  $�  �R  �R  8�  �R  �R  H�  �R  �R  d�  �R  �S  x�  �S   T  ��   T  T  8�   T  iT  ��  iT  �U  ��  �U  |V  ȍ  |V  �V  ܍  �V  �V  �  �V  ^W  x�  `W  ,X  ��  ,X  5Y  �  5Y  kY  ,�  �Y  �Y  @�  �Y  �Y  ��  �Y  �Z  L�  �Z  �Z  ��  �Z  [[  D�  \[  �[  T�  �[  =\  `�  @\  �\  ��  �\  �]  l�  �]  ,^  ��  ,^  __  �  `_  �_  x�  �_  L`  4�  L`  o`  ��  �`  �`  ��  �`  �`  ��  �`  (a  ��  (a  =a  ��  @a  ha  ��  ha  }a  ��  �a  �a  ��  �a  b  ��  b  $b  ��  $b  mb  ��  pb  �b  ��  �b  �c  @�  �c  �c  ��  �c  �c  ��  �c  >e  h�  @e  |e  �  |e  �e  �  �e  Yg  x�  �g  �g  ��  �g  ;h  ��  Ph  Rh  ��  Rh  ih  ��  ih  �h  ��  �h  �h  ��  �h  �h  ,�  �h  �h  `�                                                                                                                                                                                                                                                                                                                                                                                                                                               �   8  �                  P  �                  h  �               	  �                  	  �   ��  �          ��  }          �4   V S _ V E R S I O N _ I N F O     ���                                       <   S t r i n g F i l e I n f o      0 4 0 9 0 4 B 0   *   C o m p a n y N a m e     n o n e     >   F i l e D e s c r i p t i o n     l i b e n c h a n t     ,   F i l e V e r s i o n     2 . 2 . 8   >   I n t e r n a l N a m e   l i b e n c h a n t - 2 . 2     p &  L e g a l C o p y r i g h t   C o p y r i g h t   ( C )   2 0 0 2 - 2 0 0 7   D o m   L a c h o w i c z   N   O r i g i n a l F i l e n a m e   l i b e n c h a n t - 2 . 2 . d l l     6   P r o d u c t N a m e     l i b e n c h a n t     0   P r o d u c t V e r s i o n   2 . 2 . 8   D    V a r F i l e I n f o     $    T r a n s l a t i o n     	�    <?xml version='1.0' encoding='UTF-8' standalone='yes'?>
<assembly xmlns='urn:schemas-microsoft-com:asm.v1' manifestVersion='1.0'>
  <trustInfo xmlns="urn:schemas-microsoft-com:asm.v3">
    <security>
      <requestedPrivileges>
        <requestedExecutionLevel level='asInvoker' uiAccess='false' />
      </requestedPrivileges>
    </security>
  </trustInfo>
</assembly>
                                                                                                                                                                                                                                                                    p  $   ���������� ����X�p�x�� �(�                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                            This snapshot was automatically generated on
Tue, 16 Nov 2021 22:28:06 +0000

Version: 8.0.13
Branch: HEAD
Build: D:\a\php-ftw\php-ftw\php\vs16\x64\obj\Release_TS

Built-in Extensions
===========================
Core
bcmath
calendar
ctype
date
filter
hash
iconv
json
SPL
pcre
readline
Reflection
session
standard
mysqlnd
tokenizer
zip
zlib
libxml
dom
PDO
openssl
SimpleXML
xml
xmlreader
xmlwriter
curl
ftp
sqlite3
Phar
mbstring
mysqli


Dependency information:
Module: php_curl.dll
===========================
	libcrypto-1_1-x64.dll
	libssl-1_1-x64.dll
	libssh2.dll
	nghttp2.dll

Module: libssl-1_1-x64.dll
===========================
	libcrypto-1_1-x64.dll

Module: libssh2.dll
===========================
	libcrypto-1_1-x64.dll

Module: php_enchant.dll
===========================
	libenchant2.dll

Module: libenchant2.dll
===========================
	glib-2.dll
	gmodule-2.dll

Module: gmodule-2.dll
===========================
	glib-2.dll

Module: php_ftp.dll
===========================
	libcrypto-1_1-x64.dll
	libssl-1_1-x64.dll

Module: php_intl.dll
===========================
	icuuc68.dll
	icuin68.dll
	icuio68.dll
	vcruntime140_1.dll

Module: icuuc68.dll
===========================
	icudt68.dll
	vcruntime140_1.dll

Module: icuin68.dll
===========================
	icuuc68.dll
	vcruntime140_1.dll

Module: icuio68.dll
===========================
	icuuc68.dll
	icuin68.dll
	vcruntime140_1.dll

Module: php_ldap.dll
===========================
	libsasl.dll
	libcrypto-1_1-x64.dll
	libssl-1_1-x64.dll

Module: libsasl.dll
===========================
	libcrypto-1_1-x64.dll

Module: php_openssl.dll
===========================
	libcrypto-1_1-x64.dll
	libssl-1_1-x64.dll

Module: php_pgsql.dll
===========================
	libpq.dll

Module: libpq.dll
===========================
	libssl-1_1-x64.dll
	libcrypto-1_1-x64.dll

Module: php_snmp.dll
===========================
	libcrypto-1_1-x64.dll

Module: php_sodium.dll
===========================
	libsodium.dll

Module: php_sqlite3.dll
===========================
	libsqlite3.dll

Module: php_pdo_pgsql.dll
===========================
	libpq.dll

Module: php_pdo_sqlite.dll
===========================
	libsqlite3.dll

                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                    num)
  {}
  bool fix_length_and_dec() override
  {
    set_maybe_null();
    max_length= MAX_BLOB_WIDTH;
    return FALSE;
  }
  /* Mark that collation can change between calls */
  bool dynamic_result() override { return 1; }

  LEX_CSTRING func_name_cstring() const override
  {
    static LEX_CSTRING name= {STRING_WITH_LEN("column_get") };
    return name;
  }
  String *val_str(String *) override;
  longlong val_int() override;
  longlong val_int_signed_typecast() override
  {
    unsigned_flag= false;   // Mark that we want to have a signed value
    longlong value= val_int(); // val_int() can change unsigned_flag
    if (!null_value && unsigned_flag && value < 0)
      push_note_converted_to_negative_complement(current_thd);
    return value;
  }
  longlong val_int_unsigned_typecast() override
  {
    unsigned_flag= true; // Mark that we want to have an unsigned value
    longlong value= val_int(); // val_int() can change unsigned_flag
    if (!null_value && unsigned_flag == 0 && value < 0)
      push_note_converted_to_positive_complement(current_thd);
    return value;
  }
  double val_real() override;
  my_decimal *val_decimal(my_decimal *) override;
  bool get_dyn_value(THD *thd, DYNAMIC_COLUMN_VALUE *val, String *tmp);
  bool get_date(THD *thd, MYSQL_TIME *ltime, date_mode_t fuzzydate) override;
  void print(String *str, enum_query_type query_type) override;
  Item *get_copy(THD *thd) override
  { return get_item_copy<Item_dyncol_get>(thd, this); }
};


class Item_func_dyncol_list: public Item_str_func
{
public:
  Item_func_dyncol_list(THD *thd, Item *str): Item_str_func(thd, str)
    {collation.set(DYNCOL_UTF);}
  bool fix_length_and_dec() override
  {
    set_maybe_null();
    max_length= MAX_BLOB_WIDTH;
    return FALSE;
  }
  LEX_CSTRING func_name_cstring() const override
  {
    static LEX_CSTRING name= {STRING_WITH_LEN("column_list") };
    return name;
  }
  String *val_str(String *) override;
  Item *get_copy(THD *thd) override
  { return get_item_copy<Item_func_dyncol_list>(thd, this); }
};

/*
  this is used by JOIN_TAB::keep_current_rowid
  and stores handler::position().
  It has nothing to do with _rowid pseudo-column, that the parser supports.
*/
class Item_temptable_rowid :public Item_str_func
{
public:
  TABLE *table;
  Item_temptable_rowid(TABLE *table_arg);
  const Type_handler *type_handler() const override
  { return &type_handler_string; }
  Field *create_tmp_field(MEM_ROOT *root, bool group, TABLE *table)
  { return create_table_field_from_handler(root, table); }
  String *val_str(String *str) override;
  enum Functype functype() const override { return  TEMPTABLE_ROWID; }
  LEX_CSTRING func_name_cstring() const override
  {
    static LEX_CSTRING name= {STRING_WITH_LEN("<rowid>") };
    return name;
  }
  bool fix_length_and_dec() override;
  Item *get_copy(THD *thd) override
  { return get_item_copy<Item_temptable_rowid>(thd, this); }
};
#ifdef WITH_WSREP

#include "wsrep_api.h"

class Item_func_wsrep_last_written_gtid: public Item_str_ascii_func
{
  String gtid_str;
public:
  Item_func_wsrep_last_written_gtid(THD *thd): Item_str_ascii_func(thd) {}
  LEX_CSTRING func_name_cstring() const override
  {
    static LEX_CSTRING name= {STRING_WITH_LEN("wsrep_last_written_gtid") };
    return name;
  }
  String *val_str_ascii(String *) override;
  bool fix_length_and_dec() override
  {
    max_length= WSREP_GTID_STR_LEN;
    set_maybe_null();
    return FALSE;
  }
  Item *get_copy(THD *thd) override
  { return get_item_copy<Item_func_wsrep_last_written_gtid>(thd, this); }
};

class Item_func_wsrep_last_seen_gtid: public Item_str_ascii_func
{
  String gtid_str;
public:
  Item_func_wsrep_last_seen_gtid(THD *thd): Item_str_ascii_func(thd) {}
  LEX_CSTRING func_name_cstring() const override
  {
    static LEX_CSTRING name= {STRING_WITH_LEN("wsrep_last_seen_gtid") };
    return name;
  }
  String *val_str_ascii(String *) override;
  bool fix_length_and_dec() override
  {
    max_length= WSREP_GTID_STR_LEN;
    set_maybe_null();
    return FALSE;
  }
  Item *get_copy(THD *thd) override
  { return get_item_copy<Item_func_wsrep_last_seen_gtid>(thd, this); }
};

class Item_func_wsrep_sync_wait_upto: public Item_int_func
{
  String value;
public:
 Item_func_wsrep_sync_wait_upto(THD *thd, Item *a): Item_int_func(thd, a) {}
 Item_func_wsrep_sync_wait_upto(THD *thd, Item *a, Item* b): Item_int_func(thd, a, b) {}
  const Type_handler *type_handler() const override
  { return &type_handler_string; }
  LEX_CSTRING func_name_cstring() const override
  {
    static LEX_CSTRING name= {STRING_WITH_LEN("wsrep_sync_wait_upto_gtid") };
    return name;
  }
  longlong val_int() override;
  Item *get_copy(THD *thd) override
  { return get_item_copy<Item_func_wsrep_sync_wait_upto>(thd, this); }
};
#endif /* WITH_WSREP */

#endif /* ITEM_STRFUNC_INCLUDED */
