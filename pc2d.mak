# This makefile will make the pc2d program
#
# clean and clear are defined in the isis.rules file
# and are not needed in each makefile.  clean removes
# objects and executables.  clear removes only objects.
#
#  Include the ISIS definitions file
include $(ISISMAKE)/isis.rules

# Set X and Motif specific paths
EXTRALIBS       = $(ISISX3DLIBS)
EXTRALIBDIRS    = $(ISISXLIBDIRS) -L$(PGPLOT_DIR) -L$(ISIS3RDPARTY)/lib
EXTRA_CINCLUDES = $(ISISXINCLUDES) -I$(PGPLOT_DIR)

#  Set up program specific files
#
SRCS = calcautostr.c createdisplay.c createdisplaymenu.c createlistdb.c        \
       listoptiondb.c opendb.c roamoptiondb.c \
       sizeoptiondb.c stretchoptiondb.c createtoolbar.c viewdb.c  \
       zoomoptiondb.c dacallbacks.c fsbroutines.c listarea.c        \
       listcallbacks.c loadcube.c loadstrlut.c percent.c  \
       positionroutines.c pc2d.c pc2droutines.c roamroutines.c \
       stretchline.c stretchroutines.c toolbarcallbacks.c updateroutines.c     \
       zoomroutines.c linkdb.c paneldb.c linetopoints.c preloadcube.c \
       opencube.c colormaproutines.c medcut.c browseroutines.c \
       imageinfo.c histinfo.c pc2dcallbacks.c calchist.c \
       initdisplay.c initpcp.c firstguess.c \
       createpcptools.c pcptoolcallbacks.c pcproutines.c paramoptiondb.c \
       createpcplogw.c pcplog.c doneoptiondb.c writetopo.c \
       opendisplaydb.c hinterror.c pcp_bind_c.c \
       LiteClue.c xcnewt_gui.F xcmain_gui.F synthdiff.F synthetic.F \
       xcsep.F pcp_bind_F.c 

OBJS = calcautostr.o createdisplay.o createdisplaymenu.o createlistdb.o        \
       listoptiondb.o opendb.o roamoptiondb.o \
       sizeoptiondb.o stretchoptiondb.o createtoolbar.o viewdb.o  \
       zoomoptiondb.o dacallbacks.o fsbroutines.o listarea.o         \
       listcallbacks.o loadcube.o loadstrlut.o percent.o  \
       positionroutines.o pc2d.o pc2droutines.o roamroutines.o  \
       stretchline.o stretchroutines.o toolbarcallbacks.o updateroutines.o     \
       zoomroutines.o linkdb.o paneldb.o linetopoints.o preloadcube.o \
       opencube.o colormaproutines.o medcut.o browseroutines.o \
       imageinfo.o histinfo.o pc2dcallbacks.o calchist.o \
       initdisplay.o initpcp.o firstguess.o \
       createpcptools.o pcptoolcallbacks.o pcproutines.o paramoptiondb.o \
       createpcplogw.o pcplog.o doneoptiondb.o writetopo.o \
       opendisplaydb.o hinterror.o pcp_bind_c.o \
       LiteClue.o xcnewt_gui.o xcmain_gui.o synthdiff.o synthetic.o \
       xcsep.o pcp_bind_F.o 

EXES = pc2d

pc2d: $(OBJS)
	  $(FC) $(LDFLAGS) $(ISISXLINKOPTS) $(LIBDIRS) -o $@ $(OBJS) \
            -lisispht -lnrec -lpgplot \
            -lisislev -lisisker -lisisspi -lisismath -lisismap -lcspice \
            -lisistools -lisis $(ISISLINKLIB)  $(DLLIBS) $(SYSLIBS)
