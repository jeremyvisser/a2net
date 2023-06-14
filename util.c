#include <stdint.h>

#include "util.h"

#if __CC65__
uint16_t bswap_16(uint16_t swap) {
  uint8_t apsw[2];
  apsw[0] = ((uint8_t*)&swap)[1];
  apsw[1] = ((uint8_t*)&swap)[0];
  return (*((uint16_t*)&apsw));
}
#endif

