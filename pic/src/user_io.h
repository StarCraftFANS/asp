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

#ifndef __USER_IO_H__
#define __USER_IO_H__

#include <stdint.h>

/////////////////////////////////////////////////////////////////////////////
//               Definition of macros
/////////////////////////////////////////////////////////////////////////////
// Return codes
#define USER_IO_SUCCESS       0
#define USER_IO_FAILURE      -1
#define USER_IO_WOULD_BLOCK  -2

/////////////////////////////////////////////////////////////////////////////
//               Definition of exported functions
/////////////////////////////////////////////////////////////////////////////
extern void user_io_put(const char *msg,
			unsigned len);

extern void user_io_put_line(const char *msg,
			     unsigned len);

extern void user_io_new_line(void);

extern int user_io_get(uint8_t *data);

#endif // __USER_IO_H__
