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

#ifndef __PLATFORM_H__
#define __PLATFORM_H__

#ifdef __XC8

// Microchip XC8 toolchain
// Predefined macro: __XC8
#include <xc.h>
#include "sdcc_defs.h"

#else

// Assume SDCC toolchain
#include <pic18fregs.h>

#endif

#endif // __PLATFORM_H__
