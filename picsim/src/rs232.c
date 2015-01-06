// ************************************************************************
// *                                                                      *
// * Copyright (C) 2014 Bonden i Nol (hakanbrolin@hotmail.com)            *
// *                                                                      *
// * This program is free software; you can redistribute it and/or modify *
// * it under the terms of the GNU General Public License as published by *
// * the Free Software Foundation; either version 2 of the License, or    *
// * (at your option) any later version.                                  *
// *                                                                      *
// ************************************************************************

#include <stdio.h>
#include <string.h>
#include <termios.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <limits.h>
#include <errno.h>

#include "rs232.h"

/////////////////////////////////////////////////////////////////////////////
//               Definition of macros
/////////////////////////////////////////////////////////////////////////////
#define MAX_NUMBER_OF_COMPORTS  10

/////////////////////////////////////////////////////////////////////////////
//               Function prototypes
/////////////////////////////////////////////////////////////////////////////
static int rs232_open(int comport_number,
		      int baudrate,
		      const char *mode);

static int rs232_close(int comport_number);

static int rs232_recv_byte(int comport_number,
			   unsigned char *data);

static int rs232_send_byte(int comport_number,
			   unsigned char data);

static int rs232_purge_rx(int comport_number);

/////////////////////////////////////////////////////////////////////////////
//               Global variables
/////////////////////////////////////////////////////////////////////////////
static int Cport[MAX_NUMBER_OF_COMPORTS];

static struct termios new_port_settings;
static struct termios old_port_settings[MAX_NUMBER_OF_COMPORTS];

static char comports[MAX_NUMBER_OF_COMPORTS][16] = 
  {"/dev/ttyS0",
   "/dev/ttyS1",
   "/dev/ttyS2",
   "/dev/ttyS3",
   "/dev/ttyS4",
   "/dev/ttyUSB0",
   "/dev/ttyUSB1",
   "/dev/ttyUSB2",
   "/dev/ttyUSB3",
   "/dev/ttyUSB4"
  };

static const int g_comport_nr = 0;            // /dev/ttyS0
static const int g_baudrate = 9600;           // 9600 bit/s
static const char g_mode[] = {'8','N','1',0}; // 8-N_1

/////////////////////////////////////////////////////////////////////////////
//               Public functions
/////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////

int rs232_initialize(void)
{
  if (rs232_open(g_comport_nr, g_baudrate, g_mode)) {
    printf("*** ERROR : Open comport\n");
    return RS232_FAILURE;
  }
  return RS232_SUCCESS;
}

////////////////////////////////////////////////////////////////

int rs232_finalize(void)
{
  if (rs232_close(g_comport_nr)) {
    printf("*** ERROR : Close comport\n");
    return RS232_FAILURE;
  }
  return RS232_SUCCESS;
}

////////////////////////////////////////////////////////////////

int rs232_recv(uint8_t *data)
{
  int rc;

  rc = rs232_recv_byte(g_comport_nr,
		       (unsigned char *)data);
  if (rc == 0) {
    return RS232_SUCCESS;
  }
  else if (rc == 2) {
    return RS232_WOULD_BLOCK;
  }
  else {
    return RS232_FAILURE;
  }
}

////////////////////////////////////////////////////////////////

int rs232_recv_timeout(uint8_t *data,
		       unsigned timeout_s)
{
  int rc;
  int timeout_cnt;

  // Define timeout for receive operation
  timeout_cnt = timeout_s * 1000; // ms

  // Receive one character
  do {
    rc = rs232_recv(data);
    if ( timeout_s && (rc == RS232_WOULD_BLOCK) ) {
      usleep(1000); // 1ms
      timeout_cnt--;
    }
    else {
      break;
    }
  } while (timeout_cnt);

  return rc;
}

////////////////////////////////////////////////////////////////

int rs232_send(uint8_t data)
{
  if (rs232_send_byte(g_comport_nr,
		      (unsigned char)data)) {
    return RS232_FAILURE;
  }
  return RS232_SUCCESS;
}

////////////////////////////////////////////////////////////////

int rs232_send_bytes(const uint8_t *data,
		     uint8_t nbytes)
{
  int rc;
  uint8_t i;

  for (i=0; i < nbytes; i++) {
    rc = rs232_send(data[i]);
    if (rc != RS232_SUCCESS) {
      return rc;
    }
  }
  return RS232_SUCCESS;
}

////////////////////////////////////////////////////////////////

int rs232_purge_receiver(void)
{
  if (rs232_purge_rx(g_comport_nr)) {
    return RS232_FAILURE;
  }
  return RS232_SUCCESS;
}

/////////////////////////////////////////////////////////////////////////////
//               Private functions
/////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////

static int rs232_open(int comport_number,
		      int baudrate,
		      const char *mode)
{
  int baudr,status;
  int error;

  if((comport_number>(MAX_NUMBER_OF_COMPORTS-1))||(comport_number<0))
  {
    printf("*** ERROR : Illegal comport number\n");
    return(1);
  }

  switch(baudrate)
  {
    case      50 : baudr = B50;
                   break;
    case      75 : baudr = B75;
                   break;
    case     110 : baudr = B110;
                   break;
    case     134 : baudr = B134;
                   break;
    case     150 : baudr = B150;
                   break;
    case     200 : baudr = B200;
                   break;
    case     300 : baudr = B300;
                   break;
    case     600 : baudr = B600;
                   break;
    case    1200 : baudr = B1200;
                   break;
    case    1800 : baudr = B1800;
                   break;
    case    2400 : baudr = B2400;
                   break;
    case    4800 : baudr = B4800;
                   break;
    case    9600 : baudr = B9600;
                   break;
    case   19200 : baudr = B19200;
                   break;
    case   38400 : baudr = B38400;
                   break;
    case   57600 : baudr = B57600;
                   break;
    case  115200 : baudr = B115200;
                   break;
    case  230400 : baudr = B230400;
                   break;
    case  460800 : baudr = B460800;
                   break;
    case  500000 : baudr = B500000;
                   break;
    case  576000 : baudr = B576000;
                   break;
    case  921600 : baudr = B921600;
                   break;
    case 1000000 : baudr = B1000000;
                   break;
    case 1152000 : baudr = B1152000;
                   break;
    case 1500000 : baudr = B1500000;
                   break;
    case 2000000 : baudr = B2000000;
                   break;
    case 2500000 : baudr = B2500000;
                   break;
    case 3000000 : baudr = B3000000;
                   break;
    case 3500000 : baudr = B3500000;
                   break;
    case 4000000 : baudr = B4000000;
                   break;
    default      : printf("invalid baudrate\n");
                   return(1);
                   break;
  }

  int cbits=CS8,
    cpar=IGNPAR,
    bstop=0;

  if(strlen(mode) != 3)
  {
    printf("*** ERROR : invalid mode \"%s\"\n", mode);
    return(1);
  }

  switch(mode[0])
  {
    case '8': cbits = CS8;
              break;
    case '7': cbits = CS7;
              break;
    case '6': cbits = CS6;
              break;
    case '5': cbits = CS5;
              break;
    default : printf("*** ERROR : Invalid number of data-bits '%c'\n", mode[0]);
              return(1);
              break;
  }

  switch(mode[1])
  {
    case 'N':
    case 'n': cpar = IGNPAR;
              break;
    case 'E':
    case 'e': cpar = PARENB;
              break;
    case 'O':
    case 'o': cpar = (PARENB | PARODD);
              break;
    default : printf("*** ERROR : Invalid parity '%c'\n", mode[1]);
              return(1);
              break;
  }

  switch(mode[2])
  {
    case '1': bstop = 0;
              break;
    case '2': bstop = CSTOPB;
              break;
    default : printf("*** ERROR : Invalid number of stop bits '%c'\n", mode[2]);
              return(1);
              break;
  }

/*
http://pubs.opengroup.org/onlinepubs/7908799/xsh/termios.h.html

http://man7.org/linux/man-pages/man3/termios.3.html
*/

  Cport[comport_number] = open(comports[comport_number],
			       O_RDWR | O_NOCTTY | O_NDELAY);
  if(Cport[comport_number]==-1)
  {
    perror("*** ERROR : Unable to open comport ");
    return(1);
  }

  error = tcgetattr(Cport[comport_number], old_port_settings + comport_number);
  if(error==-1)
  {
    close(Cport[comport_number]);
    perror("*** ERROR : Unable to read portsettings");
    return(1);
  }
  memset(&new_port_settings, 0, sizeof(new_port_settings));  /* clear the new struct */

  new_port_settings.c_cflag = cbits | cpar | bstop | CLOCAL | CREAD;
  new_port_settings.c_iflag = IGNPAR;
  new_port_settings.c_oflag = 0;
  new_port_settings.c_lflag = 0;
  new_port_settings.c_cc[VMIN] = 1;  /* block untill n bytes are received */
  new_port_settings.c_cc[VTIME] = 0; /* block untill a timer expires (n * 100 mSec.) */

  cfsetispeed(&new_port_settings, baudr);
  cfsetospeed(&new_port_settings, baudr);

  error = tcflush(Cport[comport_number], TCIOFLUSH);
  if(error==-1)
  {
    close(Cport[comport_number]);
    perror("*** ERROR : Unable to flush modem line");
    return(1);
  }

  error = tcsetattr(Cport[comport_number], TCSANOW, &new_port_settings);
  if(error==-1)
  {
    close(Cport[comport_number]);
    perror("*** ERROR : Unable to adjust portsettings");
    return(1);
  }

  if(ioctl(Cport[comport_number], TIOCMGET, &status) == -1)
  {
    perror("*** ERROR : Unable to get portstatus");
    return(1);
  }

  status |= TIOCM_DTR;    /* turn on DTR */
  status |= TIOCM_RTS;    /* turn on RTS */

  if(ioctl(Cport[comport_number], TIOCMSET, &status) == -1)
  {
    perror("*** ERROR : Unable to set portstatus");
    return(1);
  }

  return(0);
}

////////////////////////////////////////////////////////////////

static int rs232_close(int comport_number)
{
  int status;

  if(ioctl(Cport[comport_number], TIOCMGET, &status) == -1)
  {
    perror("*** ERROR : Unable to get portstatus");
    return 1;
  }

  status &= ~TIOCM_DTR;    /* turn off DTR */
  status &= ~TIOCM_RTS;    /* turn off RTS */

  if(ioctl(Cport[comport_number], TIOCMSET, &status) == -1)
  {
    perror("*** ERROR : Unable to set portstatus");
    return 1;
  }

  tcsetattr(Cport[comport_number], TCSANOW, old_port_settings + comport_number);
  close(Cport[comport_number]);

  return 0;
}

////////////////////////////////////////////////////////////////

static int rs232_recv_byte(int comport_number,
			   unsigned char *data)
{
  int n;

  n = read(Cport[comport_number], data, 1);
  if (n == -1) {    
    if (errno == EWOULDBLOCK) {
      return 2; // No char available
    }
    else {
      perror("*** ERROR : Read comport");
      return 1;
    }
  }
  if (n != 1) {
    printf("*** ERROR : Read, unexpected bytes, got=%d, expected=%d\n",
	   n, 1);
    return 1;
  }
  return 0;
}

////////////////////////////////////////////////////////////////

static int rs232_send_byte(int comport_number,
			   unsigned char data)
{
  int n;

  n = write(Cport[comport_number], &data, 1);
  if (n == -1) {
    perror("*** ERROR : Write comport");
    return 1;
  }
  if (n != 1) {
    printf("*** ERROR : Write, unexpected bytes, got=%d, expected=%d\n",
	   n, 1);
    return 1;
  }
  return 0;
}

////////////////////////////////////////////////////////////////

static int rs232_purge_rx(int comport_number)
{
  int rc;
  int timeout_cnt;

  timeout_cnt = 10; // 1000ms = 1s
  
  while (timeout_cnt > 0) {
    rc = tcflush(Cport[comport_number], TCIFLUSH);
    if (rc == -1) {
      perror("*** ERROR : Unable to flush rx line");
      return(1);
    }
    usleep(100000); // 100ms
    timeout_cnt--;
  }
  return 0;
}
