### Makefile for nurfana ##################
###
###  To configure, copy m.config.example to m.config 
###
###  I may switch to CMake or a fancy built system eventually, but
###  but I really dislike CMake and nothing else is widely deployed. 
##########################################



## These are the files that must be built 
SRCS := FFT.cc FrequencyReprsentation.cc Interpolation.cc 

## Public includes 
INCLUDES := Angle.h Channel.h Event.h FFT.h FrequencyRepresentation.h \
						Interpolation.h TimeRepresentation.h Waveform.h 


all: shared 

m.config: 
	@echo "**** COPYING m.config.example to m.config ******" 
	@cp m.config.example m.config

include m.config 

.PHONY: build_system shared clean install all doc


# Checks to make sure build system is up to date
build_system: Makefile m.config


BUILDDIR=build
SRCDIR=src

LIBS=${ROOT_LIBS} ${GSL_LIBS} ${FFTW3_LIBS} 
CXXFLAGS +=-pthread -std=gnu++11
INCFLAGS := -Iinclude -I${GSL_INCDIR} -I${ROOT_INCDIR} -I${FFTW3_INCDIR}  
CXXFLAGS += $(INCFLAGS) 
DEPFLAGS = -MT $@ -MD -MP -MF $(BUILDDIR)/$(*F).Td


OBJS := $(addprefix $(BUILDDIR)/, ${SRCS:.cc=.lo} ) 
DEPS := $(addprefix $(BUILDDIR)/, ${SRCS:.cc=.d} ) 
DICT = nurfanaDict.lo


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

$(BUILDDIR)/%.lo: $(SRCDIR)/%.cc $(BUILDDIR)/%.d build_system | $(BUILDDIR) 
	@echo Compiling $(*F) 
	@$(LIBTOOL) --mode=compile $(CXX) $(DEPFLAGS) -c $(CXXFLAGS) $< -o $@
	@mv -f $(BUILDDIR)/$(*F).Td $(BUILDDIR)/$(*F).d && touch $@ 


shared: $(BUILDDIR)/libnurfana.la 

$(BUILDDIR)/libnurfana.la: $(OBJS) $(DICT) build_system | $(BUILDDIR) 
	@echo Building shared library
	@$(LIBTOOL) --mode=link $(CXX) $(LDFLAGS) -avoid-version -rpath=$(PREFIX)/lib  $(OBJS) $(DICT) $(LIBS) -o $@ 

## Generate the dictionary 
$(BUILDDIR)/nurfanaDict.C:  $(INCLUDES) LinkDef.h build_system | $(BUILDDIR) 
	@echo Generating ROOT dictionary 
	@$(ROOTCLING) -f R@ -c -p $(INCFLAGS)  $(INCLUDES) LinkDef.h 

$(BUILDDIR)/nurfanaDict.lo: $(BUILDDIR)/nurfanaDict.C 
	@echo Compiling ROOT dictionary
	@$(LIBTOOL) --mode=compile $(CXX) -c $(CXXFLAGS) $< -o $@

# Empty prerequisite to avoid complaining about missing .d files on first compile
$(BUILDDIR)/%.d: ; 

install: 
	@echo Installing to $(PREFIX) 
	@$(LIBTOOL) --mode=install install -c $(BUILDDIR)/libnurfana.la $(PREFIX)/lib 
	@install -c $(INCLUDES) $(PREFIX)/include

doc: 
	@echo "make doc not implemented yet" 

clean: 
	@echo cleaning...
	@rm -rf $(BUILDDIR) 
	@rm -rf .libs 


.PRECIOUS: $(DEPS) 

# the wildcard is to only worry about things that aren't missing
include $(wildcard $(DEPS)) 
