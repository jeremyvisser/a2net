#ifndef _IP_H
#define _IP_H

#include <stdint.h>
#include <stddef.h>

#include "log.h"

enum {
  IPPROTO_ICMPV6 = 58,
  IPPROTO_NONE = 59
};

typedef struct in6_addr {
  union {
    uint8_t	 __u6_addr8[16];
    uint16_t __u6_addr16[8];
    uint32_t __u6_addr32[4];
  };
} in6_addr;

struct ip6_hdr {
  union {
	  struct ip6_hdrctl {
	    uint32_t ip6_un1_flow;   /* 4 bits version, 8 bits TC, 20 bits flow-ID */
	    uint16_t ip6_un1_plen;   /* payload length */
	    uint8_t  ip6_un1_nxt;    /* next header */
	    uint8_t  ip6_un1_hlim;   /* hop limit */
	  } ip6_un1;
  	uint8_t ip6_un2_vfc;       /* 4 bits version, top 4 bits tclass */
  } ip6_ctlun;
  in6_addr ip6_src;     /* source address */
  in6_addr ip6_dst;     /* destination address */
  uint8_t payload[];
};

struct ip6_pseudo_hdr {
  union {
    struct {
      in6_addr ip6_src;
      in6_addr ip6_dst;
      uint32_t ip6_plen;
      uint8_t  zero[3];
      uint8_t  ip6_nxt;
    };
    uint16_t s[20];
  };
};

extern void ipv6_set_addr(in6_addr *ip);
extern in6_addr *ip6_myaddr;
extern uint8_t* inet_ntoa(struct in6_addr *ip);
extern void handle_ipv6(const struct ip6_hdr *p, size_t plen);

#endif

