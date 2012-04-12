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


PICOFLAGS=-fno-strict-aliasing
PICOFLAGS=-DTRACE

ifdef DBG
CFLAGS+=-O0
CFLAGS+=-ggdb
PICOFLAGS+=-ggdb
PICOFLAGS+=-O0
else
PICOFLAGS+=-O3
PICOFLAGS+=-DNDEBUG
CFLAGS+=-DNDEBUG
CFLAGS+=-O3
CFLAGS+=-DNCHECK
endif

ifdef NOO
CFLAGS+=-O0
endif

CFLAGS += -Wall -DBUILDDATE='"$(BUILDDATE)"' -DDISTDATE='"$(DISTDATE)"'
CFLAGS += -DCHANGESET='"$(CHANGESET)"' -DRELDATE='"$(RELDATE)"'
CFLAGS+=-D __STDC_LIMIT_MACROS -D __STDC_FORMAT_MACROS -Wno-parentheses -Wno-deprecated 
# CFLAGS+=-std=c++0x
LIBS+=-lz

ifdef STATIC
CFLAGS+=-static
LNFLAGS+=-static
endif

.PHONY: all

all: picosat.o $(COBJS) 

depend:
	rm -f $(DEPFILE)
	@echo $(DEPTOKEN) > $(DEPFILE)
	makedepend $(DEPFLAGS) -- $(CFLAGS) -- $(CSRCS)

picosat.o: picosat.c
	@echo Compiling: $@
	gcc $(PICOFLAGS)  -c -o $@ $<

## Build rule
%.o:	%.cc
	@echo Compiling: $@
	$(CXX) $(CFLAGS) -c -o $@ $<

## Clean rule
clean:
	rm -f *.o

### Makefile ends here
sinclude $(DEPFILE)
