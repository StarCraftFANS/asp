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

#ifndef __SDCC_DEFS_H__
#define	__SDCC_DEFS_H__

// Implementation notes:
// 1. SDCC macro definitions for the PIC18F2510 MCU.
//    /opt/sdcc/share/sdcc/non-free/include/pic16/pic18f2510.h
//
// 2. This file shall be included when generating code using the
//    Microchip XC8 toolchain. Needed to make code portable when
//    swtiching between XC8 and SDCC.
// 

/////////////////////////////////////////////////////////////////////////////
//               Definition of macros
/////////////////////////////////////////////////////////////////////////////
#define _TRISA0                 0x01
#define _RA0                    0x01
#define _TRISA1                 0x02
#define _RA1                    0x02
#define _TRISA2                 0x04
#define _RA2                    0x04
#define _TRISA3                 0x08
#define _RA3                    0x08
#define _TRISA4                 0x10
#define _RA4                    0x10
#define _TRISA5                 0x20
#define _RA5                    0x20
#define _TRISA6                 0x40
#define _RA6                    0x40
#define _TRISA7                 0x80
#define _RA7                    0x80

#define _TRISC0                 0x01
#define _RC0                    0x01
#define _TRISC1                 0x02
#define _RC1                    0x02
#define _TRISC2                 0x04
#define _RC2                    0x04
#define _TRISC3                 0x08
#define _RC3                    0x08
#define _TRISC4                 0x10
#define _RC4                    0x10
#define _TRISC5                 0x20
#define _RC5                    0x20
#define _TRISC6                 0x40
#define _RC6                    0x40
#define _TRISC7                 0x80
#define _RC7                    0x80

#define _RX9D                   0x01
#define _OERR                   0x02
#define _FERR                   0x04
#define _ADDEN                  0x08
#define _ADEN                   0x08
#define _CREN                   0x10
#define _SREN                   0x20
#define _RX9                    0x40
#define _SPEN                   0x80

#define _TX9D                   0x01
#define _TRMT                   0x02
#define _BRGH                   0x04
#define _SENDB                  0x08
#define _SYNC                   0x10
#define _TXEN                   0x20
#define _TX9                    0x40
#define _CSRC                   0x80

#endif // __SDCC_DEFS_H__

