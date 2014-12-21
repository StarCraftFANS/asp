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

#include <stdint.h>

#include "asp_ui.h"
#include "asp.h"
#include "user_io.h"

/////////////////////////////////////////////////////////////////////////////
//               Definitions of macros
/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
//               Function prototypes
/////////////////////////////////////////////////////////////////////////////
static void print_menu(void);
static void clear_errors(void);

/////////////////////////////////////////////////////////////////////////////
//               Global variables
/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
//               Public functions
/////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////

void asp_ui_execute(void)
{
  int rc;
  uint8_t value;

  do {
    print_menu();
    
    user_io_put("ASP-R1A01>", 10);
    while (user_io_get(&value) != USER_IO_SUCCESS) {
      ;
    }

    clear_errors(); // Clear any previous error
    
    // Excecute command
    switch(value) {
    case 0x31: // '1'
      rc = asp_chip_probe();
      break;
    case 0x32: // '2'
      rc = asp_chip_erase();
      break;
    case 0x33: // '3'
      rc = asp_chip_read();
      break;
    case 0x34: // '4'
      rc = asp_chip_write();
      break;
    case 0x35: // '5'
      rc = asp_chip_verify();
      break;
    default:
      rc = ASP_SUCCESS; // Assume OK
      break;
    }

    // Check command
    if (rc != ASP_SUCCESS) {
      user_io_new_line();
      user_io_put("***ERROR", 8);
    }
  } while (1);
}

/////////////////////////////////////////////////////////////////////////////
//               Private functions
/////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////

static void print_menu(void)
{
  user_io_new_line();
  user_io_new_line();
  user_io_put_line("----------------", 16);
  user_io_put_line("--  ASP MENU  --", 16);
  user_io_put_line("----------------", 16);
  user_io_put_line("  1. probe",  10);
  user_io_put_line("  2. erase",  10);
  user_io_put_line("  3. read",    9);
  user_io_put_line("  4. write",  10);
  user_io_put_line("  5. verify", 11);
  user_io_new_line();
}

////////////////////////////////////////////////////////////////

static void clear_errors(void)
{
}
