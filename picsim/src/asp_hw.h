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

#ifndef __ASP_HW_H__
#define __ASP_HW_H__

/////////////////////////////////////////////////////////////////////////////
//               Definition of macros
/////////////////////////////////////////////////////////////////////////////
// Button pin
#define BUT_PIN

// Target RESET pin
#define TARGET_RESET_PIN

// LED I/O pins
#define DBG_LED_PIN
#define ACTIV_LED_PIN
#define HOST_ERR_LED_PIN
#define TARGET_ERR_LED_PIN

// LED on/off
#define DBG_LED_ON
#define DBG_LED_OFF

#define ACTIV_LED_ON
#define ACTIV_LED_OFF

#define HOST_ERR_LED_ON
#define HOST_ERR_LED_OFF

#define TARGET_ERR_LED_ON
#define TARGET_ERR_LED_OFF

/////////////////////////////////////////////////////////////////////////////
//               Definition of exported functions
/////////////////////////////////////////////////////////////////////////////
extern void asp_hw_initialize(void);

#endif // __ASP_HW_H__
