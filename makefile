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

# requires git, used to make distribution source tarball
# for creating packages with, variables are evaluated when
# the dist target is run so systems without git can still `make`
REPONAME=$(shell basename `git rev-parse --show-toplevel`)
VERSION=$(shell git describe --abbrev=0 | sed s/^v//)
BRANCH=master


.PHONY: all clean install dist distclean

all: $(BINARY)

$(BINARY): $(SRCFILES)

clean: 
	rm -f *.o $(BINARY)

# copy files to destinations
install:
	install -d $(INSTALLDIR)
	cp $(BINARY) $(INSTALLDIR)
	cp jsonData.json $(INSTALLDIR)

dist:
	git archive --prefix='$(REPONAME)-$(VERSION)/' -o $(REPONAME)_$(VERSION).orig.tar.gz -9 refs/heads/$(BRANCH)

distclean: clean
	rm -f *.orig.tar.gz

