# This makefile will make the pho_emp_local program
#
# clean and clear are defined in the isis.rules file
# and are not needed in each makefile.  clean removes
# objects and executables.  clear removes only objects.
#
#  Include the ISIS definitions file
include $(ISISMAKE)/isis.rules

EXTRALIBDIRS=-L$(ISIS3RDPARTY)/lib

# Source and object files
SRCS = pho_emp_local.F phoemp_bind_c.c rando.c
OBJS = pho_emp_local.o phoemp_bind_c.o rando.o
EXES = pho_emp_local

# Make the pho_emp_local program
pho_emp_local:        $(OBJS)
	$(FC) $(LDFLAGS) $(LIBDIRS) -o $@ $(OBJS) \
	   -lisispht -lisismath -lnrec -lisis -lspice  \
           $(ISISLINKLIB) $(DLLIBS) $(SYSLIBS)
