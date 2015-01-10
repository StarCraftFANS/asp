// ************************************************************************
// *                                                                      *
// * Copyright (C) 2015 Bonden i Nol (hakanbrolin@hotmail.com)            *
// *                                                                      *
// * This program is free software; you can redistribute it and/or modify *
// * it under the terms of the GNU General Public License as published by *
// * the Free Software Foundation; either version 2 of the License, or    *
// * (at your option) any later version.                                  *
// *                                                                      *
// ************************************************************************

#ifndef __ASP_PROD_INFO_H__
#define __ASP_PROD_INFO_H__

/////////////////////////////////////////////////////////////////////////////
//               Definitions of macros
/////////////////////////////////////////////////////////////////////////////
#define ASP_RSTATE  R1A05
#define STR(x)  #x

#ifdef __XC8
// Microchip XC8 toolchain
// Predefined macro: __XC8
#define ASP_PROD_INFO_STR(RSTATE)  "ASP-" STR(RSTATE) "-XC8"
#define ASP_PROD_INFO_LEN  13
#else
// Assume SDCC toolchain
#define ASP_PROD_INFO_STR(RSTATE)  "ASP-" STR(RSTATE) "-SDCC"
#define ASP_PROD_INFO_LEN  14
#endif

#define ASP_PROD_INFO  ASP_PROD_INFO_STR(ASP_RSTATE)

#endif // __ASP_PROD_INFO_H__
