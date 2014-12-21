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

#include "rs232.h"
#include "asp_ui.h"

/////////////////////////////////////////////////////////////////////////////
//               Definition of macros
/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
//               Definition of types
/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
//               Function prototypes
/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
//               Global variables
/////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////

int main(void)
{
  int rc;  

  // Initalize RS232
  rc = rs232_initialize();
  if (rc != RS232_SUCCESS) {
    return 1;
  }

  asp_ui_execute();

  // Finalize RS232
  rc = rs232_finalize();
  if (rc != RS232_SUCCESS) {
    return 1;
  }

  return 0;
}

