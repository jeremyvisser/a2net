#include <stdint.h>

#define  SLIP_END      0xC0
#define  SLIP_ESC      0xDB
#define  SLIP_ESC_END  0xDC
#define  SLIP_ESC_ESC  0xDD

extern void send_slip(uint8_t *buf, uint16_t len);
extern uint16_t recv_slip(uint8_t *buf, uint16_t maxlen);

