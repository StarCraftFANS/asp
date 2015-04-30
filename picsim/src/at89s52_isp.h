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

#ifndef __AT89S52_ISP_H__
#define __AT89S52_ISP_H__

/////////////////////////////////////////////////////////////////////////////
//               Definition of macros
/////////////////////////////////////////////////////////////////////////////
// Return codes
#define AT89S52_ISP_SUCCESS   0
#define AT89S52_ISP_FAILURE  -1

/////////////////////////////////////////////////////////////////////////////
//               Definition of exported functions
/////////////////////////////////////////////////////////////////////////////
extern int at89s52_isp_probe(void);
extern int at89s52_isp_erase(void);
extern int at89s52_isp_read_bin(void);
extern int at89s52_isp_write_bin(void);
extern int at89s52_isp_verify_bin(void);
extern int at89s52_isp_read_hex(void);
extern int at89s52_isp_write_hex(void);
extern int at89s52_isp_verify_hex(void);

#endif // __AT89S52_ISP_H__
