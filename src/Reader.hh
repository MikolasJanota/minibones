/* 
 * File:   Reader.hh
 * Author: mikolas
 *
 * Created on January 12, 2011, 4:19 PM
 */

#ifndef READER_HH
#define	READER_HH
#include <istream>
#include <utility>
#include <stdio.h>
using std::istream;

class Reader {
public:
    Reader(istream& stream);
    Reader(const Reader& orig);
    virtual ~Reader();
    int operator *  ();
    void operator ++ ();

  void skip_line();
  void skip_whitespace();
private:
//    StreamBuffer* zip;
    istream& s;
    int c;
};

#endif	/* READER_HH */

