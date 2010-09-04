SHELL := /bin/sh

BIN := ../bin
LIB := ../lib
DOXYDIR := ../doxy
DOXYGEN := doxygen
MKDIR := mkdir -p

CXXFLAGS := -ggdb
LDLIBS := -lrt

NTEE_SOURCE := ntee_main.cpp \
               Arguments.cpp \
               Builder.cpp \
               Error.cpp \
               NTee.cpp \
               FileRecorder.cpp \
               Buffer.cpp \
               IPAddress.cpp \
               TCPSocket.cpp \
               comm.cpp \
               BinaryDataRecorder.cpp \
               UnixSignalHub.cpp
               
NTEE_OBJ := $(subst .cpp,.o,$(NTEE_SOURCE))               
NTEE_DEPS := $(patsubst %,.%,$(subst .cpp,.d,$(NTEE_SOURCE)))
               

PLAYER_SRC := player_main.cpp \
              BinaryDataReader.cpp \
              Player.cpp
PLAYER_OBJ := $(subst .cpp,.o,$(PLAYER_SRC))
PLAYER_DEPS := $(patsubst %,.%,$(subst .cpp,.d,$(PLAYER_SRC)))
              
               
.PHONY: all code doxy clean
code: $(BIN)/ntee $(BIN)/testCli $(BIN)/testSrv $(BIN)/ntee_player
all: code doxy
doxy: $(DOXYDIR)/index.html

$(DOXYDIR)/index.html: $(BIN)/testCli $(BIN)/testSrv
	-$(MKDIR) $(DOXYDIR)
	(cd ..; $(DOXYGEN))

$(LIB)/ntee.a: $(NTEE_OBJ)
	-$(MKDIR) $(LIB)
	$(AR) $(ARFLAGS) $@ $^
	ctags -R .

$(BIN)/ntee: $(LIB)/ntee.a
	-$(MKDIR) $(BIN)
	$(LINK.cpp) $^ $(LOADLIBES) $(LDLIBS) -o $@

$(BIN)/testCli: test_client.cpp $(LIB)/ntee.a
	-$(MKDIR) $(BIN)
	$(LINK.cpp) $^ $(LOADLIBES) $(LDLIBS) -o $@

$(BIN)/testSrv: test_server.cpp $(LIB)/ntee.a
	-$(MKDIR) $(BIN)
	$(LINK.cpp) $^ $(LOADLIBES) $(LDLIBS) -o $@

$(BIN)/ntee_player: $(PLAYER_OBJ) $(LIB)/ntee.a
	-$(MKDIR) $(BIN)
	$(LINK.cpp) $^ $(LOADLIBES) $(LDLIBS) -o $@ 

clean:
	-rm $(NTEE_OBJ) $(NTEE_DEPS) $(LIB)/ntee.a $(BIN)/{ntee,testCli,testSrv}
	-rm $(PLAYER_OBJ) $(PLAYER_DEPS) $(BIN)/ntee_player



## Include all the .d files (fails the first time, but that's ok, make
## just keeps on processing)... These are formatted to LOOK like dependency
## targets specific to each cpp file.
include $(NTEE_DEPS) $(PLAYER_DEPS)


## This is the rule for generating a .d from a .cpp.  It utilizes the fact
## that gcc offers the option for pre-parsing code to generate makefile 
## header dependency lists. Sed is used to filter the data, $* is the stem 
## of the filename (file without the suffix). The $$$$ is just a pid. The
## resulting target looks like "X.o X.d : X.cpp wha-wha-from-gcc"
.%.d:%.cpp
	$(CPP) -MM $(CPPFLAGS) $< > $@.$$$$; \
	sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; \
	rm -f $@.$$$$
        
