# This makefile will make the chardip program
#
# clean and clear are defined in the isis.rules file
# and are not needed in each makefile.  clean removes
# objects and executables.  clear removes only objects.
#
#  Include the ISIS definitions file
include $(ISISMAKE)/isis.rules

#  Set other paths
EXTRA_CINCLUDES+=-I$(PGPLOT_DIR)
EXTRALIBDIRS+=-L$(PGPLOT_DIR) -L$(ISIS3RDPARTY)/lib

#  Set up program specific files
#
SRCS = chardip.c 
OBJS = chardip.o 
EXES = chardip

chardip: $(OBJS)
	$(CC) $(LDFLAGS) $(LIBDIRS) -o $@ $(OBJS) \
	       -lisispht -lnrec -lisislev -lisisker -lisisspi -lisismath \
               -lisismap -lcspice -lisistools -lisis \
               $(ISISLINKLIB) $(DLLIBS) $(SYSLIBS)

# 
#  Set up source specific dependencies here
