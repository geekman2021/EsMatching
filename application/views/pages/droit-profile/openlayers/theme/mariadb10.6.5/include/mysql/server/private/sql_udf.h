Priv    authNoPriv      authPriv    GET GETNEXT GETBULK SET     RESPONSE    INFORM  TRAP2   REPORT      ?0x%2X? Unknown error: %d       %s (%s) Unknown Error %d     (%s)       udp udp6        snmptrap    :161    udp tcp udp6    tcp6    :10161      dtlsudp tlstcp  /36879  ipx :162    :10162  /36880      dumpPacket      reverseEncodeBER        defaultPort     defCommunity    noTokenWarnings noRangeCheck    persistentDir   tempFilePattern noDisplayHint   16bitIDs        clientaddr      serverSendBuf   serverRecvBuf   clientSendBuf   clientRecvBuf   noPersistentLoad        noPersistentSave        noContextEngineIDDiscovery      timeout retries ..\..\snmplib\snmp_api.c        snmp_store      storing stuff...
       probing for engineID using security model callback...
  configuring transport
  snmp_sess       opening transport: %x
  couldn't interpret peername
    done opening transport: %x
     tcp,tcp6        udp,udp6        _sess_open      couldn't enable UDP_BROADCAST
  fd %d
  snmp_sess_add   adding v3 session -- maybe engineID probe now
  engine ID probe failed
 closing session %p, subsession %p
      snmp_sess_close 0 == (pdu->flags & 0x200)       snmp_secmods    ERROR: undefined        Building SNMPv3 message (secName:"%s", secLevel:%s)...
 snmp_build      SNMPv3 Message  SNMP Version Number     msgGlobalData   msgID   msgMaxSize      msgFlags        msgSecurityModel        contextEngineID contextName     ScopedPdu       SM msgSecurityParameters        no such security service available: %d
 security service %d doesn't support reverse encoding.
  security service %d doesn't support forward out encoding.
      Building SNMPv%ld message...
   snmp_send       Community String        SNMPv1 Message  SNMPv2c Message snmp_build: unknown failure
    request_id      error status    error index     enterprise OBJID        agent Address   generic trap number     specific trap number    timestamp       VarBindList     VarBind starting
       snmp_pdu_realloc_rbuild wrapped
        version SNMP Version    message bad parse of version    error parsing msgID 0   <       Received bad msgID (%ld %s %s).
        error parsing msgMaxSize        Received bad msgMaxSize (%lu < 484).
   msgMaxSize %lu received
        snmpv3_parse    error parsing msgFlags  error parsing msgSecurityModel  unknown security model: %ld
    invalid message, illegal msgFlags       security service %ld can't decode packets
      ScopedPDU       error parsing PDU       Security Model %d can't free state references
  Can't find security model to free ptr: %d
      Report type: %d
    report      Parsing SNMPv%ld message...
    SNMPv1 message
 SNMPv2c message
    PDU Parsed SNMPv3 message (secName:%s, secLevel:%s): %s
    snmp_parse      starting context ID discovery
  snmpv3_contextid          One correct variable found
     responding with our engineID
   sent it off!
 sending a response to the context engineID probe failed
        failed to get our own engineID!
        received an odd context engineID probe
 error parsing snmp message version      unsupported snmp message version            Command %s
 Bad PDU type received: 0x%.2x
  varbinds    Value       bad type returned (%x)
 plaintext scopedPDU     error parsing contextEngineID from scopedPdu    error parsing contextName from scopedPdu        error copying contextName from scopedPdu        send fail: closing...
  sess_async_send delayed probe for engineID
     snmpv3_build    couldn't malloc initial packet buffer
  encoding failure
       length of packet (%lu) exceeds session maximum (%lu)
   length of packet (%lu) exceeds transport maximum (%lu)
 sending message id#%ld reqid#%ld len %I64u
     sess_process_packet     can't malloc space for PDU
     session %p fd %d pkt %p length %d
      
Received %d byte packet from %s
       pre-parse fail
 pdu failed to be created
       received message id#%ld reqid#%ld len %u
       parse fail
     post-parse fail
        unmatched msg id: %ld != %ld
   unhandled PDU
  read fail: closing...
  sess_read       transport->sock got negative fd value %d
       not reading %d (fdset %p set %d)
       perform callback with op=CONNECT
       can't malloc %I64u bytes for rxbuf
     can't malloc %I64u more for rxbuf (%I64u tot)
  perform callback with op=DISCONNECT
    fd %d closed
     loop packet_len %I64u, PDU length %I64u
      Received broken packet. Closing session.
       pkt not complete (need %I64u got %I64u so far)
 too large packet_len = %I64u, dropping connection %d
   end: memmove(%p, %p, %I64