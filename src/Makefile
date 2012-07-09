# BUILDDATE = $(shell date +'%Y-%b-%d %R')
# DISTDATE  = $(shell date +'%Y-%m-%d')
# CURRDATE  = $(shell date +'%Y%m%d')
# CHANGESET = $(shell $(SRCDIR)/scripts/hg-changeset)

DEPFILE	 = .depends
DEPTOKEN = '\# MAKEDEPENDS'
DEPFLAGS = -Y -f $(DEPFILE) -s $(DEPTOKEN) 
CSRCS    = $(wildcard *.cc) 
COBJS    = $(CSRCS:.cc=.o)
LIBD =
LIBS =
CXX?=g++

ifdef PROF 
CFLAGS+=-pg
LNFLAGS+=-pg
endif

ifdef DBG
CFLAGS+=-O0
CFLAGS+=-ggdb
CFLAGS+=-DDBG
MSAT=libd
else
MSAT=libr
CFLAGS+=-DNDEBUG
CFLAGS+=-O3
endif

ifdef NOO
CFLAGS+=-O0
endif

CFLAGS += -Wall -DBUILDDATE='"$(BUILDDATE)"' -DDISTDATE='"$(DISTDATE)"'
CFLAGS += -DCHANGESET='"$(CHANGESET)"' -DRELDATE='"$(RELDATE)"'
CFLAGS+=-D __STDC_LIMIT_MACROS -D __STDC_FORMAT_MACROS -Wno-parentheses -Wno-deprecated -D _MSC_VER
CFLAGS+=-I./minisat/
CFLAGS+=-I./picosat/
CFLAGS+=-std=c++0x
LIBS+=-lz -lminisat

ifdef STATIC
CFLAGS+=-static
LNFLAGS+=-static
endif

ifdef USE_PICOSAT
CFLAGS+=-DUSE_PICOSAT
OTHER="./picosat/*.o"
endif

.PHONY: m p t

all:  minibones

minibones:  m p $(COBJS) 
	@echo Linking: $@
	@$(CXX)  -o $@ $(COBJS) $(OTHER) $(LNFLAGS) $(LIBD) $(LIBS)

m: 
	export MROOT=`pwd`/minisat ; cd ./minisat/core; make CXX=$(CXX) LIB=minisat $(MSAT)


lib: all
	ar -rcsv libmifumax.a `ls -1 *.o | grep -v 'main.o'`

p:
	cd picosat ; make

depend:
	rm -f $(DEPFILE)
	@echo $(DEPTOKEN) > $(DEPFILE)
	makedepend $(DEPFLAGS) -- $(CFLAGS) -- $(CSRCS)

t:
	bash -c './minibones ../examples/t1.cnf ; [ $$? == 10 ]'

## Build rule
%.o:	%.cc
	@echo Compiling: $@
	@$(CXX) $(CFLAGS) -c -o $@ $<

## Clean rule
clean:
	@rm -f minibones minibones.exe $(COBJS)
	@export MROOT=`pwd`/minisat ; cd ./minisat/core; make CXX=$(CXX) clean
	@rm -rf ./minisat/objs
	@cd picosat ; make clean
	@rm -f $(DEPFILE)

### Makefile ends here
sinclude $(DEPFILE)
