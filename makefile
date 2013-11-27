BINARY      := hwserial
SRCFILES    := $(wildcard *.cpp)
OUTDIR      := .

#CC          := g++
CXXFLAGS    := -Wall -Wextra -static
DEBUGFLAGS  := -g
RELFLAGS    :=
LDLIBS      := -lboost_system -lboost_thread -pthread -lrt

# by default set dest dir to root
# deb pkg maker will set it to a temp dir while creating a package
DESTDIR     ?= ""
INSTALLDIR  := $(DESTDIR)/usr/local/razberi/$(BINARY)


.PHONY: all clean install

all: $(BINARY)

$(BINARY): $(SRCFILES)

clean: 
	rm -f *.o $(BINARY)

# copy files to destinations
install:
	install -d $(INSTALLDIR)
	cp $(BINARY) $(INSTALLDIR)
	cp jsonData.json $(INSTALLDIR)
