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
#include "asp_hw.h"
#include "asp.h"
#include "user_io.h"

/////////////////////////////////////////////////////////////////////////////
//               Definitions of macros
/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
//               Definitions of types
/////////////////////////////////////////////////////////////////////////////
// Commands
typedef enum {
  UI_CMD_PROBE,
  UI_CMD_ERASE,
  UI_CMD_READ,
  UI_CMD_WRITE,
  UI_CMD_VERIFY,
  UI_CMD_UNKNOWN  // Must be last enum
} UI_CMD;

// Function pointer associated with a command
typedef int (*UI_CMD_FUNC)(void);

/////////////////////////////////////////////////////////////////////////////
//               Function prototypes
/////////////////////////////////////////////////////////////////////////////
static void print_menu(void);
static void clear_errors(void);

/////////////////////////////////////////////////////////////////////////////
//               Global variables
/////////////////////////////////////////////////////////////////////////////
static UI_CMD_FUNC g_ui_cmd_func[UI_CMD_UNKNOWN] =
{
  asp_chip_probe,
  asp_chip_erase,
  asp_chip_read,
  asp_chip_write,
  asp_chip_verify  
};

/////////////////////////////////////////////////////////////////////////////
//               Public functions
/////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////

void asp_ui_execute(void)
{
  int rc;
  uint8_t value;
  UI_CMD ui_cmd;

  do {
    print_menu();   
    user_io_put("ASP-R1A03>", 10);

    // Wait for command
    do {
      while (user_io_get(&value) != USER_IO_SUCCESS) { ; }
      switch(value) {
      case 0x31: // '1'
	ui_cmd = UI_CMD_PROBE;
	break;
      case 0x32: // '2'
	ui_cmd = UI_CMD_ERASE;
	break;
      case 0x33: // '3'
	ui_cmd = UI_CMD_READ;
	break;
      case 0x34: // '4'
	ui_cmd = UI_CMD_WRITE;
	break;
      case 0x35: // '5'
	ui_cmd = UI_CMD_VERIFY;
	break;
      default:
	ui_cmd = UI_CMD_UNKNOWN;
      }
    } while (ui_cmd == UI_CMD_UNKNOWN);

    clear_errors(); // Clear any previous errors
    
    // Excecute command
    rc = g_ui_cmd_func[ui_cmd]();
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
  // Clear any indicated errors
  HOST_ERR_LED_OFF;
  TARGET_ERR_LED_OFF;
}
