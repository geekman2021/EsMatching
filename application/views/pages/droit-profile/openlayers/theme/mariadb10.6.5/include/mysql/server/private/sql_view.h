security model name: %s.  Forcing USM instead.
 parse bitstring build bitstring parse uint64    parse int64     parse int       parse float     parse uint      build int       parse double    build uint      parse string    build string    build seq       parse length    build length    parse objid     %s size %lu: s/b %lu    %s type %d      %s length %lu too large: exceeds %lu    %s: message overflow: %d len + %d delta > %d len        %s: bad header, length too short: %lu < %lu     %s: length %d too small recv    dumpx_%s:%*s    dumpv_%s:%*s      Integer:	%ld (0x%.2lX)
         UInteger:	%ld (0x%.2lX)
  send          String:	%s
     String:	[TRUNCATED]
    String:	%s [TRUNCATED]
       parse header: NULL pointer      can't process ID >= 30  parse header    parse opaque header     %s header type %02X: s/b %02X   bad header length < 1 :%lu, %lu %s: length %d < 4: PUNT parse length: NULL pointer      %s: indefinite length not supported     %s: data length %d > %lu not supported  %s: negative data length %ld
   %s: bad length < 1 :%lu, %lu    %s: bad length < 2 :%lu, %lu    %s: bad length < 3 :%lu, %lu    subidentifier syntax error      OID length exceeds buffer size    ObjID:        build objid: bad first subidentifier    build objid: bad second subidentifier   build objid       NULL
   Bitstring:    no string passed into asn_build_bitstring
      parse opaque uint64     Counter64: %s
  build uint64    build counter u64       build opaque u64        parse opaque int64      %s: wrong type: %d, len %d, buf bytes (%02X,%02X)       Integer64: %s
  build int64     parse opaque float      parse seq float Opaque float: %f
       build float     parse opaque double     parse seq double          Opaque Double:	%f
    build double      Opaque double: %f
    ..\..\snmplib\asn1.c     old_pkt %8p, old_pkt_len %lu
  asn_realloc      new_pkt %8p, new_pkt_len %lu
   memmove(%8p + %08x, %8p, %08x)
         CANNOT REALLOC()
      %s: bad length < 1 :%ld, %lu    bad header length < 1 :%ld, %lu   String: [NULL]
         UInteger:	%lu (0x%.2lX)
        Bitstring: [NULL]
      Bitstring:	%s
          Bitstring:	[TRUNCATED]
         Bitstring:	%s [TRUNCATED]
      U64:	%lu %lu
   UInt64:	%lu %lu
      Opaque Float:	%f
   1   nanp    2   anp 3   ap  Unknown security level: %s      ..\..\snmplib\snmpv3.c  default secLevel set to: %s = %d
       Can't set up engineID of type text from an empty string.
 