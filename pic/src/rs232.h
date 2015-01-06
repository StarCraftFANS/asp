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

#ifndef __RS232_H__
#define __RS232_H__

#include <stdint.h>

/////////////////////////////////////////////////////////////////////////////
//               Definition of macros
/////////////////////////////////////////////////////////////////////////////
// Return codes
#define RS232_SUCCESS       0
#define RS232_FAILURE      -1
#define RS232_WOULD_BLOCK  -2

/////////////////////////////////////////////////////////////////////////////
//               Definition of exported functions
/////////////////////////////////////////////////////////////////////////////
extern int rs232_initialize(void);
extern int rs232_finalize(void);

extern int rs232_recv(uint8_t *data);
extern int rs232_recv_timeout(uint8_t *data,
			      unsigned timeout_s);

extern int rs232_send(uint8_t data);
extern int rs232_send_bytes(const uint8_t *data,
                            uint8_t nbytes);

extern int rs232_purge_receiver(void);

#endif // __RS232_H__
