//jpms:bc
/*----------------------------------------------------------------------------*\
 * File:        tool_config.hh
 *
 * Description: Tool configuration.
 *
 * Author:      jpms
 * 
 *                                     Copyright (c) 2009, Joao Marques-Silva
\*----------------------------------------------------------------------------*/
//jpms:ec

#ifndef _TOOL_CONFIG_H
#define _TOOL_CONFIG_H 1

//jpms:bc
/*----------------------------------------------------------------------------*\
 * Class: ToolConfig
 *
 * Purpose: Top-level configuration.
\*----------------------------------------------------------------------------*/
//jpms:ec

class ToolConfig {

public:
  virtual set_verbosity(int nverb) = 0;
  virtual int get_verbosity() = 0;

  virtual void set_logging() = 0;
  virtual bool get_logging() = 0;

  virtual void set_timeout(ULINT tout) = 0;
  virtual bool timeout() = 0;
};

#endif /* _TOOL_CONFIG_H */

/*----------------------------------------------------------------------------*/
