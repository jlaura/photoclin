##########################################################################
#_Title Makefile Make file to generate a generic ISIS library
#_Args  NONE
#
#_Descr This is a Makefile for creating a generic ISIS library.
#
#_Hist  Feb 07 2005 Kris Becker, USGS, Flagstaff Original Version
#_Ver $Id: Makefile,v 1.3 2005/02/18 22:15:39 kbecker Exp $
# _End
###########################################################################
include $(ISISMAKE)/isis.conf

#
# set LIBRARY to be name of library
#
INSTALL_DIR    = $(CURDIR)
LIBRARY        = libisispht
LIBNAME        = $(INSTALL_DIR)/$(LIBRARY).a
SHAREDLIBNAME  = $(LIBRARY).$(SHAREDLIBEXT)

#
#  Set up library dependancies
SRCS = $(wildcard *.c *.F *.f *.cpp *.cxx *.cc *.C)
OBJS  = $(addsuffix .o, $(basename $(SRCS)))

#  Define all required targets
.PHONY : all install $(SHAREDLIBNAME) clean
all: $(LIBNAME)

install: INSTALL_DIR=$(ISISLIB)
install: $(LIBNAME)
	@echo "Done installing $(LIBRARY)..."

	
$(LIBNAME): $(OBJS)
	$(AR) $(ARFLAGS) $(LIBNAME) $(OBJS)
	if [ -n "$(RANLIB)" -a -x "$(RANLIB)" ]; then $(RANLIB) $(LIBNAME); fi

#  Shared library target
shared:
	$(MAKE) -C $(ISISLIB) -f $(CURDIR)/Makefile $(SHAREDLIBNAME)

		
#   Use the default shared library target		      
$(SHAREDLIBNAME): 
	$(call make-shared-lib, $(ISISLIB))

clean:
	$(RM) $(OBJS) $(LIBNAME)

