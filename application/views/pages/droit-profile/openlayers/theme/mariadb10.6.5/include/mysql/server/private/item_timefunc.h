#ifndef ITEM_TIMEFUNC_INCLUDED
#define ITEM_TIMEFUNC_INCLUDED
/* Copyright (c) 2000, 2011, Oracle and/or its affiliates.
   Copyright (c) 2009-2011, Monty Program Ab

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


/* Function items used by mysql */

#ifdef USE_PRAGMA_INTERFACE
#pragma interface			/* gcc class implementation */
#endif

class MY_LOCALE;


bool get_interval_value(THD *thd, Item *args,
                        interval_type int_type, INTERVAL *interval);


class Item_long_func_date_field: public Item_long_func
{
  bool check_arguments() const override
  { return args[0]->check_type_can_return_date(func_name_cstring()); }
public:
  Item_long_func_date_field(THD *thd, Item *a)
   :Item_long_func(thd, a) { }
};


class Item_long_func_time_field: public Item_long_func
{
  bool check_arguments() const override
  { return args[0]->check_type_can_return_time(func_name_cstring()); }
public:
  Item_long_func_time_field(THD *thd, Item *a)
   :Item_long_func(thd, a) { }
};


class Item_func_period_add :public Item_long_func
{
  bool check_arguments() const override
  { return check_argument_types_can_return_int(0, 2); }
public:
  Item_func_period_add(THD *thd, Item *a, Item *b): Item_long_func(thd, a, b) {}
  longlong val_int() override;
  LEX_CSTRING func_name_cstring() const override
  {
    static LEX_CSTRING name= {STRING_WITH_LEN("period_add") };
    return name;
  }
  bool fix_length_and_dec() override
  {
    max_length=6*MY_CHARSET_BIN_MB_MAXLEN;
    return FALSE;
  }
  Item *get_copy(THD *thd) override
  { return get_item_copy<Item_func_period_add>(thd, this); }
};


class Item_func_period_diff :public Item_long_func
{
  bool check_arguments() const override
  { return check_argument_types_can_return_int(0, 2); }
public:
  Item_func_period_diff(THD *thd, Item *a, Item *b): Item_long_func(thd, a, b) {}
  longlong val_int() override;
  LEX_CSTRING func_name_cstring() const override
  {
    static LEX_CSTRING name= {STRING_WITH_LEN("period_diff") };
    return name;
  }
  bool fix_length_and_dec() override
  {
    decimals=0;
    max_length=6*MY_CHARSET_BIN_MB_MAXLEN;
    return FALSE;
  }
  Item *get_copy(THD *thd) override
  { return get_item_copy<Item_func_period_diff>(thd, this); }
};


class Item_func_to_days :public Item_long_func_date_field
{
public:
  Item_func_to_days(THD *thd, Item *a): Item_long_func_date_field(thd, a) {}
  longlong val_int() override;
  LEX_CSTRING func_name_cstring() const override
  {
    static LEX_CSTRING name= {STRING_WITH_LEN("to_days") };
    return name;
  }
  bool fix_length_and_dec() override
  {
    decimals=0; 
    max_length=6*MY_CHARSET_BIN_MB_MAXLEN;
    set_maybe_null();
    return FALSE;
  }
  enum_monotonicity_info get_monotonicity_info() const override;
  longlong val_int_endpoint(bool left_endp, bool *incl_endp) override;
  bool check_partition_func_processor(void *int_arg) override {return FALSE;}
  bool check_vcol_func_processor(void *arg) override { return FALSE;}
  bool check_valid_arguments_processor(void *int_arg) override
  {
    return !has_date_args();
  }
  Item *get_copy(THD *thd) override
  { return get_item_copy<Item_func_to_days>(thd, this); }
};


class Item_func_to_seconds :public Item_longlong_func
{
  bool check_arguments() const override
  { return check_argument_types_can_return_date(0, arg_count); }
public:
  Item_func_to_seconds(THD *thd, Item *a): Item_longlong_func(thd, a) {}
  longlong val_int() override;
  LEX_CSTRING func_name_cMZ�       ��  �       @                                      � �	�!�L�!This program cannot be run in DOS mode.
$       �z�	��Z��Z��Z�c*Z��Z�~�[��Z�~�[��Z�~�[��Z�~�[��Z�y�[��Z�y�[��Z�y�[��Z0x�[��Z��Z��Z�}�[��Z�}�[��Z�}FZ��Z�}�[��ZRich��Z                        PE  d� ���Z        � "  h   :      P        �                        �          `A                                    �  x   ��  �    �  �   �  |           �  @   ��  T                           �              �  x                          .text   �g      h                    `.rdata  v%   �   &   l              @  @.data   (   �      �              @  �.pdata  |   �      �              @  @.rsrc   �   �      �              @  @.reloc  @    �      �              @  B                                                                                                                                                                                                                                        H��  ���������H�T$L�D$L�L$ SVWH��0H��H�t$`H�������L��H�t$(H������H�D$     L��H��H�H����p  ��H�H��0_^[���L�D$L�L$ SUVWH��8I��H�l$xH��H���k���H�l$(L��L��H�D$     H��H�H���op  �������H�H��8_^][�����������ff�     H;�  uH��f����u�H���B   ��@SH�� H��3��o  H���o  ��n  H�Ⱥ	 �H�� [H�%$o  ������������H�L$H��   H�m�  ��n  H�X�  H�D$HE3�H�T$PH�L$H��n  H�D$@H�|$@ tBH�D$8    H�D$XH�D$0H�D$`H�D$(H��  H�D$ L�L$@L�D$HH�T$P3��cn  �"H��$�   H��  H��$�   H��H�o�  H�Ƞ  H�9�  H��$�   H�:�  ��  	 ��
�     ��     �   Hk� H��  H�   H���  H�D$hH���  H�D$p3���m  H��o  ��m  ��m  �	 �H����m  H�Ĉ   ����H��(�   �   H��(�̉L$H��(H��  ��m  H�D$(H���  H�D$(H��H���  H��  H�U�  �;�  	 ��5�     �?�     �   Hk� H�7�  �T$0H�H��n  �����H��(���H�\$ UH��H�� 3�H�2��-�+  H�EH���  H��tH;�tH��H���  �   H�M ��l  H�E H�E��l  ��H1E��l  ��H1E��l  ��H��H1E��l  ��H�MH3EH3�H�MH�E��l  �EH�������  H�� H3EH3EH#�HE�H��  H��H��  H�\$HH�� ]���������H��(��t9��t(��t��t
�   H��(��Z  ��+  ��H��(�I��H��(�   M����H��(�  H�\$H�t$H�|$ AVH�� H��L��3���  ��u3���   �N  �؈D$@@��=Z�   ��   �J�     �  ��tO�  ��  ��  H��l  H��l  �
  ��u)�6  ��t H��l  H��l  �e
  ���     @2����  @���l����I  H��H�8 t$H���L  ��tH�H���  L�ƺ   I�����N�  �   H�\$0H�t$8H�|$HH�� A^ù   �  �����H�\$H�t$WH�� @���  3ۅ�3��P�ȉ��  �5  @���D$8�=C�  uC�L  ��  �F  �,�  �g  @���;  3�@���U  ���Ë�H�\$0H�t$@H�� _ù   �p  ����L�D$�T$H�L$SVWAVAWH��@I����L���u9i�  3��  �B���wQH��k  H��u
�D$0   � H����  L�Ƌ�I���Ӌ؉D$0����   L�Ƌ�I�������؉D$0����   L�Ƌ�I���JT  �؉D$0��u=��u9L��3�I���.T  L��3�I���A���L�=Zk  M��tI���y  L��3�I��A�ׅ�t��uFL�Ƌ�I�������؉D$0��t/H�k  H��u�   �\$0�H���/  L�Ƌ�I���Ӌ؉D$0�3ۉ\$0��H��@A_A^_^[��������������H�\$H�t$WH�� I����H���u����L�ǋ�H��H�\$0H�t$8H�� _�o������H�	�  H�%�h  ��H���  �  H���  �H��(�C���H������H�H��(��H��(�?  ��t!eH�%0   H�H�H;�t3��H���  u�2�H��(ð�����H��(�  ��t�6  ���  ���8  ��t2���1  �H��(�H��(3��A  ����H��(����H��(�'  ��u2���  ��u�  ��H��(�H��(��  ��  �H��(����H�\$H�l$H�t$WH�� I��I����H���\  ��u��uH���  L��3�H����H�T$X�L$PH�\$0H�l$8H�t$@H�� _�e  ���H��(�  ��tH���  H��(�]  �n  ��u�U  H��(�H��(3��Q  H��(�H  @SH�� ���  �ɻ   DÈ��  ��  �!  ��u2���  ��u	3��	  ���H�� [����H�\$UH��H��@�=H�   ����   ����   �b  ��t-��u)H��  �  ��t2��   H���  �  ��tg��H��  �@   ��?+�H���H��H3�H�E�H�E�E�H�E��M���  H�E�H�E�E�H�E����  �M���  ���  ���  �H�\$PH��@]ù   ��   ��H��L���MZ  f9����uyHc����H�q���H��9PE  u_�  f9AuTL+��AH�QH��AH��L��H�$I;�t�JL;�r
�B�L;�rH��(��3�H��u2���z$ }2��
��2��2�H������@SH�� ���  3҅�t��uH�ʛ  H�� [�@SH�� �=��   ��t��u���@  ���9  �H�� [��H���  Ã%��   �H�\$UH��$@���H���  �ٹ   ��  ��t���)�   �����3�H�M�A��  �  H�M���d  H���   H���  H��E3��`d  H��t<H�d$8 H���  H���  L��H�L$0L��H���  H�L$(H�M�H�L$ 3��d  H���  H�L$PH���   3�H���  A��   H��H���   �	  H���  H�D$`�D$P  @�D$T   �d  ��H�D$PH�D$@H�E���H�D$H3���c  H�L$@��c  ��u��u�H����H��$�  H���  ]���H�\$H�t$WH�� H��z  H�5�z  �H�;H��t
H���i   ��H��H;�r�H�\$0H�t$8H�� _���H�\$H�t$WH�� H��z  H�5�z  �H�;H��t
H���   ��H��H;�r�H�\$0H�t$8H�� _���H�%%d  �H�\$H�l$VWAVH��3����     3����     �D��D�ҁ�cAMD��enti��E3ہ�AuthD���A��ineI�A��ntelD��A�C3�A��Genu�EЉ$Eщ\$��L$���T$uPH�9�  �%�?�=� t(=` t!=p t������ w$H�     H��sD�ޘ  A��D�Ә  �D�ʘ  ��u�� ��� ` rA��D���  �   D;�|'3���$D�ۉ\$�L$�T$��	sA��D�|�  ��sn���     �~�     ��sT��sN3��H�� H�H�T$0H�D$0$<u2�P�  ���?�     �=�  A�� t�� �&�     �$�  H�\$83�H�l$@H��A^_^���̸   ���3�9�  ����������������  ��������������%:a  �%\a  �%>a  �%�a  �%�a  �%�a  �%�a  �%�a  �%ja  �%la  �%Va  �%�`  ���3���H�\$WH�� H��  H��H����a  H�OL�8n  3���H�\$0�����H�� _���������������������H����   H�t$WH�� �y H��H�\$0H��H�l$8tH��tH�QH����P  �G    H�oH��tH�^@H���Aa  H����H�G    H�o(H��tH�^@H���a  H����H�G(    H�8 H�W8tH���VM  H�H H�WHtH���CM  H�oPH��tH�^@H����`  H����H�^@H����`  H����H�l$8H�\$0H�t$@H�� _������������������H�\$WH��@L�T$x3�L��$�   H��I�A�A��   v3H�ZH��  H���Y`  H�KL��p  3��׸����H�\$PH��@_Ë9�υ�tl��tS��t7H�BH��   H���`  D��L��p  �   3��Ӹ����H�\$PH��@_�H��$�   H��H�D$8�d   �=L�\$0H��L�T$(�   �)H��$�   H��H�D$8H�D$pL�\$0L�T$(H�D$ �  ��t�H�\$PH��@_��������������������@SVWAUAVAWH��   H��  H3�H��$�   H��$(  3��|$@E��I��L��L��A��s-H�ZH��  H���!_  H�KL��n  3��׸�����5  A�   H��n  H����^  ��t-I�^H��  H����^  H�KL�n  3��׸������  H��$�   �,   L��$�   L�cI����]  H��H��t	H+�H���A�o�I�FH���   H���v^  H�D$DA�   L�D$xH�D$ ��I����L��$�   H��$�   ��t-I�^H��  H���2^  H�KL�n  3��׿�����D  �|$Dt-I�^H��  H����]  H�KL�#n  3��׿�����  ��]  L��^  A�   H��I�UdH�D$@H�D$0H�D$HH�D$(H�D$ 
   ��]  H��u-I�^H��  H����]  H�KL�T^  3��׿�����   Ic]@�\]  M�M8H�T$HH��A�   H�D$@H�D$0H�D$`H�D$(H�\$ �#]  H��t�H���D`8Dxu+H��H��|��   H�~ H�~(H�~0H�~8H�~@���   �(I�^H��  H����\  H�KL�Fm  3��׿������H��$�   H3��+���H�ĸ   A_A^A]_^[�����������@SUVWATAUAVAWH��  H��  H3�H��$  H��$�  M��H��$�   H��H��$�  L��H��$�   3�A��D��L��$�   D��H�D$HH�D$PH�D$@H�D$p�D$XE��u H�ZH��  H���\  L�f]  ��
  ����
  A�x=��
  A� H�z<mu$H��  H����[  H�OL�_  3����
  <rt$H��  H����[  H�OL�.i  3����e
  H�(�]H����[  ����H��$�   H��H��u3H�^H��  H���e[  H�KL�j\  A�X  3��׸�����
  L��H�l$xI��H��H���P  D�d H������H��H��D8$u�H;�t-H�^H��  H����Z  H�KL��\  3��׻�����1  H�{�,   H����Y  H��H��u-H�^H��  H����Z  H�KL�eh  3��׻������  A�   D� H�{h  H�H�MZ  ��t-H�^H��  H���mZ  H�KL�h  3��׻�����  L�c�,   I���RY  H��H��u0H�^H��  H���&Z  L�h  H�K3��׻����M���U  A�   D�(H�*h  H�H��Y  ��t0H�^H��  H����Y  H�KL��g  3��׻����M���  H���,   H����X  H��tD�(�<Y  A�
   H��$�   H��D�(��X  A�G`H��$�   H;���  D8(��  ��X  D9(��  A�`   v H�^H��  H���7Y  L��g  ����H��H��D8,u�H�� �z  I�WHA�    H����X  ���_  H�FH�X@H����X  I�OH��H�NM�GHH���;G  M9oHu6H�^H��  H����X  A��  H�KL��Y  3��׻����M����  L��I��G8,4u�M��u H�^H��  H���rX  L�[g  �G���A��t H�^H��  H���LX  L�Mg  �!���I��H��H�@I�OXH�YH�FH�x(H���X  H����H�D$`I�GPH��uH�^H��  H����W  A��  �7���H�FA�_XH���   H����W  L�D$`H�D$hD�KH�D$ A��I���ׅ�t H�^H��  H����W  L��f  �t����D$hI�GXI�G�8pu)H���   H��u
��M���   L�@�@H�D$HL�D$P�M��I��E3�A�   M��LE�3�Mw H���������I��H��H�RL;�AE�H��H�D$`M����   H�FH�x(H����V  I�N��H�D$@H��H����   H�^H��  H����V  A��  H�KL��W  3��׻����L�d$@H�FH�x@H����V  H��$�   ��H�l$pH��tH�FH�x@H���V  H����M��tH�FH�x@H���dV  I����M��tH�FH�x@H���IV  I���׋��
  M�G H��I�W�K  I�O L�D$HH�H�T$P��J  H�FH�\$`H�x(H���V  H�K��L��H��uH�^H��  H����U  A�	  ����H�D$@H��uI�GH�\$`H�D$PH�FH���   H����U  H�L$PD�KM��H�D$     A���ׅ�tH�^H��  H���vU  A�	  ����H�D$`H��B�( I�GHf��| H�Iu�H��f�B�|( H�@u�D�H�NA��%M�G0I�W(�F  ��tH�^H��  H���U  A� 	  �2���M�OHH�ed  I�O(M������I�G(L��I��B�<0 u�H�NI�_@I�H\$xH�y8H����T  I�O8H�S��H��H��uH�^H��  H����T  A�3	  ����I�G@H�\$xH��$�   L��I�O8�,I�G8I�O@H��H��<I  I�O@M��I�G8H��D,I�O@I�G8H�I�W(H��H��I  I�G8�KI�I�_dIO@I�O@H�\$0� I�WA�G`M�OPH�ND�IԉD$(I�GXH�D$ �>8  ��S  L��T  A�   H��H��H�D$XH�D$0H��$�   H�D$(H�D$ 
   ��S  H��u(H�^H��  H����S  H�KL�ST  3�������L��$�   �   H��$�   �?S  H��u(H�^H��  H���FS  H�KL�#T  3����s���Ic_@�S  M�O8H��$�   H��A�   H�D$XH�D$0H��$�   H�D$(H�\$ ��R  H��u(H�^H��  H����R  H�KL��S  3��������o�$�   3��o�$�   f����$�   fff�     ���   2��   ���   H�II��u�H�FH�X(H���gR  A�L$��H�~H�D$pH��u*H��  H���BR  H�OL�GS  A��	  3����i���H���   H���R  H�|$pH��$�   A�   H�D$     L��A�Q��Ӌ؅��/����GH�[a  I�O(L��I��h���I�G(H��$�   H�I�G(D  H�ŀ<( u�H��$�   �   �(�����H�^H��  H����Q  L�`  �\���H�^H��  H���gQ  L��_  �<���H�ZH��  H���GQ  L��R  H�K3��׸����H��$  H3�����H��  A_A^A]A\_^][��������D�L$ L�D$USVWAUAWH�l$�H��   �Bp3�L��L���H�uOH�uǉu_�uWH�u�H�u�H�u�H�u�9BPv6H�ZH��  H����P  H�KL�3]  3��׍F�H�Ĉ   A_A]_^[]�L��$�   L�eL��$�   L�ugI9t$�	  H�zH�uOM��t%I�H��t���t��@  ��   �H0H��0��u�H���   H���P  H�OL�M�L�E�@  �Ӆ���   H�]�H����   H����O  H�M�L�EOE3ɺ@  �Ӆ�usH9uOumH��  H����O  H�OL��`  A�e  3��ӿ�������  H�H H��u+H��  H����O  H�OL��`  3��ӿ�������Y  H�MO������������  I9t$��   M�gH�u�M��t!I�H��t���t��@  t:�H0H��0��u�I��$�   H���O  I�L$L�M�L�E��@  �Ӄ��u�u_�CH�@ H�Eǉu_�6��u$H�]�H��tH����N  H�M�L�E�E3ɺ@  �ӉE_�������  I9uu"I�OM�EM��I�U�KA  �EW������q  M��t&I�H�E�H��tI�GH�X@H���cN  H�M���I�6�E_�MW����  ����  ����  I9uu3I�_H��  H���N  H�KL�D[  A��  3��׸������  L�uI9v��   H�E�H��t[@80tVI���   I�_H����M  H�UOA�   H�KE3�L�t$ �׋����  I���   I�_H����M  H�U�A�   �I���   I�_H���zM  H�UOA�   H�KE3�L�t$ �׋�����  A���   A�n   ��tA��t,��u;I���   H�������H�A�p   H�U�A�Ex   �A�Ex   A�y   �A�uxI�GH�X(H����L  �!   ��I�EHI�H��u"H��  H����L  A��  L��M  �  H��H����2  H��u5I�_H��  H����L  H�KL��M  A��  3��׿�������b  H�E�I������H�]H��tL@80tGH�KL�E�H�U��43  ��t2I�_H��  H���4L  H�KL�9M  A��  3���A�~��A  H�KL�E�H�UW��2  ��t3I�_H��  H����K  A��  H�KL��L  3��׿�����  H�E�H��tI��H��@84u��H��H�E�H��tI��H�ƀ<0 u�I�MHI��H���< u�H�]WI��H���< u�H�M�E0H�I�OH�I�U(D�N��<  ��tI�_H��  H���IK  A��  �W���H�E�L��X  H��L��X  I��H�{X  HE�M��H�U�LE�L�D$8H��H�L$0L�dX  I�M(I��HE�ME�H�D$(H�VE��L�T$ �����I�U(I���     H���< u�I�E M�EI�O�9  I�M(H�X  IM L��M�MH�+���I�U(M�E8IU I�O��8  I�M8H��uI�_H��  H���aJ  A�  �o���I��H���< u�I�U(I��I�E@H�EoH�H���<
 u�H�Ew�   �L�e�M��tFM�oM��t=I���    H��A�<4 u�I���   H����I  ��I����I�]@H����I  I����L�e�M���   I�wH���   I��C�<4 u�H���   H����I  A��I����H�^@H����I  I���Ӌ��i  ��H�3V  H�LV  L��LD�L���H�`V  I�LD����H��HD�H��H�MW����M��PD�M��BD�u-H��  H���I  H�OL��Z  3���H����������   H�_(D�<@H��A����H  A����H��H��u1H��  H����H  A�#  L��Y  H�O3��ӿ�������   M��3�H���h���H�EWI�H��tH�yZ  �@  H�KH�CH�sH��0M��tH�oZ  �@  H�CL�kH�sH��0M��tH�eZ  �@  H�CL�cH�sH��0�3�   H�sH�sH�sL��$�   L��$�   H�Ĉ   A_A]_^[]�������������H�\$H�t$WH�� H�BI��H��H�X(H����G  ��   ��H��H��u>H�_H��  H����G  H�KL��H  A�  3��׸����H�\$0H�t$8H�� _�3�A��   H���9���H�3�H�\$0H�t$8H�� _���������H�\$WH�� H����}/H��  H���&G  H�OL��S  3��Ӹ����H�\$0H�� _�A�    H��u  I�H�D$P�    H���   H����F  H�OH�`|  A�   ��H�\$03�H�� _��������������������H���  H�t$WH�� H�y H��H�QH�\$0H��H�l$8tH����2  H� H�WtH���2  H� H�WtH���2  H�( H�W(tH���2  H�8 H�W8tH���~2  H�o@H��tH�^@H���F  H����H���   H��tH�^@H����E  H����HǇ�       H���   H��tH�^@H����E  H����HǇ�       H�^@H����E  H����H�l$8H�\$0H�t$@H�� _�����������@USVWATAUAVH�l$�H��`  H��s  H3�H�EP3�L�M�H��P  H�t$pH�E�L��H�BPD��H�u�D��L�D$PH�t$`H��p  H�t$XH�u�H����  H���E  E3�3�3��Ӆ���  I�^PH�{ H����D  H�KL�M�L��P  H�t$ H��P  ��H�M�H���F  �<1t<yt<ou�ynt<t�)  I�~I�^I�vPL��$�  A�����H����   H�L$PA�WG��C  L��H��uJH��tH��D8+uH��L�D$XH���2  ������   H�T$PL�D$`H���2  L�l$`��L�d$X�   L�D$XH��H�P�z2  E�,$��H�^(M��L+l$PH��I��L�l$`��C  I����L��H��tL�D$`H��H�T$P��C  A�$@L�d$X�^H��  H����C  H�NL��T  A��  3��ӿ����A�$@L�d$X�(H��  H���}C  H�NL�ZT  A��  3���A��I�VPH�T$h��tLH��  H���HC  H�L$hL�4O  3�H�I�Ӌ�L��$�  H�MPH3�����H��`  A^A]A\_^[]�H������M���v  M���m  H���     A�| H�@u�H�ΐB�|) H�Iu�H�z(H�H����B  H�K��H�D$`L��H��u5H�|$hH��  H����B  H�OL��S  A�  3���A������  I��I��I+�f���H�I��u�I�H��y H�Iu��yT  f�I�H�fD  �y H�Iu�3�@ A��H�R��u����   �  H�}� ��  I�^PE3�L�|$hH�{ H����A  H�KL�L$PL�WF  L�|$ H�cF  ��L9|$Pto�\A  E�G
H�T$xD�8H�L$P��@  H�L$x�D$XH9L$PtD89u�*A  D98u	L9|$Pu3�)I�FPH��   H���hA  L�	F  �   3���L�|$P�D$X   I�^PH���   H���7A  H�KH�U�A�   ��D���   H�D$hH�U�L�M�I�NPH�D$@H�E8H�D$8H�E H�D$0�D$X�D$(H�D$    �"  D����t2H�|$h ��  I�^PH��  H����@  L�D$h3�H�K����  I�FPH���   H����@  3�L�E�H�M�H�|$ D�O�W��M�~P��t0I��  H���e@  A�U  I�OL�dA  3���A������l  @�}�I���   H���1@  A�   H�|$ L�E H�M A�Q���M�~P��tI��  H��� @  A�c  �@�}I���   H����?  A�   H�|$ L�E�H�M8A�Q���M�~P��tI��  H����?  A�r  �I���@�}�I�_(H����?  �}   ��H�E�L��H��u4I�^PH��  H���p?  H�KL�u@  A��  3���A������w  D�L$XH�E�H�D$0L��C  H�E H�D$(H�CK  H�E�I�OH�D$ ����H��H��A8|u�A��3�H�}�I�FPH��  H����>  3���H�D$pL��H��uD���   I�FPH��(  H����>  H�U�I����D����ujH�E�H��t�8L�x�L��I�FPH��X  H����>  H�L$pH�J  D��M����D����u'I�^PH��p  H���Y>  L�D$`H�T$pH�K��D��H�|$p tI�FPH��H  H���*>  H�L$p��I�~PE��t!H��  H���
>  H�OL�'J  3���� H��   H����=  L�2J  �   3���I�NPH�L$xH��toH�D$`H��H�ǀ<8 u�H���   H����=  H�L$`����H�\$xH�[@H����=  H�L$`���*H��  H���v=  H�L$hL�RN  A��  3�H�I��M��tII�FPH�D$xH��t;H��H��B�</ u�H���   H���,=  ��I����H�\$xH�[@H���=  I����M��tDI�~PH��t;�     H��A�<4 u�H���   H����<  ��I����H�_@H����<  I����L�e�M��t6I�vPH��t-H���   H����<  A�$I�L$��H�^@H����<  I����A���L���3��M���I�^PH��  H���c<  H�KL��G  3��׸����� ���������������������@SH��@L�\$p3�H�\$xL��I��H��u	�A�H��@[�H�|$PA��   v3H�ZPH��  H����;  H�KL�G  3���H�|$P�����H��@[Ë9�υ�td��tz��t7H�BPH��   H����;  D��L�G  �   3���H�|$P�����H��@[�I��H�\$xL�\$pA�   H�|$PH��@[�K   A�   E��uA�AH�|$PH��@[�I��H�\$xL�\$pH�|$PH��@[�
  ��������������������@USWATAUAVAWH�l$�H��0  H��i  H3�H�EH���   3�H�E�M��H���   L��H�E�L��H���   H�D$PE��L�E�H�\$`�\$@H�\$p�\$DE��u*H�ZPH��  H����:  H�KL��;  3��׸�����vA����	  A�x=��	  H�JPA�~H��$(  H�q(H���K:  ����H�D$HH��uUI�_PH��  H���):  H�KL�.;  A�k  3��׸����H��$(  H�MH3��c���H��0  A_A^A]A\_[]�M��I��H���.  H�L$HI������I��A�H��8u�I;�t(I�_PH��  H����9  H�KL�3;  3����  A�   H��@  �G9  ��t(I�_PH��  H���g9  H�KL��@  3����A  H�|$H�,   H�O�O8  H��u(I�_PH��  H���&9  H�KL��;  3����   �M��H��H�D$hI��B8\7u�I�GPH�p(H����8  I�N��H��H�D$`I�GPH���   H����8  L��H�D$@H�t$HE��A��H�D$ H�N�ׅ�t(I�_PH��  H����8  H�KL��?  3����l  M���   �D$@I;��.  H�|$`I���   H���8  ���  A���   �����  �t$@A+��   �t$@u(I�_PH��  H���8  H�KL�*@  3�����  M���   M���   M��I�H�D$xL+��B� +�uH����u��t2I�_PH��  H����7  L�L$xL�@  H�K3�L�t$ ���  E�rA;�t/I�_PH��  H���7  H�KL�,@  E�Ήt$ 3����R  I���   I�RH�D���),  ��t(I�_PH��  H���67  H�KL�C@  3����  H�t$HH�|$hH�;  H��A�   ��6  ��t(I�_PH��  H����6  H�KL�'@  3�����  H���,   H����5  L��H��u(I�_PH��  H����6  H�KL�@  3����  �I��I�U8H+���+�uH�ǅ�u��t(I�_PH��  H���^6  H�KL��?  3����8  A8tE@ f�     A�   H��?  I����5  ��tS�,   I��� 5  L��H��tI��A8u�L�t$hH��H��u9I�_PH��  H����5  H�KL��?  3����  I�~L+�I��H�|$p�º,   H����4  H��t(I�_PH��  H����5  H�KL��?  3����m  I��@ H��8u�H��t(I�_PH��  H���V5  H�KL��?  3����0  I�OPI�}0H�q8H���-5  I�M(H�WI���H��H��u3I�_PH��  H���5  A�  H�KL�6  3��׻������  H�U�M��I�M(IM0�)  Mu0I�M0I�E(�Ic}0��4  M�M(I�UTH��A�   H�D$DH�D$0H�E�H�D$(H�|$ �q4  H��u-I�_PH��  H���p4  H�KL�55  3��׻�����J  L�t$pI���    H��A86u�I�GPH���   H���+4  H�D$XA�   L�E�H�D$ ��I���ׅ�t(I�_PH��  H����3  H�KL�u>  3�����  �|$Xt(I�_PH��  H����3  H�KL��>  3����  �oM�H�˺   �oE�f���M�f��D�2D��D�H�IH��u�L�EȺ   H�M��N3  H��u-I�_PH��  H���U3  H�KL�24  3��׻�����/  H�M�M��L+�H���    H�M�H�B�DT8��  H��H��|�Ic}0��2  M�M(I�UiH��A�   H�D$DH�D$0H�E�H�D$(H�|$ ��2  I�PH��u)H��  H����2  H�OL�}3  3��ӻ�����  M�E A�   I�UH����#  ��tI�_PH��  H���q2  A�b  �h���I�E� vI�E�@=I�GPI�}H���   H���?2  A�   H�\$ L�GH�M�A�Q��օ�t-I�_PH��  H���2  H�KL�#=  3��׻������   I�EH�M��XI�EH�I�E�    I��B8 u�H�E�D� A���   ��t1��t��H�L$Pu*�Ax   �!H�L$P�Ax   I���   H�A`�H�L$P�Yx�   H�Y H�Y(H�Y0H�Y8H�Y@���   �TI�_PH��  H���M1  H�KL�J<  3����*I�_PH��  H���(1  M���   L��8  H�K3��׻����I�GPH�x@H����0  H�L$H��H�t$`H��tI�OPH�y@H����0  H���׋�����H�ZPH��  H����0  H�KL�'2  3��׸�����������������@USVWATAUAVAWH�l$�H��H  H� _  H3�H�E8H���   L��H�zPM��H�E�L��H���   H�E�H���   H�D$PH�s1  H�L$X3�H�E�H�q1  H�E�A��L�E�H�L$xH�M�E��u)H��  H��� 0  H�OL�M1  3��Ӹ������  ��
s)H��  H����/  H�OL�?1  3��Ӹ�����  H�(�^H����/  ����H�D$pH��H��u3I�]PH��  H����/  H�KL��0  A�  3��׸�����L  H��L��I��H�]�H���1$  I�������> I��H���< u�H;�t.I�]PH��  H��� /  H�KL��0  3���A������  �<n��   <ptB<yt.I�]PH��  H����.  H�KL�,1  3���A������q  Aǆ�      �   �=H�Ot.I�]PH��  H����.  H�KL�D0  3���A������)  H�A�,   I���   �w-  M���   H��H��u1I� I�]PH��  H���A.  H�KL�>0  3���A�������  �  I���   I�MP�|  �,Aǆ�      �Aǆ�       H�_�;,t.I�]PH��  H����-  H�KL�y0  3���A������n  �CH��H�s<a��   �>=��   H�K�,   I�N��,  M�FH��H��u/I� I�]PH��  H���p-  H�KL�50  3���D�f��  �  H��I�VH+�H��I���   I�MP�  I�N�  ��t.I�]PH��  H���-  H�KL�0  3���A������  I���   �B<,t.I�]PH��  H����,  H�KL�y/  3���A������n  � H+�H��I���   I�EPH�x(H����,  H�K��I���   H��u4I�]PH��  H���{,  A��  H�KL�z-  3���A������  M���   H��H�T$pI���%!  I���   I���   �D�,I���   I���   � �~=t.I�]PH��  H���,  H�KL�r-  3���A������  �<mu.I�]PH��  H����+  H�KL�/  3���A������b  <nt.I�]PH��  H����+  H�KL�/  3���A������0  H�^�,   H����*  H��H��u,I�]PH��  H���V+  H�KL��.  3���D�f���  H���  �  I�}P��t*H��  H���+  H�OL��.  3���A������  M�FH��H���Z  A�   H��.  H�N��*  ��t.I�]PH��  H����*  H�KL�X.  3���A������U  H���,   H����)  H��t
�  �    I��B�<& u�I�EPL�e�H�x(H���i*  I�L$!��I�F8H��H��uI�]PH��  H���B*  A��  ������H�v�H�I��u�I�V8I�MPI��.  I�}PH��u0H��  H����)  A��  H�OL��*  3���A������
  H��(  H����)  I���   H�U���D�����a
  �pI�FH��tD8 u�   I���   I�]PH����)  H�D$PD��I�VE3�H�KH�D$ ��D����t(I�]PH��  H���V)  H�KL�[-  3�����	  I�vH��t8�> t3I���   I�]PH��� )  H�D$PA�   H�KE3�H��H�D$ ��D��I�}PE��t$H��  H����(  H�OL�-  3����	  H��8  H����(  I���   L�E�H�U��Ӆ��	  L�E�H�E�H�M�H�U�M��tH��uH����  H����  M����  H����  I�uPE3�I�~L�d$XH�^(H���P(  A�L$��I������H��M��@ I��F8$u�H�\$0L��]  �D$(   H��H��H�D$    �_  I�FH   I�^@I�]PH�{ H����'  H�KL�L$PL�Z,  L�d$ H�f,  ��L9d$Ptu�_'  A�
   H�T$`D� H�L$P��&  A�FPH�D$`H9D$PtD8 u�+'  D9 uL9d$Pt/A�NP�2I�EPH��   H���e'  L�,  �   3���L�d$P�   A�NPH�E�M�NTM��H�I�FiI��F8$u�L�T$XL�T$@H�D$8I�FHL�L$0M�N@�L$(I�MPH�D$ �  D������  H�t$XH����  I�]PH��  H����&  H�KL��3����m  H���   H����  H�8 A������g  H�D$`    3�H��; u H�À; t�A�   H�+  H���2&  ���  H���; u	�H�À; t��;$��  H�À; u�    H�À; t��:   H���2%  H��H����  H;���  H��H+�H��~ I�]PH��  H����%  L��*  �}  L��H�M H���  H����  �D=  �]%  A�
   H�U�H�M �     ��$  A�FPH�M H�E�H;��  �8 �  �%  �8 ��  H�^H��H�\$hH��*  �%  L�<L;���  A�? ��  I�EPM��L+�H�X(H���/%  I����H�\$XH��H�C@H��HI�EPH���   H���%  H�L$hE��L��H�\$ A���ׅ�t-I�]PH��  H����$  H�KL�v*  3���L�t$X�d  A�? uf�I��A�? t�A�?$t)I�EPH�X@H����$  L�t$XI�N@��I�F@    �$  I��A�? u	I��A�? t��:   I���p#  H�D$hH��H��t�I;�t�I�EP��L�t$XA+�H���   H���0$  H�D$PA�   M�FTH�D$ ��I���օ�tGI�]PH��  H����#  H�KL��)  3���I�EPH�X@H����#  I�N@��I�F@    �n  �|$PtGI�]PH��  H����#  H�KL��)  3���I�EPH�X@H����#  I�N@��I�F@    �   L�|$h�    I��I���v"  H��A��H��uH������H��A�<7 u�I�I�EP+�H���   H���1#  H�D$PA�   M�FiH�D$ ��I���ׅ�tDI�]PH��  H����"  H�KL�[)  3���I�EPH�X@H����"  I�N@��I�F@    �r�|$P��   I�]PH��  H����"  H�KL�T)  3���I�EPH�X@H����"  I�N@��I�F@    �#I�]PH��  H���k"  L��'  H�K3���H�T$`H�E�H��H�T$`H��    H�< �����I�]PH��  H���$"  H�KL�)  3����  I������I�EPH��h  H����!  H�U�I���   ��I�NHH���������H��H��H�RH+�H��H���H�4�I�EPH�x(H����!  H�N��I�}PL��H�D$xH��uH��  H����!  A��  ����H���   H���m!  A�VHD�NI�N@M��H�D$     �ׅ�tI�]PH��  H���;!  A��  ����H�]�M�F B�& I�VI�MPD�K=D��w  ��tI�]PH��  H����   A�  �w���A�FPH�(  M�F8M��I�N�D$ �e���L�e�H�K>M+��   H�I�I�N0I�EPH�YH�x(H����   H����I�F(H��H��uI�]PH��  H���z   A�"  �����H�U�M��I��   �=  I�F(I�T$A�,I�NIV(@ ff�     �H�I�H�R��u�I�N(I��     �y H�Iu��G'  f�I��I�N(D  H���< u�H�M�I�F0I�FH�I�F@ I��B�<8 u�H�E�A�   D�8A�   �dI�]PH��  H����  H�KL��&  3��׸�����uI�]PH��  H���z  H�KL��#  3���I���    �����A�����DE�I�EPH�X@H���C  H�L$p��H�|$xH��tI�EPH�X@H���!  H����A��H�M8H3��u���H��H  A_A^A]A\_^[]�����������������H�\$WH�� H�BPH��H�X(H����  ��   ��H��H��u9H�_PH��  H����  H�KL��  A��  3��׸����H�\$0H�� _�3�A��   H���F���H�D$PH�3�H�\$0H�� _����������������������@UVATAUAVAWH��   H��L  H3�H��$�   L��$   3�L��$  M��L��$  H��L�L$PI�6H�T$H�t$@M��uH��  I��F��l  H��$�   H��$�   H�y(H����  I�O��H��H��u�G��)  H�T$HM�GH�HD�8�c  D�H�D$XL�L$PH�WH�D$0H�͋�$  �D$(H��$   H�D$ �  �G  L�  A�   H��H�T$XH�D$@H�D$0H�D$pH�D$(H�D$ 
   �  H��t_M��H�L$p�   ��  H��u	H��  �E��  L��  A�   H��H�T$XH�D$@H�D$0L�l$(H�D$ 
   ��  H��uH��  I������H���   H����  �H�O��H�]@H����  H���Ӌ�H��$�   H��$�   H��$�   H3��ȫ��H�Ĩ   A_A^A]A\^]�������H�\$H�t$H�|$AVH��@H�Y(H��H��M��H���  �   ��H��I������@ I��B�< u�H�\$0L��Q  �D$(   H��H��H�D$    �.   H�t$XH��H�\$PH�|$`I�   H��@A^���������������H�\$H�l$H�t$ WATAUAVAWH��@H�y(L��H��$�   H��I���D$p    M��L���c  H�M��L��H��$�   H��H��L���%  A�.   I�](H���/  �   �ӍMH��Hc��	  H��$�   M��H��E��H�D$pI��H�D$0H�t$(H�\$ ��  �F�G��$�   ����   H�nL�w�X���  L��E��H��I��H�D$pH�D$0H�|$(H�D$    �|  3�I;�wH;�s�o�   �of���L��H�1L+ƺ   H+�f�     A� 0H�@H��u�H���y���L��$�   I�]@H���(  I����I�]@H���  H����L�\$@I�[8I�k@I�sHI��A_A^A]A\_������������H�\$H�l$H�t$WH��0H�Y(H��H��H����  �   ��H��H��t`H���   H����  H�MA�   H����H���   H����  A�!   H�D$     L��H��A�Q���H�]@H�˅�t�W  H����3���H  H����H���F  H�\$@H�l$HH�t$PH��0_����������������H�\$H�t$WH�� �E3�I��H��H�ل���   <,t<=uA���AH����u�E����   H������@ H�< u�C�	H��Hc�H���  H�H�H��u�����H�\$0H�t$8H�� _����t4��,t��=t�H���f� =3�@D�	f� =2�@CH���KH�Ä�u��  3�H�\$0H�t$8H�� _�H�t$83�H�H�\$0I�     H�� _����������L��H����tXH����=u?���tC��2u�xCu8H���,A�I����р�3u"�xDuH���=A�I����A�I���몸�����3����������H�\$WH�� H����}/H��  H����  H�OL��#  3��Ӹ����H�\$0H�� _�A�    H�!F  I�H�D$P�    H���   H���;  H�OH��L  A�   ��H�\$03�H�� _��������������������@WH�� H����}4H�\$0H��  H����  H�OL�R#  3���H�\$0�����H�� _�H��E  A�    I�H�D$P�    3�H�� _����������̸   �����������H��tH�H��t���t;�t�H0H��0��u�3�������������H�\$H�l$VAVAWH�� M��I��L��H��H����   M����   M����   H������H�|$@H���    A�| H�Ru�@ A�| H�@u�H�y(H�H����  H�K��H�|$@I�H��u/H��  H����  H�ML��&  A�  3��Ӹ�����   H��@ ff�     �H�v�
H�R��u�H�P��z H�Ru���'  H��f�
@ �x H�@u�3�@ A��H�R��u�3��*H��  H���$  H�ML�&  A��  3��Ӹ����H�\$HH�l$PH�� A_A^^���������������H�\$H�l$VWAVH�� H�D$`M��I�    A���H��H��t$H� H��tD�E��tf�D;�tCD�H0H��0E��u�H���   H����  H�NL�L$XL�D$`���Ӄ��uF����   3��   ��t(H�x  u!H��  H���E  H�NL��%  3����dH�@ I�3��^��uZH�\$`H��tPH���  H�L$XE3�M�Ƌ��Ӆ�u4��t0I�> u*H��  H����  H�NL��$  A�e  3��Ӹ����H�\$@H�l$HH�� A^_^�����������������H����   H�l$ AVH�� L��H��H��tiH�t$8H�2H��tWH�\$0H������H�|$@ H�À< u�H���   H���O  ��H����H�]@H���;  I���H�|$@H�\$0I�    H�t$8H�l$HH�� A^�������������H�\$H�t$ WATAWH�� I��M��L��H��H����   H�t$`H����   H�l$@�@   H�l$hH��L�t$H��  L��H��uCH��tH��8uH��M��H����   ������   M��H��H����   L�t$H��H�l$@�   M��H�PH����   A� M��H�_(��H��L+��B  I�L$��I�H��tM�D$H��H����  �*H��  H���  H�OL�#  A��  3��Ӿ����A�@L�t$H��H�l$@�*H��  H����  H�OL��"  A��  3��Ӹ����H�\$PH�t$XH�� A_A\_���������������H�\$H�l$H�|$ AVH�� M��H��H��H�������|: H�u�H����   H��twM��trH�t$0H�q(H���C  H�O��H�t$0H��I�H��u5H��  H���  H�ML�*"  A�  3��Ӹ�����Hf�     �H�[�H�I��u�3��*H��  H����  H�ML��!  A�  3��Ӹ����H�\$8H�l$@H�|$HH�� A^������������H��trH�l$ VH�� L�t$@H��L�2H��M��tFH�\$0H�|$8H���   H���\  A�I�N��H�]@H���F  H���H�|$8H�\$0H�    L�t$@H�l$HH�� ^���������H�\$H�l$VWATAUAVH�� H�l$pE3�H��$�   M��A��M����L����H��H��D�HDD�H��A�AAD���u(H��  H����  H�OL�p!  3���A�E���   H�Y(L�|$PH��D�<@A����  A����H��H��u/H��  H���d  H�OL�q   A�#  3��Ӹ�����   M��3�H������I�$M��tH� !  �@  H�CL�sL�kH��0H��tH�!  �@  H�CH�kL�kH��0H��tH�!  �@  H�CH�sL�kH��0D�+3�L�kL�kL�kL�|$PH�\$XH�l$`H�� A^A]A\_^����������@SUATAVAWH�� A��M��L��L��H����   H����   M����   H�t$PH�|$XL�l$`L�*M��u<H�y(H���@  ����I�$H��uA�I��  H���   A��   �PA�.3��]A�0;�su�;�s�;�r�H�y8H����  ��I����I�$H��uIA�I��  H����  A��   I�OL��  3��Ӹ����H�|$XH�t$PL�l$`H�� A_A^A\][�A�63���H��  H����  I�OL�^  A��   3��Ӹ����H�� A_A^A\][�������������������@SUVAVAWH�� 3�M��H�L��A� H��M��t$I�1H��t���t=@  ��   �F0H��0��u�H���   H����  H�ML�L$`L�D$X�@  �Ӆ�u[H�\$XH��tQH����  H�T$`M��H�MA�@  �Ӆ�u0I�> u*H��  H����  H�ML�u  A��  3��Ӹ����H�� A_A^^][�H�~  u0H��  H���[  H�ML��  3��Ӹ����H�� A_A^^][�H�Y(H��H�|$P�~(�&  H�O	��H�|$PH��I�H��u6H��  H���   H�ML�  A��  3��Ӹ����H�� A_A^^][ËF(�I�D�F(H��H�V �   I���D 3�A�   H�� A_A^^][���H��(M�A8H��I���   �   H��(����@SE�H��A���L��A� L��tA�@McP��L�Hc�L#�Ic�J�H�C�HH�C�Dt�D���L�L3�I��[鍚���%?
  �%A
  �������ff�     ����������������������ff�     M3���%�
  �I����I����I����I����I���������̐I��H�����uH�$dL�$�,   �)<�t#L��<�tL��<�tM��<�tM��<�tM3�L3�I���������ffffff�     ��%i
  ������ ��@UH�� H��M@H�� ]������@UH�� H���ա���M8H�� ]餣���@UH��0H��H��H�L$(�T$ L�}���L���   �UxH�Mp�����H��0]��@UH��H�3Ɂ8  �����]��                                                                          �      ��      F�      f�      J�      6�      �      �      ��      Ƥ      ܤ      ��      �      �      2�              �      �      J�      b�      l�      *�              f�              ��              X�      $�      @�      �      ֢      Ȣ      ��      �      ��              ��              p�      ��      z�              ܡ      ԡ      ȡ              0 �   pv �   �v �   0w �                                                                   @     P  0  �  �   �9  �:  �;  @E  �b  �i  pj       � �   �� �           empty secret    Client Key      HMAC-SHA1 call failed   SHA1 call failed        Server Key      Out of Memory in scram.c near line %d   *userPassword   *authPassword   SCRAM-SHA-1 input expected      Invalid SCRAM-SHA-1 input       NULs found in SCRAM-SHA-1 input The initial 'p' needs to be followed by '=' in SCRAM-SHA-1 input                Channel binding name must be terminated by a comma in SCRAM-SHA-1 input         The initial SCRAM-SHA-1 client response needs to start with 'y', 'n' or 'p'     ',' expected in SCRAM-SHA-1 input       At least nonce is expected in SCRAM-SHA-1 input Invalid authorization identity encoding in SCRAM-SHA-1 input    Unsupported mandatory extension to SCRAM-SHA-1  Username (n=) expected in SCRAM-SHA-1 input     Nonce expected after the username in SCRAM-SHA-1 input  Invalid username encoding in SCRAM-SHA-1 input  r=      unable to canonify user and get auxprops        unable to canonify authorization ID     no secret in database   scram_iteration_counter SCRAM-SHA-1     Invalid iteration-count in scram_iteration_count SASL option: not a number. Using the default instead.          Invalid iteration-count in SCRAM-SHA-1 input: the value is too big      Invalid iteration-count in SCRAM-SHA-1 input: not a number  $           Invalid base64 encoding of the salt in SCRAM-SHA-1 stored value Invalid base64 encoding of StoredKey in SCRAM-SHA-1 per-user storage    Invalid StoredKey in SCRAM-SHA-1 per-user storage       Invalid base64 encoding of ServerKey in SCRAM-SHA-1 per-user storage    Invalid ServerKey in SCRAM-SHA-1 per-user storage       No valid SCRAM-SHA-1 secret found       Have neither type of secret     r=%s,s=%s,i=%u  ,   c=  Channel binding expected in SCRAM-SHA-1 input   Invalid base64 encoding of the channel bindings in SCRAM-SHA-1  Channel bindings prefix doesn't match the one received in the GS2 header of SCRAM-SHA-1. Expected "%s"  Channel bindings data expected in SCRAM-SHA-1           Unsupported channel bindings type received in SCRAM-SHA-1. Expected: %s, received: %s           Unsupported channel bindings length received in SCRAM-SHA-1. Expected lenght: %d, received: %d  Channel bindings mismatch in SCRAM-SHA-1        Nonce expected in SCRAM-SHA-1 input     At least proof is expected in SCRAM-SHA-1 input Nonce mismatch SCRAM-SHA-1 input    p=  Client proof is expected in SCRAM-SHA-1 input   No extension data is allowed after the client proof in SCRAM-SHA-1 input        Invalid client proof length in SCRAM-SHA-1 input                Invalid base64 encoding of the client proof in SCRAM-SHA-1 input        Invalid client proof (truncated) in SCRAM-SHA-1 input   StoredKey mismatch      Internal error  SCRAM-SHA-1 input longer than (MAX_CLIENTIN_LEN) bytes  Invalid SCRAM-SHA-1 server step %d
     authPassword    SCRAM-SHA-1: auxprop backend can't store properties     scram_secret_generate   SCRAM   SCRAM-SHA-1: Error parsing user %s$%u:%s$%s:%s  Error putting SCRAM-SHA-1 secret        Setpass for SCRAM-SHA-1 successful
     SCRAM-SHA-1 version mismatch    SSF requested of SCRAM-SHA-1 plugin     Please enter your password      Please enter your authentication name   Please enter your authorization name    Parameter Error in scram.c near line %d     a=  =       %c%s%s,%s%s,    n=%s,r=%s       Nonce (r=) expected in SCRAM-SHA-1 input        Salt expected after the nonce in SCRAM-SHA-1 input  s=  iteration-count expected after the salt in SCRAM-SHA-1 input    i=      Invalid iteration-count in SCRAM-SHA-1 input    iteration-count is too big, refusing to compute The nonce received from the server doesn't start from the nonce sent by the client      The salt can't be empty Invalid base64 encoding of the salt     Invalid base64 encoding of the salt in SCRAM-SHA-1 input        c=%s,r=%s   ,p=%s       Invalid SCRAM-SHA-1 input expected  v=  ServerSignature expected in SCRAM-SHA-1 input           Invalid base64 encoding of the server proof in SCRAM-SHA-1 input        Invalid server proof (truncated) in SCRAM-SHA-1 input   ServerSignature mismatch        SCRAM-SHA-1 input longer than (MAX_SERVERIN_LEN) bytes  Invalid SCRAM-SHA-1 client step %d
     Parameter Error in plugin_common.c near line %d Out of Memory in plugin_common.c near line %d   Unexpectedly missing a prompt result in _plug_get_simple        Unexpectedly missing a prompt result in _plug_get_password      make_prompts() called with no actual prompts    Authorization Name      Authentication Name     Password    @       ���Z       I   �  �      ���Z          \�  \�      ���Z       P  p�  p�                                                                                                      � �                   x� �   �� �   ؁ �           5                                                                                                             RSDSe�tK\��M���}>ř5   E:\repo\winlibs_cyrus-sasl\plugins\saslSCRAM.pdb                     GCTL   `f  .text$mn    `v  �   .text$mn$00 2w  �   .text$x  �  x  .idata$5    x�      .00cfg  ��     .CRT$XCA    ��     .CRT$XCZ    ��     .CRT$XIA    ��     .CRT$XIZ    ��     .CRT$XPA    ��     .CRT$XPZ    ȁ     .CRT$XTA    Ё     .CRT$XTZ    ؁  8   .gfids  �     .rdata  �  �  .rdata$zzzdbg   ��     .rtc$IAA    ȗ     .rtc$IZZ    З     .rtc$TAA    ؗ     .rtc$TZZ    ��  @  .xdata   �  x   .edata  ��  �   .idata$2    8�     .idata$3    P�  x  .idata$4    ȡ  �  .idata$6     �     .data    �    .bss     �  |  .pdata   �  `   .rsrc$01    `�  `  .rsrc$02                                     Rp`0 bp`P0          B   B     20 4	 2P t	 d 4 2�@     �  �  2w      ]  h  2w       2P d 4 2p@     �  �  Iw      �  �  Iw      	 r��p`0@     0  /  ew  /   RP d 4 2p	 "  @     s  �  �w  �   P   4
 rP d T 4 2p 4� � P   T 4 �p`                   
 
4 
2p d 2p! T 4 �  �  |�  !   �  �  |�  
 
4
 
rp#  
���p`0�u  �   ! � T  "  �"  ��  !    "  �"  ��  &
 # ��	��p`P0�u    � �� ��  ��p`0P d 4 2p
 
4 
2p d 2p! T 4 �:  �:  X�  !   �:  �:  X�  %	 , �	��p`0P  �u  P  ! �4 �;  �<  ��  !   �;  �<  ��  !   �4 �;  �<  ��  !   �;  �<  ��   r0! t
 @E  hE  ��  !   t
 @E  hE  ��  !   t
 @E  hE  ��  !   t
 @E  hE  ��  !   t
 @E  hE  ��  &	 & �
���p0P  �u    ! d% `F  G  `�  !   `F  G  `�  !   d% `F  G  `�  !   `F  G  `�  '
 ) ��	��p`0P�u  8  
 
4 
2p$  �	���`P�u  �   ! t 4 0c  �c  ��  !   0c  �c  ��   t d 4
 r� d T 4 r����p d
 T	 4 Rp d 4 2p
 
4 
2p 2p! 4 pj  ~j  ��  !   pj  ~j  ��   T
 4	 2��`! t  k  ak  Ĝ  !    k  ak  Ĝ   T	 4 2�p` T	 2�! d �m  �m  �  ! t 4 �m  �m  �  !   �m  �m  �  !   �m  �m  �   d 4
 2��p! �	 T pn  �n  h�  !   �	  T pn  �n  h�  !   pn  �n  h�   t	 T 4 2�! d �o  #p  ��  !   �o  #p  ��   T	 2`! � �p  �p  ��  !
 
t 4 �p  q   �  !   �p  q   �  !   �p  �p  ��  
 T 4 2���p`! �
 `q  �q  L�  !   `q  �q  L�   2	���P0! � 
t d
 �r  s  ��  !   �  t  d
 �r  s  ��  !   �r  s  ��   2��`P0! t
 0t  Du  ��  !   0t  Du  ��   0          ����    \�           H�  P�  X�  pj  �i  j�  ��     saslSCRAM.dll sasl_client_plug_init sasl_server_plug_init   ��          �  X�  P�          ��   �  Р          T�  ��  �          b�  ��  ��          ��  8�  (�          ��  ؀  x�          ȣ  (�  �          �  Ȁ                          �      ��      F�      f�      J�      6�      �      �      ��      Ƥ      ܤ      ��      �      �      2�              �      �      J�      b�      l�      *�              f�              ��              X�      $�      @�      �      ֢      Ȣ      ��      �      ��              ��              p�      ��      z�              ܡ      ԡ      ȡ              WEVP_sha1  rHMAC  �
SHA1  libcrypto-1_1-x64.dll KERNEL32.dll  @ strchr   __C_specific_handler  % __std_type_info_destroy_list  > memset  VCRUNTIME140.dll  d strtoul � strcspn � strncpy � strncmp ! _errno   __stdio_common_vsprintf  malloc  6 _initterm 7 _initterm_e ? _seh_filter_dll  _configure_narrow_argv  3 _initialize_narrow_environment  4 _initialize_onexit_table  " _execute_onexit_table  _cexit  api-ms-win-crt-convert-l1-1-0.dll api-ms-win-crt-string-l1-1-0.dll  api-ms-win-crt-runtime-l1-1-0.dll api-ms-win-crt-stdio-l1-1-0.dll api-ms-win-crt-heap-l1-1-0.dll  �RtlCaptureContext �RtlLookupFunctionEntry  �RtlVirtualUnwind  �UnhandledExceptionFilter  sSetUnhandledExceptionFilter GetCurrentProcess �TerminateProcess  IQueryPerformanceCounter GetCurrentProcessId  GetCurrentThreadId  �GetSystemTimeAsFileTime 
GetTickCount  gInitializeSListHead }IsDebuggerPresent �IsProcessorFeaturePresent ; memcmp  < memcpy                                                                                                                                            2��-�+  �] �f�������          /                              � �       S                  �b �   @E �   �: �           �; �                                           � �       S                          �9 �   �  �   � �                                                                                                                                                                                                                                                                                     n  ��  p  �  �  �  �   �     4  �  @  �  �  �  �  �  �  "  �  $  �  $�     P  �  P  i  0�  l  �  t�  �  C  ��  P  �  ܘ  �  �  �  �  	  �    @  �  @  U  �  X  �  �  �  �  �  �  �   �  �  ,  �  ,  @  �  @  �  �  �  k  �  l    �    ,  �  ,  W  �  h  �  4�  �  �  ܘ     J  ܘ  T    D�  �  �  p�  �  �  |�  �  �   ��  �   �   ��  �   �!  ��   "  �"  ��  �"  2#  ؙ  2#  �$  �  �$  i0   �  p0  �9   �  �9  (:  <�  0:  �:  L�  �:  �:  X�  �:  �;  d�  �;  �;  |�  �;  �<  ��  �<  N>  ��  N>  l>  ��  l>  �D  К  �D  .E  �  @E  hE  ��  hE  �E  ��  �E  �E  �  �E  F  $�  F  0F  8�  0F  LF  L�  `F  G  `�  G  qG  ��  qG  �G  ��  �G  �P  ��  �P  �P  ��  �P  �b  ț  �b  c  �  0c  �c  ��  �c  e  �  e  *e  (�  0e  �e  8�  �e  �g  L�  �g  Qh  h�  `h  Xi  |�  �i  ]j  ��  pj  ~j  ��  ~j  �j  ��  �j  �j  ��   k  ak  Ĝ  ak  �k  ؜  �k  �l  �  �l  �m  ��  �m  �m  �  �m  �m  �  �m  Sn  0�  Sn  Xn  H�  Xn  dn  X�  pn  �n  h�  �n  o  |�  o  �o  ��  �o  �o  ��  �o  #p  ��  #p  Kp  Н  Kp  �p  �  �p  �p  ��  �p  q   �  q  Hq  �  Hq  Wq  ,�  Wq  Xq  <�  `q  �q  L�  �q  �r  d�  �r  �r  x�  �r  s  ��  s  �s  ��  �s  �s  ��  �s  t  О  0t  Du  ��  Du  hu  �  hu  �u  �  �u  �u  �  �u  Sv  �  pv  rv  X�  �v  �v  `�  �v  w  d�   w  'w  h�  0w  2w  l�  2w  Iw  l�  Iw  ew  l�  ew  �w  Ԙ  �w  �w  �                                                                                                                                                          �                  0  �               	  H   `�  `                  `4   V S _ V E R S I O N _ I N F O     ���             ?                        �   S t r i n g F i l e I n f o   �   0 4 0 9 0 4 b 0   V   C o m p a n y N a m e     C a r n e g i e   M e l l o n   U n i v e r s i t y     \   F i l e D e s c r i p t i o n     C M U   S A S L   s a s l S C R A M   p l u g i n   2 	  F i l e V e r s i o n     2 . 1 . 2 7 . 0     4 
  I n t e r n a l N a m e   s a s l S C R A M   � 3  L e g a l C o p y r i g h t   C o p y r i g h t   ( c )   C a r n e g i e   M e l l o n   U n i v e r s i t y   2 0 0 2 - 2 0 1 7     D   O r i g i n a l F i l e n a m e   s a s l S C R A M . d l l   `    P r o d u c t N a m e     C a r n e g i e   M e l l o n   U n i v e r s i t y   S A S L   6 	  P r o d u c t V e r s i o n   2 . 1 . 2 7 - 0     D    V a r F i l e I n f o     $    T r a n s l a t i o n     	�                                                                 �     x��������� �     h������� �     @�`�h�p�����ؠ��                                                                                                                                                                                                                                                                                                                                                                                                                                                                  <div align="center">
    <a href="https://php.net">
        <img
            alt="PHP"
            src="https://www.php.net/images/logos/new-php-logo.svg"
            width="150">
    </a>
</div>

# The PHP Interpreter

PHP is a popular general-purpose scripting language that is especially suited to
web development. Fast, flexible and pragmatic, PHP powers everything from your
blog to the most popular websites in the world. PHP is distributed under the PHP
License v3.01.

[![Build status](https://travis-ci.org/php/php-src.svg?branch=master)](https://travis-ci.org/php/php-src)
[![Build status](https://ci.appveyor.com/api/projects/status/meyur6fviaxgdwdy?svg=true)](https://ci.appveyor.com/project/php/php-src)
[![Build Status](https://dev.azure.com/phpazuredevops/php/_apis/build/status/php.php-src?branchName=PHP-7.4)](https://dev.azure.com/phpazuredevops/php/_build/latest?definitionId=1&branchName=PHP-7.4)

## Documentation

The PHP manual is available at [php.net/docs](https://php.net/docs).

## Installation

### Prebuilt packages and binaries

Prebuilt packages and binaries can be used to get up and running fast with PHP.

For Windows, the PHP binaries can be obtained from
[windows.php.net](https://windows.php.net). After extracting the archive the
`*.exe` files are ready to use.

For other systems, see the [installation chapter](https://php.net/install).

### Building PHP source code

*For Windows, see [Build your own PHP on Windows](https://wiki.php.net/internals/windows/stepbystepbuild_sdk_2).*

PHP uses autotools on Unix systems to configure the build:

    ./buildconf
    ./configure [options]

*See `./configure -h` for configuration options.*

    make [options]

*See `make -h` for make options.*

The `-j` option shall set the maximum number of jobs `make` can use for the
build:

    make -j4

Shall run `make` with a maximum of 4 concurrent jobs: Generally the maximum
number of jobs should not exceed the number of cores available.

## Testing PHP source code

PHP ships with an extensive test suite, the command `make test` is used after
successful compilation of the sources to run this test suite.

It is possible to run tests using multiple cores by setting `-jN` in
`TEST_PHP_ARGS`:

    make TEST_PHP_ARGS=-j4 test

Shall run `make test` with a maximum of 4 concurrent jobs: Generally the maximum
number of jobs should not exceed the number of cores available.

The [qa.php.net](https://qa.php.net) site provides more detailed info about
testing and quality assurance.

## Installing PHP built from source

After a successful build (and test), PHP may be installed with:

    make install

Depending on your permissions and prefix, `make install` may need super user
permissions.

## PHP extensions

Extensions provide additional functionality on top of PHP. PHP consists of many
essential bundled extensions. Additional extensions can be found in the PHP
Extension Community Library - [PECL](https://pecl.php.net).

## Contributing

The PHP source code is located in the Git repository at
[git.php.net](https://git.php.net). Contributions are most welcome by forking
the [GitHub mirror repository](https://github.com/php/php-src) and sending a
pull request.

Discussions are done on GitHub, but depending on the topic can also be relayed
to the official PHP developer mailing list internals@lists.php.net.

New features require an RFC and must be accepted by the developers. See
[Request for comments - RFC](https://wiki.php.net/rfc) and
[Voting on PHP features](https://wiki.php.net/rfc/voting) for more information
on the process.

Bug fixes **do not** require an RFC but require a bug tracker ticket. Open a
ticket at [bugs.php.net](https://bugs.php.net) and reference the bug id using
`#NNNNNN`.

    Fix #55371: get_magic_quotes_gpc() throws deprecation warning

    After removing magic quotes, the get_magic_quotes_gpc function caused a
    deprecated warning. get_magic_quotes_gpc can be used to detect the
    magic_quotes behavior and therefore should not raise a warning at any time.
    The patch removes this warning.

Pull requests are not merged directly on GitHub. All PRs will be pulled and
pushed through [git.php.net](https://git.php.net). See
[Git workflow](https://wiki.php.net/vcs/gitworkflow) for more details.

### Guidelines for contributors

See further documents in the repository for more information on how to
contribute:

- [Contributing to PHP](/CONTRIBUTING.md)
- [PHP coding standards](/CODING_STANDARDS.md)
- [Mailinglist rules](/docs/mailinglist-rules.md)
- [PHP release process](/docs/release-process.md)

## Credits

For the list of people who've put work into PHP, please see the
[PHP credits page](https://php.net/credits.php).
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                          public Func_handler_date_add_interval
{
public:
  bool fix_length_and_dec(Item_handled_func *item) const
  {
    uint dec= MY_MAX(item->arguments()[0]->datetime_precision(current_thd),
                     interval_dec(item->arguments()[1], int_type(item)));
    item->fix_attributes_datetime(dec);
    return false;
  }
  bool get_date(THD *thd, Item_handled_func *item,
                MYSQL_TIME *to, date_mode_t fuzzy) const
  {
    Datetime::Options opt(TIME_CONV_NONE, thd);
    Datetime dt(thd, item->arguments()[0], opt);
    if (!dt.is_valid_datetime() ||
         dt.check_date_with_warn(thd, TIME_NO_ZERO_DATE | TIME_NO_ZERO_IN_DATE))
      return (item->null_value= true);
    dt.copy_to_mysql_time(to);
    return (item->null_value= add(thd, item->arguments()[1],
                                  int_type(item), sub(item), to));
  }
};


class Func_handler_date_add_interval_datetime_arg0_time:
        public Func_handler_date_add_interval_datetime
{
public:
  bool get_date(THD *thd, Item_handled_func *item,
                MYSQL_TIME *to, date_mode_t fuzzy) const;
};


class Func_handler_date_add_interval_date:
        public Item_handled_func::Handler_date,
        public Func_handler_date_add_interval
{
public:
  bool get_date(THD *thd, Item_handled_func *item,
                MYSQL_TIME *to, date_mode_t fuzzy) const
  {
    /*
      The first argument is known to be of the DATE data type (not DATETIME).
      We don't need rounding here.
    */
    Date d(thd, item->arguments()[0], TIME_CONV_NONE);
    if (!d.is_valid_date() ||
         d.check_date_with_warn(thd, TIME_NO_ZERO_DATE | TIME_NO_ZERO_IN_DATE))
      return (item->null_value= true);
    d.copy_to_mysql_time(to);
    return (item->null_value= add(thd, item->arguments()[1],
                                  int_type(item), sub(item), to));
  }
};


class Func_handler_date_add_interval_time:
        public Item_handled_func::Handler_time,
        public Func_handler_date_add_interval
{
public:
  bool fix_length_and_dec(Item_handled_func *item) const
  {
    uint dec= MY_MAX(item->arguments()[0]->time_precision(current_thd),
                     interval_dec(item->arguments()[1], int_type(item)));
    item->fix_attributes_time(dec);
    return false;
  }
  bool get_date(THD *thd, Item_handled_func *item,
                MYSQL_TIME *to, date_mode_t fuzzy) const
  {
    Time t(thd, item->arguments()[0]);
    if (!t.is_valid_time())
      return (item->null_value= true);
    t.copy_to_mysql_time(to);
    return (item->null_value= add(thd, item->arguments()[1],
                                  int_type(item), sub(item), to));
  }
};


class Func_handler_date_add_interval_string:
        public Item_handled_func::Handler_temporal_string,
        public Func_handler_date_add_interval
{
public:
  bool fix_length_and_dec(Item_handled_func *item) const
  {
    uint dec= MY_MAX(item->arguments()[0]->datetime_precision(current_thd),
                     interval_dec(item->arguments()[1], int_type(item)));
    item->Type_std_attributes::set(
      Type_temporal_attributes_not_fixed_dec(MAX_DATETIME_WIDTH, dec, false),
      DTCollation(item->default_charset(),
                  DERIVATION_COERCIBLE, MY_REPERTOIRE_ASCII));
    item->fix_char_length(item->max_length);
    return false;
  }
  bool get_date(THD *thd, Item_handled_func *item,
                MYSQL_TIME *to, date_mode_t fuzzy) const
  {
    if (item->arguments()[0]->
          get_date(thd, to, Datetime::Options(TIME_CONV_NONE, thd)) ||
        (to->time_type != MYSQL_TIMESTAMP_TIME &&
         check_date_with_warn(thd, to, TIME_NO_ZEROS, MYSQL_TIMESTAMP_ERROR)))
      return (item->null_value= true);
    return (item->null_value= add(thd, item->arguments()[1],
                                  int_type(item), sub(item), to));
  }
};


class Func_handler_sign
{
protected:
  int m_sign;
  Func_handler_sign(int sign) :m_sign(sign) { }
};


class Func_handler_add_time_datetime:
        public Item_handled_func::Handler_datetime,
        public Func_handler_sign
{
public:
  Func_handler_add_time_datetime(int sign)
   :Func_handler_sign(sign)
  { }
  bool fix_length_and_dec(Item_handled_func *item) const
  {
    THD *thd= current_thd;
    uint dec0= item->arguments()[0]->datetime_precision(thd);
    uint dec1= Interval_DDhhmmssff::fsp(thd, item->arguments()[1]);
    item->fix_attributes_datetime(MY_MAX(dec0, dec1));
    return false;
  }
  bool get_date(THD *thd, Item_handled_func *item,
                MYSQL_TIME *to, date_mode_t fuzzy) const
  {
    DBUG_ASSERT(item->fixed());
    Datetime::Options opt(TIME_CONV_NONE, thd);
    Datetime dt(thd, item->arguments()[0], opt);
    if (!dt.is_valid_datetime())
      return (item->null_value= true);
    Interval_DDhhmmssff it(thd, item->arguments()[1]);
    if (!it.is_valid_interval_DDhhmmssff())
      return (item->null_value= true);
    return (item->null_value= (Sec6_add(dt.get_mysql_time(),
                                        it.get_mysql_time(), m_sign).
                               to_datetime(to)));
  }
};


class Func_handler_add_time_time:
        public Item_handled_func::Handler_time,
        public Func_handler_sign
{
public:
  Func_handler_add_time_time(int sign)
   :Func_handler_sign(sign)
  { }
  bool fix_length_and_dec(Item_handled_func *item) const
  {
    THD *thd= current_thd;
    uint dec0= item->arguments()[0]->time_precision(thd);
    uint dec1= Interval_DDhhmmssff::fsp(thd, item->arguments()[1]);
    item->fix_attributes_time(MY_MAX(dec0, dec1));
    return false;
  }
  bool get_date(THD *thd, Item_handled_func *item,
                MYSQL_TIME *to, date_mode_t fuzzy) const
  {
    DBUG_ASSERT(item->fixed());
    Time t(thd, item->arguments()[0]);
    if (!t.is_valid_time())
      return (item->null_value= true);
    Interval_DDhhmmssff i(thd, item->arguments()[1]);
    if (!i.is_valid_interval_DDhhmmssff())
      return (item->null_value= true);
    return (item->null_value= (Sec6_add(t.get_mysql_time(),
                                        i.get_mysql_time(), m_sign).
                                 to_time(thd, to, item->decimals)));
  }
};


class Func_handler_add_time_string:
        public Item_handled_func::Handler_temporal_string,
        public Func_handler_sign
{
public:
  Func_handler_add_time_string(int sign)
   :Func_handler_sign(sign)
  { }
  bool fix_length_and_dec(Item_handled_func *item) const
  {
    uint dec0= item->arguments()[0]->decimals;
    uint dec1= Interval_DDhhmmssff::fsp(current_thd, item->arguments()[1]);
    uint dec= MY_MAX(dec0, dec1);
    item->Type_std_attributes::set(
      Type_temporal_attributes_not_fixed_dec(MAX_DATETIME_WIDTH, dec, false),
      DTCollation(item->default_charset(),
                  DERIVATION_COERCIBLE, MY_REPERTOIRE_ASCII));
    item->fix_char_length(item->max_length);
    return false;
  }
  bool get_date(THD *thd, Item_handled_func *item,
                MYSQL_TIME *to, date_mode_t fuzzy) const
  {
    DBUG_ASSERT(item->fixed());
    // Detect a proper timestamp type based on the argument values
    Temporal_hybrid l_time1(thd, item->arguments()[0],
                            Temporal::Options(TIME_TIME_ONLY, thd));
    if (!l_time1.is_valid_temporal())
      return (item->null_value= true);
    Interval_DDhhmmssff l_time2(thd, item->arguments()[1]);
    if (!l_time2.is_valid_interval_DDhhmmssff())
      return (item->null_value= true);
    Sec6_add add(l_time1.get_mysql_time(), l_time2.get_mysql_time(), m_sign);
    return (item->null_value= (l_time1.get_mysql_time()->time_type ==
                                 MYSQL_TIMESTAMP_TIME ?
                               add.to_time(thd, to, item->decimals) :
                               add.to_datetime(to)));
  }
};


class Func_handler_str_to_date_datetime_sec:
        public Item_handled_func::Handler_datetime
{
public:
  bool fix_length_and_dec(Item_handled_func *item) const
  {
    item->fix_attributes_datetime(0);
    return false;
  }
  bool get_date(THD *thd, Item_handled_func *item,
                MYSQL_TIME *to, date_mode_t fuzzy) const
  {
    return static_cast<Item_func_str_to_date*>(item)->
             get_date_common(thd, to, fuzzy, MYSQL_TIMESTAMP_DATETIME);
  }
};


class Func_handler_str_to_date_datetime_usec:
        public Item_handled_func::Handler_datetime
{
public:
  bool fix_length_and_dec(Item_handled_func *item) const
  {
    item->fix_attributes_datetime(TIME_SECOND_PART_DIGITS);
    return false;
  }
  bool get_date(THD *thd, Item_handled_func *item,
                MYSQL_TIME *to, date_mode_t fuzzy) const
  {
    return static_cast<Item_func_str_to_date*>(item)->
             get_date_common(thd, to, fuzzy, MYSQL_TIMESTAMP_DATETIME);
  }
};


class Func_handler_str_to_date_date: public Item_handled_func::Handler_date
{
public:
  bool get_date(THD *thd, Item_handled_func *item,
                MYSQL_TIME *to, date_mode_t fuzzy) const
  {
    return static_cast<Item_func_str_to_date*>(item)->
             get_date_common(thd, to, fuzzy, MYSQL_TIMESTAMP_DATE);
  }
};


class Func_handler_str_to_date_time: public Item_handled_func::Handler_time
{
public:
  bool get_date(THD *thd, Item_handled_func *item,
                MYSQL_TIME *to, date_mode_t fuzzy) const
  {
    if (static_cast<Item_func_str_to_date*>(item)->
         get_date_common(thd, to, fuzzy, MYSQL_TIMESTAMP_TIME))
      return true;
    if (to->day)
    {
      /*
        Day part for time type can be nonzero value and so
        we should add hours from day part to hour part to
        keep valid time value.
      */
      to->hour+= to->day * 24;
      to->day= 0;
    }
    return false;
  }
};


class Func_handler_str_to_date_time_sec: public Func_handler_str_to_date_time
{
public:
  bool fix_length_and_dec(Item_handled_func *item) const
  {
    item->fix_attributes_time(0);
    return false;
  }
};


class Func_handler_str_to_date_time_usec: public Func_handler_str_to_date_time
{
public:
  bool fix_length_and_dec(Item_handled_func *item) const
  {
    item->fix_attributes_time(TIME_SECOND_PART_DIGITS);
    return false;
  }
};


#endif /* ITEM_TIMEFUNC_INCLUDED */
