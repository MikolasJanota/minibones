/* 
 * File:   Reader.cc
 * Author: mikolas
 * 
 * Created on January 12, 2011, 4:19 PM
 */

#include "Reader.hh"
#include <iostream>
Reader::Reader(istream& stream) : s(stream) { ++(*this); }
Reader::Reader(const Reader& orig) : s(orig.s), c(orig.c) {}
Reader::~Reader() {}

int  Reader::operator *  () { return c; }

void Reader::operator ++ () { 
    if (s.eof()) c=EOF;
    else c = s.get();
}

void Reader::skip_whitespace()  {
      while (((**this) >= 9 && (**this) <= 13) || (**this) == 32) ++(*this);
  }

void Reader::skip_line(){
    for (;;) {
        if ((**this) == EOF) return;
        if ((**this) == '\n') { ++(*this); return; }
        if ((**this) == '\r') { ++(*this); return; }
        ++(*this);
    }
}
