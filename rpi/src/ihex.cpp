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

#include <string.h>
#include <iostream>
#include <fstream>

#include "ihex.h"

/////////////////////////////////////////////////////////////////////////////
//               Public member functions
/////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////

ihex::ihex(string ihex_file)
{
  m_ihex_file = ihex_file;
  m_root_node = 0;
}

////////////////////////////////////////////////////////////////

ihex::~ihex(void)
{
  this->erase();
}

////////////////////////////////////////////////////////////////

long ihex::parse(void)
{
  // Check if already parsed
  if (m_root_node) {
    this->erase();
  }
  
  ifstream ihex_file(m_ihex_file.c_str());
  string line;
  IHEX_RECORD ihex_record;

  // Parse file, line by line
  if ( ihex_file.is_open() ) {
    while ( getline(ihex_file, line) ) {
			
      if (!parse_ihex_line(line.c_str(),
			   &ihex_record) ) {
	ihex_file.close();
	return IHEX_FORMAT_ERROR;
      }
      else {
	if (ihex_record.type == IHEX_REC_DATA) {
	  m_root_node = insert_bst(m_root_node,
				   ihex_record.addr, // Key
				   &ihex_record);    // Data
	}
      }      
      line.clear();
    }
    ihex_file.close();
  }
  else {
    return IHEX_FILE_ERROR;
  }
  return IHEX_SUCCESS;
}

////////////////////////////////////////////////////////////////

bool ihex::get_record(uint16_t addr, IHEX_RECORD *rec)
{
  const BST_NODE *node = search_bst(m_root_node, addr);

  // Check if key found
  if (node) {
    memcpy(rec, &node->data, sizeof(*rec));
    return true;
  }
  else {
    return false;
  }
}

////////////////////////////////////////////////////////////////

bool ihex::get_min_addr(uint16_t &addr)
{
  const BST_NODE *node = min_key_bst(m_root_node);

  // Check if key found
  if (node) {
    addr = node->key;
    return true;
  }
  else {
    return false;
  }
}

////////////////////////////////////////////////////////////////

bool ihex::get_in_order_successor(uint16_t addr,
				  uint16_t &succ_addr)
{
  const BST_NODE *node = search_bst(m_root_node, addr);

  // Check if key found
  if (node) {

    const BST_NODE *succ_node = in_order_successor_bst(m_root_node, node);

    // Check if successor key found
    if (succ_node) {
      succ_addr = succ_node->key;
      return true;
    }
    else {
      return false;
    }
  }
  else {
    return false;
  }
}

////////////////////////////////////////////////////////////////

int ihex::nr_records(void)
{
  return nodes_bst(m_root_node);
}

////////////////////////////////////////////////////////////////

void ihex::erase(void)
{
  erase_bst(&m_root_node);
}

/////////////////////////////////////////////////////////////////////////////
//               Private member functions
/////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////

int ihex::parse_ihex_line(const char *line, IHEX_RECORD *rec)
{
  unsigned int value;
  int sum;
  char *ptr = (char *)line;
  
  if (ptr[0] != ':') return 0;
  if (strlen(line) < 11) return 0;
  
  ptr += 1;
  if (!sscanf(ptr, "%02x", &value)) return 0;
  rec->nbytes = value;
  
  if ( (int)strlen(line) < ((int)11 + (rec->nbytes * 2)) ) return 0;
  
  ptr += 2;
  if (!sscanf(ptr, "%04x", &value)) return 0;
  rec->addr = value;
  
  ptr += 4;
  if (!sscanf(ptr, "%02x", &value)) return 0;
  rec->type = value;
  
  ptr += 2;
  sum = (rec->nbytes & 0xff) +
    ((rec->addr >> 8) & 0xff) + (rec->addr & 0xff) + 
    (rec->type & 0xff);
  for (int i=0; i < rec->nbytes; i++) {
    if (!sscanf(ptr, "%02x", &value)) return 0;
    rec->data[i] = value;
    ptr += 2;
    sum += rec->data[i] & 0xff;
  }
  
  if (!sscanf(ptr, "%02x", &value)) return 0;
  rec->chksum = value;
  
  if ( ((sum & 0xff) + (rec->chksum & 0xff)) & 0xff ) return 0; // Checksum error
  
  return 1;
}

////////////////////////////////////////////////////////////////

BST_NODE* ihex::new_bst_node(int key, const IHEX_RECORD *data)
{
  BST_NODE *node = new BST_NODE;

  node->key = key;
  memcpy(&node->data, data, sizeof(*data));
 
  node->left  = 0;
  node->right = 0;

  return node;
}

////////////////////////////////////////////////////////////////

BST_NODE* ihex::insert_bst(BST_NODE *node,
			   int key,
			   const IHEX_RECORD *data)
{
  // 1. If the tree is empty, return a new single node
  if (!node) {
    return new_bst_node(key, data);
  }
  else {
    // 2. Otherwise, recurse down the tree
    if (key <= node->key) {
      node->left = insert_bst(node->left, key, data);
    }
    else {
      node->right = insert_bst(node->right, key, data);
    }

    return node; // return the (unchanged) node pointer
  }
}

////////////////////////////////////////////////////////////////

const BST_NODE* ihex::search_bst(const BST_NODE *node, int key)
{
  // 1. Base case is empty tree
  // target is not found so return false
  if (!node) { 
    return 0;
  }
  else {
    // 2. see if found here 
    if (key == node->key) {
      return node;
    }
    else {
      // 3. otherwise recurse down the correct subtree
      if (key < node->key) {
	return search_bst(node->left, key);
      }
      else {
	return search_bst(node->right, key);
      }
    }
  }
}

////////////////////////////////////////////////////////////////

BST_NODE* ihex::min_key_bst(const BST_NODE *node)
{
  BST_NODE *current = (BST_NODE *)node;
  
  // Loop down to find the leftmost node
  while (current->left) {
    current = current->left;
  }
  return current;
}

////////////////////////////////////////////////////////////////

BST_NODE* ihex::in_order_successor_bst(BST_NODE *root,
				      const BST_NODE *node)
{
  // If right subtree of node is not NULL, then succ lies
  // in right subtree. Go to right subtree and return the
  // node with minimum key value in right subtree.
  if (node->right) {
    return min_key_bst(node->right);
  }
  
  BST_NODE *succ = 0;
  
  // Start from root and search for successor down the tree.
  // If a node’s data is greater than root’s data then go
  // right side, otherwise go to left side.
  while (root) {
    if (node->key < root->key) {
      succ = root;
      root = root->left;
    }
    else if (node->key > root->key) {
      root = root->right;
    }
    else {
      break;
    }
  }
  
  return succ;
}

////////////////////////////////////////////////////////////////

int ihex::nodes_bst(const BST_NODE *node)
{
  if (!node) {
    return 0; // No nodes in tree
  } else {
    // Compute the number of nodes in a tree
    return (nodes_bst(node->left) + 1 + nodes_bst(node->right));
  }
}

////////////////////////////////////////////////////////////////

void ihex::erase_bst(BST_NODE **node)
{
  if (*node) {
    erase_bst(&(*node)->left);
    erase_bst(&(*node)->right);
		
    delete *node;

    *node = 0;
  }
}
