t specific config %s
   Address starts with '/', so assume "unix" domain
   unix        Use user specified default domain "%s"
 No default domain found, assume "udp"
  Use application default domains  "%s"   trying domain "%s" address "%s" default address "%s"
   transport domain %s uses deprecated f_create_from_tstring
      No support for any checked transport domain
    No domain(s) in registration of defDomain "%s"  Default transport already registered for this application       ..\..\snmplib\snmp_service.c    netsnmp_lookup_default_domain("%s") ->  defaults         "[NIL]"
       No domain and target in registration of defTarget "%s"  No target in registration of defTarget "%s" "%s"        Trailing junk found             Default target already registered for this application-domain combination       netsnmp_lookup_default_target("%s", "%s") -> "%s"
      application domain      defDomain   snmp:       application domain target       defTarget       auth message    ..\..\snmplib\snmp_auth.c       SNMP version    community string        bad parse of community  xdump: malloc failed. packet-dump skipped
  %.4d:       var_op  ..\..\snmplib\snmp.c    Name    No OID for variable     No header for value     Can't build OID for variable    wrong type in snmp_build_var_op: %d     wrong type in snmp_realloc_rbuild_var_op: %d    enum    ..\..\snmplib\snmp_enum.c       ((void *)0) != snmp_enum_lists  %d:%d   %d: read        vacmviews   write   notify  log execute net     ..\..\snmplib\snmp_alarm.c      snmp_alarm      ALL alarms unregistered
        alarm_tm        ((&sa_ptr->t_nextM)->tv_sec || (&sa_ptr->t_nextM)->tv_usec)     no alarms found to schedule
    ..\..\snmplib\container.c         _factory_free_list() called for %s
   container       container list  binary_array    table_container sorted_singly_linked_list       linked_list     ssll_container  cstring string_binary_array     netsnmp_container_free_list() called
   replacing previous container factory
   container_registry      registered container factory %s (%s)
   error on subcontainer '%s' insert (%d)
 error on subcontainer '%s' cfree (%d)
  netsnmp_container_simple_free) called for %p/%p
        verbose:container       ..\..\snmplib\large_fd_set.c    fd != (SOCKET)(~0)      %u.%u.%u.%u %x  ..\..\snmplib\transports\snmpUDPBaseDomain.c    got source addr: %s
    udpbase:recv    r2 == 0 dstip->sa_family == 2   got destination (local) addr %s, iface %d
      sending from [%d] %s
   udpbase:sendto  sending from [%d] %s failed: %d
        netsnmp_udp     recvfrom fd %d got %d bytes (from %s)
  recvfrom fd %d err %d ("%s")
   send %d bytes from %p to %s on fd %d
   sendto error, rc %d (errno %d)
 s != (-1)       WSARecvMsg() not found (errno %ld)
     WSASendMsg() not found (errno %ld)
 UDP TCP     ..\..\snmplib\transports\snmpTCPDomain.c        accept: malloc failed
  netsnmp_tcp     accept failed rc %d errno %d "%s"
      accept succeeded (from %s)
     couldn't f_getfl of fd %d
                      UDP/IPv6        netsnmp_udp6    ..\..\snmplib\transports\snmpUDPIPv6Domain.c    local   remote  open %s %s
     couldn't set IPV6_V6ONLY to %d bytes: %s
   ipv6    udpv6       udpipv6 TCP/IPv6        ..\..\snmplib\transports\snmpTCPIPv6Domain.c    netsnmp_tcp6    accept: couldn't f_getfl of fd %d
      connect returns %d
 tcpv6       tcpipv6 ..\..\snmplib\container_binary_array.c  t!=((void *)0)  c->compare!=((void *)0) linear search on sorted container %s?!?
        non-exact search on unsorted container %s?!?
   skipping duplicate key in %s
   not inserting duplicate key
    c->ncompare     binary arry duplicate does not supprt flags yet
         no memory for binary array duplicate
  no memory for binary array duplicate
   couldn't allocate memory
       ((void *)0) != it       ((void *)0) != it->base.container       ((void *)0) != it->base.container->container_data       out of sync
    container:iterator  empty
      end of container
       ((void *)0) != t        unsorted_singly_linked_list-lifo        unsorted_singly_linked_list-fifo        unsorted_singly_linked_list lifo    fifo        ..\..\snmplib\container_list_ssll.c     ..\..\snmplib\container_null.c  in
     container:null:find     container:null:find_next        container:null:insert   container:null:remove   container:null:free     container:null: container:null:size     container:null:for_each container:null:get_subset       container:null:clear    container:null:get_null container:null:get_null_factory ..\..\snmplib\transports\snmpSocketBaseDomain.c Current %s is %d
       verbose:socket:buffer:max       Requested %s <= current buffer
 Maximized %s: %d
       socket:buffer:max       Get %s failed ... giving up!
   server send buffer      client send buffer      server receive buffer   client receive buffer   unknown buffer  Requested %s is %d
     socket:buffer   verbose:socket:buffer   Original %s is %d
      New %s size is smaller than original!
  %s not valid or not specified; using OS default(%d)
    Set %s to %d
   Now %s is %d
   couldn't set %s to %d
  ..\..\snmplib\transports\snmpIPv4BaseDomain.c   addr %p, inpeername "%s", default_target "%s"
  netsnmp_sockaddr_in     check user service %s
  servname not numeric, check if it really is a destination)
     servname not numeric
   check destination %s
   Explicit UDP broadcast
 couldn't resolve hostname
      return { AF_INET, %s:%hu }
     %s: unknown     %s: [%s]:%hu->  [%s]:%hu        ..\..\snmplib\transports\snmpUDPIPv4BaseDomain.c        t != ((void *)0)        netsnmp_udpbase openned socket %d as local=%d
  UDPBase couldn't set IP_PKTINFO: %d
    set IP_PKTINFO
 binding socket: %d
     failed to bind for clientaddr: %d %s
   rc2 == 0        client open %s
 0123456789abcdef        0123456789ABCDEF        ..\..\snmplib\transports\snmpTCPBaseDomain.c    recv fd %d err %d ("%s")
       netsnmp_tcpbase recv fd %d got %d bytes
                        ..\..\snmplib\transports\snmpIPv6BaseDomain.c   fmtaddr: t = %p, data = %p, len = %d
   netsnmp_ipv6    %s: [%s%s]:%hu  addr %p, peername "%s", default_target "%s"
    netsnmp_sockaddr_in6_2  totally numeric: %d
    IPv6 address with port suffix :%d
      invalid port number: %d IPv6 address with square brackets
      just IPv6 address
      hostname(?) with port suffix :%d
       hostname(?) with embedded ':'?
 empty hostname
 hostname (not AF_INET6!)
       NULL peername   return { AF_INET6, [%s]:%hu }
  pj�   k�           host    community       object_id       timeout retries type    value   d       quick_print     enum_print      oid_forma