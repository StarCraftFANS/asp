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

#include "asp.h"
#include "at89s52_isp.h"

using namespace std;

/////////////////////////////////////////////////////////////////////////////
//               Definitions of macros
/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
//               Support types
/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
//               Function prototypes
/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
//               Global variables
/////////////////////////////////////////////////////////////////////////////
static at89s52_isp *g_at89s52_isp = 0;

/////////////////////////////////////////////////////////////////////////////
//               Public functions
/////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////

long asp_initialize(void)
{
  // Create AT89S52 ISP object
  g_at89s52_isp = new at89s52_isp();

  // Initialize AT89S52 ISP object
  if (g_at89s52_isp->initialize() != AT89S52_ISP_SUCCESS) {
    return ASP_FAILURE;
  }
  return ASP_SUCCESS;
}

////////////////////////////////////////////////////////////////

long asp_finalize(void)
{
  // Finalize AT89S52 ISP object
  if (g_at89s52_isp->finalize() != AT89S52_ISP_SUCCESS) {
    delete g_at89s52_isp;
    return ASP_FAILURE;
  }
  delete g_at89s52_isp;
  return ASP_SUCCESS;
}

////////////////////////////////////////////////////////////////

long asp_chip_probe()
{
  if (g_at89s52_isp->probe() != AT89S52_ISP_SUCCESS) {
    return ASP_FAILURE;
  }
  return ASP_SUCCESS;
}

////////////////////////////////////////////////////////////////

long asp_chip_erase()
{
  if (g_at89s52_isp->erase() != AT89S52_ISP_SUCCESS) {
    return ASP_FAILURE;
  }
  return ASP_SUCCESS;
}

////////////////////////////////////////////////////////////////

long asp_chip_read(const string file_path)
{
  if (g_at89s52_isp->read_bin(file_path) != AT89S52_ISP_SUCCESS) {
    return ASP_FAILURE;
  }
  return ASP_SUCCESS;
}

////////////////////////////////////////////////////////////////

long asp_chip_verify(const string file_path,
		     bool hex_format)
{
  if (hex_format) {
    if (g_at89s52_isp->verify_hex(file_path) != AT89S52_ISP_SUCCESS) {
      return ASP_FAILURE;
    }
  }
  else {
    if (g_at89s52_isp->verify_bin(file_path) != AT89S52_ISP_SUCCESS) {
      return ASP_FAILURE;
    }
  }
  return ASP_SUCCESS;
}

////////////////////////////////////////////////////////////////

long asp_chip_write(const string file_path,
		    bool hex_format)
{
  if (hex_format) {
    if (g_at89s52_isp->write_hex(file_path) != AT89S52_ISP_SUCCESS) {
      return ASP_FAILURE;
    }
  }
  else {
    if (g_at89s52_isp->write_bin(file_path) != AT89S52_ISP_SUCCESS) {
      return ASP_FAILURE;
    }
  }
  return ASP_SUCCESS;
}

/////////////////////////////////////////////////////////////////////////////
//               Private functions
/////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////
