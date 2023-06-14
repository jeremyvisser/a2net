#include "packet.h"
#include "slip.h"

// Writes SLIP to serial, empies send queue.
// If send queue is empty, returns.
void flush_send_queue() {
  const struct pktbuf *buf;
  while ((buf = pop_send_q())) {
    send_slip(buf->data, buf->plen);
  }
}

// Reads SLIP from serial, fills up recv queue until full.
// If no data available, returns.
void fill_recv_queue() {
  struct pktbuf *buf;
  for (;;) {
    if (!(buf = push_recv_q())) {
      return;
    }
    buf->plen = recv_slip(buf->data, sizeof(buf->data));
    if (buf->plen == 0) {
      // Cancel queue allocation (technically a stack at time of writing)
      pop_recv_q();
      break;
    }
    // This worked, so receive another
  }  
}

