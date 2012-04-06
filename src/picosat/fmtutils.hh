//jpms:bc
/*----------------------------------------------------------------------------*\
 * File:        fmtutils.hh
 *
 * Description: Utils for parsing DIMACS-based formats.
 *
 * Author:      jpms
 * 
 * Revision:    $Id$.
 *
 *                                     Copyright (c) 2009, Joao Marques-Silva
\*----------------------------------------------------------------------------*/
//jpms:ec

#ifndef _FMTUTILS_H
#define _FMTUTILS_H 1


//jpms:bc
/*----------------------------------------------------------------------------*\
 * Utils for DIMACS format parsing
 * (This borrows **extensively** from the MiniSAT parser)
\*----------------------------------------------------------------------------*/
//jpms:ec

#define CHUNK_LIMIT 1048576
#define SMALL_CHUNK_LIMIT 1024

class StreamBuffer {
protected:
  gzFile  in;
  char    *buf; //buf[CHUNK_LIMIT];
  int     pos;
  int     size;

  void assureLookahead() {
    if (pos >= size) {
      pos  = 0;
      size = gzread(in, buf, sizeof(buf)); } }

  virtual void resize_buffer() { buf = new char[CHUNK_LIMIT]; }

public:
  StreamBuffer(gzFile i) : in(i), pos(0), size(0) {
    resize_buffer(); assureLookahead(); }

  virtual ~StreamBuffer() { delete buf; }

  int  operator *  () { return (pos >= size) ? EOF : buf[pos]; }
  void operator ++ () { pos++; assureLookahead(); }
};


class SmallStreamBuffer : public StreamBuffer {
protected:
  virtual void resize_buffer() { buf = new char[SMALL_CHUNK_LIMIT]; }

public:
  SmallStreamBuffer(gzFile i) : StreamBuffer(i) { }
  //virtual ~SmallStreamBuffer() { delete buf; }
  virtual ~SmallStreamBuffer() { }
};


//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

template<class B>
static void skipWhitespace(B& in) {
    while ((*in >= 9 && *in <= 13) || *in == 32)
        ++in; }

template<class B>
static void skipTrueWhitespace(B& in) {     // not including newline
    while (*in == ' ' || *in == '\t')
        ++in; }

template<class B>
static void skipTabSpace(B& in) { while (*in == 9 || *in == 32) ++in; }

template<class B>
static void skipLine(B& in) {
    for (;;){
        if (*in == EOF) return;
        if (*in == '\n') { ++in; return; }
        if (*in == '\r') { ++in; return; }
        ++in; } }

template<class B>
static bool skipEndOfLine(B& in) {     // skip newline AND trailing comment/empty lines
    if (*in == '\n' || *in == '\r') ++in;
    else             return false;
    skipComments(in);
    return true; }

template<class B>
static bool skipText(B& in, char* text) {
    while (*text != 0){
        if (*in != *text) return false;
        ++in, ++text; }
    return true; }

template<class B>
static string readString(B& in) {
  string rstr;
  skipWhitespace(in);
  while (*in >= '0' && *in <= '9' ||
	 *in >= 'a' && *in <= 'z' ||
	 *in >= 'A' && *in <= 'Z' ||
	 (*in == '_') || (*in == '.')) {
    rstr.push_back(*in); ++in;
  }
  return rstr; }

template<class B>
static LINT parseInt(B& in) {
    LINT    val = 0;
    bool    neg = false;
    skipWhitespace(in);
    if      (*in == '-') neg = true, ++in;
    else if (*in == '+') ++in;
    if (*in < '0' || *in > '9') fprintf(stderr, "PARSE ERROR! Unexpected char: %c\n", *in), exit(3);
    while (*in >= '0' && *in <= '9')
        val = val*10 + (*in - '0'),
        ++in;
    return neg ? -val : val; }

template<class B>
static XLINT parseLongInt(B& in) {
    XLINT   val = 0;
    bool    neg = false;
    skipWhitespace(in);
    if      (*in == '-') neg = true, ++in;
    else if (*in == '+') ++in;
    if (*in < '0' || *in > '9') fprintf(stderr, "PARSE ERROR! Unexpected char: %c\n", *in), exit(3);
    while (*in >= '0' && *in <= '9')
        val = val*10 + (*in - '0'),
        ++in;
    XLINT rval = (neg) ? (-1*val) : val;
    return rval; }

// Additional functions for WBO/PBO

template<class B>
static void parseIdent(B& in, string & tmp)
{
	// 'tmp' is cleared, then filled with the parsed string. '(char*)tmp' is returned for convenience.
	skipWhitespace(in);
    //if ((*in < 'a' || *in > 'z') && (*in < 'A' || *in > 'Z') && *in != '_') throw nsprintf("Expected start of identifier, not: %c", *in);
    tmp.clear();
    tmp+=*in;
    ++in;
    while ((*in >= 'a' && *in <= 'z') || (*in >= 'A' && *in <= 'Z') || (*in >= '0' && *in <= '9') || *in == '_')
        tmp+=*in,
        ++in;
    //tmp+="0";
    //return (char*)tmp;
}

template<class B>
void parseExpr(B& in,vector<string> & out_ps, vector<XLINT> & out_Cs, string & tmp)
    // NOTE! only uses "getVar()" method of solver; doesn't add anything.
    // 'tmp' is a tempory, passed to avoid frequent memory reallocation.
{
    bool empty = true;
    for(;;){
        skipWhitespace(in);
        if ((*in < '0' || *in > '9') && *in != '+' && *in != '-') break;
        out_Cs.push_back(parseLongInt(in));
        skipWhitespace(in);
        //if (*in != ' ') throw xstrdup("Missing ' ' after coefficient.");
        //++in;
        parseIdent(in, tmp);
        string varId = tmp;
        out_ps.push_back(varId);
        //out_ps.push(Lit(solver.getVar(parseIdent(in, tmp))));
        empty = false;
    }
    //if (empty) throw xstrdup("Empty expression.");
}


template<class B>
int parseInequality(B& in)
{
    int ineq;
    skipWhitespace(in);
    if (*in == '<'){
        ++in;
        if (*in == '=') ineq = -1, ++in;
        else            ineq = -2;
    }else if (*in == '>'){
        ++in;
        if (*in == '=') ineq = +1, ++in;
        else            ineq = +2;
    }else{
        if (*in == '='){
            ++in;
            if (*in == '=') ++in;
            ineq = 0;
        }
    //else throw nsprintf("Expected inequality, not: %c", *in);
    }
    return ineq;
}

template<class B>
bool parse_pb_constraint(B& in, vector<string> & rvars, vector<XLINT> & weights, int & ineq, XLINT & rhs)
{
    string tmp;
	parseExpr(in, rvars, weights, tmp);
	ineq = parseInequality(in);
	rhs  = parseLongInt(in);
	tmp.clear();
	return true;
}

template<class B>
void show_pb_constraint(B& in, vector<LINT> & rvars, vector<XLINT> & weights, int & ineq, XLINT & rhs)
{
	// Make positive the negative weights
	for(unsigned int i=0;i<weights.size(); i++)
	{
		cout << weights[i] << " " << rvars[i] << " ";
	}
	if(ineq == -2) cout << "<";
	else if(ineq == -1) cout << "<=";
	else if(ineq == 0) cout << "=";
	else if(ineq == 1) cout << ">=";
	else cout << ">";
	cout << " ";
	cout << rhs << " ;" << endl;
}

template<class B>
void normalize_pb_constraint(B& in, vector<LINT> & rvars, vector<XLINT> & weights, int & ineq, XLINT & rhs)
{
	// -1 <= AtMost
	// 1 >= AtLeast
	if(ineq == -2) { rhs--; ineq++; } // from < 20 to <= 19
	else if(ineq == 2) { rhs++; ineq--; } // from > 20 to >= 21

	// In case of AtLeast, convert to AtMost
	if(ineq > 0)
	{
		// Negate all coeffients and change operator
		for(unsigned int i=0; i < weights.size(); i++)
		{
			weights[i]=-weights[i];
		}
		rhs=-rhs;
		ineq = -ineq;
	}

	// Make positive the negative weights
	XLINT sumNeg=0;
	for(unsigned int i=0;i<weights.size(); i++)
	{
		if(weights[i]<0)
		{
			weights[i]=-weights[i];
			sumNeg+=weights[i];
			rvars[i]=-rvars[i];
		}
	}
	rhs+=sumNeg;
}

template<class B>
bool is_card_constraint(B& in, vector<LINT> & rvars, vector<XLINT> & weights)
{
	for(unsigned int i=0;i<weights.size(); i++)
	{
		if(weights[i]!=1) return false;
	}
	return true;
}

template<class B>
static void skip_comments(B& in)
{
	bool end=false;
	while(!end)
	{
		if(*in == '*') skipLine(in);
		else end=true;
	}
}

#endif /* _FMTUTILS_H */

/*----------------------------------------------------------------------------*/
