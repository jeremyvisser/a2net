#include <stdint.h>
#include <stddef.h>
#include <string.h>

#include "ip.h"
#include "icmp.h"
#include "util.h"

in6_addr ip6_localhost = {
  {
    // ::1
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01
  }
};

in6_addr *ip6_myaddr = &ip6_localhost;

void ipv6_set_addr(in6_addr *ip) {
  ip6_myaddr = ip;
}

uint8_t
ipv6_addr_equal(const in6_addr *ip1, const in6_addr *ip2) {
  return memcmp(ip1, ip2, sizeof(*ip1)) == 0;
}

uint8_t
ipv6_addr_this_host(const in6_addr *ip) {
  return
    ipv6_addr_equal(ip, ip6_myaddr)
    || ipv6_addr_equal(ip, &ip6_localhost);
}

// In future, should format properly. For now, it formats like:
// 2001:db8::1 -> "20010db8000000000000000000000001"
uint8_t*
inet_ntoa(struct in6_addr *ip) {
  static uint8_t a[33];
  uint8_t i, j, x;
  for (i = j = 0; i < sizeof(ip->__u6_addr8); ++i) {
    a[j++] = ('0' <= (x = ip->__u6_addr8[i] >>   4) <= '9') ? '0'+x : 'A'+x;
    a[j++] = ('0' <= (x = ip->__u6_addr8[i] & 0x0F) <= '9') ? '0'+x : 'A'+x;
  }
  a[32] = NULL;
  return a;
}

// plen - size of entire packet including ipv6 header
// ip6_un1_plen - size of payload (everything after ipv6 header)
void handle_ipv6(const struct ip6_hdr *p, size_t plen) {
  // at least size of ipv6 header?
  if (plen < sizeof(*p)) {
    logf(WARNING, "handle_ipv6: invalid length %u (want >=%u)\n",
      plen, sizeof(*p));
    return;
  }
  // addressed to us? if not, drop.
  if (!ipv6_addr_this_host(&p->ip6_dst)) {
    log(DEBUG, "handle_ipv6: ignore packet not addressed to host");
    return;
  }
  // payload length nonsensical? drop.
  if (ntohs(p->ip6_ctlun.ip6_un1.ip6_un1_plen) > (plen - sizeof(*p))) {
    logf(WARNING, "handle_ipv6: invalid length %u (want <=%u)\n",
      ntohs(p->ip6_ctlun.ip6_un1.ip6_un1_plen), plen - sizeof(*p));
    return;
  }
  // determine next layer to process packet:
  switch (p->ip6_ctlun.ip6_un1.ip6_un1_nxt) {
    case IPPROTO_ICMPV6:
      handle_icmpv6((struct icmp6_hdr*)p->payload, ntohs(p->ip6_ctlun.ip6_un1.ip6_un1_plen), p);
      break;
    default:
      return; // TODO
      break;
  }
}

