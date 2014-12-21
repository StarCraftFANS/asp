// Build:
// /opt/sdcc/bin/sdcc -mmcs51 test.c -o test.hex --out-fmt-ihx

// Simulate:
// /opt/sdcc/bin/s51 -X 12M test.hex
// /opt/sdcc/bin/s51 -X 12M -s /dev/ttyUSB0 test.hex

// HEX2BIN:
// /opt/sdcc/bin/makebin test.hex test.bin
// /opt/sdcc/bin/sdobjcopy -I ihex -O binary test.hex test.bin
// objcopy -I ihex -O binary test.hex test.bin
// hex2bin test.hex

// BIN2HEX:
// /opt/sdcc/bin/sdobjcopy -I binary -O ihex test.bin test.hex
// objcopy -I binary -O ihex test.bin test.hex


//Alternative : #include <mcs51/8052.h>
#include <mcs51/at89x52.h>

void delay (int value)
{
  unsigned char k;
  while (value--) {
    for (k=0; k < 255; k++);
  }
}
 
char get_char (void)
{
  char c;

  while (!RI); /* wait to receive */
  c = SBUF;    /* receive from serial */
  RI = 0;
  return c;
}
 
void put_char (char c)
{
  while (!TI); /* wait end of last transmission */
  TI = 0;
  SBUF = c;    /* transmit to serial */
}
 
void uart_init()
{
  SCON = 0x50;  /* serial (mode 1) : 8 data bit, 1 start bit, 1 stop bit */
  TMOD = 0x20;  /* timer1 (mode 2) : 8-bit, auto reload */
  TH1 = 0xF3;   /* baud rate 2400 */
  TL1 = 0xF3;   /* baud rate 2400 */
  PCON |= SMOD; /* double baud rate => 4800 */
  TR1 = 1;      /* enable timer */
  TI = 1;       /* enable transmitting */
  RI = 0;       /* waiting to receive */
}

void uart_init_joe()
{
  SCON = 0x50;  /* serial (mode 1) : 8 data bit, 1 start bit, 1 stop bit */
  TMOD = 0x20;  /* timer1 (mode 2) : 8-bit, auto reload */
  TH1 = 0xFA;   /* baud rate 4800 */
  TR1 = 1;      /* enable timer */
  TI = 1;       /* enable transmitting */
}
 
void main()
{
  uart_init();

  while (1) {
    put_char('K');
    delay(255);
    put_char('L');
    delay(255);
    put_char('A');
    delay(255);
    put_char('R');
    delay(255);
    put_char('A');
    delay(255);
    put_char('\n');
    delay(255);
  }
}
