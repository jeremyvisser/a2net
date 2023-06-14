#include <stdint.h>

#define MTU 1500

struct pktbuf {
  uint16_t plen;
  uint8_t data[MTU];
};

#define Q_MAXLEN 4
struct pktq {
  uint8_t c;
  struct pktbuf q[Q_MAXLEN];
};

// why not export push_q/pop_q directly? because this allows underlying queue
// struct to remain unexported.
extern struct pktbuf* push_send_q();
extern const struct pktbuf* pop_send_q();

extern struct pktbuf* push_recv_q();
extern const struct pktbuf* pop_recv_q();

extern void handle_recv_queue();

#ifdef _TEST_C
// exported for debugging only:
extern const struct pktq recv_q;
extern const struct pktq send_q;
#endif
