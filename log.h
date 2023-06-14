#ifndef _LOG_H
#define _LOG_H

#include <stdint.h>
#include <stdio.h>

enum {
  DEBUG   = 0,
  INFO    = 1,
  WARNING = 2,
  ERROR   = 3
};

extern uint8_t log_severity;
extern void log_set_level(uint8_t severity);


#ifdef __CC65__

#define log(severity, msg) \
    if (severity >= log_severity) { \
      puts(msg); \
    }
extern void logf(uint8_t severity, uint8_t *msg, ...);

#else

#define log(severity, msg) \
    if (severity >= log_severity) { \
      fputs(msg "\n", stderr); \
    }
#define logf(severity, fmt, args...) \
    if (severity >= log_severity) { \
      fprintf(stderr, fmt , ## args); \
    }

#endif

#endif
