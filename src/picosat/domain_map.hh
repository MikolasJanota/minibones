/*----------------------------------------------------------------------------*\
 * Version: $Id: domain_map.hh 73 2007-07-26 15:16:48Z jpms $
 *
 * Author: jpms
 * 
 * Description: Definition of the DomainMap interface.
 *
 *                                    Copyright (c) 2006, Joao Marques-Silva
\*----------------------------------------------------------------------------*/

#ifndef _DOMAIN_MAP_HH_
#define _DOMAIN_MAP_HH_ 1


/*----------------------------------------------------------------------------*\
 * Class: DomainMap
 *
 * Purpose: Maintains an ID map (the domain map) and provides access to it
\*----------------------------------------------------------------------------*/

class DomainMap {

public:

  DomainMap(IDMap& map) : _dmap(map) { }

  inline IDMap& dmap() { return _dmap; }

protected:

  IDMap& _dmap;  // ID map between two domains

};

#endif /* _DOMAIN_MAP_HH_ */

/*----------------------------------------------------------------------------*/
