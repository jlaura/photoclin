# This makefile will make the pcsi program
#
# clean and clear are defined in the isis.rules file
# and are not needed in each makefile.  clean removes
# objects and executables.  clear removes only objects.
#
#  Include the ISIS definitions file
include $(ISISMAKE)/isis.rules

#  Set other paths
EXTRA_CINCLUDES =-I$(PGPLOT_DIR)
EXTRALIBDIRS    =-L$(PGPLOT_DIR) -L$(ISIS3RDPARTY)/lib

#  Set up program specific files
#
SRCS = pcsi.c initpcsi.c pcsifirstguess.c pcsitopo.c pcsizout.c \
       pcsi_bind_F.c pcsi_bind_c.c pcsi_xcsep.F pcsi_ipars.F \
       xcmain_pcsi.F xcnewt_pcsi.F
OBJS = pcsi.o initpcsi.o pcsifirstguess.o pcsitopo.o pcsizout.o \
       pcsi_bind_F.o pcsi_bind_c.o pcsi_xcsep.o pcsi_ipars.o \
       xcmain_pcsi.o xcnewt_pcsi.o
EXES = pcsi

pcsi: $(OBJS)
	$(CC) $(LDFLAGS) $(LIBDIRS) -o $@ $(OBJS) \
	       -lisispht -lnrec -lisislev -lisisker -lisisspi -lisismath \
               -lisismap -lcspice -lisistools -lisis \
               $(ISISLINKLIB) $(DLLIBS) $(SYSLIBS)

# 
#  Set up source specific dependancies here
