/*
 * cl_id_manager.hh
 *
 *  Created on: Aug 28, 2011
 *      Author: fheras
 */

#pragma once

#include "globals.hh"

/* Manager for clause IDs. 
 * Note: not MT-safe 
 */
class ClauseIdManager
{
public:
  
  static ClauseIdManager * Instance(void) { 
    if (!instance) 
      instance = new ClauseIdManager();
    return instance;
  }

  ULINT new_id(void) { return _id++; }

  ULINT id(void) { return _id; }

protected:

  ClauseIdManager() : _id(1) {}

private:

  ULINT _id;

  static ClauseIdManager * instance;

};
