�Y �               �07a       �    �   w      �07a          ��  �w  session_info    Pavan Naik, Andrey Hristov      Test session information        test_session_info       ============================================================================================
   localhost       127.0.0.1   root                � �   � �   � �   � �   � �     �    �     �   � �   @ �    �   P �   0 �   p �   � �     �   � �   � �   0 �   "�   ،     ��     P�             �� �   @ �   �Y �   �Y �   �& �   �& �   �& �   8� �   � �   P0 �   / �   �& �   �& �   �& �   generic unknown error   �� �   � �   `0 �   �/ �   �& �   �& �   �& �   iostream        iostream stream error   @� �    �   p0 �   �/ �   P& �   �& �   �& �   system  %d  [NULL]  %llu    %lld    +   -       %s%d.%d(%d)[%s] %3.7g   %s%4d-%02d-%02d %s%02d:%02d:%02d        %s%4d-%02d-%02d %02d:%02d:%02d  %s      
       num_cols      : %d
     nb rows       : %d
     error     : %d
 error msg : %s
 %s
     test_session_info - ret code : %d at %s:%u              test_session_info - expected error but command did not fail at %s:%u    affected rows : %d
     server status : %d
     warn count    : %d
     current_db before init_db : %s
 run_statement code: %d
 current_db after init_db  : %s
 Opening Session 1
      Opening Session 1 failed        Opening Session 2
      Opening Session 2 failed                Session 1 : srv_session_info_get_thd and srv_session_info_get_session_id
       Thread handler id is NOT equal to session id srv_session_info_get_session_id(session_1)         Thread handler id IS equal to session id returned by srv_session_info_get_session_id(Session_1)

               Session 2 : srv_session_info_get_thd and srv_session_info_get_session_id
       Thread handler id is NOT equal to session id srv_session_info_get_session_id(session_2)         Thread handler id IS equal to session id returned by srv_session_info_get_session_id(Session_2)

               SELECT name,type,processlist_id,processlist_user,processlist_host,processlist_db,processlist_command,processlist_state,processlist_info,role,instrumented,history,connection_type FROM performance_schema.threads WHERE processlist_id =  %lu   test_sql        Session 1 : srv_session_info_get_current_db
    /*Session_1*/ SHOW TABLES LIKE '%slave%'    mysql       /*Session_1*/ USE information_schema    current_db after 'USE db_name' command : %s

   test    /*Session_1*/ SHOW TABLES       Session 2 : srv_session_info_get_current_db
    /*Session_2*/ SHOW TABLES LIKE '%slave%'        Session 2's view
       /*Session_2*/ USE information_schema    /*Session_2*/ SHOW TABLES       Session 1 : srv_session_info_set/get_client_port
       Port before srv_session_info_set_client_port : %d
      Port after srv_session_info_set_client_port  : %d

     Session 1's view
               /*Session_1*/ SELECT host FROM INFORMATION_SCHEMA.PROCESSLIST WHERE info LIKE 'PLUGIN%' ORDER BY id             /*Session_2*/ SELECT host FROM INFORMATION_SCHEMA.PROCESSLIST WHERE info LIKE 'PLUGIN%' ORDER BY id     Session 2 : srv_session_info_set/get_client_port
       Session 1 : srv_session_info_set_connection_type
               SELECT CONNECTION_TYPE, CONNECTION_TYPE IS NULL FROM performance_schema.threads WHERE PROCESSLIST_ID =  %lu /*session_1_id*/    Setting NO_VIO_TYPE on session_1
               srv_session_info_set_connection_type(NO_VIO_TYPE) should fail but did not       SELECT CONNECTION_TYPE FROM performance_schema.threads WHERE PROCESSLIST_ID =  %lu /*session_1_id*/     Setting VIO_TYPE_TCPIP on session_1
    Setting VIO_TYPE_NAMEDPIPE on session_1
        Session 2 : srv_session_info_set_connection_type
               SELECT CONNECTION_TYPE FROM performance_schema.threads WHERE PROCESSLIST_ID =  %lu /*session_2_id*/     Setting VIO_TYPE_SSL on session_2
      SELECT CONNECTION_TYPE FROM performance_schema.threads WHERE PROCESSLIST_ID =  %lu  /*session_2_id*/    BEFORE kill of Session 1
               SELECT ID, USER, HOST, DB, COMMAND, INFO FROM INFORMATION_SCHEMA.PROCESSLIST WHERE info LIKE 'PLUGIN%' ORDER BY id      srv_session_info_killed(Session_1) : %d
        srv_session_info_killed(Session_2) : %d
        Killing Session 1
      KILL CONNECTION %lu /*session_1_id*/    AFTER kill of Session 1
        srv_session_info_killed(Session 1) : %d
        srv_session_info_killed(Session 2) : %d
        Closing Session 1
      Closing Session 1 failed as expected. It was already closed by EXEC_TEST_CMD
   Get/Set session info with closed session(Session 1)
    srv_session_info_get_thd             : %d
      srv_session_info_get_session_id      : %d
      srv_session_info_set_client_port     : %d
      srv_session_info_get_client_port     : %d
      srv_session_info_get_current_db      : %s
      srv_session_info_set_connection_type : %d
      Perform KILL QUERY and suicide (KILL CONNECTION) on Session 2
  KILL QUERY %i /*session_2_id*/  KILL CONNECTION %i  /*session_2_id*/    Get/Set session info with killed session(Session 2)
    Opening Session 3 failed                Session 2 got killed but not closed, thus it will appear in the processlist as Killed
          /*Session 3*/SELECT ID, USER, HOST, DB, COMMAND, INFO FROM INFORMATION_SCHEMA.PROCESSLIST WHERE info LIKE 'PLUGIN%' ORDER BY id Closing Session 2
      Closing Session 2 failed        Closing Session 3
      Closing Session 3 failed        init thread
    srv_session_init_thread failed. deinit thread
  .log    Could not create test session thread    Installation.   test_service_sql        Test in a server thread
        Follows threaded run
   Uninstallation. string too long invalid string position        ȋ �   �[ �           � �   �� �                       p                                                                                       �� �                   RSDSi��Ő�K�*�����A   C:\build\sb_1-4394188-1631005756.63\release\plugin\test_service_sql_api\RelWithDebInfo\libtest_session_info.pdb                                  ��  �  ��                              �          �              ��          ����    @   �                         ȡ  `�  8�                             x�          ��  �                  ȡ         ����    @   `�                          �  ��  ��                             ��          �  ��  �                       �         ����    @   ��                         8�  h�  @�                             ��          ��  ��  �                      8�         ����    @   h�                         p�  ��  ȋ                             �          �              p�          ����    @   ��                                               20 B   B   20 20 d T 4 2p d 4 2p B  
 
4 
2p! b�p`0  �Z  �s  ����    ����                                 �              4�  @           �f  8   @           $g  H           p  �����         �����f      �f     �f     g      2g     
 
2P�Z  �s   BP0  �Z  �s  - -T 
4	 
2p! d �   �   ��  ! � �   !  č  !   �   !  č  !   �   �   ��  
 
4 
2p! d �!  �!  �  !   �!  �!  �   B   B   R0 20
 
4 
Rp 20 R0 20
 
4 
Rp
 
4 
2p 20 R0 20
 
4 
2p 4 '	 T� 4� � �p`  �Z     %	 4� � �
�p`P  �Z     '	 T� 4� � �p`  �Z  @  / hC T� 4� � �p`  �Z     (	 d� 4� � ��p  �Z  @  %	 4� � �
�p`P  �Z  @  %	 4� � �
�p`P  �Z  P   d 4 2p H p  �Z     ! �F dG 4K �*  +  ��  ! TL +  �+  ̏  !   +  �+  ̏  !   dG �*  +  ��  !   �*  +  ��   4I F p  �Z       L �p`P0  �Z  P  ! 4Y R p`P  �Z  �  8 't� 'd� '4� '� ����P  �Z  p  $ dJ 4I F p  �Z      I �Z  0   �   	H 0  �Z  0  	 	B   B   B   20�`     CZ  �Z  bg       2P 20 20 B      d 4 2p � t
 T	 4 2��� 20 t
 d	 4 R��`     __  B`  }g  B`  Y_  H`  �g       2P 2P d 4 2p B   b   B   20	 	b   b�p`0  �`     �c  �c  �g       2P	 4 r�p`�`     d   d  �g   d   2P B  	
 
4 
2p�`     �d  �d  0h  �d   2P 4	 2P B  
 
4 
2p
 
4 
2p                �07a    R�     	   	   ��  �  @�  0�  �   �  `�  h�  P�  p�  X�  H�  k�  ��  ��  ȓ  ؓ  �  �  �  4�            libtest_session_info.dll _mysql_plugin_declarations_ _mysql_plugin_interface_version_ _mysql_sizeof_struct_st_plugin_ command_service my_plugin_log_service my_snprintf_service security_context_service srv_session_info_service srv_session_service 0�          ��  �q   �          ��  Pp  0�          <�  �p  ��          ��   p                          �      ��      r�      \�      F�      ,�      �      ��      ܙ              �      @�      b�      ��      �              ��      ޗ      �      �      ��      �      Η      ��      2�      J�      R�      \�      ��      x�      ��      ��      ��      ��            И      ܘ      �       �      �      2�      P�      n�      ��      ��      (�      j�              �      ̖      ��      ��      ��      ��      ��      x�              ��my_thread_join  j�my_open ��my_close  1�my_write  ��fn_format ~�thd_get_thread_id ��my_charset_utf8_general_ci  ,�PSI_server  mysqld.exe  �?_Xbad_alloc@std@@YAXXZ �?_Xlength_error@std@@YAXPEBD@Z  �?_Xout_of_range@std@@YAXPEBD@Z  �?_Syserror_map@std@@YAPEBDH@Z �?_Winerror_map@std@@YAPEBDH@Z MSVCP120.dll  _purecall p ??2@YAPEAX_K@Z  r ??3@YAXPEAX@Z �memmove strncpy �_hypot  E_CxxThrowException  b__CxxFrameHandler3  �memcpy  �memset  MSVCR120.dll  [_lock �_unlock 	_calloc_crt �__dllonexit \__C_specific_handler  _onexit ]__CppXcptFilter �_amsg_exit  Pfree  l_malloc_crt �_initterm �_initterm_e �__crt_debugger_hook �__crtUnhandledException �__crtTerminateProcess }__crtCaptureCurrentContext  ~__crtCapturePreviousContext 3?terminate@@YAXXZ ?_type_info_dtor_internal_method@type_info@@QEAAXXZ {__clean_type_info_names_internal  %EncodePointer � DecodePointer jIsDebuggerPresent pIsProcessorFeaturePresent 0QueryPerformanceCounter GetCurrentProcessId GetCurrentThreadId  �GetSystemTimeAsFileTime DisableThreadLibraryCalls KERNEL32.dll  �_unlink                                                                                                                                                                                                                                                                                                                                 �� �   Hr �            0�   h                      � �   �r �   Xr �   xr �          0 �   � �                                                                                                                                                  �r �   �r �   s �    s �   1s �   ,s �   @t �   �t �   �t �                                                    2��-�+  �] �f���u�  ����x� �           .?AVerror_category@std@@        x� �           .?AV_Generic_error_category@std@@       x� �           .?AV_Iostream_error_category@std@@      x� �           .?AV_System_error_category@std@@        x� �           .?AVtype_info@@                                                                                                                                                                                                                                                                                                                                                                                 0  ~  ܐ  �  �  �  �  �  ��  �  9  Ď  @    �    I  �  P  '  $�  0  b  H�  p  �  h�  �    ��     ~  ��  @  g  ��  0  l  x�  �  �  ��  �    ��  �  �  h�  �    x�    6  ��  @  f  T�  p  r   ��  �   �   ��  �   !  č  !  5!  ؍  5!  L!  �  L!  w!  ��  �!  �!  �  �!  "  �  "  8"  ,�  @"  R"  <�  `"  r"  D�  �"  �"  p�  �"  �"  h�  �"  $  ��  0$  _%  ��  �%  �%  `�  �%  '&  Đ  P&  �&  ��  �&  �&  L�   '  �'  ��  �'  (  ��  (  �*  H�  �*  +  ��  +  �+  ̏  �+  �,  �  �,  �,  ��  �,  �-  �  �-  �-   �  �-  s.  0�  /  r/  \�  �/  �/  p�  �/  20  ��  �0  2  ��   2  u2  ��  �2  =4  d�  @4  �4  Ԑ  �4  �W  ��  �W  Y  ��  �Y  
Z  0�  Z  �Z  �  �Z  �Z   �  �Z  [  @�  [  w[  8�  �[  �[  H�  �[  :\  L�  <\  �\  x�  �\  �^  \�  �^  1_  ̑  4_  j`  ��  �`  �`  ��  �`  �a  ��  �a  �a  �  �a  Wb  ܑ  Xb  fc  �  hc  �c  �  �c  �c  d�  �c  .d  4�  �d  �d  l�  e  �e  ��  �e  �e  ��  �e  $f  ��  $f  \f  ��  �f  $g  ��  $g  bg  ��  bg  }g  (�  }g  �g  ��  �g  �g  đ  �g  �g  ,�  �g  )h  \�  0h  Ph  ��                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                           �   8  �                  P  �                  h  �               	  �                  	  �   ��  H          ��  }          H4   V S _ V E R S I O N _ I N F O     ���       $     $ ?                         �    S t r i n g F i l e I n f o   �    0 4 0 9 0 4 E 4   2 	  F i l e V e r s i o n     5 . 7 . 3 6 . 0     6 	  P r o d u c t V e r s i o n   5 . 7 . 3 6 . 0     D    V a r F i l e I n f o     $    T r a n s l a t i o n     	�<?xml version='1.0' encoding='UTF-8' standalone='yes'?>
<assembly xmlns='urn:schemas-microsoft-com:asm.v1' manifestVersion='1.0'>
  <trustInfo xmlns="urn:schemas-microsoft-com:asm.v3">
    <security>
      <requestedPrivileges>
        <requestedExecutionLevel level='asInvoker' uiAccess='false' />
      </requestedPrivileges>
    </security>
  </trustInfo>
</assembly>
                                                                                                                                                            p  p   Сء��� �@�H�P�X�`�h�p�x�������������������ȣУ ���� �(�0�8�@�H�P�X�`�h������������������� ���� �     p�x������   �  4    ��8�@�H�P�`�h� ���� �(�0�8�@���ȡ �8�p�                                                                                                                                                                                                                                                                                                                                        8    0�+	*�H����0�10	`�He 0\
+�7�N0L0
+�70	 ��� 010	`�He  �OlhD��}(T�,�Ů��VF}�̣��
�0�00��	_ջfuSC�o�P0	*�H�� 0e10	UUS10U
DigiCert Inc10Uwww.digicert.com1$0"UDigiCert Assured ID Root CA0131022120000Z281022120000Z0r10	UUS10U
DigiCert Inc10Uwww.digicert.com110/U(DigiCert SHA2 Assured ID Code Signing CA0�"0	*�H�� � 0�
� �ӳ�gw�1I���E��:�D�娝�2�q�v�.����C�����7׶�𜆥�%�y(:~��g���)'��{#��#��w����#fT3Pt�(&�$i��R�g��E�-���, ��J����M`��Ĳ�p1f3q>�p����|˒��;1���
�W�J��t�+�l�~t96���N���j
���gN����� %#�d>R����Ŏ���,Q�s����b�sA��8�js �ds<���3���%�� ���0��0U�0� 0U��0U%0
+0y+m0k0$+0�http://ocsp.digicert.com0C+0�7http://cacerts.digicert.com/DigiCertAssuredIDRootCA.crt0��Uz0x0:�8�6�4http://crl4.digicert.com/DigiCertAssuredIDRootCA.crl0:�8�6�4http://crl3.digicert.com/DigiCertAssuredIDRootCA.crl0OU H0F08
`�H��l 0*0(+https://www.digicert.com/CPS0
`�H��l0UZĹ{*
���q�`�-�euX0U#0�E뢯��˂1-Q���!��m�0	*�H�� � >�Z$��"��,|%)v�]-:��0a�~`��=į���*� U7���ђuQ�n��Z�^$�N��?q�cK��_Dy�6���FN\��������Q$�$��'*�)(:q(<.���%�G�zhh���\ \�q������h��@�@D���d%B2�6�$�/r~�IE��Y��tdk��fCڳ������ Ι1c=���OƓ�������I�bn�S���.���hlD2�f����dQ�0�0�g�	X�땝;ȹ�����0	*�H�� 0r10	UUS10U
DigiCert Inc10Uwww.digicert.com110/U(DigiCert SHA2 Assured ID Code Signing CA0200107000000Z220111120000Z0��10	UUS10U
California10URedwood City10U
Oracle America, Inc.10USoftware Engineering10UOracle America, Inc.1#0!	*�H��	pkiadm_us@oracle.com0�"0	*�H�� � 0�
� ��A��l%����(9T�8�_ô
V�9�����dQ�-�؊�8�@�S�h�%��N����!��uʘB�J��lx�b]�e6I�kɚ+z�e�m����R!?��f~C����vC�.�r,k�ҭX���' b~�A!�Q�V(ߙ/t"WY-��2��I��̿���\��ם�1�q�:X�'�0v��&1SH��'�Ii����w�7�.W�x��E%=z@�"��ŒrO�e�%ۿ�W7Ǿ�̰���Q���; ���0��0U#0�ZĹ{*
���q�`�-�euX0UST�O".�)3�<���u�7��0U��0U%0
+0wUp0n05�3�1�/http://crl3.digicert.com/sha2-assured-cs-g1.crl05�3�1�/http://crl4.digicert.com/sha2-assured-cs-g1.crl0LU E0C07	`�H��l0*0