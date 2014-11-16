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

// GPIO header P1 pin names (ModelB) mapped to BCM2835 GPIO signals
#define PIN_P1_03    2   // I2C1_SDA
#define PIN_P1_05    3   // I2C1_SCL
#define PIN_P1_07    4
#define PIN_P1_08   14   // UART0_TXD
#define PIN_P1_10   15   // UART0_RXD 
#define PIN_P1_11   17
#define PIN_P1_12   18
#define PIN_P1_13   27
#define PIN_P1_15   22
#define PIN_P1_16   23
#define PIN_P1_18   24
#define PIN_P1_19   10   // SPI0_MOSI
#define PIN_P1_21    9   // SPI0_MISO
#define PIN_P1_22   25
#define PIN_P1_23   11   // SPI0_CLK
#define PIN_P1_24    8   // SPI0_CE0
#define PIN_P1_26    7   // SPI0_CE1

// RESET pin
#define ASP_PIN_RESET  PIN_P1_22

// SPI definitions
#define ASP_SPI_DEV    "/dev/spidev0.0" // CE0, Raspberry Pi (Model B, GPIO P1)
#define ASP_SPI_SPEED  500000           // Bitrate [Hz] (> XTAL/16) (XTAL=12MHz)

#endif // __ASP_HW_H__
