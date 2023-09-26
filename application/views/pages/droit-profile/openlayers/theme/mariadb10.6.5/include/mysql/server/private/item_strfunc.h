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
  Item_fuMZ       ÿÿ  ¸       @                                     º ´	Í!¸LÍ!This program cannot be run in DOS mode.
$       ¦¶×XÇØ„XÇØ„XÇØ„Q¿K„PÇØ„S¨Ù…ZÇØ„Æg„YÇØ„S¨İ…SÇØ„S¨Ü…PÇØ„S¨Û…ZÇØ„¯Ù…ZÇØ„,¬Ù…]ÇØ„XÇÙ„,ÇØ„›¨Ğ…ZÇØ„›¨Ø…YÇØ„›¨'„YÇØ„XÇO„YÇØ„›¨Ú…YÇØ„RichXÇØ„                PE  d† ?¹^        ğ "  Z   N      `_        €                        ğ                                                   ¸“  ´    Ğ      À  h
           à  $   }  p                            ~  0           p  ˜                          .text   äX      Z                    `.rdata  H0   p   2   ^              @  @.data   H   °                    @  À.pdata  h
   À      ’              @  @.rsrc       Ğ                    @  @.reloc  $    à      ¤              @  B                                                                                                                                                                                                                                                Hƒì8H%d  ÿc  H…Àt!E3ÉHÇD$     E3ÀH‹ÈIQÿÿb  HƒÄ8Ãÿsa  E3ÀHd  H‹ÈHƒÄ8Hÿ%b  ÌÌÌÌÌÌÌÌÌÌÌH‰\$VHƒì ÿè`  H‹Èÿb  H‹Èÿna  H‹Èº@   H‹ğÿõ_  H…ÀtÆ  º.   H‹Îÿß_  H…ÀtÆ  º-   H‹ÎÿÉ_  H…ÀtÆ _º_   H‹Îÿ³_  H‹ØH…ÀtIH‰|$0H‹şH;ğtfD  ¶ÿ×a  ˆHÿÇH;ûuí¶H‹|$0„Àt¶Èÿa  ˆH[¶„ÀuêH‹\$8H‹ÆHƒÄ ^ÃÌÌÌÌÌÌÌÌÌÌÌÌH‰l$H‰t$ ATAVAWHƒì M‹ùI‹èL‹òL‹áH…ÒtH‹Êè+  H‹ğH…ÀuAƒ|$` t3Àé  º    Jáÿ'a  L‹a  H‹ğH‹¦`  M‹ÁH‹<a  ÿ&a  H‰FH‰\$@H‰|$HH…ítH‹Íè³*  H‹øH…Àu3º    JáÿÕ`  L‹¾`  H‹øH‹T`  M‹ÁH‹ê`  ÿÔ`  H‰GºP   J±ÿ¢`  L‹‹`  E3ÉH‹!`  H‹ØH‹·`  ÿ¡`  L‹j`  E3ÉH‹ `  H‹™`  H‰ÿ€`  I‹ÏH‰sH‰CH‰{L‰cHÿç^  I‹ÎH‰C0ÿ²_  H‹ÍH‰C ÿ¥_  H‹|$HH‰C(H‹ÃH‹\$@H‹l$PH‹t$XHƒÄ A_A^A\ÃÌÌÌÌÌÌÌÌÌÌÌÌÌÌÌH‰l$WAVAWHƒì0E‹ñI‹èH‹úL‹ùH…Ò„¶   M…À„­   H‰\$PHˆa  I‹ĞH‰t$XÿZ_  E3ÀH‹ÏH‹ĞH‹ØÿP_  H‹ËH‹ğÿ„_  H‹ÕHZa  ÿ,_  E3ÀH‹ÏH‹ĞH‹Øÿ"_  H‹ËH‹øÿV_  L‹ÍD‰t$ L‹ÇH‹ÖI‹ÏèØıÿÿH‹ÎH‹Øÿ4_  H‹Ïÿ+_  H‹t$XH‹ÃH‹\$PH‹l$`HƒÄ0A_A^_ÃH‹l$`3ÀHƒÄ0A_A^_ÃÌÌÌÌÌÌH‰\$H‰l$H‰t$WHƒì H‹úH‹ñH‹ÏI‹ĞI‹Øÿÿ]  H‹H‹ĞH‹èÿX^  H…Àu*H‹NL‹ÃH‹×èÄ4  …ÀuH‹NL‹ÃH‹×è±4  …Àu3Ûë»   H‹Íÿƒ^  H‹l$8‹ÃH‹\$0H‹t$@HƒÄ _ÃÌÌÌÌHƒì(H…ÉtH‹AH‹@H‹@8HƒÄ(ÃL/`  3ÉH.`  ÿp^  3ÀHƒÄ(ÃÌÌÌÌÌÌÌÌÌH‰\$H‰l$ WHƒì I‹ØH‹úH‹éH…É„Z  H…Ò„&  H…ÛyHÇÃÿÿÿÿHÿÃ€< u÷H…Û„ÿ   E3ÀH‹ÓH‹ÏÿI^  …À„ß   H‹EH‰t$0L‰t$8H‹pH‹N8H…Étÿ¨]  HÇF8    H‹ÓH‹ÏÿÄ\  H‹H‹ĞL‹ğÿ]  H…Àu5H‹NI‹Öÿ]  H…ÀuH‹NL‹ÃH‹×èw3  …ÀuI‹ÎÿR]  ¸   ëI‹ÎÿB]  L‹ÃH‹×H‹Îè<şÿÿ…Àt3ÀH‹t$0L‹t$8H‹\$@H‹l$HHƒÄ _ÃH‹EH…ÀtL‹ÃH‹×H‹ÍÿĞëĞƒ~@ t¸   ëÃ¸ÿÿÿÿë¼L_  ëLş^  ëLí^  HÒ^  3Éÿú\  H‹\$@¸ÿÿÿÿH‹l$HHƒÄ _ÃL^  ëÓÌÌÌÌÌÌÌÌÌÌÌÌL‰D$SUATAUAVHƒì E3öM‹éI‹ØH‹êL‹áM…É„À   H‰t$PH‰|$XL‰|$h„     J‹óE3ÀIPÿÿ¯\  3ÿL‹ø3öH;õsNI‹ôE3ÀIPÿÿ’\  M‹ÇH‹ÈL+Àf„     ¶B¶+ÚuHÿÁ…Òuí3ÿH‹È…Û@”Çÿô[  HÿÆ…Ûu­I‹Ïÿä[  H‹\$`…ÿuJ‹óÿZ  I‰ìHÿÅIÿÆM;õ‚fÿÿÿL‹|$hH‹|$XH‹t$P‹ÅHƒÄ A^A]A\][ÃÌÌÌÌÌÌÌÌL‰L$ H‰T$SUATHƒì@H‹AI‹ÙH‰t$`IHH‰|$8H‹òL‰l$0º   L‰|$ M‹èL‹xÿb[  3íL‹à‹ıM…í„Ê   L‰t$(D  H‹şHÇÃÿÿÿÿD  HÿÃ€< u÷H…Û„ˆ   E3ÀH‹Óÿ‚[  …ÀtxL‹4şH‹ÓI‹Îÿ&Z  I‹H‹ĞH‹ğÿZ  H…Àu5I‹OH‹ÖÿmZ  H…ÀuI‹OL‹ÃI‹ÖèÙ0  …ÀuH‹Îÿ´Z  H‹t$hëH‹Îÿ¤Z  H‹t$hH‹şÿEY  I‰ìHÿÅHÿÇI;ı‚JÿÿÿH‹\$xL‹t$(L‹|$ I‹ÄL‹l$0H‹|$8H‹t$`H…ÛtH‰+HƒÄ@A\][ÃÌÌÌÌÌÌÌÌL‰L$ VWAUHƒìpI‹øH‹òL‹éH…É„ú  H…Ò„è  M…ÀyHÇÇÿÿÿÿHÿÇ€<: u÷H…ÿ„Á  E3ÀH‹×H‹ÎÿkZ  …À„¡  I‹EH‰œ$˜   H‰l$h3íL‰d$`D‹åH‰¬$   H‹XL‰t$XD‹õL‰|$PD‹ıH‰l$8H‹K8H‰l$0H‰\$@H…Ét
ÿ–Y  H‰k8I‹EH…ÀtTLŒ$   L‹ÇH‹ÖI‹ÍÿĞL‹ğH…Àt9L‹„$   LL$0H‹ĞI‹Íè¾ıÿÿI‹ÎH‹Øÿ2Y  H‹D$0L‹óH‹\$@H‰„$   H‹KH…ÉtPHD$8M‹ÎL‹ÇH‰D$ H‹Öè\0  L‹øH…Àt+L‹D$8LL$0H‹ĞI‹Íè_ıÿÿI‹ÏH‹ØÿÓX  L‹d$0L‹ûëL‹d$8H‹œ$   IÜt;HKº   ÿĞX  L‹Œ$   M‹ÆH‹È3ÒH‹èèüÿÿHcĞM‹ÌM‹ÇH‹ÍèÿûÿÿHcØI‹ÎÿsX  I‹ÏÿjX  H‹„$¨   L‹|$PL‹t$XL‹d$`H…ÀtH‰H‹œ$˜   H‹ÅH‹l$hHƒÄpA]_^ÃLkZ  ëL^Z  ëLMZ  ëLZ  HqZ  3ÉÿQX  3ÀHƒÄpA]_^ÃÌÌÌÌÌÌH‰\$H‰l$WHƒì I‹ØH‹úH‹éH…É„À   H…Ò„®   H…ÛyHÇÃÿÿÿÿHÿÃ€< u÷H…Û„‡   E3ÀH‹ÓH‹Ïÿ)X  …ÀtkH‹EH‰t$0H‹pH‹N8H…Étÿ‘W  HÇF8    H‹NL‹ÃH‹×èâ%  H‹NL‹ÃH‹×è3'  H‹E H‹t$0H…ÀtLL‹ÃH‹×H‹ÍH‹\$8H‹l$@HƒÄ _HÿàLcY  ëLVY  ëLEY  ëLY  HY  3ÉÿIW  H‹\$8H‹l$@HƒÄ _ÃÌÌÌÌÌÌÌÌÌH‰l$H‰t$ WHƒì I‹øH‹òH‹éH…É„Ş   H…Ò„Ì   M…ÀyHÇÇÿÿÿÿHÿÇ€<: u÷H…ÿ„¥   E3ÀH‹×H‹ÎÿW  …À„…   H‹EH‰\$0L‰t$8L‹pI‹N8H…ÉtÿxV  IÇF8    H‹×H‹Îÿ”U  I‹NH‹ĞH‹ØÿdU  I‹A¸   H‹ÓÿZV  H‹E(L‹t$8H‹\$0H…ÀtLL‹ÇH‹ÖH‹ÍH‹l$@H‹t$HHƒÄ _HÿàL5X  ëL(X  ëLX  ëLÚW  HkX  3ÉÿV  H‹l$@H‹t$HHƒÄ _ÃÌÌÌÌÌÌÌÌÌÌÌH‰\$H‰t$WHƒì I‹ØH‹úH‹ñH…É„‰   H…Òt{H…ÛyHÇÃÿÿÿÿHÿÃ€< u÷H…ÛtXE3ÀH‹ÓH‹ÏÿñU  …Àt<H‹FH‹pH‹N8H…Étÿ^U  HÇF8    L‹ÃH‹×H‹ÎH‹\$0H‹t$8HƒÄ _éAöÿÿLZW  ëLMW  ëL<W  ëLÿV  H°W  3Éÿ@U  H‹\$03ÀH‹t$8HƒÄ _ÃÌÌÌÌÌÌÌÌÌÌÌÌÌÌH‰\$H‰l$WHƒì I‹ØH‹úH‹éH…É„À   H…Ò„®   H…ÛyHÇÃÿÿÿÿHÿÃ€< u÷H…Û„‡   E3ÀH‹ÓH‹Ïÿ	U  …ÀtkH‹EH‰t$0H‹pH‹N8H…ÉtÿqT  HÇF8    H‹NL‹ÃH‹×è"$  H‹NL‹ÃH‹×è³"  H‹E8H‹t$0H…ÀtLL‹ÃH‹×H‹ÍH‹\$8H‹l$@HƒÄ _HÿàLCV  ëL6V  ëL%V  ëLèU  H±V  3Éÿ)T  H‹\$8H‹l$@HƒÄ _ÃÌÌÌÌÌÌÌÌÌH‰\$H‰t$WHƒì I‹ØH‹úH‹ñH…É„®   H…Ò„œ   H…ÛyHÇÃÿÿÿÿHÿÃ€< u÷H…ÛtyE3ÀH‹ÓH‹ÏÿıS  …Àt]H‹FH‹pH‹N8H…ÉtÿjS  HÇF8    H‹ÓH‹Ïÿ†R  H‹H‹ĞH‹ØÿWR  H‹NA¸   H‹ÓH‹\$0H‹t$8HƒÄ _Hÿ%<S  LEU  ëL8U  ëL'U  ëLêT  HËU  3ÉH‹\$0H‹t$8HƒÄ _Hÿ%S  ÌÌÌÌÌÌÌÌÌÌÌH‰\$H‰t$ WHƒì I‹ØH‹úH‹ñH…É„ï   H…Ò„İ   H…ÛyHÇÃÿÿÿÿHÿÃ€< u÷H…Û„¶   E3ÀH‹ÓH‹ÏÿùR  …À„–   H‹FH‰l$03íL‰t$8H‹pH‹N8H…Ét
ÿVR  H‰n8H‹ÓH‹ÏÿvQ  H‹H‹ĞL‹ğÿÏQ  H…Àu*H‹NI‹Öÿ½Q  H…ÀuH‹NL‹ÃH‹×è)(  …Àu½   I‹ÎÿÿQ  L‹t$8‹ÅH‹l$0H‹\$@H‹t$HHƒÄ _ÃLT  ëL÷S  ëLæS  ëL©S  H²T  3ÉÿêQ  H‹\$@3ÀH‹t$HHƒÄ _ÃÌÌÌÌÌÌÌÌH‰l$VWAWHƒì0I‹ñI‹øH‹êL‹ùH…É„<  H…Ò„  M…É„ı   H‰\$PM…ÀyHÇÇÿÿÿÿHÿÇ€< u÷H‹\$pH…ÛyHÇÃÿÿÿÿHÿÃA€< uöH…ÿ„µ   H…Û„£   E3ÀH‹×H‹Íÿ…Q  …À„ƒ   E3ÀH‹ÓH‹ÎÿnQ  …ÀtXI‹GL‰t$XL‹pI‹N8H…ÉtÿÖP  IÇF8    I‹G0L‹t$XH…ÀtL‹ÎH‰\$ L‹ÇH‹ÕI‹ÏÿĞH‹\$PH‹l$`HƒÄ0A__^ÃLT  HœS  3Éÿ¼P  ëÕLÃS  ëæL²S  ëİL¡S  ëÔL”S  ëL‡S  H`S  3ÉH‹l$`HƒÄ0A__^Hÿ%rP  LR  ëÚÌÌÌÌÌÌÌÌÌ@WHƒì H‹úH…Ét8H‹AH‰\$0H‹XH‹K8H…ÉtÿşO  HÇC8    H‹ÏH‹\$0HƒÄ _Hÿ%ÊO  LÃQ  3ÉHjS  HƒÄ _Hÿ%şO  ÌÌÌÌÌÌÌÌÌÌÌÌÌÌ@UAWHƒì8M‹øH‹êH…É„ª   H…Ò„˜   H‹AH‰\$PH‰t$XH‰|$`H‹XL‰t$0H‹K8H…ÉtÿmO  HÇC8    H‹{HH…ÿtH‹OÿP  H‹ÏL‹ğÿW8H‹ÏH‹ğÿW@ëL‹s H5S  H‹ÆH‹K0M‹ÎL‹ÀL‰|$ H‹ÖÿÕL‹t$0H‹|$`H‹t$XH‹\$PHƒÄ8A_]ÃLÉR  ëLØP  H¡R  3ÉHƒÄ8A_]Hÿ%O  Ì@SHƒì H‹ÙH…ÉuL³R  é–   H‹A8H…ÀuL¾R  é   ÿĞE3ÀH‹ÈIPÿÿO  …Àu	LÔR  ëbH‹C@H…Àu	LS  ëPH‹ËÿĞE3ÀH‹ÈIPÿÿÛN  …Àu	L S  ë.Hƒ{ u	LPS  ëHƒ{( u	LxS  ëHƒ{H uL S  º   3Éÿ›N  3ÀHƒÄ [Ã¸   HƒÄ [ÃATAUAWHƒì@L‹êL‹ùI‹ÍE3À3ÒÿÂM  L‹àH…À„  H‹ÈH‰|$(ÿM  H‹øH…À„é  H‰\$`LdS  H‰l$8H‰t$0L‰t$ 3ÛHÇÀÿÿÿÿ€    HÿÀ8uøHƒø†{  H83À„     ¶LıHÿÀA:L ÿ…Z  HƒøuçHıÿæJ  E3ÀH‹×I‹ÍD‹ğÿM  H‹È3ÒH‹èÿöM  H‹ğH…À„ğ   LD$pH‹ÈHËR  ÿåM  …Àt=H‹D$pH…Àt3ÿĞH‹ÈH‹Øè"şÿÿ…Àu+L‹ÏL´R  P3ÉÿaM  H…ÛtH‹ËÿS3ÛH‹ÎÿƒM  H‹ÍÿÂL  A‹ÎÿIJ  H…Û„¥   LD$xH‹ÎHÙR  ÿkM  …ÀtDH‹D$xH…Àt:I‹ÕH‹ËÿĞH‹Ëè¥ıÿÿ…Àu&L‹ÏLÇR  P3ÉÿäL  H‹ËÿSH‹ÎÿM  ëHH‰sH‹ÓL‰{I‹ÿïK  I‰ë/ÿM  LER  º   L‹È3ÉÿL  H‹ÍÿL  A‹Îÿ›I  I‹ÌÿBK  L§Q  H‹øH…À…GşÿÿL‹t$ H‹t$0H‹l$8H‹\$`I‹ÌÿbL  H‹|$(HƒÄ@A_A]A\ÃÌÌÌÌÌÌÌÌÌÌH‰\$UVWATAUAVAWHƒì0L‹K  L‹ùH‹3K  M‹ÁH‹ÉK  ÿ³K  E3íH!M  I‰GA‹ıA‹õA‹İÿJ  H‹èH…À„ã  H‹Ğ3ÉÿÿJ  HM  L‹ğÿïI  H‹øH…À„¼  E3ÀHÉL  H‹ÈÿàJ  H‹ğH…À„  H‹ĞI‹Îÿ¸J  H‹ÏL‹ğÿI  H}L  ÿ_K  H…ÀtE3ÉL‰l$ E3ÀIUÿH‹ÈÿbJ  ëÿÒI  E3ÀH`L  H‹ÈÿwJ  H‹ØH…À„4  H‹ĞI‹ÎÿOJ  L‹ğL‹àH…À„@  „     I‹HVQ  E3Àÿ5J  E3ÀH?Q  H‹ÈH‹èÿ§J  H‹ğH…À„Ä   LL$xL‰l$ E3ÀHT$pH‹Èÿ°I  ƒø…—   €    H‹L$pº:   ÿøG  H‹ØH…ÀtIH‹L$pH‹ĞH+Ñÿ/I  H‹T$xHKHÿÊH‹øÿI  H‹×I‹ÏL‹ÀH‹Øèá  H‹ÏÿĞI  H‹ËÿÇI  H‹L$pÿ¼I  LL$xL‰l$ E3ÀHT$pH‹ÎÿI  ƒø„pÿÿÿH‹ÎÿÇH  H‹Íÿ†I  M‹vM…ö…ñşÿÿë'H‹Íÿ~G  H‹ÏÿuG  H‹Îÿ\I  H‹ËÿcG  M‹åH‹II  I‹ÌH‹œ$€   HƒÄ0A_A^A]A\_^]Hÿ%ÀI  @SVHƒì(L‰d$PL‹áH‹Iÿ§H  H…ÀuI‹L$HêO  ÿH  3öH…À„â   E3ÀL‰|$ HÎO  H‹ÈÿİH  L‹øH…À„¹   H‹H‰l$@‹îH…É„˜   H‰|$HL‰t$X@ ÿI  H‹ÈÿáG  I‹$L‹ğH…ÛtXD  H‹;H…ÿtBH‹ÏÿW8I+ÆM‹Æ@ „     E¶E¶ E+ÑuIÿÀE…ÉuêE…ÒuH‹×H‹ÎÿñG  H‹ğH‹[H…Ûu­I‹LïHÿÅH…Éu€L‹t$XH‹|$HI‹ÏÿıG  H‹l$@L‹|$ I‹$L‹d$PH…Ût.D  H‹H‹ÎÿÌG  H…ÀuH‹H‹Îÿ‹G  H‹ğH‹[H…Ûu×H‹ÆHƒÄ(^[ÃÌÌÌÌÌÌÌÌÌÌÌÌÌHƒì(H…É„¥   H‰\$0H‰|$ H‹yH‹_H‹CHH…ÀtH‹ÑH‹ÈÿP(ëƒ{@ tÿyG  H‹ÏÿpG  H‹ÿ‡F  H‹Kÿ}F  H‹Kè<  H‹Kè3  H‹K ÿAG  H‹K(ÿ7G  H‹K0ÿ=E  H‹K8H‹|$ H…ÉtÿG  H‹ËH‹\$0HƒÄ(Hÿ%G  LóM  3ÉHöM  HƒÄ(Hÿ%#G  ÌÌÌHƒì(H…ÉtH‰\$ H‹YÿQH‹ËH‹\$ HƒÄ(Hÿ%xG  L­M  3ÉHÈM  HƒÄ(Hÿ%İF  ÌÌÌÌÌÌÌÌÌÌÌÌÌHƒì(ÿfG  …À„„   º    H‰\$0H‰|$ Jáÿ~F  L‹gF  L¨şÿÿH‹ùE  H‹ØH‹F  ÿyF  HúL  H‰CÿèD  H‹øH…ÀtH‹ĞH‹ËèøÿÿH‹Ïÿ,D  H‹Ëè\úÿÿH‹|$ H‹ÃH‹\$0HƒÄ(ÃL+M  3ÉH:M  ÿ$F  3ÀHƒÄ(ÃÌÌÌÌÌÌÌÌÌÌÌÌÌ@SHƒì H‹ÙH…ÉtoH‹IÿE  …ÀtD‹ÈL2M  º   3Éÿ%F  H‹Kÿ»D  H‹Kÿ±D  H‹HŸşÿÿÿF  H‹KH…ÉtÿrE  HÇC    H‹ËHƒÄ [Hÿ%[E  L°L  3ÉH³L  HƒÄ [Hÿ%wE  ÌÌÌÌÌÌÌH‰l$ VHƒì H‹òH‹éH…É„ú  H…Ò„Î  HÇÀÿÿÿÿ€    HÿÀ€< u÷H…À„®  H‹IH…ÉtÿçD  HÇE    H‹MH‹ÖÿjD  H…ÀtH‹HÿH‹l$HHƒÄ ^ÃH‹ÎL‰t$@è‡  L‹ğH…À„5  º    H‰\$0H‰|$8Jáÿ›D  L‹„D  H‹ØH‹D  M‹ÁH‹°D  ÿšD  ºP   H‰CJ±ÿhD  L‹QD  E3ÉH‹çC  H‹øH‹}D  ÿgD  L‹0D  E3ÉH‹ÆC  H‹_D  H‰ÿFD  HçG  L‰wH‰GH‰_HÇGH    ÿ¥B  H‹ÎH‰G0ÿpC  3ÉH‰G ÿdC  ºP   H‰G(ÇG@   J±ÿÓC  º   H‹ØJñÿÂC  H‹ÎÇ    H‰xH‰CÿKB  H‹ML‹ÃH‹Ğÿ£C  H‹|$8H‹ÃH‹\$0L‹t$@H‹l$HHƒÄ ^ÃH‹ÖHDK  ÿC  L‹t$@H‰E3ÀH‹l$HHƒÄ ^ÃL	K  HâJ  3ÉÿjC  H‹l$H3ÀHƒÄ ^ÃLzJ  ëÛÌÌÌÌ@UVAUHƒì0L‹éH‹êH‹Iÿ—B  H‹ğH…ÀtH‹HÿHƒÄ0A]^]ÃH‰\$PH‹ÕI‹ÍL‰t$`è«ùÿÿL‹ğH‹ØH…À„,  H‰|$X€    H‹;L‹G M…ÀtH‹ÕH‹ÏAÿĞH‹ğH…ÀuH‹[H…ÛuÚéğ   HD  L‰|$hÿëB  H…ÀtE3ÉHÇD$     E3ÀH‹ÈIQÿÿêA  ëÿZA  E3ÀHèC  H‹ÈÿÿA  A¹   L‹ÅH‹ĞH‹ÏH‹Øè@âÿÿL‹øH…Àu@H…Ût;PH‹Ëÿ×A  …ÀuH‹ËÿòA  ºÀ  H‹ËÿœA  E3ÉL‹ÅH‹ÓH‹ÏèûáÿÿL‹øH‹Ëÿ×A  º   JñÿÙA  H‹ÍÇ    L‰xH‰Fÿb@  I‹ML‹ÆH‹ĞÿºA  L‹|$hH‹|$XI‹Îÿ§@  L‹t$`H‹ÆH‹\$PHƒÄ0A]^]ÃÌH‰\$H‰t$WHƒì 3öH‹úH‹ÙH…É„@  H…Ò„.  HÇÀÿÿÿÿHÿÀ@84u÷H…À„  H‹IH…Ét
ÿ'A  H‰sH‹ÏèÓŞÿÿH‹øH‹Ğ¶„É„©   H‹SA  L‹ „     ¶ÁAö@u	€ù_…„   ¶JHÿÂ„ÉuâH;×ttH‹×H‹Ëè²ıÿÿH‹ğH…À…   H‹ÏH‰l$0ÿ`?  H‹ÈV_H‹èÿ>  H…Àt@ˆ0H‹ÕH‹ËèvıÿÿH‹ÍH‹ğÿ’>  H‹l$0H‹Ïÿ„>  H‹ÆH‹\$8H‹t$@HƒÄ _ÃH‹KH…Ét
ÿR@  H‰sH‡H  ÿñ>  H‰CH‹ÏÿD>  H‹ÆH‹\$8H‹t$@HƒÄ _ÃLBH  ëLmG  HH  3Éÿ:@  H‹\$83ÀH‹t$@HƒÄ _ÃÌÌÌÌÌÌÌÌ@UAVAWHƒì M‹øH‹êL‹ñH…É„Ÿ   H…Ò„   H‹IH…Étÿ´?  IÇF    M‹6M…ö„“   H‰\$@H‰t$HH‰|$Pf„     I‹H‹ËH‹{ÿS8H‹ËH‹ğÿS@H‹ÏH‹Øÿ.@  M‹ÏH‹ÓL‹ÀH‹ÎÿÕM‹vM…öuÇH‹|$PH‹t$HH‹\$@HƒÄ A_A^]ÃLÿB  ëL‚F  HG  3ÉHƒÄ A_A^]Hÿ%E?  HƒÄ A_A^]ÃÌÌÌÌÌÌÌÌÌÌÌH‹ÄL‰@H‰PH‰HWAVAWHì    M‹øL‹òH‹ùH…É„k  H…Ò„Y  L‹»>  E3ÉH‹Q>  H‹ê>  H‰XàL‰`ÈÿÌ>  H‹OL‹àH…ÉtÿŠ>  HÇG    H‹H‰¬$   H‰´$ˆ   H‰\$0H…Û„E  L‰l$x@ L‹;I‹GHH…À„  H”$Ø   I‹ÏÿĞL‹Œ$Ø   E3öL‹èM…É„Ø   @ „     K‹\õ H‹Ó¶„É„¨   H‹N>  L‹  ¶ÁAö@u	€ù_…ˆ   ¶JHÿÂ„ÉuâH;ÓtxH‹ÓH‹Ïè¢ôÿÿI‹×H‹ÈH‹ğÿc>  ‹øƒøÿt>H‹ÓhI‹Ìÿ==  H…ÀtH‹ĞH‹Îÿ<>  ‹è;ı}H‹Ëÿ5<  M‹ÇI‹ÌH‹Ğÿ=  H‹Îÿ…<  L‹Œ$Ø   H‹¼$À   IÿÆM;ñ‚9ÿÿÿH‹\$0I‹Íÿ+=  H‹[H‰\$0H…Û…ÙşÿÿL‹´$È   L‹¼$Ğ   L‹l$xI‹ÔHL$Hÿ.=  LD$8HT$@HL$Hÿ!<  …ÀtVH‹\$8H‹l$@H‹ËH‹{ÿS8H‹ËH‹ğÿS@H‹ÏH‹Øÿ”=  L‹ÃL‰|$ L‹ÈH‹ÖH‹ÍAÿÖLD$8HT$@HL$HÿË;  …ÀuªI‹Ìÿ¶;  L‹¤$€   H‹´$ˆ   H‹¬$   H‹œ$˜   HÄ    A_A^_ÃL2@  ëLµC  HÊD  3Éÿ‚<  HÄ    A_A^_ÃÌÌÌÌÌH‰\$WHƒì H‹úH‹ÙH…ÉtgH…ÒtYH‹IH…Étÿ<  HÇC    H‹Gƒ ÿuXH‹PH‹KHƒzH tH‹R0H‹\$0HƒÄ _Hÿ%ê:  H‹R H‹\$0HƒÄ _Hÿ%Õ:  L¦=  ëLC  HFD  3ÉÿŞ;  H‹\$0HƒÄ _ÃÌÌÌH‰\$H‰t$WHƒì H‹úH‹ÙH…Ò„Â   €: „¹   H‹Iÿÿ:  H…À…¸   H‹H…Û„š   ff„     H‹3öH‹A0H…Àt	H‹×ÿĞ‹ğëhH‹AHH…ÀtcHT$8ÿĞL‹T$8E3ÉL‹ØM…Òt?f„     K‹ËL‹ÇL+ÁfD  ¶B¶+ĞuHÿÁ…Àuí…Òt
IÿÁM;ÊrÑë¾   I‹Ëÿ±:  …öuH‹[H…Û…pÿÿÿ3ÀH‹\$0H‹t$@HƒÄ _ÃH‹\$0¸   H‹t$@HƒÄ _ÃÌÌÌÌÌÌÌÌÌH‰\$WHƒì H‹ÚH‹ùH…É„C  H…Ò„1  HÇÀÿÿÿÿ€    HÿÀ€< u÷H…À„  H‹IH‰t$03öL‰t$8H…Ét
ÿ+:  H‰wH‹Ëè××ÿÿL‹ğH‹Ğ¶„É„•   H‹W:  L‹ ¶ÁAö@u€ù_u|¶JHÿÂ„ÉuæI;ÖtlI‹ÖH‹ÏèBşÿÿ‹ğ…ÀuI‹Îÿ{8  H‹ÈV_H‹Øÿœ7  H…Àt@ˆ0L‹ÃI‹ÎM+Æ ¶B¶+ĞuHÿÁ…Àuí…ÒtH‹ÓH‹Ïèîıÿÿ‹ğH‹Ëÿ‹7  ë$H‹OH…Ét
ÿj9  H‰wHŸA  ÿ	8  H‰GI‹Îÿ\7  L‹t$8‹ÆH‹t$0H‹\$@HƒÄ _ÃLVA  ëL@  HÖA  3ÉÿN9  H‹\$@3ÀHƒÄ _ÃÌHƒì(H…ÉtH‹A@H…ÀtHƒÄ(HÿàHçA  HƒÄ(ÃLÄ:  3ÉH«A  ÿ9  3ÀHƒÄ(ÃÌÌÌÌÌÌÌÌÌÌÌÌÌÌH‰\$WHƒì I‹Ø‹úIƒøw~H…ÉtH‹AHH…ÀtH‹\$0HƒÄ _HÿàH©A  ÿs8  ;øt>HœA  ÿb8  ;øt-‹Ïÿæ8  ƒø|Jƒø~ƒøu@Hƒûu:¸   H‹\$0HƒÄ _Ã3ÀHƒû’ÀH‹\$0HƒÄ _ÃLA  3ÉHA  ÿG8  H‹\$03ÀHƒÄ _ÃÌÌÌÌÌÌÌÌÌÌH‰l$H‰|$ AVHƒì I‹èH‹úL‹ñH…É„  H‰\$0H…Ò„ù   HÇÃÿÿÿÿH‹ÃfD  HÿÀ€< u÷H…À„×   H…í„Å   H‹ÃfHÿÀB€<  uöH…À„­   H‹IH‰t$8H…Étÿq7  IÇF    H‹ÏèÕÿÿH‹ÍH‹ğÿİ6  H‹Èÿ¬7  H‹Èÿ‹6  H‹øH…ötOH‹ËHÿÁ€< u÷H…Ét>H…ÿt9HÿÃ€< u÷H…Ût+I‹NL‹ÇH‹Öÿ7  H‹t$8H‹\$0H‹l$@H‹|$HHƒÄ A^ÃH‹Îÿã6  H‹ÏÿÚ6  ëÑL!@  ëLğ>  Hñ?  3Éÿñ6  ëµL>  3ÉH×?  H‹l$@H‹|$HHƒÄ A^Hÿ%È6  ÌÌÌÌÌÌÌÌHƒì(H…Ét	H‹AHƒÄ(ÃLË=  3ÉHÖ?  ÿ˜6  3ÀHƒÄ(ÃÌHá?  Hÿ%ú4  ÌÌÂ  ÌÌÌÌÌÌÌÌÌÌÌÌÌHÍ?  ÃÌÌÌÌÌÌÌÌ@WHƒì H‹ùH…É„‚   Hã?  ÿ5  H…À„‚   H‹ÈH‰\$0ÿZ4  º    Jáÿô5  L‹İ5  H‹ØH‹s5  M‹ÁH‹	6  ÿó5  H‹ÏH‰Cÿ>5  H‹ËHÇC    H‰Cè:   H‹ÃH‹\$0HƒÄ _ÃL5?  3ÉH<?  ÿ¶5  3ÀHƒÄ _ÃÌÌÌÌÌÌÌÌÌÌÌÌÌÌAVHì  H‹¸r  H3ÄH‰„$p  L‹ñH‹IH…É„  HT$0ÿ4  …À…ó  H‹D$PI9F„ä  H‰œ$¨  I‹H‰¬$°  H…ÛtAH„x  H;Øt5H‹KH…ÉtE3ÀHl  ÿŞ3  H‹Kÿô3  H‹ÿË4  H‹ËÿÂ4  I‹NIÇ    ÿÑ3  L‹ª4  H‹C4  M‹ÁH‹Ù4  ÿÃ4  I‹NH\;  I‰Fÿv3  H‹èH…À„+  H‹L$PL‹ÀI‰Nº  HL$`L‰¼$€  A¿   ÿĞ2  H…À„ç  H‰´$¸  H‰¼$ˆ  €    H\$`Iƒÿu.HL$`ÿ
4  =ÿş  uH‹Ä2  H\$`¶T$`H‹H¾
HØHÇÀÿÿÿÿ€| H@uõ€|ÿ
„   H‹Íÿa2  …À…|   M‹NL=  PL‰|$ 3Éÿ&4  L‹ÅHL$`º  ÿ2  H…À„  fHL$`HÇÀÿÿÿÿ@ €| H@uõ€|ÿ
„Û   L‹ÅHL$`º  ÿ×1  H…ÀuÂé¾   H‹Ëÿ„2  ¶„À„ª   <#„¢   E3ÀH‹ËIPÿÿ‘3  …ÀtqHÇÇÿÿÿÿfD  HÿÇ€<; u÷E3ÀH‹×H‹ËÿX3  I‹NH‹ĞH‹ğÿx2  H…ÀtH‹ÎÿÒ2  ëIH‹×H‹Ëÿô1  I‹NH‹ÖL‹ÀÿÌ2  I‹H‹ÖèA  I‰ëM‹NL¡<  º   L‰|$ 3Éÿ3  L‹ÅHL$`º  IÿÇÿù0  H…À…@şÿÿH‹¼$ˆ  H‹´$¸  H‹Íÿ·0  L‹¼$€  H‹œ$¨  H‹¬$°  H‹Œ$p  H3Ìèÿ  HÄ  A^ÃÌÌÌÌÌH‰\$WHƒì H‹H‹ùH…ÛtAH„u  H;Øt5H‹KH…ÉtE3ÀHl  ÿŞ0  H‹Kÿô0  H‹ÿË1  H‹ËÿÂ1  H‹Oÿ¸1  H‹OÿÎ0  H‹ÏH‹\$0HƒÄ _Hÿ%š1  ÌÌH‰\$ UVWHƒì`H‹ån  H3ÄH‰D$PI‹ğH‹êH‹ÙèüÿÿE3ÀH‹ÖH‹ÍÿÈ1  H‹KH‹ĞH‹øÿè0  H…ÀtH‹ÏÿB1  ë*H‹ÖH‹Íÿd0  H‹KH‹×L‹Àÿ<1  H‹H‹×è±  H‰H‹KH…É„¨   HÖ:  ÿô/  H‹øH…À„   H‹KHT$ ÿÉ/  …Àu	H‹L$@H‰KºÿÿÿÿH‹ÏDBÿ2/  …Àu-H‹Ïÿ5/  3ÒH‹Ï‹ØDBÿ/  ƒû
tH‹×¹
   ÿ	/  L‹ÏL‹Æº   H‹Íÿ/  H;ÆuH‹×¹
   ÿâ.  H‹ÏÿÉ.  H‹L$PH3Ìè,  H‹œ$˜   HƒÄ`_^]ÃÌÌÌÌÌÌÌÌÌÌÌÌ@VATAVHì   H‹„m  H3ÄH‰D$`M‹àH‹òL‹ñè&  ƒø„3  H‰œ$È   I‹ÎH‰¼$€   è…úÿÿE3ÀI‹ÔH‹ÎÿF0  I‹NH‹ĞH‹øÿŞ.  …ÀtDI‹H‹×è·  I‹H…Ût1Hƒ{ u*Hƒ; u$H+s  H;Øt3Éÿ–/  H‹Ëÿ/  IÇ    H‹Ïÿ}/  I‹NH…É„  E3ÉLD$(HT$ ÿÍ.  …À„s  I‹NHz9  H‰¬$ˆ   ÿ4.  H‹èH…À„;  L‰l$xI‹ÔH‹ÎL‰|$pÿJ.  H‹L$ L‹èÿ/  =ÿş  u1H‹L$ H‹¹-  D¶	L‹º   O¾<L‹ÍLùM‹ÇL+Áÿf-  ëL‹|$ I‹ÕI‹ÏI‹ßÿ°,  H‹øH…ÀtmI4<I;ÿt¶Gÿ<
t<u&H‹L$(HL$ H;ñ„Ø   ¶<
„Í   <„Å   L‹ÇL‹ÍL+Ãº   IÿÀH‹Ëÿó,  H_I‹ÕH‹ËÿC,  H‹øH…Àu“L‹D$ L‹ÍL+Ãº   LD$(H‹Ëÿ½,  I‹Íÿ$.  I‹NHT$0ÿı,  L‹|$pL‹l$x…Àu	H‹D$PI‰FH‹Íÿ],  H‹L$ ÿê-  H‹¬$ˆ   H‹œ$È   H‹¼$€   H‹L$`H3Ìè  HÄ   A^A\^ÃH+ûL‹ÍL‹Çº   H‹Ëÿ1,  H‹ŞfD  ¶<
t<…*ÿÿÿHÿÃëìÌÌÌÌÌÌÌÌÌÌÌÌH‰\$H‰l$VWAVHƒì I‹ÀL‹ÊL‹ñ3íH‹Ğ‰l$@I‹ÉE3Àÿ°-  H‹øHÇÃÿÿÿÿfD  HÿÃ@8,u÷ºH   J¹ÿq,  HKº   H‹ğH‰(ÿ-  H‹ÈH‰FL‹Çff„     A¶ M@ˆHI„ÀuîH‹Ïÿİ,  HÿÃH‰nH‹Ëº   ÿØ,  H‰FH‹Ö@ˆ(Hß  H‰F8HD$@H‰F@H‰^ H‰n(‰n0I‹è  H‹Nÿ,  H‹Nÿƒ,  H‹Îÿz,  9l$@H‹\$H@•Å‹ÅH‹l$PHƒÄ A^_^ÃÌÌÌÌÌ@SHƒì H‹ÙH…É„~   €9 tyH‰t$0H4H‰|$83ÿH;Îs@@ H‹Ëÿ,  ‹ÈÿŸ,  ƒèt8ƒèt3ƒøu‹øH‹¿*  ¶H‹H¾
HØH;ŞrÄH‹t$0‹ÇH‹|$8HƒÄ [ÃH‹t$03ÀH‹|$8HƒÄ [ÃLù5  3ÉH 6  ÿê+  3ÀHƒÄ [ÃÌÌH‰t$WHƒì H‹òH‹ùH…É„±   €9 „¨   H‰\$0ÿs+  ‹È‹Øÿù+  ƒÀøƒøwy‹Ëÿ,  ;Øum¶H‹*  H‹H¾
HßHşH;ßs;„     H‹Ëÿ'+  ‹Èÿ¯+  ƒÀøƒøv/H‹Ø)  ¶H‹H¾
HØH;ßrÍH‹\$0¸   H‹t$8HƒÄ _ÃH‹\$03ÀH‹t$8HƒÄ _ÃL5  3ÉH.5  ÿ +  H‹t$83ÀHƒÄ _ÃÌÌÌ@VHƒì0H‹h  H3ÄH‰D$(ÿu)  H‹ÈH‹ğÿ‰*  ‹Èÿ)+  ‹ÈHT$ ÿ\)  HcÈHƒùszH‹,)  ÆD  ¶H‰\$PH‹H‰|$XH¾
HÇÂÿÿÿÿHÎÿš)  E3ÀHL$ H‹ĞH‹øÿş(  H‹ÎH‹Øÿ"*  H‹Ïÿ*  H‹|$XH‹ÃH‹\$PH‹L$(H3Ìè×  HƒÄ0^Ãèø  ÌÌÌÌÌÌÌÌÌÌÌÌH‰\$H‰l$H‰t$H‰|$ ATAVAWHƒì I‹èL‹òL‹áèTôÿÿL‹ÅI‹ÖI‹Ìè6üÿÿ…À…¢   H‹ÕI‹ÎE3ÿèğıÿÿ…ÀuH‹ÕI‹Îè1ıÿÿD‹ø…ÀtwH‹ÕI‹ÎÿÎ(  HÇÇÿÿÿÿH‹ğL‹ÇIÿÀB€<  uöH‹ÖI‹ÌèÛûÿÿH‹Î‹ØÿH)  …Ûu@E…ÿt4H‹ÕI‹Îè|şÿÿH‹ğHÿÇ€<8 u÷L‹ÇH‹ÖI‹Ìè¢ûÿÿH‹Î‹Øÿ)  …Ûu¸   ë3ÀH‹\$@H‹l$HH‹t$PH‹|$XHƒÄ A_A^A\ÃÌÌÌÌÌÌÌÌÌÌÌ@SHƒì H‹ÚÿÉ(  H‹C@ÿ HƒÄ [ÃÌÌÌÌÌH‰\$H‰l$H‰t$ H‰L$WATAUAVAWHƒì@M‹ñM‹àL‹êA¿   M…ÉtoE3ÀI‹ÔI‹ÍÿÜ(  H‹ÈHÇÂÿÿÿÿH‹èÿÉ'  I‹‹ğH…Ét8E3ÀIPÿÿ²(  H‹ĞH‹ÍH‹Øèô  H‹Ë‹øÿ1(  I‹NMv;şL÷H…ÉuÈH‹Íÿ(  ëA‹÷L‹t$pA;÷I‹ÎDLşè•òÿÿº   Jÿÿ'  º   H‰D$ Jÿì'  3öE3ÀI‹ÔH‰D$(I‹ÍH‰t$0ÿ)(  H‹øHÇÃÿÿÿÿHÿÃ@84u÷H‹ÓH‹Ïÿú&  H‹ÏH‹èÿ–'  ºH   J¹ÿØ&  IcÏº   HÙH‹ø‰0D‰xHKÿz'  H‰GH‹Õfff„     ¶
HRˆH@„ÉuïH‹Íÿ>'  HÿÃH‰wH‹Ëº   ÿ9'  H‰GH‹×@ˆ0H@  H‰G8HD$ H‰G@H‰_ H‰w(ÇG0   I‹èÜ  H‹Oÿê&  H‹Oÿà&  H‹Ïÿ×&  H‹L$(ÿÌ&  H‹L$ I‹ÔH‹D$0H‰4ÁH‹Œ$   H‹D$0H‰I‹Íèûúÿÿ…Àt	H-àûÿÿëI‹ÔI‹Íè3úÿÿ…ÀH‹îHE-V%  H9t$0vwH‹T$ I‹NH‹òÿü%  H‹øHÇÃÿÿÿÿfHÿÃ€< u÷H…ítH‹ÓH‹Ïèçùÿÿ…Àu
H‹ÓH‹ÏÿÕëH‹ÓH‹ÏÿU%  H‹L$ H‹ØH‹ñÿ&  H‹D$ H‰ğHÿÆH;t$0r‰H‹D$ L\$@I‹[8I‹k@I‹sHI‹ãA_A^A]A\_ÃÌÌÌÌÌH‰l$WAVAWHƒì D‹L‹úH‹éL‹r@D;R}D‰RM‹^3ÿM…ÛtaM‹NE9:I‹H‹ÅL‹ùL+Åff„     ¶B¶ +ÑuHÿÀ…Éuí…ÒtHÿÇIƒÁI;ûrÁHƒÿrH‹ÍH‹l$PHƒÄ A_A^_Hÿ%G%  H‰\$@H‹ßH‰t$H¾   I;ûs „     I‹H‹Ùÿ%  ÿÎHÿÃI;^rèI‹H‹\$@H‰,øA‹I‹NH‹l$P‰¹HcÆIFH‹t$HHƒÄ A_A^_ÃÌÌÌÌÌÌÌH…ÉtNSHƒì HOh  H‹ÙH;Èt5H‹IH…ÉtE3ÀH4   ÿ¦#  H‹Kÿ¼#  H‹ÿ“$  H‹ËÿŠ$  HƒÄ [ÃÌÌÌÌÌÌÌÌÌÌÌÌH‹Êé˜ÿÿÿÌÌÌÌÌÌÌÌH‰t$AVHƒì L‹òH‹ñH…ÉuQNÿV$  H‹ğHƒ> H‰\$0…Ì   Hƒ~ u I‹Îÿ¬#  H‰H‹ÆH‹\$0H‹t$@HƒÄ A^ÃA¶„Àu5Hı,  ÿ€#  H‹NLug  H‹Ğÿü#  H‹ÆH‹\$0H‹t$@HƒÄ A^ÃH‹ĞH‰|$8H‹"  H‹H¾
I‹ÎH‹Óÿİ"  H‹NH‹ĞH‹øÿ5#  H‹ÈJ3è)ÿÿÿH‹NL‹ÀH‹×ÿ™#  H‹|$8H‹ÆH‹\$0H‹t$@HƒÄ A^ÃL‹a#  E3ÉH‹÷"  H‹#  ÿz#  H‹H‹ÎH‹ÓH‰FHÇ    èÉşÿÿI‹ÖH‹Îè¾şÿÿH‹Ëÿ#  H‹\$0H‹ÆH‹t$@HƒÄ A^ÃÌH…É„¬  VWHƒì(H‹ùH‹òH‹	H…É…•  H‹OH…É„  ¶H‰\$@H‰l$HL‰|$ L=If  „ÀuHµ+  ÿÀ!  é¤   H‹ĞL‰t$PH‹\!  H‹H¾
H‹ÎH‹Óÿ¸!  H‹OH‹ĞH‹èÿ"  H‹ÈH3L‹ğèaÿÿÿIƒ~ uMIƒ> uGH‹OH‹Õÿ_!  M;÷t5I‹NH…ÉtE3ÀH¿ıÿÿÿ1!  I‹NÿG!  I‹ÿ"  I‹Îÿ"  H‹Íÿ"  L‹t$PH‹OÿÍ!  ƒø…Œ   H‹Oÿ²   H‹OH‹ğH‹(H‹Õÿo!  H‹ØH‹H…Òt[E3ÀH‹Íÿ   H‰I;ßt5H‹KH…ÉtE3ÀH-ıÿÿÿŸ   H‹Kÿµ   H‹ÿŒ!  H‹Ëÿƒ!  H‹Oÿ™   HÇG    H‹ÎÿÀ!  H‹l$HH‹\$@L‹|$ HƒÄ(_^ÃH‹ÁH+ñ@ D¶ ¶0D+ÂuHÿÀ…ÒuìE…ÀuÖÿ)!  HÇ    HƒÄ(_^ÃÌÌÌ@SAVHƒì(H‹ÚL‹ñH…Ò„Y  H…É„ï   ‹JH‰|$P‹:;ùØ   Hgd  H‰t$HL;ğuQH‹BHÇÆÿÿÿÿfHÿÆ€<0 u÷H‹BH;ğ~+ğ>‰;ÁH‹Jÿ)   H‹ÈH‹ÓÿS8H‹t$H‰;H‹|$PHƒÄ(A^[ÃH‰l$@I‹.H…ítvƒz0uHÇÂÿÿÿÿH‹Íÿ¹  H‹èH‹SH‹ÍHSè  ƒ{08‰uH‹Íÿ:   ‹;KI‹E3ÀH‹Kÿó  H‹ÈH‹ÓÿS8‰;H‹l$@H‹t$HH‹|$PHƒÄ(A^[ÃL‹JH‹JH‹²  IÉL‰d$XH‹D¶M¾$MáI‹ÔI+Ñÿÿ  I‹NH‹èH…É„Ë   H…À„Â   H‹ĞL‰|$ ÿ@  L‹øHÇÆÿÿÿÿH…Àu:ƒ{0…–   H‹ÖH‹Íÿ[  H‹ÍH‹øÿw  I‹NH‹×H‹ïÿÿ  L‹øH…ÀtfH‹ÕH‹Ëè\  H‹{H‹ÓI‹ÏL‰cè)şÿÿH‹K(H‰{H…Éx%HÿÆ€<. u÷HcÆH+ÈH‰K(y3ÉH‰K(H‹CÆ ëL¨)  3ÉH·)  ÿ1  L‹|$ H‹Íÿë  H‹CH‹{ÿ€< tH‹ÓL‰cI‹Îè´ıÿÿH‰{I‹NH5   L‹Ãÿ´  ÿL‹d$XéşÿÿL)  3ÉH)  HƒÄ(A^[Hÿ%Â  ÌÌ@SWAUAVHƒì(M‹XL‹êI‹PI‹øH‹(  IÓL‹ñD¶L‹K¾
IÛL‹ÃM+Ãÿï  …À„4  H‰l$XI‹ÖH‰t$`H‹ÏL‰|$ è-  H‹×I‹ÍèıÿÿH‹oH‹×I‹ÍH‰l$PH‰_èêüÿÿH‹O(E3ÿHÇÆÿÿÿÿH…Éx,H‹Æ@ HÿÀE8<u÷H˜H+ÈH‰O(yL‰(I‹ÏH‹GDˆ<ëL\(  3ÉHk(  ÿå  H‹OH+İHÍH‹ÓÿÊ  I‹MH‹èH…É„^  H…À„U  H‹Ğÿ  H‹ØH…Àu>ƒ0…:  H‹ÖH‹Íÿ2  H‹ÍH‹ØÿN  I‹MH‹ÓH‹ëÿÖ  H‹ØH…À„  L‹OH‹WH‹ã  IÑL‰d$hD¶H‹M¾$I‹ÎMáM‹ÄM+Áÿ¥  …À…Ä   I‹ÖL‰gH‹Ïèî   H‹ÕH‹Ïèã   H‹×H‹Ëè¸ûÿÿH‹O(H…Éx(H‹ÆHÿÀD8<(u÷H˜H+ÈH‰O(yL‰(I‹ÏH‹GDˆ<ëL8'  3ÉHG'  ÿÁ  H‹O(H…Éx9„     HÿÆE8<6u÷HcÆH+ÈH‰O(yH‹GL‰(AÆ ë#H‹GL‹ùÆ ëLà&  3ÉHï&  ÿi  L‹d$hH‹Íÿ#  H‹D$PL‹|$ H‹t$`H‹l$XH‰GHƒÄ(A^A]_[ÃÌÌÌÌÌÌÌÌH‰\$H‰t$WHƒì H‹òH‹ùHÇÃÿÿÿÿ@ HÿÃ€< u÷H‹I(H‹W HH;Â|#H‹OHƒÂ
HÓA¸   H‰W ÿ  H‹O(H‰GE3ÀH…Û~*fff„     H‹WA¶0IĞH‹O(IÿÀˆ
L;Ã|åH‹O(H‹GHËH‹\$0H‹t$8H‰O(Æ HƒÄ _ÃÌÌ@SWATAUHƒì8H‹ÚLD$xºÿÿÿÿÿ  LD$pºÿÿÿÿH‹ËL‹àÿ  ‹T$pL‹è‹L$xÿÂÿÁ¯ÑHcÊº   ÿ  D‹L$x3ÒH‹øAI…É~(ff„     ‹L$pÿÁ¯ÊHcÉ‰ÿÂD‹L$xAA;Ğ|â‹T$p3ÉB…À~L‹ÇfA‰M@‹T$pÿÁB;È|ìD‹L$xA»   AAA;Ã	  H‰l$`DBH‰t$0Il$üL‰t$(L‰|$ €    Aº   E‹òE;Â«   I]üff„     ‹UAKşD‹KA3öA;Ñ@•ÆA¯ÀAÂLcøB‹¿ÿÀAƒûv%Iƒş~;uD9M uA¯ÈAÊHcÉ‹LøÎ;ÁMÁB‹T¿üE¯ÃÖEÂMcÈF‹DüAÿÀA;ÀDLÀD;ÂALĞAÿÂB‰IÿÆ‹T$pHƒÃDBE;ĞŒhÿÿÿD‹L$xAÿÃAAHƒÅD;ØŒ/ÿÿÿL‹|$ L‹t$(H‹t$0H‹l$`BI‹ÌA¯ÁÂH˜‹‡ÿ\  I‹ÍÿS  H‹ÏÿJ  ‹ÃHƒÄ8A]A\_[ÃÌÌÌÌÌÌÌÌÌÌÌff„     H;V  òuHÁÁf÷ÁÿÿòuòÃHÁÉé7   ÌÌÌ@SHƒì H‹Ù3Éÿ[  H‹ËÿZ  ÿD  H‹Èº	 ÀHƒÄ [Hÿ%(  H‰L$Hƒì8¹   èÇ  …Àt¹   Í)HãV  èÊ  H‹D$8H‰ÊW  HD$8HƒÀH‰ZW  H‹³W  H‰$V  H‹D$@H‰(W  ÇşU  	 ÀÇøU     ÇV     ¸   HkÀ HúU  HÇ   ¸   HkÀ H‹ŠU  H‰L ¸   HkÀH‹mU  H‰L Ha  è ÿÿÿHƒÄ8ÃÌÌÌHƒì(¹   è   HƒÄ(ÃÌ‰L$Hƒì(¹   èà  …Àt‹D$0‹ÈÍ)HûU  èr   H‹D$(H‰âV  HD$(HƒÀH‰rV  H‹ËV  H‰<U  Ç"U  	 ÀÇU     Ç&U     ¸   HkÀ HU  ‹T$0H‰H¯  èNşÿÿHƒÄ(ÃÌH‰\$ WHƒì@H‹ÙÿÁ  H‹»ø   HT$PH‹ÏE3Àÿ¡  H…Àt2Hƒd$8 HL$XH‹T$PL‹ÈH‰L$0L‹ÇHL$`H‰L$(3ÉH‰\$ ÿb  H‹\$hHƒÄ@_ÃÌÌÌ@SVWHƒì@H‹ÙÿS  H‹³ø   3ÿE3ÀHT$`H‹Îÿ1  H…Àt9Hƒd$8 HL$hH‹T$`L‹ÈH‰L$0L‹ÆHL$pH‰L$(3ÉH‰\$ ÿò  ÿÇƒÿ|±HƒÄ@_^[ÃÌÌÌHƒì(…Òt9ƒêt(ƒêtƒút
¸   HƒÄ(Ãè  ëèÓ  ¶ÀHƒÄ(ÃI‹ĞHƒÄ(é   M…À•ÁHƒÄ(é  H‰\$H‰t$H‰|$ AVHƒì H‹òL‹ñ3Éèr  „À„È   èù  ŠØˆD$@@·ƒ=Y   …Å   ÇY     èD  „ÀtOèS  è~  è¥  HÚ  HË  è~
  …Àu)èá  „Àt Hª  H›  èX
  Ç¬X     @2ÿŠËèV  @„ÿu?èœ  H‹ØHƒ8 t$H‹Èè£  „ÀtL‹Æº   I‹ÎH‹L‹6  AÿÑÿ5X  ¸   ë3ÀH‹\$0H‹t$8H‹|$HHƒÄ A^Ã¹   èP  ÌÌÌH‰\$WHƒì0@Šù‹õW  …À3ÀH‹\$@HƒÄ0_ÃÿÈ‰ÜW  èß  ŠØˆD$ ƒ=úW  u7èó  è  è…  ƒ%âW   ŠËè  3Ò@ŠÏè©  öØÛƒãèõ  ‹Ãë¢¹   èË  ÌH‹ÄH‰X L‰@‰PH‰HVWAVHƒì@I‹ğ‹úL‹ñ…Òu9XW  3Àéğ   BÿƒøwEH‹   H…Àu
ÇD$0   ëÿ#  ‹Ø‰D$0…À„´   L‹Æ‹×I‹Îè ıÿÿ‹Ø‰D$0…À„™   L‹Æ‹×I‹Îè‘  ‹Ø‰D$0ƒÿu8…Àu}L‹Æ3ÒI‹Îèu  H…ö•ÁèÆşÿÿH‹'  H…ÀtYL‹Æ3ÒI‹Îÿ¬  ëI…ÿtƒÿu@L‹Æ‹×I‹Îè,ıÿÿ‹Ø‰D$0…Àt)H‹ë  H…Àu	X‰\$0ëL‹Æ‹×I‹Îÿg  ‹Ø‰D$0ë3Û‰\$0‹ÃH‹\$xHƒÄ@A^_^ÃÌH‰\$H‰t$WHƒì I‹ø‹ÚH‹ñƒúuè   L‹Ç‹ÓH‹ÎH‹\$0H‹t$8HƒÄ _éşÿÿÌÌÌH‰\$ UH‹ìHƒì H‹TP  H»2¢ß-™+  H;ÃutHƒe HMÿ†  H‹EH‰Eÿp  ‹ÀH1Eÿ\  ‹ÀHM H1EÿD  ‹E HMHÁà H3E H3EH3ÁH¹ÿÿÿÿÿÿ  H#ÁH¹3¢ß-™+  H;ÃHDÁH‰ÑO  H‹\$HH÷ĞH‰ºO  HƒÄ ]ÃHƒì(ƒúuHƒ=³   uÿû  ¸   HƒÄ(ÃÌHIU  Hÿ%ê  ÌÌH9U  éê  H=U  ÃH=U  ÃHƒì(èçÿÿÿHƒ$èæÿÿÿHƒHƒÄ(ÃÌHƒì(è£  …Àt!eH‹%0   H‹HëH;Èt3ÀğH±U  uî2ÀHƒÄ(Ã°ë÷ÌÌÌHƒì(èg  …Àtè²  ëèO  ‹Èè€  …Àt2Àëèy  °HƒÄ(ÃHƒì(3Éè=  „À•ÀHƒÄ(ÃÌÌÌHƒì(èo  „Àu2Àëèb  „ÀuèY  ëì°HƒÄ(ÃHƒì(èG  èB  °HƒÄ(ÃÌÌÌH‰\$H‰l$H‰t$WHƒì I‹ùI‹ğ‹ÚH‹éèÀ  …ÀuƒûuL‹Æ3ÒH‹ÍH‹Çÿê  H‹T$X‹L$PH‹\$0H‹l$8H‹t$@HƒÄ _é®  Hƒì(è{  …ÀtHT  HƒÄ(é©  èº  …Àuè¡  HƒÄ(ÃHƒì(3Éè  HƒÄ(é”  @SHƒì ¶¿S  …É»   DÃˆ¯S  èr  èm  „Àu2Àëè`  „Àu	3ÉèU  ëêŠÃHƒÄ [ÃÌÌÌ@SHƒì €=tS   ‹ÙugƒùwjèÙ  …Àt(…Ûu$H^S  è  …ÀuHfS  èñ  …Àt.2Àë3foa  HƒÈÿó-S  H‰6S  ó6S  H‰?S  Æ	S  °HƒÄ [Ã¹   èú   ÌÌHƒìL‹Á¸MZ  f9ñœÿÿuvHc$ÿÿHáœÿÿHÊ9PE  u]¸  f9AuRL+Â·AHQHĞ·AH€LÊH‰$I;Ñt+‹JL;Ár‹BÁL;ÀsH…Òtƒz$ }2Àë°ëHƒÂ(ëÌ2Àë2Àë2ÀHƒÄÃÌÌ@SHƒì ŠÙèÃ  3Ò…Àt„ÛuH‡6R  HƒÄ [Ã@SHƒì €=+R   ŠÙt„Òuèæ  ŠËèß  °HƒÄ [ÃÌÌÌHUR  Ãƒ%5R   ÃH‰\$UH¬$@ûÿÿHìÀ  ‹Ù¹   è  …Àt‹ËÍ)¹   èÅÿÿÿ3ÒHMğA¸Ğ  èD  HMğÿî  H‹è   H•Ø  H‹ËE3ÀÿÌ  H…Àt<Hƒd$8 Hà  H‹•Ø  L‹ÈH‰L$0L‹ÃHè  H‰L$(HMğH‰L$ 3Éÿƒ  H‹…È  HL$PH‰…è   3ÒH…È  A¸˜   HƒÀH‰…ˆ   è­  H‹…È  H‰D$`ÇD$P  @ÇD$T   ÿ  ƒøHD$PH‰D$@HEğ”ÃH‰D$H3Éÿş
  HL$@ÿû
  …Àu„ÛuHè¿şÿÿH‹œ$Ğ  HÄÀ  ]ÃÌÌH‰\$WHƒì H  H=ˆ  ëH‹H…Àtÿ@  HƒÃH;ßréH‹\$0HƒÄ _ÃH‰\$WHƒì Hc  H=\  ëH‹H…Àtÿ  HƒÃH;ßréH‹\$0HƒÄ _ÃH‰\$H‰t$WHƒì3À3É¢D‹ÁE3ÛD‹ËAğntelAñGenuD‹Ò‹ğ3ÉACEÈ¢AòineI‰$EÊ‰\$‹ù‰L$‰T$uPHƒJ  ÿ%ğ?ÿ=À t(=` t!=p t°ùüÿƒø w$H¹     H£ÁsD‹ØO  AƒÈD‰ÍO  ëD‹ÄO  ¸   DHû;ğ|&3É¢‰$D‹Û‰\$‰L$‰T$ºã	s
EÁD‰‘O  ÇsI     D‰pI  ºçƒ‘   D‰[I  »   ‰TI  ºçsyºçss3ÉĞHÁâ HĞH‰T$ H‹D$ "Ã:ÃuW‹&I  ƒÈÇI     ‰I  AöÃ t8ƒÈ ÇüH     ‰úH  ¸  ĞD#ØD;ØuH‹D$ $à<àuƒÛH  @‰ÑH  H‹\$(3ÀH‹t$0HƒÄ_ÃÌÌÌ¸   ÃÌÌ3À9ÄH  •ÀÃÿ%2	  ÿ%	  ÿ%	  ÿ%X	  ÿ%J	  ÿ%<	  ÿ%V	  ÿ%(	  ÿ%Z	  ÿ%<	  ÿ%F	  ÿ%À  °ÃÌ3ÀÃÌHƒì(M‹A8H‹ÊI‹Ñè   ¸   HƒÄ(ÃÌÌÌ@SE‹H‹ÚAƒãøL‹ÉAö L‹ÑtA‹@McP÷ØLÑHcÈL#ÑIcÃJ‹H‹C‹HH‹CöDt¶DƒàğLÈL3ÊI‹É[éEñÿÿÌÌÌÌÌÌÌÌÌÌÌff„     ÿà@UHƒì H‹êŠM@HƒÄ ]é,ûÿÿÌ@UHƒì H‹êŠM èûÿÿHƒÄ ]ÃÌ@UHƒì H‹êHƒÄ ]é{ùÿÿÌ@UHƒì0H‹êH‹‹H‰L$(‰T$ LŒóÿÿL‹Ep‹UhH‹M`è¼øÿÿHƒÄ0]ÃÌ@UH‹êH‹3É8  À”Á‹Á]ÃÌ                                                                                                                                                                                                                                                                                            rŸ      ^Ÿ      @Ÿ      $Ÿ      Ÿ      ö      â      Úœ      ¢Ÿ      ¼Ÿ      ÒŸ      èŸ                    4       †Ÿ              øœ      $            D                    `              ô      È      º      ®      2      Ú      J                    h      r      z      ‚      Š      œ      ¦              ’      T              °›      ¾›      Ö›      ä›      œ      œ      &œ      <œ      ˜›       ™      
™      ú˜      â˜      È˜      ¼˜      ¦˜      ”˜      †˜      Š›      x›      j›      Z›      D›      *›      ›      ›      öš      àš      Òš      ºš      ¨š      –š      ‚š      tš      `š      Rš      Bš      4š      (š      š      š      ş™      ğ™      Ú™      À™      ®™      –™      z™      l™      T™      D™      Fœ      8™      r˜      f˜      T˜      L˜      :˜      ,˜      ˜      ô›      ˜              ’œ      ‚œ      rœ      `œ      ¤œ      ºœ              `< €   `< €   Ph €   Ph €   Ph €                                                                   @° €   à° €                   ÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿENCHANT_CONFIG_DIR      enchant /usr/local/share/enchant        /usr/local/etc  %s.dic  %s.exc  dict    enchant_dict_get_error  enchant_dict_check  word    len g_utf8_validate(word, len, NULL)        enchant_dict_suggest    enchant_dict_add        enchant_dict_add_to_session     enchant_dict_is_added   enchant_dict_remove     enchant_dict_remove_from_session        enchant_dict_is_removed enchant_dict_store_replacement  mis cor mis_len cor_len g_utf8_validate(mis, mis_len, NULL)     g_utf8_validate(cor, cor_len, NULL)     enchant_dict_free_string_list   enchant_dict_describe   fn      Personal Wordlist       EnchantProvider cannot be NULL
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
        enchant_broker_request_pwl_dict pwl && strlen(pwl)      Couldn't open personal wordlist '%s'    enchant_broker_request_dict     tag && strlen(tag)      invalid tag character found     enchant_broker_describe enchant_broker_list_dicts       enchant_broker_free_dict        enchant_broker_dict_exists      enchant_dict_get_extra_word_characters  n <= 2  enchant_dict_is_word_character  '   â€™ enchant_broker_set_ordering     ordering && strlen(ordering)    enchant_broker_get_error        /usr/local  2.2.8       file != NULL    enchant_pwl_init_with_file  a+  Line too long (ignored) in %s at line:%zu
      Bad UTF-8 sequence in %s at line:%zu
   wb      word && *word   enchant_is_all_caps     enchant_is_title_case   matcher enchant_trie_find_matches       matcher->path_pos >= 0  enchant_trie_matcher_poppath    ÿş      0¶ €       ?¹^       R   0  0m      ?¹^          „  „m      ?¹^       @  ˜  ˜m      ?¹^                   0                                                                                      ° €                   ˜s €   ¨s €                                                                                                                                                           s €   °s €   ¸s €   RSDSØJ0é"6K²<K†2   D:\git\winlibs\enchant\msvc16\x64\Release\libenchant2.pdb                    GCTL   @X  .text$mn    @h     .text$mn$00 Rh  ’   .text$x  p  ˜  .idata$5    ˜s  (   .00cfg  Às     .CRT$XCA    Ès     .CRT$XCZ    Ğs     .CRT$XIA    Øs     .CRT$XIZ    às     .CRT$XPA    ès     .CRT$XPZ    ğs     .CRT$XTA    øs     .CRT$XTZ     t  0  .rdata  0  ¨  .rdata$zzzdbg   Ø     .rtc$IAA    à     .rtc$IZZ    è     .rtc$TAA    ğ     .rtc$TZZ    ø  ¨  .xdata       .edata  ¸“      .idata$2    X”     .idata$3    p”  ˜  .idata$4    ˜  @  .idata$6     °  @   .data   @°    .bss     À  h
  .pdata   Ğ      .rsrc$01     Ğ  `  .rsrc$02                                     b  
 
4 
2`! t `  İ   ‚  !   `  İ   ‚   d T
 2ğàÀ!
 
t	 4 @  ¶  0‚  !   @  ¶  0‚   T R
ğàp! d 4
 À  ì  l‚  !   À  ì  l‚   d T 4 2p B   T	 4 2p!
 
ä d €  ã  À‚  !   ä  d €  ã  À‚  !   €  ã  À‚   2àĞ	ÀP0! ô 
t d
   9  ƒ  !     9  ƒ   rÀP0!   ô Ô t d   )  Lƒ  ! ä )  f  Xƒ  !   )  f  Xƒ  !     )  Lƒ   Ò	Ğp`! T 4 `  Á  ¬ƒ  !! !ô
 ä Ä Á  Ğ  ¸ƒ  !   Á  Ğ  ¸ƒ  !   `  Á  ¬ƒ   T 4 2p! d    ÿ  „  !      ÿ  „   d	 T 2p!
 
ä 4 °    @„  !   °    @„   d 4 2p! d À    „  !   À    „   d	 4 2p! ä T Ğ  3   ¬„  !   Ğ  3   ¬„   T R	ğp`! 4
 !  D!  ä„  ! ä D!  º!  ô„  !   D!  º!  ô„  !   4
 !  D!  ä„  !   !  D!  ä„   2p! 4 €"  ’"  P…  !   €"  ’"  P…   bğP  ! ä t 
d 4
 ğ"  #  |…  !   ğ"  #  |…   20
 
rğĞÀ! t  $  Í$  À…  ! ä d T 4 Í$  ä$  Ì…  !   Í$  ä$  Ì…  !    $  Í$  À…  
 4 RğàĞ
Àp`P B`0  ! Ä
 €)  ‡)  8†  ! ô ‡)  ¾)  D†  ! T ¾)  â)  X†  !
 
ä t	 â)  ò)  l†  !   â)  ò)  l†  !   ¾)  â)  X†  !   ‡)  ¾)  D†  !   €)  ‡)  8†  ! 4 ğ*  ı*  ¸‚  ! t ı*  +  Ø†  !   ı*  +  Ø†  !   ğ*  ı*  ¸‚  ! 4 À+  É+  ¸‚  !   À+  É+  ¸‚  !
 
t 4 ,  ',  ¸‚  !   ,  ',  ¸‚  
 
T	 
2`!% %t  4 ä p-  ï-  l‡  !   ä p-  ï-  l‡  !   p-  ï-  l‡  @ @ä 54
 	RĞ`P! t /  ä/  ¸‡  ! ô ä/  "0  Ì‡  !   ä/  "0  Ì‡  !   /  ä/  ¸‡   d 4 2p! T 01  í1  ˆ  !   01  í1  ˆ  
 
2ğàP! t
 
d	 4 À2  3  Hˆ  !   À2  3  Hˆ    ğàp  !; ;d 3T Ä 4 °3  ş3  €ˆ  ! Ô ş3  G4  ˆ  !   ş3  G4  ˆ  !   °3  ş3  €ˆ  
 
4 
2p d 4 2p
 
4 
2p! ä d  8  f8   ‰  !    8  f8   ‰   t	 T 2à! 4 °:  Ò:  4‰  ! d Ò:  ';  D‰  !   Ò:  ';  D‰  !   d  4 °:  Ò:  4‰  !   4 °:  Ò:  4‰  !   °:  Ò:  4‰  ! 4 €<  «<  P…  !   €<  «<  P…   	R à  Àg  p  ! TV 4U @=  =  Ü‰  ! ôP =  L>  ğ‰  ! tQ dW L>  i>  Š  !   L>  i>  Š  !   =  L>  ğ‰  !   @=  =  Ü‰   4 ²p`PÀg  P     àÀ`  Àg  `   ! t 4 pB  £B  |Š  ! T £B  ^C  ”Š  ! ô Ô ^C  xC  ¬Š  !   ^C  xC  ¬Š  !   £B  ^C  ”Š  !   pB  £B  |Š  ! 
  ô  Ô  t  T  4 pB  £B  |Š   T
 4	 2àp`! t d PF  gF  ¸…  !   t  d PF  gF  ¸…  !   PF  gF  ¸…  
 
d 
2p! 4  G  "G  €‹  !   4  G  "G  €‹  !    G  "G  €‹   R`Àg  (   ! t 4
 ğG  DH  Ä‹  !   ğG  DH  Ä‹   t d
 T	 4 2ğàÀ! !d !T !4 !rğàĞÀp
 d	 –4 T
 2
ğàp
 
20. .4 d 2à! t @N  ÛN  TŒ  !   @N  ÛN  TŒ   Bp
`  ! ô 
T	 4  O  ÑO  ˆŒ  ! ä
 ÑO   P  ”Œ  !   ÑO   P  ”Œ  !    O  ÑO  ˆŒ   Bà0  ! d	 t
 Q  ³Q  äŒ  ! T  t
  d	 Q  ³Q  äŒ  !   t
 Q  ³Q  äŒ  !   Q  ³Q  äŒ  !   t
  d	  T Q  ³Q  äŒ  ! Ä §R  ¹R  H  ! ô ¹R  õR  d  !   ¹R  õR  d   BàĞp0  ! ô d T  T  iT  œ  ! Ä iT  ˆU  ¬  !   iT  ˆU  ¬  !    T  iT  œ   bĞÀp0  ! ô ä d T `W  ,X  ü  !   `W  ,X  ü          B  	 	b  
 
4 
rp rp`0 t	 d 4 2àpg     È\  7]  Rh      š]  ¥]  Rh       2P
 
4 
Rppg     ß]  ş]  ih      Ô]  ^  ‚h      ^  *^  ih      ^  +^  ‚h      	 4 ràp`pg     a^  I_  –h  I_   RP 4	 2P	 "  pg     c  ‹c  Ìh  ‹c   P   4º ¸ P   d 4 p    0                  ÿÿÿÿ    à           È  8  ¨  À2   8  Ğ,  p6   <  ,  °3  01  p-  °:     °  €  ğ"  €"  @   9  à  Ğ  ğ9  À  Ğ  !  `  P<     p<  `<  ğ  ‘  #‘  7‘  P‘  i‘  }‘  —‘  ³‘  Ó‘  ï‘   ’  ’  /’  E’  c’  z’  ¡’  ·’  Ï’  î’  “  #“  B“  W“  n“  Š“  “            	 
                  libenchant2.dll enchant_broker_describe enchant_broker_dict_exists enchant_broker_free enchant_broker_free_dict enchant_broker_get_error enchant_broker_init enchant_broker_list_dicts enchant_broker_request_dict enchant_broker_request_pwl_dict enchant_broker_set_ordering enchant_dict_add enchant_dict_add_to_session enchant_dict_check enchant_dict_describe enchant_dict_free_string_list enchant_dict_get_error enchant_dict_get_extra_word_characters enchant_dict_is_added enchant_dict_is_removed enchant_dict_is_word_character enchant_dict_remove enchant_dict_remove_from_session enchant_dict_store_replacement enchant_dict_suggest enchant_get_prefix_dir enchant_get_user_config_dir enchant_get_version enchant_set_prefix_dir    Ø•          Tœ  hq  Ğ—          Ìœ  `s  p”          êœ   p  ø”          N  ˆp  (•          ^  ¸p  €•          ~  q  À•            Pq  8•          À  Èp                          rŸ      ^Ÿ      @Ÿ      $Ÿ      Ÿ      ö      â      Úœ      ¢Ÿ      ¼Ÿ      ÒŸ      èŸ                    4       †Ÿ              øœ      $            D                    `              ô      È      º      ®      2      Ú      J                    h      r      z      ‚      Š      œ      ¦              ’      T              °›      ¾›      Ö›      ä›      œ      œ      &œ      <œ      ˜›       ™      
™      ú˜      â˜      È˜      ¼˜      ¦˜      ”˜      †˜      Š›      x›      j›      Z›      D›      *›      ›      ›      öš      àš      Òš      ºš      ¨š      –š      ‚š      tš      `š      Rš      Bš      4š      (š      š      š      ş™      ğ™      Ú™      À™      ®™      –™      z™      l™      T™      D™      Fœ      8™      r˜      f˜      T˜      L˜      :˜      ,˜      ˜      ô›      ˜              ’œ      ‚œ      rœ      `œ      ¤œ      ºœ              :g_slist_index 9g_slist_free_full Vg_dir_close ‘g_unichar_type  wg_log ¼g_utf8_validate £g_getenv  ¬g_utf8_normalize  g_strchomp  Zg_dir_read_name g_io_channel_unref  Ùg_strndup ²g_hash_table_iter_next  §g_hash_table_destroy  7g_slist_free  »g_hash_table_remove g_get_user_config_dir g_strchug " g_ascii_table g_io_channel_new_file –g_str_hash  Õg_return_if_fail_warning  ¹g_hash_table_new_full # g_ascii_tolower ±g_hash_table_iter_init  ¯g_hash_table_insert ²g_malloc0_n ßg_strsplit  ‡g_free  ©g_utf8_get_char Óg_remove  ªg_strfreev  4g_slist_find  Xg_dir_open  ¾g_hash_table_size ug_file_test  g_build_filename  ¥g_strdup_printf /g_slist_append  ëg_mkdir_with_parents  “g_str_equal ¶g_hash_table_lookup g_filename_to_utf8  ¤g_strdup  $ g_ascii_toupper g_io_channel_read_line  og_file_get_contents ³g_utf8_strlen ©g_realloc_n ²g_utf8_strdown  ³g_malloc_n  ©g_hash_table_foreach  °g_utf8_skip ¬g_hash_table_get_keys ¡g_strconcat ·g_utf8_strup  g_unichar_totitle g_unichar_to_utf8 {g_stat  ºg_utf8_to_ucs4_fast ‚g_fopen ]g_list_free glib-2.dll  	 g_module_symbol  g_module_name  g_module_open  g_module_close   g_module_supported   g_module_error  gmodule-2.dll #SetErrorMode  KERNEL32.dll  @ strchr  B strstr   __C_specific_handler  % __std_type_info_destroy_list  > memset  VCRUNTIME140.dll   free  t fclose  ‡ fseek ‘ putc  ‹ getc  z fgets  strncmp Š fwrite  u feof  6 _initterm 7 _initterm_e ? _seh_filter_dll  _configure_narrow_argv  3 _initialize_narrow_environment  4 _initialize_onexit_table  " _execute_onexit_table  _cexit  ) _strdup api-ms-win-crt-heap-l1-1-0.dll  api-ms-win-crt-stdio-l1-1-0.dll api-ms-win-crt-string-l1-1-0.dll  api-ms-win-crt-runtime-l1-1-0.dll ÓRtlCaptureContext ÚRtlLookupFunctionEntry  áRtlVirtualUnwind  ¼UnhandledExceptionFilter  {SetUnhandledExceptionFilter GetCurrentProcess šTerminateProcess  ‰IsProcessorFeaturePresent PQueryPerformanceCounter GetCurrentProcessId "GetCurrentThreadId  ğGetSystemTimeAsFileTime "DisableThreadLibraryCalls lInitializeSListHead ‚IsDebuggerPresent                                                                                                                                                                                                                                                                                                                                                                                                                                                         Í] ÒfÔÿÿ2¢ß-™+  ÿÿÿÿ          /        ø                                                                                                                                                                                                                                                                                                                                                                                                                                                                                        U  ø  `  İ   ‚  İ    ‚    4   ‚  @  ¶  0‚  ¶  œ  D‚  œ  ±  \‚  À  ì  l‚  ì  ™  |‚  ™  ª  ”‚  °  <  ¤‚  @  w  ¸‚  €  ã  À‚  ã  ”  Ğ‚  ”  ¾  è‚  ¾     ƒ    9  ƒ  9  ù   ƒ  ù    <ƒ    )  Lƒ  )  f  Xƒ  f  0  xƒ  0  L  Œƒ  L  X  œƒ  `  Á  ¬ƒ  Á  Ğ  ¸ƒ  Ğ  B  Ğƒ  B  ^  ìƒ  ^  š  üƒ     ÿ  „  ÿ  K  „  K  §  0„  °    @„    y  P„  y  Õ  h„  à  ²  x„  À    „    k  ˆ„  k  Ç  œ„  Ğ  Å  x„  Ğ  3   ¬„  3   Å   ¼„  Å   !  Ô„  !  D!  ä„  D!  º!  ô„  º!  è!  …  è!  "  …  "  A"  ,…  A"  w"  @…  €"  ’"  P…  ’"  Æ"  X…  Æ"  â"  l…  ğ"  #  |…  #  ¨#  ˆ…  ¨#  Ï#  ¨…  Ğ#   $  ¸…   $  Í$  À…  Í$  ä$  Ì…  ä$  Í&  à…  Í&  Û&   †  Û&  æ&  †  ğ&  €)   †  €)  ‡)  8†  ‡)  ¾)  D†  ¾)  â)  X†  â)  ò)  l†  ò)  Š*  €†  Š*  ˜*  ˜†  ˜*  *  ¨†  *  °*  ¸†  °*  ã*  È†  ğ*  ı*  ¸‚  ı*  +  Ø†  +  ‰+  ì†  ‰+  ¢+   ‡  ¢+  ½+  ‡  À+  É+  ¸‚  É+  è+   ‡  è+  ,  4‡  ,  ',  ¸‚  ',  ¦,  D‡  ¦,  Ã,  \‡  Ğ,  i-  ¸…  p-  ï-  l‡  ï-  :/  x‡  :/  `/  ”‡  `/  Œ/  ¨‡  /  ä/  ¸‡  ä/  "0  Ì‡  "0  1  à‡  1  1  ô‡  1  /1  ˆ  01  í1  ˆ  í1  G2  $ˆ  G2  ¸2  8ˆ  À2  3  Hˆ  3  r3  Tˆ  r3  ¥3  pˆ  °3  ş3  €ˆ  ş3  G4  ˆ  G4  Œ5  °ˆ  Œ5  ?6  Äˆ  ?6  k6  Ôˆ  p6  7  äˆ  7  8  ğˆ   8  f8   ‰  f8  s9  ‰  s9  Ÿ9  $‰   9  â9  ¸‚  ğ9  ¦:  äˆ  °:  Ò:  4‰  Ò:  ';  D‰  ';  ¦;  X‰  ¦;  ¼;  l‰  ¼;  Ğ;  |‰  Ğ;  ñ;  ”‰  ñ;  <  ¨‰   <  O<  ¸‚  €<  «<  P…  «<  =  ¸‰  =  2=  Ì‰  @=  =  Ü‰  =  L>  ğ‰  L>  i>  Š  i>  P@  Š  P@  a@  4Š  a@  q@  DŠ  q@  ‹@  TŠ  @  A  äˆ  A  dB  dŠ  pB  £B  |Š  £B  ^C  ”Š  ^C  xC  ¬Š  xC  ¡D  ÀŠ  ¡D  ÆD  ØŠ  ÆD  ÖD  èŠ  ÖD  ğD  øŠ  ğD  $E  ‹  0E  KF  ,‹  PF  gF  ¸…  gF  ÎF  @‹  ÎF  àF  X‹  àF  şF  p‹   G  "G  €‹  "G  ¸G  Œ‹  ¸G  ÊG   ‹  ÊG  íG  ´‹  ğG  DH  Ä‹  DH  ¯H  Ô‹  ¯H  µH  ì‹  ÀH  ÅI  ü‹  ĞI  ëI  ¸…  ğI  ËL  Œ  ĞL  ÉM  4Œ  ĞM  $N  LŒ  @N  ÛN  TŒ  ÛN  @O  dŒ  @O  ŸO  xŒ   O  ÑO  ˆŒ  ÑO   P  ”Œ   P  ¡P  °Œ  ¡P  OQ  ÄŒ  OQ  Q  ÔŒ  Q  ³Q  äŒ  ³Q  $R  ğŒ  $R  šR    šR  ŸR  $  ŸR  §R  8  §R  ¹R  H  ¹R  õR  d  õR  ´S  x  ´S   T  Œ   T  T  8   T  iT  œ  iT  ˆU  ¬  ˆU  |V  È  |V  V  Ü  V  ¨V  ì  °V  ^W  x„  `W  ,X  ü  ,X  5Y    5Y  kY  ,  €Y  ¡Y  @  ¤Y  ØY  ¸…  ØY  ©Z  L  ¬Z  ¿Z  ¸‚  ÀZ  [[  D  \[  É[  T  Ì[  =\  `  @\  \  ¸‚  \  ¦]  l  ¨]  ,^  °  ,^  __    `_  _  x„   _  L`  4  L`  o`  ¸‚  œ`  ·`  ¸‚  ¸`  ñ`  ¸‚  ô`  (a  ¸‚  (a  =a  ¸‚  @a  ha  ¸‚  ha  }a  ¸‚  €a  àa  ¤‚  àa  b  ¸‚  b  $b  ¸‚  $b  mb  ¸…  pb  ûb  ¸…  üb  ’c  @  ”c  ¸c  ¸…  ¸c  ác  ¸…  ôc  >e  h  @e  |e  äˆ  |e  ¸e  äˆ  ¸e  Yg  x  Àg  İg  ¸‚  àg  ;h  Œ  Ph  Rh  ˆ  Rh  ih  ¨  ih  ‚h  ¨  ‚h  –h  ¨  –h  Ìh  ,  Ìh  äh  `                                                                                                                                                                                                                                                                                                                                                                                                                                               €   8  €                  P  €                  h  €               	  €                  	      Ğ  Ü          €Ó  }          Ü4   V S _ V E R S I O N _ I N F O     ½ïş                                       <   S t r i n g F i l e I n f o      0 4 0 9 0 4 B 0   *   C o m p a n y N a m e     n o n e     >   F i l e D e s c r i p t i o n     l i b e n c h a n t     ,   F i l e V e r s i o n     2 . 2 . 8   >   I n t e r n a l N a m e   l i b e n c h a n t - 2 . 2     p &  L e g a l C o p y r i g h t   C o p y r i g h t   ( C )   2 0 0 2 - 2 0 0 7   D o m   L a c h o w i c z   N   O r i g i n a l F i l e n a m e   l i b e n c h a n t - 2 . 2 . d l l     6   P r o d u c t N a m e     l i b e n c h a n t     0   P r o d u c t V e r s i o n   2 . 2 . 8   D    V a r F i l e I n f o     $    T r a n s l a t i o n     	°    <?xml version='1.0' encoding='UTF-8' standalone='yes'?>
<assembly xmlns='urn:schemas-microsoft-com:asm.v1' manifestVersion='1.0'>
  <trustInfo xmlns="urn:schemas-microsoft-com:asm.v3">
    <security>
      <requestedPrivileges>
        <requestedExecutionLevel level='asInvoker' uiAccess='false' />
      </requestedPrivileges>
    </security>
  </trustInfo>
</assembly>
                                                                                                                                                                                                                                                                    p  $   ˜£ £¨£°£¸£ ¤¤ˆ­X®p®x®¯ ¯(¯                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                            This snapshot was automatically generated on
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
