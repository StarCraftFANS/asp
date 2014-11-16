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

#ifndef __IHEX_H__
#define __IHEX_H__

#include <stdint.h>
#include <string>

using namespace std;

/////////////////////////////////////////////////////////////////////////////
//               Definition of macros
/////////////////////////////////////////////////////////////////////////////
// Return codes
#define IHEX_SUCCESS        0
#define IHEX_FILE_ERROR    -1
#define IHEX_FORMAT_ERROR  -2

// Intel HEX standard record types
#define IHEX_REC_DATA  0x00
#define IHEX_REC_EOF   0x01

#define IHEX_RECORD_MAX_BYTES  32

/////////////////////////////////////////////////////////////////////////////
//               Class support types
/////////////////////////////////////////////////////////////////////////////
// Intel HEX file record
typedef struct {
	uint8_t  nbytes;
	uint16_t addr;
	uint8_t  type;
	uint8_t  data[IHEX_RECORD_MAX_BYTES];
	uint8_t  chksum;
} IHEX_RECORD;

// Binary Search Tree
struct bst_node;
typedef struct bst_node BST_NODE;

typedef struct bst_node {
  int         key;
  IHEX_RECORD data;
  BST_NODE    *left;
  BST_NODE    *right;
} BST_NODE;

/////////////////////////////////////////////////////////////////////////////
//               Definition of classes
/////////////////////////////////////////////////////////////////////////////

class ihex {

 public:
  ihex(string ihex_file);
  ~ihex(void);

  long parse(void);

  bool get_record(uint16_t addr, IHEX_RECORD *rec);

  bool get_min_addr(uint16_t &addr);

  bool get_in_order_successor(uint16_t addr,
			      uint16_t &succ_addr);

  int nr_records(void);

  void erase(void);

 private:
  string m_ihex_file;

  BST_NODE *m_root_node;

  int parse_ihex_line(const char *line, IHEX_RECORD *rec);

  BST_NODE* new_bst_node(int key, const IHEX_RECORD *data);

  BST_NODE* insert_bst(BST_NODE *node,
		       int key,
		       const IHEX_RECORD *data);

  const BST_NODE* search_bst(const BST_NODE *node, int key);

  BST_NODE* min_key_bst(const BST_NODE *node);

  BST_NODE* in_order_successor_bst(BST_NODE *root,
				   const BST_NODE *node);

  int nodes_bst(const BST_NODE *node);

  void erase_bst(BST_NODE **node);
};

#endif // __IHEX_H__
