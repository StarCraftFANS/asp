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

#include <fstream>

#include "at89s52_isp.h"
#include "ihex.h"
#include "asp_hw.h"
#include "delay.h"

/////////////////////////////////////////////////////////////////////////////
//               Definition of macros
/////////////////////////////////////////////////////////////////////////////
#define ISP_BUFFER_SIZE  1024  // Bytes

/////////////////////////////////////////////////////////////////////////////
//               Definition of constants
/////////////////////////////////////////////////////////////////////////////
// Internal exceptions
const int internal_error = 1;

/////////////////////////////////////////////////////////////////////////////
//               Public member functions
/////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////

at89s52_isp::at89s52_isp(void)
{
  // Create GPIO object for RESET
  m_gpio = new gpio();

  // Create AT89S52 I/O object
  m_at89s52_io = new at89s52_io(ASP_SPI_DEV,
				ASP_SPI_SPEED);

  // Create data buffer
  m_buffer = new uint8_t[ISP_BUFFER_SIZE];
}

////////////////////////////////////////////////////////////////

at89s52_isp::~at89s52_isp(void)
{
  delete[] m_buffer;
  delete m_at89s52_io;
  delete m_gpio;  
}

////////////////////////////////////////////////////////////////

long at89s52_isp::initialize(void)
{
  long rc;

  // Initialize GPIO
  rc = m_gpio->initialize();
  if (rc != GPIO_SUCCESS) {
    printf("*** ERROR : Initialize GPIO, rc=%ld\n", rc);   
    return AT89S52_ISP_FAILURE;
  }
  m_gpio->get_function(ASP_PIN_RESET, m_pin_reset_func);
  m_gpio->set_function(ASP_PIN_RESET, GPIO_FUNC_OUT);

  // Initialize AT89S52 I/O
  rc = m_at89s52_io->initialize();
  if (rc != AT89S52_SUCCESS) {
    printf("*** ERROR : Initialize AT89S52, rc=%ld\n", rc);
    m_gpio->set_function(ASP_PIN_RESET, m_pin_reset_func);
    m_gpio->finalize();
    return AT89S52_ISP_FAILURE;
  }

  return AT89S52_ISP_SUCCESS;
}

////////////////////////////////////////////////////////////////

long at89s52_isp::finalize(void)
{
  long rc;

  // Finalize AT89S52 I/O
  rc = m_at89s52_io->finalize();
  if (rc != AT89S52_SUCCESS) {
    printf("*** ERROR : Finalize AT89S52, rc=%ld\n", rc);
    m_gpio->set_function(ASP_PIN_RESET, m_pin_reset_func);
    m_gpio->finalize();
    return AT89S52_ISP_FAILURE;
  }

  // Finalize GPIO
  m_gpio->set_function(ASP_PIN_RESET, m_pin_reset_func);
  rc = m_gpio->finalize();
  if (rc != GPIO_SUCCESS) {
    printf("*** ERROR : Finalize GPIO, rc=%ld\n", rc);
    return AT89S52_ISP_FAILURE;
  }

  return AT89S52_ISP_SUCCESS;
}

////////////////////////////////////////////////////////////////

long at89s52_isp::probe(void)
{
  try {
    this->enable_isp();  // Activate RESET pin
    this->isp_probe();
    this->disable_isp(); // Deactivate RESET pin
  }
  catch (...) {
    this->disable_isp(); // Deactivate RESET pin
    return AT89S52_ISP_FAILURE;
  }

  return AT89S52_ISP_SUCCESS;
}

////////////////////////////////////////////////////////////////

long at89s52_isp::erase(void)
{
  try {
    this->enable_isp();  // Activate RESET pin
    this->isp_erase();
    this->disable_isp(); // Deactivate RESET pin
  }
  catch (...) {
    this->disable_isp(); // Deactivate RESET pin
    return AT89S52_ISP_FAILURE;
  }

  return AT89S52_ISP_SUCCESS;
}

////////////////////////////////////////////////////////////////

long at89s52_isp::read_bin(const string file_path)
{
  try {
    this->enable_isp();  // Activate RESET pin
    this->isp_read_bin(file_path);
    this->disable_isp(); // Deactivate RESET pin
  }
  catch (...) {
    this->disable_isp(); // Deactivate RESET pin
    return AT89S52_ISP_FAILURE;
  }

  return AT89S52_ISP_SUCCESS;
}

////////////////////////////////////////////////////////////////

long at89s52_isp::write_hex(const string file_path)
{
  try {
    this->enable_isp();  // Activate RESET pin
    this->isp_erase();
    this->isp_write_hex(file_path);
    this->disable_isp(); // Deactivate RESET pin
  }
  catch (...) {
    this->disable_isp(); // Deactivate RESET pin
    return AT89S52_ISP_FAILURE;
  }

  return AT89S52_ISP_SUCCESS;
}

////////////////////////////////////////////////////////////////

long at89s52_isp::write_bin(const string file_path)
{
  try {
    this->enable_isp();  // Activate RESET pin
    this->isp_erase();
    this->isp_write_bin(file_path);
    this->disable_isp(); // Deactivate RESET pin
  }
  catch (...) {
    this->disable_isp(); // Deactivate RESET pin
    return AT89S52_ISP_FAILURE;
  }

  return AT89S52_ISP_SUCCESS;
}

/////////////////////////////////////////////////////////////////////////////
//               Private member functions
/////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////

void at89s52_isp::enable_isp(void)
{
  long rc;

  m_gpio->set_pin_high(ASP_PIN_RESET);     // Activate RESET pin

  // RESET must be active for a while, before any command is sent
  rc = delay(AT89S52_RESET_ACTIVE_MIN_TIME);
  if (rc != DELAY_SUCCESS) {
    printf("*** ERROR : RESET delay failed for AT89S52, rc=%ld\n", rc);    
    throw internal_error;
  }

  rc = m_at89s52_io->enable_programming(); // Enable serial programming
  if (rc != AT89S52_SUCCESS) {
    printf("*** ERROR : Enable isp AT89S52, rc=%ld\n", rc);    
    throw internal_error;
  }
}

////////////////////////////////////////////////////////////////

void at89s52_isp::disable_isp(void)
{
  m_gpio->set_pin_low(ASP_PIN_RESET); // Deactivate RESET pin
}

////////////////////////////////////////////////////////////////

void at89s52_isp::isp_probe(void)
{
  long rc;
  uint32_t signature;
  const uint32_t expected_signature = AT89S52_SIGNATURE;

  rc = m_at89s52_io->read_signature(signature);
  if (rc != AT89S52_SUCCESS) {
    printf("*** ERROR : Read signature AT89S52, rc=%ld\n", rc);
    throw internal_error;
  }

  if (signature == expected_signature) {
    printf("Found AT89S52, signature=0x%08x\n", signature);
  }
  else {
    printf("*** ERROR : Unexpected AT89S52 signature=0x%08x, expected=0x%08x\n",
	   signature, expected_signature);
    throw internal_error;
  }
}

////////////////////////////////////////////////////////////////

void at89s52_isp::isp_erase(void)
{
  long rc;

  rc = m_at89s52_io->chip_erase();
  if (rc != AT89S52_SUCCESS) {
    printf("*** ERROR : Chip erase AT89S52, rc=%ld\n", rc);
    throw internal_error;
  }
}

////////////////////////////////////////////////////////////////

void at89s52_isp::isp_read_bin(const string file_path)
{
  // Destination file to where data shall be written
  ofstream data_file(file_path.c_str(),
		     ios::out | ios::binary | ios::trunc);

  if (!data_file.is_open()) {
    printf("*** ERROR : Open failed for file %s\n", file_path.c_str());
    throw internal_error;
  }

  // Get size of chip in bytes and buffers
  int nr_bytes = AT89S52_FLASH_MEMORY_SIZE;
  int nr_buffers = nr_bytes / ISP_BUFFER_SIZE;

  long rc;
  uint16_t addr = 0;

  // Read data from chip (one buffer at a time), write to file
  try {
    // Read full buffers from chip
    while (nr_buffers) {
      rc = m_at89s52_io->read_flash(addr,
				    m_buffer,
				    ISP_BUFFER_SIZE);
      if (rc != AT89S52_SUCCESS) {
	printf("*** ERROR : Read flash AT89S52, addr=0x%04x, rc=%ld\n",
	       addr, rc);
	throw internal_error;
      }
      data_file.write((char *)m_buffer, ISP_BUFFER_SIZE);
      addr += ISP_BUFFER_SIZE;
      nr_bytes -= ISP_BUFFER_SIZE;
      nr_buffers--;
    }

    // Read remaining bytes from chip
    if (nr_bytes) {
      rc = m_at89s52_io->read_flash(addr,
				    m_buffer,
				    nr_bytes);
      if (rc != AT89S52_SUCCESS) {
	printf("*** ERROR : Write flash AT89S52, addr=0x%04x, rc=%ld\n",
	       addr, rc);
	throw internal_error;
      }
      data_file.write((char *)m_buffer, nr_bytes);
    }
  }
  catch (...) {
    data_file.close();
    throw;
  }
  data_file.close();
}

////////////////////////////////////////////////////////////////

void at89s52_isp::isp_write_hex(const string file_path)
{
  ihex *ihex_p = new ihex(file_path);
  int ihex_records;
  uint16_t addr, succ_addr;
  IHEX_RECORD ihex_rec;
  long rc;
  
  try {
    // Parse Intel HEX file
    if (ihex_p->parse() != IHEX_SUCCESS) {
      printf("*** ERROR : Parsing HEX file %s\n", file_path.c_str());
      throw internal_error;
    }

    // Check parse result
    ihex_records = ihex_p->nr_records();
    if (!ihex_records) {
      printf("*** ERROR : No records in HEX file %s\n", file_path.c_str());
      throw internal_error;
    }
    if (!ihex_p->get_min_addr(addr)) {
      printf("*** ERROR : No min addr in HEX file %s\n", file_path.c_str());
      throw internal_error;
    }

    // Handle all Intel HEX records
    while (ihex_records) {
      if (!ihex_p->get_record(addr, &ihex_rec)) {
	printf("*** ERROR: No record found for addr=0x%04x\n", addr);
	throw internal_error;
      }
      if (--ihex_records) {
	if (!ihex_p->get_in_order_successor(addr, succ_addr)) {
	  printf("*** ERROR: No successor for addr=0x%04x\n", addr);
	  throw internal_error;
	}
	addr = succ_addr;      
      }
      // Write data to chip
      rc = m_at89s52_io->write_flash(ihex_rec.addr,
				     ihex_rec.data,
				     ihex_rec.nbytes);
      if (rc != AT89S52_SUCCESS) {
	printf("*** ERROR : Write flash AT89S52, addr=0x%04x, rc=%ld\n",
	       ihex_rec.addr, rc);
	throw internal_error;
      }
    }
  }
  catch (...) {
    delete ihex_p;
    throw;
  }
  delete ihex_p;
}

////////////////////////////////////////////////////////////////

void at89s52_isp::isp_write_bin(const string file_path)
{
  // Source file from where data shall be read
  ifstream data_file(file_path.c_str(),
		     ios_base::in | ios::binary);

  if (!data_file.is_open()) {
    printf("*** ERROR : Open failed for file %s\n", file_path.c_str());
    throw internal_error;
  }

  // Get size of file in bytes and buffers
  int nr_bytes;
  data_file.seekg(0, ios::end);
  nr_bytes = data_file.tellg();
  data_file.seekg(0, ios::beg);
  int nr_buffers = nr_bytes / ISP_BUFFER_SIZE;

  long rc;
  uint16_t addr = 0;
  
  // Read data from file (one buffer at a time), write to chip
  try {
    // Write full buffers to chip
    while (nr_buffers) {
      data_file.read((char *)m_buffer, ISP_BUFFER_SIZE);
      rc = m_at89s52_io->write_flash(addr,
				     m_buffer,
				     ISP_BUFFER_SIZE);
      if (rc != AT89S52_SUCCESS) {
	printf("*** ERROR : Write flash AT89S52, addr=0x%04x, rc=%ld\n",
	       addr, rc);
	throw internal_error;
      }
      addr += ISP_BUFFER_SIZE;
      nr_bytes -= ISP_BUFFER_SIZE;
      nr_buffers--;
    }

    // Write remaining bytes to chip
    if (nr_bytes) {
      data_file.read((char *)m_buffer, nr_bytes);
      rc = m_at89s52_io->write_flash(addr,
				     m_buffer,
				     nr_bytes);
      if (rc != AT89S52_SUCCESS) {
	printf("*** ERROR : Write flash AT89S52, addr=0x%04x, rc=%ld\n",
	       addr, rc);
	throw internal_error;
      }
    }
  }
  catch (...) {
    data_file.close();
    throw;
  }
  data_file.close();
}
