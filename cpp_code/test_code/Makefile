# gnu makefile for rat/src/fit/fitter
# MGold, Nov 2007
########################################################
ARCH          = $(shell root-config --arch)
ifeq ($(ARCH),macosx)
 include $(ROOTSYS)/test/Makefile.arch
else
CXX           = g++    					 # Which compiler should be used
CXXFLAGS      = -ggdb -O -Wall -fPIC # What flags should pass to compiler
LD            = clang++
LDFLAGS       = -O									 # What flags should pass to the linker
SOFLAGS       = -shared
endif

SYS := $(shell uname)
RATSHLIB     :=RATEvent
RATLIB       :=rat
LOCAL        := $(shell pwd)
ROOTCFLAGS   := $(shell root-config --cflags)
ROOTLIBS     := $(shell root-config --libs)
ROOTGLIBS    := $(shell root-config --glibs)
CLHEP_BASE_DIR := $(shell echo $(CLHEP_BASE_DIR))
G4INCLUDE :=$(shell echo $(G4INCLUDE))
G4INSTALL :=$(shell echo $(G4INSTALL))
#-L$(CLHEP_BASE_DIR)/lib -lCLHEP

ifeq ($(ARCH),macosx64)
	CINTCFLAGS    = -pipe -W -Wall -fsigned-char -fno-common \
                -DG__REGEXP -DG__UNIX -DG__SHAREDLIB \
                -DG__ROOT -DG__REDIRECTIO -DG__OSFDLL \
                -DG__STD_EXCEPTION
	#G4LIB = $(G4INSTALL)/lib # works on Linux g++
	G4LIB = /usr/lib # for my MAC laptop
else
	CINTCFLAGS = $(ROOTFLAGS)
	G4LIB = $(G4INSTALL)/lib64
endif


LIBNAME  := LikeFit
CXXFLAGS     += $(ROOTCFLAGS)
LIBS          = $(ROOTGLIBS)
LIBS += -L$(ROOTSYS)  -lMinuit2 -L$(RATROOT)/lib/ -l$(RATSHLIB) -l$(RATLIB) -L$(G4LIB)
CXXFLAGS     += -I$(LOCAL) -I/$/usr/include -I/usr/local/include   -I/$(RATROOT)/include -I$(G4INSTALL)/include/Geant4 -I$(CLHEP_BASE_DIR)/include
##-I/home/gold/jsoncpp/include/json
CPP= $(CXX) $(CXXFLAGS)

#------------------------------------------------------------------------------
SKIP_OBJ =
SKIP_DICT =
#SOURCE := $(wildcard *.hh) $(wildcard *.cc) # works on Linux g++
SOURCE := $(wildcard *.cc) # for working with clang
HEADERS := $(wildcard *.hh)
OBJS := $(filter-out $(SKIP_OBJ),$(patsubst %.hh,%.o,$(wildcard *.hh)))
#OBJS :=$(wildcard *.o)
DICTS := $(filter-out $(SKIP_DICT), $(patsubst %.hh,%_dict.o,$(wildcard *.hh)))
EXECUTABLE=ana
#defined the name of executable file
OBJECTS=$(SOURCE:.cc=.o)
#defined the object file name in this case all .cc file in the folder
#------------------------------------------------------------------------------
ifeq ($(ARCH),macosx)
LIBRARY := lib$(LIBNAME).$(DllSuf)
LIBLINK := $(subst .$(DllSuf),.so,$(LIBRARY))
else
LIBRARY := lib$(LIBNAME).so
LIBLINK :=
endif

#all: anavac

all: $(SOURCES) $(EXECUTABLE)

#all: $(EXECUTABLE)


copy: $(HEADERS)
	cp *.hh  $(RATROOT)/include/RAT

lib: $(DICTS) $(OBJS)
	$(LD) $(SOFLAGS) $(LIBS) $(OBJS) $(DICTS) -o $(LIBRARY)
ifeq ($(ARCH),macosx)
# We need to make both the .dylib and the .so
	ln -sf $(LIBRARY) $(subst .$(DllSuf),.so,$(LIBRARY))
endif
	@echo "$@ done"


$(EXECUTABLE): $(OBJECTS)
#	$(CPP) $(OBJECTS) -o $@ $< -L./  $(LIBS) # works on Linux g++
	$(CPP) $(OBJECTS) -o $@ -L./ $(LIBS) # for working with clang


.cc.o:
	$(CPP) $(OBJECTS)  $< -o $@

#anavac: test_code.o read_waveform.o create_hist.o
#	$(CPP) -o $@ $< -L./  $(LIBS)


#read_waveform: read_waveform.cc
#	$(CPP) -o $@ $< -L./  $(LIBS)
#create_hist: create_hist.cc
#	$(CPP) -o $@ $< -L./  $(LIBS)

%_dict.cc: %.hh dict/%_linkdef.h
	@echo "Generating dictionary $@..."
	@rootcint -f $@ -c $(CINTCFLAGS)  $^

#plot_waveform_mod.o:	plot_waveform_mod.cc functions.hh
#	$(CXX) $(CXXFLAGS) -c $<

#read_waveform.o: read_waveform.cc functions.hh
#	$(CXX) $(CXXFLAGS) -c $<

#create_hist.o: create_hist.cc functions.hh
#	$(CXX) $(CXXFLAGS) -c $<

#pulse_info.o: pulse_info.cc functions.hh
#	$(CXX) $(CXXFLAGS) -c $<

#alpha_cut.o: alpha_cut.cc functions.hh
#	$(CXX) $(CXXFLAGS) -c $<

#alpha_cut_results.o: alpha_cut_results.cc functions.hh
#	$(CXX) $(CXXFLAGS) -c $<

#SaveFile.o: SaveFile.cc functions.hh
#	$(CXX) $(CXXFLAGS) -c $<

%.o: %.cc %.hh
	$(CXX) $(CXXFLAGS) -c $<

.PHONY : clean show

show:
	@echo sys =$(SYS)
	@echo arch =$(ARCH)
	@echo G4LIB = $(G4LIB)
	@echo OBJS = $(OBJS)
	@echo DICTS = $(DICTS)
	@echo LIBS = $(LIBS)
	@echo LDFLAGS= $(LDFLAGS)
	@echo ROOTCFLAGS= $(ROOTCFLAGS)
	@echo CINTFLAGS= $(CINTCFLAGS)
	@echo CXXFLAGS= $(CXXFLAGS)
	@echo EXTRA_CFLAGS= $(EXTRA_CFLAGS)
	@echo FINK_CFLAGS= $(FINK_CFLAGS)
	@echo DllSuf= $(DllSuf)
	@echo OutPutOpt= $(OutPutOpt)
	@echo LIBRARY= $(LIBRARY)
	@echo LIBLINK $(LIBLINK)
	@echo LIBNAME=$(LIBNAME)
	@echo CPP=$(CPP)

clean:
	@rm -f $(OBJS) *_dict.o* $(LIBRARY) $(LIBLINK) anavac *.o
