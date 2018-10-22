### Makefile for nurfana ##################
###
###  To configure, copy m.config.example to m.config 
###
###  I may switch to CMake or a fancy built system eventually, but
###  but I really dislike CMake and nothing else is widely deployed 
###   (other than autotools...)
##########################################


## These are the files that must be built 
SRCS := FFT.cc FrequencyRepresentation.cc Interpolation.cc TimeRepresentation.cc Interpolation2D.cc\
				IceModel.cc Digitizer.cc Antenna.cc Waveform.cc \
				Response.cc PhasedArrayReader.cc  Impulsivity.cc Mapper.cc Ops.cc\
				Logging.cc Deconvolution.cc

CUBATURE_SRCS := hcubature.c pcubature.c

## Public includes 
INCLUDES := Angle.h Channel.h Event.h FFT.h FrequencyRepresentation.h \
						Interpolation.h TimeRepresentation.h Waveform.h Antenna.h \
						Interpolation2D.h IceModel.h Digitizer.h PhasedArray.h \
						Response.h Event.h Mapper.h SignalOps.h Logging.h Deconvolution.h

all: shared 

m.config: 
	@echo "***** COPYING m.config.example to m.config ******" 
	@cp m.config.example m.config

include m.config 

.PHONY: shared clean install all doc 


# Checks to make sure build system is up to date
BUILD_SYSTEM:= Makefile m.config


cmd_clr:= \\e[32m
tgt_clr:= \\e[36m
nrm_clr:= \\e[39m

PLATFORM := $(shell root-config --platform) 


BUILDDIR=build
SRCDIR=src
INCDIR=include/nurfana

LIBS=${ROOT_LIBS} ${GSL_LIBS} ${FFTW3_LIBS} 
CXXFLAGS +=-pthread -std=gnu++11 -fPIC -Wall -Wextra 
CFLAGS += -pthread -fPIC -Wall -Wextra 
INCFLAGS := -Iinclude -I${GSL_INCDIR} -I${ROOT_INCDIR} -I${FFTW3_INCDIR}  
CXXFLAGS += $(INCFLAGS) 
DEPFLAGS = -MT $@ -MMD -MF $(BUILDDIR)/$(*F).Td


SHLIB=so 
SHFLAG=-shared
LDFLAGS+= -Wl,-rpath=$(PREFIX)/lib -Wl,-z,defs 


## This is probably not everything that's needed. Will obviously need testing 
ifeq ($(PLATFORM),macosx)
	SHLIB=dylib
	SHFLAG=-dynamic
endif

OBJS := $(addprefix $(BUILDDIR)/, ${SRCS:.cc=.o} ${CUBATURE_SRCS:.c=.o}) 
DEPS := $(addprefix $(BUILDDIR)/, ${SRCS:.cc=.d} ${CUBATURE_SRCS:.c=.d}) 
INCLUDES := $(addprefix $(INCDIR)/, $(INCLUDES))
DICT = $(BUILDDIR)/nurfanaDict.o

ifeq ($(HAVE_NUPHASEROOT),yes)
	DEP_TARGETS+=nuphase
	CXXFLAGS+= -DHAVE_NUPHASE -I${NUPHASEROOT_INCDIR}
	LIBS+=-L${NUPHASEROOT_LIBDIR} -lnuphaseroot
endif
ifeq ($(HAVE_ARAROOT),yes)
	DEP_TARGETS+=ara
	CXXFLAGS+= -DHAVE_ARA -I${ARAROOT_INCDIR} 
	LIBS+=-L${ARAROOT_LIBDIR} -lAraEvent
endif


$(BUILDDIR):
	@mkdir -p $@

$(BUILDDIR)/%.o: $(SRCDIR)/%.c 
$(BUILDDIR)/%.o: $(SRCDIR)/%.cc 

$(BUILDDIR)/%.o: $(SRCDIR)/cubature/%.c $(BUILDDIR)/%.d $(BUILD_SYSTEM) | $(BUILDDIR) 
	@echo -e $(cmd_clr) CC $(tgt_clr)\\t [$(*F)] $(nrm_clr)
	@$(CC) $(DEPFLAGS) $(CFLAGS) -Wno-sign-compare -c $< -o $@
	@mv -f $(BUILDDIR)/$(*F).Td $(BUILDDIR)/$(*F).d && touch $@ 

$(BUILDDIR)/%.o: $(SRCDIR)/%.cc $(BUILDDIR)/%.d $(BUILD_SYSTEM) | $(BUILDDIR) 
	@echo -e $(cmd_clr) CXX $(tgt_clr)\\t [$(*F)] $(nrm_clr)
	@$(CXX) $(DEPFLAGS) $(CXXFLAGS) -c $< -o $@
	@mv -f $(BUILDDIR)/$(*F).Td $(BUILDDIR)/$(*F).d && touch $@ 


shared: $(BUILDDIR)/libnurfana.$(SHLIB)  

$(BUILDDIR)/libnurfana.$(SHLIB): $(OBJS) $(DICT) $(BUILD_SYSTEM) | $(BUILDDIR) 
	@echo -e $(cmd_clr) LD $(tgt_clr)\\t [libnurfana] $(nrm_clr)
	@$(CXX) $(SHFLAG) $(LDFLAGS) $(OBJS) $(DICT) $(LIBS) -o $@ 

## Generate the dictionary 
$(BUILDDIR)/nurfanaDict.C:  $(INCLUDES) LinkDef.h $(BUILD_SYSTEM) | $(BUILDDIR) 
	@echo -e $(cmd_clr) DICT $(tgt_clr)\\t [$(*F)] $(nrm_clr)
	@$(ROOTCLING) -f $@ -c -p -I. -Iinclude/ $(INCLUDES) LinkDef.h 

$(BUILDDIR)/nurfanaDict.o: $(BUILDDIR)/nurfanaDict.C 
	@echo -e $(cmd_clr) CXX $(tgt_clr)\\t [$(*F)] $(nrm_clr)
	@$(CXX) -c $(CXXFLAGS) -I. $< -o $@

# Empty prerequisite to avoid complaining about missing .d files on first compile
$(BUILDDIR)/%.d: ; 


## Pkg-config file
$(BUILDDIR)/nurfana.pc: $(BUILD_SYSTEM) | $(BUILDDIR) 
	@echo -e $(cmd_clr) MKPKGCFG \\t [$(*F)] $(nrm_clr) 
	@echo "prefix=$${prefix}" > $@
	@echo "exec_prefix=$${prefix}/bin" >> $@
	@echo "includedir=$${prefix}/include" >> $@
	@echo "libdir=$${prefix}/lib" >> $@
	@echo "" >>$@ 
	@echo "Name: nurfana" >> $@
	@echo "Description: The NU RF ANAlysis library" >> $@
	@echo "Version: `git rev-parse --short` HEAD" >>$@ 
	@echo "Cflags: -I$${includedir}" >> $@ 
	@echo "Libs: -L$${libdir}" -lnurfana >> $@ 

$(BUILDDIR)/nurfana_env.sh: $(BUILD_SYSTEM) | $(BUILDDIR) 
	@echo -e $(cmd_clr) MKSH \\t [$(*F)] $(nrm_clr) 
	@echo "export NURFANA_INSTALL_DIR=$(PREFIX)" > $@
	@echo "export LD_LIBRARY_PATH=$(LD_LIBRARY_PATH):$(NURFANA_INSTALL_DIR)/lib" >> $@
	@echo "export PATH=$(PATH):$(NURFANA_INSTALL_DIR)/bin" >> $@
	@echo "export PKG_CONFIG_PATH=$(PKG_CONFIG_PATH):$(NURFANA_INSTALL_DIR)/lib/pkgconfig" >> $@

install: shared $(BUILDDIR)/nurfana.pc $(BUILDDIR)/nurfana_env.sh 
	@echo -e $(cmd_clr) INSTALL $(tgt_clr) \\t [$(PREFIX)] $(nrm_clr) 
	@install -D -c $(BUILDDIR)/libnurfana.so $(PREFIX)/lib 
	@install -D -c $(BUILDDIR)/*.pcm $(PREFIX)/lib 
	@install -D -c $(BUILDDIR)/nurfana.pc $(PREFIX)/lib/pkgconfig 
	@install -D -c $(BUILDDIR)/nurfana_env.sh $(PREFIX)/bin 
	@install -d $(PREFIX)/include/nurfana
	@install -D -c $(INCLUDES) $(PREFIX)/include/nurfana

doc: 
	@echo "make doc not implemented yet" 

clean: 
	@echo -e $(cmd_clr) CLEAN  $(nrm_clr) 
	@rm -rf $(BUILDDIR) 


.submodule: src/cubature .gitmodules
	@echo -e $(cmd_clr) GIT $(tgt_clr)\\t [SUBMODULE] $(nrm_clr)
	@git submodule init 
	@git submodule update
	@touch $@


src/cubature/pcubature.c: .submodule
src/cubature/hcubature.c: .submodule 


.PRECIOUS: $(DEPS) 

# the wildcard is to only worry about things that aren't missing
include $(wildcard $(DEPS)) 
