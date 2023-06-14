#include <stdint.h>
#include <apple2enh.h>
#include <serial.h>
#include <stdio.h>
#include <conio.h>

uint8_t err;
uint8_t buf[0x100];

struct ser_params ser_params = {
  SER_BAUD_9600,
  SER_BITS_8,
  SER_STOP_1,
  SER_PAR_NONE,
  SER_HS_HW
};

void loop() {
  uint8_t i = 0;

  for (;;) {
    if (kbhit()) {
      switch (getchar()) {
        case 'q':
        case 'Q':
          return;
      }
    }

    if ((err = ser_get(&buf[i])) == SER_ERR_NO_DATA) {
      continue;
    }
    if (err != SER_ERR_OK) {
      puts("error:");
      putchar(err);
      continue;
    }


    if (buf[i] == '\0') {
      puts(buf);
      i = 0;
      continue;
    }

    if (i < sizeof(buf)-1) {
      ++i;
    }
  }

}

void main() {
  unsigned int c;

  videomode(VIDEOMODE_80x24);
  clrscr();

  if ((err = ser_install(a2e_ssc_ser)) != SER_ERR_OK) {
    printf("ser_install: %u\n", err);
    return;
  }

  for (;;) {
    printf("Select baud rate:\n"
      "0)   45.5   8)   1200\n"
      "1)   50     9)   1800\n"
      "2)   75     A)   2400\n"
      "3)  110     B)   3600\n"
      "4)  134.5   C)   4800\n"
      "5)  150     D)   7200\n"
      "6)  300     E)   9600\n"
      "7)  600     F)  19200\n");
    if (scanf("%x", &c) == 1) {
      ser_params.baudrate = c;
      break;
    }
  }

  printf("Baud rate: %x\n", ser_params.baudrate);

  if ((err = ser_open(&ser_params)) != SER_ERR_OK) {
    printf("ser_open: %u\n", err);
    return;
  }

  printf("Buffering data... (send NULL byte to print)\n");
  loop();

  ser_uninstall();
}
