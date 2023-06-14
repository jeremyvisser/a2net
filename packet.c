#include <stdint.h>
#include <stddef.h>
#include <string.h>

#include "packet.h"
#include "ip.h"
#include "log.h"
#include "slip.h"

struct pktq recv_q;
struct pktq send_q;

// Reserves the next available pktbuf in the queue and returns it.
// If no buffer is available, returns NULL.
static struct pktbuf*
push_q(struct pktq* pq) {
  if (pq->c >= (sizeof pq->q / sizeof *pq->q)) {
    return NULL;
  }
  memset(&pq->q[pq->c], 0, sizeof(*pq->q));
  return &pq->q[pq->c++];
}

// Returns the top pktbuf from the queue and pops it from the stack.
static const struct pktbuf*
pop_q(struct pktq* pq) {
  if (pq->c == 0) {
    return NULL;
  }
  return &pq->q[--pq->c];
}

struct pktbuf*
push_send_q() {
  return push_q(&send_q);
}

const struct pktbuf*
pop_send_q() {
  return pop_q(&send_q);
}

struct pktbuf*
push_recv_q() {
  return push_q(&recv_q);
}

const struct pktbuf*
pop_recv_q() {
  return pop_q(&recv_q);
}

// Decides which layer 3 protocol to handle the packet
void handle_packet(const uint8_t p[], size_t plen) {
  if (plen < 1) {
    return;
  }
  // IP version:
  switch (p[0] >> 4) {
    case 4:
      break;
    case 6:
      handle_ipv6((struct ip6_hdr*)p, plen);
      break;
    default:
      logf(WARNING, "handle_packet: ignore IPv%d packet\n", p[0] >> 4);
      break;
  }
}

// Handles each packet in the recv queue
void handle_recv_queue() {
  const struct pktbuf *buf;
  while ((buf = pop_recv_q())) {
    handle_packet(buf->data, buf->plen);
  }
}

