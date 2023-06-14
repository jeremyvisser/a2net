#include <stdint.h>
#include <stddef.h>
#include <string.h>

#include "icmp.h"
#include "ip.h"
#include "packet.h"
#include "util.h"

static
uint16_t calc_icmpv6_checksum(const struct icmp6_hdr *h, size_t plen, const in6_addr *dst) {
  struct ip6_pseudo_hdr ph;
  size_t i;
  union {
    uint8_t c[2];
    uint16_t s;
  } *hword = (void*)h;
  uint32_t cksum = 0;

  memset(&ph, 0, sizeof(ph));
  ph.ip6_src = *ip6_myaddr;
  ph.ip6_dst = *dst;
  ph.ip6_plen = htons(plen);
  ph.ip6_nxt = IPPROTO_ICMPV6;

  // add pseudo header 16-bit words:
  for (i = 0; i < sizeof(ph.s) / sizeof(*ph.s); ++i) {
    cksum += ntohs(ph.s[i]);
  }
  // add icmpv6 header 16-bit words:
  while (plen > 0) {
    // add left-over byte, if uneven:
    if (plen == 1) {
      cksum += hword->c[0];
    } else {
      cksum += ntohs(hword->s);
    }
    ++hword;
    plen -= sizeof(*hword);
  }

  // fold 32-bits into 16-bits:
  while (cksum >> 16) {
    cksum = (cksum & 0xFFFF) + (cksum >> 16);
  }
  return ~cksum;
}

// gets a buf from the send_q (returns early if none available, request dropped)
// plen - size of the icmpv6 header + any echo payload
static
void reply_echo_request(const struct icmp6_hdr *h, size_t plen, const in6_addr *tosrc) {
  struct pktbuf *buf;
  struct ip6_hdr *newip6h;
  struct icmp6_hdr *newicmp6h;

  // allocate buffer to send reply
  if (!(buf = push_send_q())) {
    log(WARNING, "reply_echo_request: no send buffer available");
    return;
  }

  log(DEBUG, "reply_echo_request: pong!");

  newip6h = (struct ip6_hdr*)buf->data;
  newicmp6h = (struct icmp6_hdr*)newip6h->payload;

  newip6h->ip6_src = *ip6_myaddr;
  newip6h->ip6_dst = *tosrc;
  newip6h->ip6_ctlun.ip6_un2_vfc = 0x60;
  newip6h->ip6_ctlun.ip6_un1.ip6_un1_plen = htons(plen);
  newip6h->ip6_ctlun.ip6_un1.ip6_un1_nxt  = IPPROTO_ICMPV6;
  newip6h->ip6_ctlun.ip6_un1.ip6_un1_hlim = 64;

  // retain original echo request, except turn it into a reply. hey, presto!
  memcpy(newicmp6h, h, plen);
  newicmp6h->icmp6_type = ICMP6_ECHO_REPLY;
  newicmp6h->icmp6_cksum = 0;
  newicmp6h->icmp6_cksum = htons(calc_icmpv6_checksum(newicmp6h, plen, tosrc));

  buf->plen = sizeof(*newip6h) + plen;
}

// plen - size of the icmpv6 header + any echo payload
void handle_icmpv6(const struct icmp6_hdr *h, size_t plen, const struct ip6_hdr *iphdr) {
  if (plen < sizeof(struct icmp6_hdr)) {
    return;
  }
  switch (h->icmp6_type) {
    // error types:
    //   TODO
    // informational types:
    case ICMP6_ECHO_REQUEST:
      reply_echo_request(h, plen, &iphdr->ip6_src);
      break;
    default:
      break;
  }
}

