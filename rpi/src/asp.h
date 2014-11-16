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

#ifndef __ASP_H__
#define __ASP_H__

#include <string>

using namespace std;

/////////////////////////////////////////////////////////////////////////////
//               Definition of macros
/////////////////////////////////////////////////////////////////////////////
// Return codes
#define ASP_SUCCESS   0
#define ASP_FAILURE  -1

/////////////////////////////////////////////////////////////////////////////
//               Definition of exported functions
/////////////////////////////////////////////////////////////////////////////
long asp_initialize(void);
long asp_finalize(void);
long asp_chip_probe();
long asp_chip_erase(void);
long asp_chip_read(const string file_path);
long asp_chip_write(const string file_path,
		    bool hex_format);

#endif // __ASP_H__
