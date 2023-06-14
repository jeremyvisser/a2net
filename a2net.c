#include <stdint.h>
#include <serial.h>
#include <apple2enh.h>
#include <conio.h>
#include <stdio.h>

#include "ip.h"
#include "comms.h"
#include "log.h"
#include "packet.h"
#include "util.h"

in6_addr myip = {
  {
    // 2001:db8::a2e
    0x20, 0x01, 0x0d, 0xb8, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0a, 0x2e
  }
};

const char *BAUD_S = "1200";
struct ser_params ser_params = {
  SER_BAUD_1200,
  SER_BITS_8,
  SER_STOP_1,
  SER_PAR_NONE,
  SER_HS_HW
};

const uint8_t ser_slot = 2;

void loop() {
  uint8_t key;
  for (;;) {
    // Check if user pressed key:
    if (kbhit()) {
      switch (key = getchar()) {
        case 'i':
        case 'I':
          printf("IP address: %s\n", inet_ntoa(&myip));
          break;
        case 'q':
        case 'Q':
          return;
          break;
        default:
          puts("Keys:");
          puts("  I - print IP address");
          puts("  Q - quit");
          break;
      }
    }

    // Send any pending packets:
    flush_send_queue();

    // Receive incoming packets:
    fill_recv_queue();

    // And process them:
    handle_recv_queue();
  }
}

uint8_t init() {
  uint8_t err;

  ipv6_set_addr(&myip);
  logf(INFO, "IP addr: %s\n", inet_ntoa(&myip));

  if ((err = ser_install(a2e_ssc_ser)) != SER_ERR_OK) {
    logf(ERROR, "ser_install failed: %u\n", err);
    return 0;
  }

  logf(INFO, "Opening SSC in slot %u...\n", ser_slot);
  ser_apple2_slot(ser_slot);

  if ((err = ser_open(&ser_params)) != SER_ERR_OK) {
    logf(ERROR, "ser_open failed: %u\n", err);
    return 0;
  }
  logf(INFO, "Serial SSC successfully opened (%s baud)\n", BAUD_S);
  return 1;
}

void deinit() {
  log(DEBUG, "Deinit");
  //ser_close(); // identical to ser_install and ser_uninstall on a2e_ssc_ser
  ser_uninstall();
}

void main() {
  clrscr();
  puts("A2Net - IPv6 stack for Apple //e");
  puts("Copyright (c) 2023 by Jeremy Visser\n");

  if (init()) {
    log(INFO, "Init OK");
    loop();
  } else {
    log(ERROR, "Error during init");
  }
  deinit();

  puts("Press the ANY key to quit...");
  getchar();
}
