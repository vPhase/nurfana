### Makefile for nurfana ##################
###
###  To configure, copy m.config.example to m.config 
###
###  I may switch to CMake or a fancy built system eventually, but
###  but I really dislike CMake and nothing else is widely deployed 
###   (other than autotools...)
##########################################


## These are the files that must be built 
SRCS := FFT.cc FrequencyRepresentation.cc Interpolation.cc 

## Public includes 
INCLUDES := Angle.h Channel.h Event.h FFT.h FrequencyRepresentation.h \
						Interpolation.h TimeRepresentation.h Waveform.h 

all: shared 

m.config: 
	@echo "**** COPYING m.config.example to m.config ******" 
	@cp m.config.example m.config

include m.config 

.PHONY: shared clean install all doc


# Checks to make sure build system is up to date
BUILD_SYSTEM:= Makefile m.config


PLATFORM := $(shell root-config --platform) 


BUILDDIR=build
SRCDIR=src
INCDIR=include/nurfana

LIBS=${ROOT_LIBS} ${GSL_LIBS} ${FFTW3_LIBS} 
CXXFLAGS +=-pthread -std=gnu++11 -fPIC 
INCFLAGS := -Iinclude -I${GSL_INCDIR} -I${ROOT_INCDIR} -I${FFTW3_INCDIR}  
CXXFLAGS += $(INCFLAGS) 
DEPFLAGS = -MT $@ -MMD -MF $(BUILDDIR)/$(*F).Td


SHLIB=so 
SHFLAG=-shared
LDFLAGS+= -Wl,-rpath=$(PREFIX)/lib

ifeq ($(PLATFORM),macosx)
	SHLIB=dylib
	SHFLAG=-dynamic
endif

OBJS := $(addprefix $(BUILDDIR)/, ${SRCS:.cc=.o} ) 
DEPS := $(addprefix $(BUILDDIR)/, ${SRCS:.cc=.d} ) 
INCLUDES := $(addprefix $(INCDIR)/, $(INCLUDES))
DICT = $(BUILDDIR)/nurfanaDict.o


ifeq ($(HAVE_NUPHASEROOT),yes)
	DEP_TARGETS+=nuphase
	CXXFLAGS+= -DHAVE_NUPHASE -I${NUPHASEROOT_INCDIR}
	LFLAGS+=-L${NUPHASEROOT_LIBDIR} -lnuphaseroot
endif
ifeq ($(HAVE_ARAROOT),yes)
	DEP_TARGETS+=ara
	CXXFLAGS+= -DHAVE_ARA -I${ARAROOT_INCDIR} 
	LIBS+=-L${ARAROOT_LIBDIR} -lAraRoot
endif


$(BUILDDIR):
	@mkdir -p $@

$(BUILDDIR)/%.o: $(SRCDIR)/%.cc 
$(BUILDDIR)/%.o: $(SRCDIR)/%.cc $(BUILDDIR)/%.d $(BUILD_SYSTEM) | $(BUILDDIR) 
	@echo Compiling  [$(*F)] 
	@$(CXX) $(DEPFLAGS) $(CXXFLAGS) -c $< -o $@
	@mv -f $(BUILDDIR)/$(*F).Td $(BUILDDIR)/$(*F).d && touch $@ 


shared: $(BUILDDIR)/libnurfana.$(SHLIB)  

$(BUILDDIR)/libnurfana.$(SHLIB): $(OBJS) $(DICT) $(BUILD_SYSTEM) | $(BUILDDIR) 
	@echo Building shared library
	@$(CXX) $(SHFLAG) $(LDFLAGS) $(OBJS) $(DICT) $(LIBS) -o $@ 

## Generate the dictionary 
$(BUILDDIR)/nurfanaDict.C:  $(INCLUDES) LinkDef.h $(BUILD_SYSTEM) | $(BUILDDIR) 
	@echo Generating ROOT dictionary 
	@$(ROOTCLING) -f $@ -c -p $(INCFLAGS) -I. $(INCLUDES) LinkDef.h 

$(BUILDDIR)/nurfanaDict.o: $(BUILDDIR)/nurfanaDict.C 
	@echo Compiling ROOT dictionary
	@$(CXX) -c $(CXXFLAGS) -I. $< -o $@

# Empty prerequisite to avoid complaining about missing .d files on first compile
$(BUILDDIR)/%.d: ; 

install: 
	@echo Installing to $(PREFIX) 
	@install -c $(BUILDDIR)/libnurfana.so $(PREFIX)/lib 
	@install -c $(BUILDDIR)/*.pcm $(PREFIX)/lib 
	@install -c $(INCLUDES) $(PREFIX)/include/nurfana

doc: 
	@echo "make doc not implemented yet" 

clean: 
	@echo cleaning...
	@rm -rf $(BUILDDIR) 
	@rm -rf .libs 


.PRECIOUS: $(DEPS) 

# the wildcard is to only worry about things that aren't missing
include $(wildcard $(DEPS)) 
