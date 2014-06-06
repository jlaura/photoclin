# This makefile will make the pho_emp_global program
#
# clean and clear are defined in the isis.rules file
# and are not needed in each makefile.  clean removes
# objects and executables.  clear removes only objects.
#
#  Include the ISIS definitions file
include $(ISISMAKE)/isis.rules

EXTRALIBDIRS=-L$(PGPLOT_DIR) -L$(ISIS3RDPARTY)/lib

# Source and object files
SRCS = pho_emp_global.F  
OBJS = pho_emp_global.o  
EXES = pho_emp_global

# Make the pho_emp_globalprogram
pho_emp_global:        $(OBJS)
	$(FC) $(LDFLAGS) $(LIBDIRS) -o $@ $(OBJS) \
	   -lisispht -lisismath -lnrec -lisis -lspice \
           $(ISISLINKLIB) $(DLLIBS) $(SYSLIBS)
