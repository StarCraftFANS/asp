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

#include <stdint.h>

#include <string>

#include "at89s52_io.h"
#include "gpio.h"

using namespace std;

/////////////////////////////////////////////////////////////////////////////
//               Definition of macros
/////////////////////////////////////////////////////////////////////////////
// Return codes
#define AT89S52_ISP_SUCCESS   0
#define AT89S52_ISP_FAILURE  -1

/////////////////////////////////////////////////////////////////////////////
//               Class support types
/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
//               Definition of classes
/////////////////////////////////////////////////////////////////////////////

class at89s52_isp {

 public:
  at89s52_isp(void);
  ~at89s52_isp(void);

  long initialize(void);
  long finalize(void);

  long probe(void);
  long erase(void);
  long read_bin(const string file_path);
  long verify_hex(const string file_path);
  long verify_bin(const string file_path);
  long write_hex(const string file_path);
  long write_bin(const string file_path);
  
 private:
  at89s52_io   *m_at89s52_io;
  gpio         *m_gpio;
  GPIO_FUNCTION m_pin_reset_func;
  uint8_t      *m_buffer1;
  uint8_t      *m_buffer2;

  int get_file_size(const string file_path);

  void enable_isp(void);
  void disable_isp(void);
  void isp_probe(void);
  void isp_erase(void);
  void isp_read_bin(const string file_path);
  void isp_verify_hex(const string file_path);
  void isp_verify_bin(const string file_path);
  void isp_write_hex(const string file_path);
  void isp_write_bin(const string file_path);
};

#endif // __AT89S52_ISP_H__
