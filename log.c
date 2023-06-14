#include <stdint.h>
#include <stdio.h>

#include "log.h"

uint8_t log_severity = DEBUG;

void log_set_level(uint8_t severity) {
  log_severity = severity;
}

#ifdef __CC65__
// for some reason, CC65 can't deal with variadic macros
void logf(uint8_t severity, uint8_t *msg, ...) {
  va_list args;
  va_start(args, msg);
  if (severity >= log_severity) {
    vprintf(msg, args);
  }
  va_end(args);
}
#endif
