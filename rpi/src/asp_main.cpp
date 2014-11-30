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

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <getopt.h>
#include <libgen.h>

#include <string>
#include <exception>
#include <sstream>

#include "asp.h"
#include "asp_test_ui.h"
#include "asp_prod_info.h"

using namespace std;

/////////////////////////////////////////////////////////////////////////////
//               Definition of macros
/////////////////////////////////////////////////////////////////////////////
#define APP_MODE_ERASE  0x01
#define APP_MODE_PROBE  0x02
#define APP_MODE_READ   0x04
#define APP_MODE_TEST   0x08
#define APP_MODE_VERIFY 0x10
#define APP_MODE_WRITE  0x20

#define FILE_FORMAT_BIN  0x01
#define FILE_FORMAT_HEX  0x02

/////////////////////////////////////////////////////////////////////////////
//               Function prototypes
/////////////////////////////////////////////////////////////////////////////
static void app_terminate(void);
static bool app_parse_command_line(int argc, char *argv[]);
static bool app_check_command_line(void);
static void app_report_help(const char *app_name);
static void app_report_prod_info(void);

/////////////////////////////////////////////////////////////////////////////
//               Global variables
/////////////////////////////////////////////////////////////////////////////
class the_terminate_handler {
public:
  the_terminate_handler() {
    set_terminate(app_terminate);
  }
};
// Install terminate function (in case of emergency)
// Install as soon as possible, before main starts
static the_terminate_handler g_terminate_handler;

// Set by command line arguments
static uint8_t g_app_mode = 0;

static string g_input_file = "";
static string g_output_file = "";

static int g_file_format = 0;

////////////////////////////////////////////////////////////////

static void app_terminate(void)
{
   // Only log this event, no further actions for now
  printf("Unhandled exception, termination handler activated\n");

  // The terminate function should not return
  abort();
}

////////////////////////////////////////////////////////////////

static bool app_parse_command_line(int argc, char *argv[])
{
  bool command_line_ok = true;

  int option_index = 0;
  int c;
  struct option long_options[] = {
    {"erase",   no_argument,       0, 'e'},
    {"help",    no_argument,       0, 'h'},
    {"probe",   no_argument,       0, 'p'},
    {"read",    required_argument, 0, 'r'},
    {"test",    no_argument,       0, 't'},
    {"verify",  required_argument, 0, 'v'},
    {"write",   required_argument, 0, 'w'},    
    {"version", no_argument,       0, 'V'},
    {0,         no_argument,       0, 'b'},
    {0,         no_argument,       0, 'x'},
    {0, 0, 0, 0}
  };

  while (1) {
    c = getopt_long(argc, argv, "ehpr:tv:w:Vbx",
		    long_options, &option_index);
    
    // Detect the end of the options
    if (c == -1) {
      break;
    }
    
    switch (c) {
    case 0:
      // If option sets a flag, do nothing else now
      if (long_options[option_index].flag) {
	break;
      }
      break;

    case 'e':      
      g_app_mode |= APP_MODE_ERASE;
      break;

    case 'h':
      app_report_help(basename(argv[0]));
      exit(EXIT_SUCCESS);
      break;

    case 'p':
      g_app_mode |= APP_MODE_PROBE;
      break;

    case 'r':
      g_app_mode |= APP_MODE_READ;
      g_output_file = optarg;
      break;

    case 't':
      g_app_mode |= APP_MODE_TEST;
      break;

    case 'v':
      g_app_mode |= APP_MODE_VERIFY;
      g_input_file = optarg;
      break;

    case 'w':
      g_app_mode |= APP_MODE_WRITE;
      g_input_file = optarg;
      break;

    case 'V':
      app_report_prod_info();
      exit(EXIT_SUCCESS);
      break;

    case 'b':
      g_file_format |= FILE_FORMAT_BIN;
      break;

    case 'x':
      g_file_format |= FILE_FORMAT_HEX;
      break;

    default:
      command_line_ok = false;
      break;
    }
  }

  return command_line_ok;
}

////////////////////////////////////////////////////////////////

static bool app_check_command_line(void)
{
  // Mode must be specified
  if (!g_app_mode) {
    return false;
  }

  // Only one mode can be used at a time
  if ( g_app_mode != (g_app_mode & -g_app_mode) ) {    
    return false;
  }

  // Read mode have no file format
  if ( g_app_mode & APP_MODE_READ ) {
    if (g_file_format) {
      return false;
    }
  }

  // Write/verify mode requires file format
  if ( (g_app_mode & APP_MODE_WRITE) ||
       (g_app_mode & APP_MODE_VERIFY) ) {
    if (!g_file_format) {
      return false; // Format must be specified
    }
    if ( g_file_format != (g_file_format & -g_file_format) ) {    
      return false; // Only one format can be used at a time
    }
  }

  return true;
}

////////////////////////////////////////////////////////////////

static void app_report_help(const char *app_name)
{
  ostringstream oss_msg;

  oss_msg << app_name << " [options]" << "\n\n"

	  << "options\n\n"

	  << "-e, --erase\n"
	  << "    Erase chip flash program memory.\n\n"

	  << "-h, --help\n"
	  << "    Print this help information and exit.\n\n"
    
	  << "-p, --probe\n"
	  << "    Probe chip and check signature.\n\n"

	  << "-r, --read <path_to_output_file>\n"
	  << "    Read from chip flash program memory, write to file in binary format.\n\n"

	  << "-t, --test\n"
	  << "    Test mode for development.\n\n"

	  << "-v, --verify <path_to_file> [-b|-x]\n"
	  << "    Verify the contents of chip flash program memory against the file in binary/HEX format.\n\n"

	  << "-w, --write <path_to_input_file> [-b|-x]\n"
	  << "    Write to chip flash program memory, read from file in binary/HEX format.\n\n"
    
	  << "-V, --version\n"
	  << "    Print the product information and exit.\n";
  
  printf("%s\n", oss_msg.str().c_str());
}

////////////////////////////////////////////////////////////////

static void app_report_prod_info(void)
{
  printf("%s - %s\n",
	 ASP_PRODUCT_NUMBER,
	 ASP_PRODUCT_REVISION);
}

////////////////////////////////////////////////////////////////

int main(int argc, char *argv[])
{
  // Parse command line arguments
  if (!app_parse_command_line(argc, argv)) {
    app_report_help(basename(argv[0]));
    exit(EXIT_FAILURE);
  }

  // Check command line arguments
  if (!app_check_command_line()) {
    app_report_help(basename(argv[0]));
    exit(EXIT_FAILURE);
  }

  long rc;

  // Initialize
  if (g_app_mode != APP_MODE_TEST) {
    if (asp_initialize() != ASP_SUCCESS) {
      asp_finalize();
      exit(EXIT_FAILURE);
    }
  }

  // Execute command
  switch (g_app_mode) {
  case APP_MODE_ERASE:
    rc = asp_chip_erase();
    break;
  case APP_MODE_PROBE:
    rc = asp_chip_probe();
    break;
  case APP_MODE_READ:
    rc = asp_chip_read(g_output_file);
    break;
  case APP_MODE_TEST:
    rc = asp_test_ui_execute();
    break;
  case APP_MODE_VERIFY:
    rc = asp_chip_verify(g_input_file,
			 g_file_format == FILE_FORMAT_HEX);
    break;
  case APP_MODE_WRITE:
    rc = asp_chip_write(g_input_file,
			g_file_format == FILE_FORMAT_HEX);
    break;
  default:
    app_report_help(basename(argv[0]));
    rc = ASP_FAILURE;
  }

  // Finalize
  if (g_app_mode != APP_MODE_TEST) {
    if (asp_finalize() != ASP_SUCCESS) {
      exit(EXIT_FAILURE);
    }
  }  

  // Check command
  if (rc == ASP_SUCCESS) {
    exit(EXIT_SUCCESS);
  }
  else {
    exit(EXIT_FAILURE);
  }
}
