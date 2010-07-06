SHELL := /bin/sh

BIN := ../bin
LIB := ../lib
DOXYDIR := ../doxy
DOXYGEN := doxygen
MKDIR := mkdir -p

CXXFLAGS := -ggdb

.PHONY: all code doxy clean
code: $(BIN)/testCli $(BIN)/testSrv
all: code doxy
doxy: $(DOXYDIR)/index.html

$(DOXYDIR)/index.html: $(BIN)/testCli $(BIN)/testSrv
	-$(MKDIR) $(DOXYDIR)
	(cd ..; $(DOXYGEN))

$(BIN)/testCli: test_client.cpp Error.cpp Error.hpp Socket.hpp
	-$(MKDIR) $(BIN)
	$(LINK.cpp) $^ $(LOADLIBES) $(LDLIBS) -o $@

$(BIN)/testSrv: test_server.cpp Error.cpp Error.hpp Socket.hpp
	-$(MKDIR) $(BIN)
	$(LINK.cpp) $^ $(LOADLIBES) $(LDLIBS) -o $@


clean:
	-rm $(BIN)/{testCli,testSrv}

