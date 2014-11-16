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
#include <stdint.h>
#include <strings.h>

#include <string>

#include "asp_test_ui.h"
#include "asp_hw.h"
#include "asp.h"
#include "gpio.h"
#include "at89s52_io.h"
#include "ihex.h"
#include "delay.h"

/////////////////////////////////////////////////////////////////////////////
//               Definitions of macros
/////////////////////////////////////////////////////////////////////////////
// Global buffer size
#define G_BUFFER_SIZE  (1 * AT89S52_FLASH_MEMORY_SIZE)

/////////////////////////////////////////////////////////////////////////////
//               Function prototypes
/////////////////////////////////////////////////////////////////////////////
static void print_menu(void);
static void print_ihex_record(IHEX_RECORD *rec);
static void enable_programming(void);
static void disable_programming(void);
static void read_signature(void);
static void chip_erase(void);
static void read_flash(void);
static void test_write_flash(void);
static void parse_intel_hex_file(void);

/////////////////////////////////////////////////////////////////////////////
//               Global variables
/////////////////////////////////////////////////////////////////////////////
static uint8_t *g_buffer = 0;
static gpio *g_gpio = 0;
static at89s52_io *g_at89s52_io = 0;

/////////////////////////////////////////////////////////////////////////////
//               Public functions
/////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////

long asp_test_ui_execute(void)
{
  long rc;
  int value;
  GPIO_FUNCTION pin_reset_old_func;

  // Create global data buffer
  g_buffer = new uint8_t[G_BUFFER_SIZE];

  // Create GPIO object for RESET
  g_gpio = new gpio();
  rc = g_gpio->initialize();
  if (rc != GPIO_SUCCESS) {
    printf("*** ERROR : Initialize GPIO, rc=%ld\n", rc);   
    delete g_gpio;
    delete[] g_buffer;
    return ASP_FAILURE;
  }
  g_gpio->get_function(ASP_PIN_RESET, pin_reset_old_func);
  g_gpio->set_function(ASP_PIN_RESET, GPIO_FUNC_OUT);

  // Create AT89S52 I/O object
  g_at89s52_io = new at89s52_io(ASP_SPI_DEV,
				ASP_SPI_SPEED);
  rc = g_at89s52_io->initialize();
  if (rc != AT89S52_SUCCESS) {
    printf("*** ERROR : Initialize AT89S52, rc=%ld\n", rc);
    delete g_at89s52_io;
    g_gpio->set_function(ASP_PIN_RESET, pin_reset_old_func);
    g_gpio->finalize();
    delete g_gpio;    
    delete[] g_buffer;
    return ASP_FAILURE;
  }

  do {
    print_menu();
    
    printf("Enter choice : ");
    if (!scanf("%d", &value)) {
      break;
    }
    
    switch(value) {
    case 1:
      enable_programming();
      break;
    case 2:
      disable_programming();
      break;
    case 3:
      read_signature();
      break;
    case 4:
      chip_erase();
      break;
    case 5:
      read_flash();
      break;
    case 6:
      test_write_flash();
      break;
    case 7:
      parse_intel_hex_file();
      break;
    case 100: // Exit
      break;
    default:
      printf("Illegal choice!\n");
    }
  } while (value != 100);

  // Clean-up
  rc = g_at89s52_io->finalize();
  if (rc != AT89S52_SUCCESS) {
    printf("*** ERROR : Finalize AT89S52, rc=%ld\n", rc);
    delete g_at89s52_io;
    g_gpio->set_function(ASP_PIN_RESET, pin_reset_old_func);
    g_gpio->finalize();
    delete g_gpio;
    delete[] g_buffer;
    return ASP_FAILURE;
  }
  g_gpio->set_function(ASP_PIN_RESET, pin_reset_old_func);
  rc = g_gpio->finalize();
  if (rc != GPIO_SUCCESS) {
    printf("*** ERROR : Finalize GPIO, rc=%ld\n", rc);
    delete g_at89s52_io;
    delete g_gpio;
    delete[] g_buffer;
    return ASP_FAILURE;
  }
  
  delete g_at89s52_io;
  delete g_gpio;
  delete[] g_buffer;

  return ASP_SUCCESS;
}

/////////////////////////////////////////////////////////////////////////////
//               Private functions
/////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////

static void print_menu(void)
{
  printf("\n");
  printf("--------------------------------------------------\n");
  printf("----             ASP TEST MENU                ----\n");
  printf("--------------------------------------------------\n");
  printf("  1. enable programming\n");
  printf("  2. disable programming\n");
  printf("  3. read signature\n");
  printf("  4. chip erase\n");
  printf("  5. read flash\n");
  printf("  6. write flash (test ramp pattern)\n");
  printf("  7. parse Intel HEX file\n");
  printf("100. Exit\n\n");
}

////////////////////////////////////////////////////////////////

static void print_ihex_record(IHEX_RECORD *rec)
{
  printf("addr=0x%04x, bytes=%d, chksum=0x%02x, type=0x%02x\n",
	 rec->addr, rec->nbytes, rec->chksum, rec->type);
  for (unsigned i=0; i < rec->nbytes; i++) {
    printf("0x%02x, ", rec->data[i]);
  }
  printf("\n");
}

////////////////////////////////////////////////////////////////

static void enable_programming(void)
{
  long rc;

  g_gpio->set_pin_high(ASP_PIN_RESET);     // Activate RESET pin
  delay(AT89S52_RESET_ACTIVE_MIN_TIME);

  rc = g_at89s52_io->enable_programming(); // Enable serial programming
  if (rc != AT89S52_SUCCESS) {
    printf("*** ERROR : Enable programming AT89S52, rc=%ld\n", rc);
    return;
  }
}

////////////////////////////////////////////////////////////////

static void disable_programming(void)
{
  g_gpio->set_pin_low(ASP_PIN_RESET); // Deactivate RESET pin
}

////////////////////////////////////////////////////////////////

static void read_signature(void)
{
  long rc;
  uint32_t signature;
  const uint32_t expected_signature = AT89S52_SIGNATURE;

  rc = g_at89s52_io->read_signature(signature);
  if (rc != AT89S52_SUCCESS) {
    printf("*** ERROR : Read signature AT89S52, rc=%ld\n", rc);
    return;
  }
  printf("Signature=0x%08x, expected=0x%08x\n",
	 signature, expected_signature);
}

////////////////////////////////////////////////////////////////

static void chip_erase(void)
{
  long rc;

  rc = g_at89s52_io->chip_erase();
  if (rc != AT89S52_SUCCESS) {
    printf("*** ERROR : Chip erase AT89S52, rc=%ld\n", rc);
    return;
  }
}

////////////////////////////////////////////////////////////////

static void read_flash(void)
{
  long rc;

  unsigned address_value;
  uint16_t address;
  
  unsigned nbytes_value;
  uint16_t nbytes;

  printf("Enter start address [hex]: 0x");
  scanf("%x", &address_value);
  address = (uint16_t)address_value;

  printf("Enter bytes to read [dec]: ");
  scanf("%u", &nbytes_value);
  nbytes = (uint16_t)nbytes_value;

  if (nbytes > G_BUFFER_SIZE) {
    nbytes = G_BUFFER_SIZE;
  }
    
  // Clear global buffer
  bzero((void *)g_buffer, G_BUFFER_SIZE);

  // Read data to buffer from chip
  rc = g_at89s52_io->read_flash(address,
				g_buffer,
				nbytes);
  if (rc != AT89S52_SUCCESS) {
    printf("*** ERROR : Read flash AT89S52, rc=%ld\n", rc);
    return;
  }

  // Print buffer as 16 byte lines
  uint8_t *line16 = (uint8_t *)g_buffer;
  int nr_lines = nbytes / 16;
  
  for (int i=0; i < nr_lines; i++) {
    printf("0x%04x : ", address);
    for (int j=0; j < 16; j++) {
      printf("0x%02x\t", line16[i*16 + j]);
    }
    printf("\n");
    address += 16;
    nbytes -= 16;    
  }

  // Print remaining bytes
  if (nbytes) {
    printf("0x%04x : ", address);
    for (int j=0; j < nbytes; j++) {
      printf("0x%02x\t", line16[nr_lines*16 + j]);
    }
    printf("\n");
  } 
}

////////////////////////////////////////////////////////////////

static void test_write_flash(void)
{
  long rc;

  // Prepare buffer with ramp data
  uint16_t *word16 = (uint16_t *)g_buffer;
  for (unsigned i=0; i < (G_BUFFER_SIZE/(sizeof(uint16_t))); i++) {
    word16[i] = i;
  }
  
  // Write from buffer to chip
  rc = g_at89s52_io->write_flash(0, g_buffer, G_BUFFER_SIZE);
  if (rc != AT89S52_SUCCESS) {
    printf("*** ERROR : Write flash AT89S52, rc=%ld\n", rc);
    return;
  }
}

////////////////////////////////////////////////////////////////

static void parse_intel_hex_file(void)
{
  char file_path[256];

  printf("Enter path to HEX file: ");
  scanf("%s", file_path);

  string ihex_file = file_path;
  ihex *ihex_p = new ihex(ihex_file);
  int ihex_records;
  uint16_t addr, succ_addr;
  IHEX_RECORD ihex_rec;

  // Parse Intel HEX file
  if (ihex_p->parse() != IHEX_SUCCESS) {
    printf("*** ERROR : Parsing file %s\n", ihex_file.c_str());
    delete ihex_p;
    return;
  }

  // Check parse result
  ihex_records = ihex_p->nr_records();
  if (!ihex_records) {
    printf("*** ERROR : No records in file %s\n", ihex_file.c_str());
    delete ihex_p;
    return;
  }
  if (!ihex_p->get_min_addr(addr)) {
    printf("*** ERROR : No min addr in file %s\n", ihex_file.c_str());
    delete ihex_p;
    return;
  }

  // Handle all Intel HEX records
  while (ihex_records) {
    if (!ihex_p->get_record(addr, &ihex_rec)) {
      printf("*** ERROR: No record found for addr=0x%04x\n", addr);
      break;
    }
    if (--ihex_records) {
      if (!ihex_p->get_in_order_successor(addr, succ_addr)) {
	printf("*** ERROR: No successor for addr=0x%04x\n", addr);
	break;
      }
      addr = succ_addr;      
    }
    print_ihex_record(&ihex_rec);
  }

  // Clean-up
  delete ihex_p;
}
