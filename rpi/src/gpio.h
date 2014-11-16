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

#ifndef __GPIO_H__
#define __GPIO_H__

#include <stdint.h>

using namespace std;

/////////////////////////////////////////////////////////////////////////////
//               Definition of macros
/////////////////////////////////////////////////////////////////////////////
// Return codes
#define GPIO_SUCCESS                 0
#define GPIO_FILE_OPERATION_FAILED  -1
#define GPIO_MEMORY_MAP_FAILED      -2
#define GPIO_PIN_NOT_ALLOWED        -3

#define GPIO_MAX_PIN  31  // This class only handles GPIO 0..31

/////////////////////////////////////////////////////////////////////////////
//               Class support types
/////////////////////////////////////////////////////////////////////////////
// GPIO Function Select Registers
typedef enum {GPIO_FUNC_INP,  // 000
	      GPIO_FUNC_OUT,  // 001
              GPIO_FUNC_ALT5, // 010
	      GPIO_FUNC_ALT4, // 011
	      GPIO_FUNC_ALT0, // 100
	      GPIO_FUNC_ALT1, // 101
	      GPIO_FUNC_ALT2, // 110
	      GPIO_FUNC_ALT3  // 111
} GPIO_FUNCTION;

/////////////////////////////////////////////////////////////////////////////
//               Definition of classes
/////////////////////////////////////////////////////////////////////////////

class gpio {
  
 public:
  gpio(void);
  ~gpio(void);

  long initialize(void);

  long finalize(void);

  long set_function(uint8_t pin,
		    GPIO_FUNCTION func);

  long get_function(uint8_t pin,
		    GPIO_FUNCTION &func);

  long set_pin_high(uint8_t pin);

  long set_pin_low(uint8_t pin);

  long get_pin(uint8_t pin,
	       uint8_t &value);

 private:
  // Memory mapped I/O access
  void              *m_gpio_map;
  volatile uint32_t *m_gpio;

  void init_members(void);
};

#endif // __GPIO_H__
