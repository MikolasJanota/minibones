# -include githeaddef

PROGRAM_NAME=minibones

GENF=CoreBased.hh CoreBased.cc
CSRCS    = $(wildcard *.cc)
COBJS    = $(CSRCS:.cc=.o)
PROF_OBJS  = $(COBJS:.o=.gcda) $(COBJS:.o=.gcno) $(COBJS:.o=.gcov)
DEPENDS = ${COBJS:.o=.d}
PDFS    = $(NSRCS:.nw=.pdf)
LIBD =
LIBS =
CXX?=g++
CC?=gcc

CCFLAGS+=-I.

ifdef PROF
CCFLAGS+=-fprofile-arcs -ftest-coverage
CCFLAGS+=-pg
CCFLAGS+=-g
LNFLAGS+=-fprofile-arcs
LNFLAGS+=-pg
LNFLAGS+=-g
endif

ifdef DBG
CCFLAGS+=-O0
CCFLAGS+=-ggdb
CCFLAGS+=-DDBG
MINISAT_DIR=./minisat/build/debug/lib
MSAT=ld
else
CCFLAGS+=-DNDEBUG
CCFLAGS+=-O3
MSAT=lr
MINISAT_DIR=./minisat/build/release/lib
endif

LIBD+=-L$(MINISAT_DIR)
MINISAT_LIB=$(MINISAT_DIR)/libminisat.a

ifdef NOO
CCFLAGS+=-O0
endif

CCFLAGS += -Wall -Wextra -DBUILDDATE='"$(BUILDDATE)"' -DDISTDATE='"$(DISTDATE)"'
CCFLAGS+=-std=c++17
CCFLAGS+=-pedantic
CCFLAGS+=-MMD
LIBS+=-lz

ifdef STATIC
CCFLAGS+=-static
LNFLAGS+=-static
endif

ifdef EXPERT # allows using giving options, without options the solver's fairly dumb
CCFLAGS+=-DEXPERT
endif


LIBS+=-lminisat
CCFLAGS+=-I./minisat

.PHONY: sources all doc satmake make_defs

all: make_defs satmake
	$(MAKE) sources
	$(MAKE) $(PROGRAM_NAME)

satmake:
	@cd ./minisat ; $(MAKE) CXX=$(CXX) $(MSAT)

make_defs:
	@./update_defs.sh

$(PROGRAM_NAME):  $(COBJS) $(MINISAT_LIB)
	@echo Linking: $@
	$(CXX) -o $@ $(COBJS) $(LNFLAGS) $(LIBD) $(LIBS)

$(MINISAT_LIB): satmake

sources: $(GENF)

%.o: %.cc Makefile
	@echo Compiling: $@
	$(CXX) $(CCFLAGS) -c -o $@ $<

clean:
	@rm -f $(DEPENDS)
	@rm -f $(GENF)
	@rm -f $(PROF_OBJS)
	@rm -f $(PROGRAM_NAME) $(PROGRAM_NAME).exe $(COBJS) *.tab.[ch]
	cd ./minisat/; $(MAKE) clean

CoreBased.hh: CoreBased.nw
	notangle -L -RCoreBased.hh CoreBased.nw >CoreBased.hh

CoreBased.cc: CoreBased.nw
	notangle -L -RCoreBased.cc CoreBased.nw >CoreBased.cc

doc: CoreBased.nw
	noweave -index -filter C++.filter CoreBased.nw | \
        sed 's/documentclass{article}/documentclass{article}\\usepackage[pdftex,colorlinks=true,linkcolor=blue]{hyperref}/' > CoreBased.tex
	pdflatex CoreBased && pdflatex CoreBased

-include ${DEPENDS}
