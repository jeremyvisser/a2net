#include <stdint.h>
#include <serial.h>

#include "log.h"
#include "slip.h"

// Sends packet from buf of length len out serial using SLIP protocol
void send_slip(uint8_t *buf, uint16_t len) {
  uint8_t err;
  logf(DEBUG, "send_slip: sending %u bytes...\n", len);
  while (len--) {
    switch (*buf) {
      case SLIP_END:
        ser_put(SLIP_ESC);
        err = ser_put(SLIP_ESC_END);
      case SLIP_ESC:
        ser_put(SLIP_ESC);
        err = ser_put(SLIP_ESC_ESC);
      default:
        err = ser_put(*buf);
        break;
    }
    if (err != SER_ERR_OK) {
      logf(DEBUG, "send_slip: ser_put: error with %d bytes unsent\n");
      break;
    }
    ++buf;
  }
  ser_put(SLIP_END);
  log(DEBUG, "send_slip: sent successfully");
}

#define SLIP_RETRIES 0xFF

// Receives packet from serial into buf, up to maxlen, and returns the length.
// If an error occurs, it returns 0, and buf will contain invalid data.
uint16_t
recv_slip(uint8_t *buf, uint16_t maxlen) {
  uint8_t c, err;
  uint16_t len = 0;
#ifdef SLIP_RETRIES
  uint8_t retries = 0;
#endif
  for (;;) {
#ifdef SLIP_RETRIES
    while ((err = ser_get(&c)) == SER_ERR_NO_DATA && retries-- > 0);
    retries = SLIP_RETRIES;
#else
    err = ser_get(&c);
#endif
    if (err != SER_ERR_OK) {
      if (len > 0) {
        logf(DEBUG, "recv_slip: ser_get: error (%u) after %u bytes\n", err, len);
      }
      return 0;
    }
    switch (c) {
      case SLIP_END:
        if (len) {
          logf(DEBUG, "recv_slip: %u bytes recv'd successfully\n", len);
          return len;
        }
        break; // continue waiting for data if premature END
      case SLIP_ESC:
        // if ESC, get another char:
#ifdef SLIP_RETRIES
        while ((err = ser_get(&c)) == SER_ERR_NO_DATA && retries-- > 0);
        retries = SLIP_RETRIES;
#else
        err = ser_get(&c);
#endif
        if (err != SER_ERR_OK) {
          logf(DEBUG, "recv_slip: ser_get: error after ESC (%u) after %u bytes\n", err, len);
          return 0;
        }
        switch (c) {
          case SLIP_ESC_END:
            c = SLIP_END;
            break;
          case SLIP_ESC_ESC:
            c = SLIP_ESC;
            break;
        }
      default:
        if (len < maxlen) {
          buf[len++] = c;
        } else {
          log(DEBUG, "recv_slip: exceeded MTU during recv");
          return 0; // exceeded MTU, abort recv
        }
    }
  }
}

