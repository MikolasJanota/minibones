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
else
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
LIBS+=-lz

ifdef STATIC
CFLAGS+=-static
LNFLAGS+=-static
endif

ifdef USE_PICOSAT
CFLAGS+=-DUSE_PICOSAT
endif

.PHONY: m p

all:  minibones

minibones:  m $(COBJS) 
	@echo Linking: $@
	@$(CXX)  -o $@ $(COBJS) ./picosat/*.o ./minisat/objs/*.o $(LNFLAGS) $(LIBD) $(LIBS)

m: 
	export MROOT=`pwd`/minisat ; cd ./minisat/core; make CXX=$(CXX) objs 
	export MROOT=`pwd`/minisat ; cd ./minisat/utils; make CXX=$(CXX) objs
	mkdir -p minisat/objs ; cd minisat/objs ; cp ../core/*.o .; cp ../utils/*.o . ; rm Main.*

p:
	cd p ; make

depend:
	rm -f $(DEPFILE)
	@echo $(DEPTOKEN) > $(DEPFILE)
	makedepend $(DEPFLAGS) -- $(CFLAGS) -- $(CSRCS)

## Build rule
%.o:	%.cc
	@echo Compiling: $@
	@$(CXX) $(CFLAGS) -c -o $@ $<

## Clean rule
clean:
	@rm -f dilpaso dilpaso.exe $(COBJS)
	@export MROOT=`pwd`/minisat ; cd ./minisat/core; make CXX=$(CXX) clean
	@rm -rf ./minisat/objs
	@rm -f $(DEPFILE)

### Makefile ends here
sinclude $(DEPFILE)
