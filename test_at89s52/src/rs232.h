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

/////////////////////////////////////////////////////////////////////////////
//               Function prototypes
/////////////////////////////////////////////////////////////////////////////
extern void rs232_initialize(void);
extern char rs232_get_char(void);
extern void rs232_put_char(char c);
extern void rs232_put_string(const char *s);
extern void rs232_putln_string(const char *s);

#endif // __RS232_H__
