#include "ip.h"
#include "log.h"

enum {
  ICMP6_ECHO_REQUEST = 128,
  ICMP6_ECHO_REPLY   = 129
};

struct icmp6_hdr {
  uint8_t     icmp6_type;   /* type field */
  uint8_t     icmp6_code;   /* code field */
  uint16_t    icmp6_cksum;  /* checksum field */
  union {
    uint32_t  icmp6_data32[1]; /* type-specific field */
    uint16_t  icmp6_data16[2]; /* type-specific field */
    uint8_t   icmp6_data8[4];  /* type-specific field */
  };
  uint8_t payload[];  /* echo payload */
};

#define icmp6_id        icmp6_data16[0]  /* echo request/reply */
#define icmp6_seq       icmp6_data16[1]  /* echo request/reply */

extern void handle_icmpv6(const struct icmp6_hdr *h, size_t plen, const struct ip6_hdr *iphdr);

