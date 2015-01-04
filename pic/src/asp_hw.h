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

#include <pic18fregs.h>

/////////////////////////////////////////////////////////////////////////////
//               Definition of macros
/////////////////////////////////////////////////////////////////////////////
// Button pin
#define BUT_PIN  PORTAbits.RA5

// Target RESET pin
#define TARGET_RESET_PIN  PORTCbits.RC2

// LED I/O pins
#define DBG_LED_PIN         PORTAbits.RA0
#define ACTIV_LED_PIN       PORTAbits.RA1
#define HOST_ERR_LED_PIN    PORTAbits.RA2
#define TARGET_ERR_LED_PIN  PORTAbits.RA3

// LED on/off
#define DBG_LED_ON   (DBG_LED_PIN = 1)
#define DBG_LED_OFF  (DBG_LED_PIN = 0)

#define ACTIV_LED_ON   (ACTIV_LED_PIN = 1)
#define ACTIV_LED_OFF  (ACTIV_LED_PIN = 0)

#define HOST_ERR_LED_ON   (HOST_ERR_LED_PIN = 1) 
#define HOST_ERR_LED_OFF  (HOST_ERR_LED_PIN = 0)

#define TARGET_ERR_LED_ON   (TARGET_ERR_LED_PIN = 1)
#define TARGET_ERR_LED_OFF  (TARGET_ERR_LED_PIN = 0)

/////////////////////////////////////////////////////////////////////////////
//               Definition of exported functions
/////////////////////////////////////////////////////////////////////////////
extern void asp_hw_initialize(void);

#endif // __ASP_HW_H__
