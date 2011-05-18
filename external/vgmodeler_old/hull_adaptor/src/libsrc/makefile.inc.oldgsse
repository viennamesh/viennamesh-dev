#
#
# Make-Include-File for library
# Joachim Schoeberl, 17.04.96
#
#
CPP_DIR=../..
LIBSRC_DIR=$(CPP_DIR)/libsrc
LIB_DIR=$(CPP_DIR)/lib/$(MACHINE)

#OCC_DIR=../../occ
#OCCINC_DIR=$(OCC_DIR)/inc
#OCCLIB_DIR=$(OCC_DIR)/lib
# OCC_DIR=/opt/OpenCASCADE5.2/ros
# OCC_DIR=/home/joachim/download/occ/Linux
# OCCINC_DIR=$(OCC_DIR)/inc  -I$(OCC_DIR)/ros/inc
# OCCLIB_DIR=$(OCC_DIR)/Linux/lib
#
include $(LIBSRC_DIR)/makefile.mach.$(MACHINE)
#
GSSE = -I$(GSSEHOME)/gsse -I$(GSSEHOME)/gsse/external/mauch -I$(BOOSTINC)
CPLUSPLUSFLAGS1 = -c -I$(LIBSRC_DIR)/include -I/localhome/stimpfl/devel/gsse_geometric_predicates/src/ -g $(GSSE)
#
ARFLAGS = r
#
LIBB=$(LIB_DIR)/lib$(lib).a
#
.PRECIOUS: .cpp .c
.SUFFIXES: .cpp .c .o 
#
.cpp.o:
	$(CPLUSPLUS) $(CPLUSPLUSFLAGS1) $(CPLUSPLUSFLAGS2) $(CPLUSPLUSFLAGSLIBRARY) $<
.c.o:
	$(CPLUSPLUS) $(CPLUSPLUSFLAGS1) $(CPLUSPLUSFLAGS2) $(CPLUSPLUSFLAGSLIBRARY) $<
#
#
$(LIBB):: $(LIB_DIR) 
#
# make lib from sources:
#
$(LIBB):: $(src) 
	$(CPLUSPLUS) $(CPLUSPLUSFLAGS1) $(CPLUSPLUSFLAGS2) $(CPLUSPLUSFLAGSLIBRARY)  $?
	@$(AR) $(ARFLAGS) $@ *.o
	-@$(RM) *.o
	-@$(RANLIB) $@
#
#
#
$(LIB_DIR) :
	-@mkdir $(CPP_DIR)/lib
	@mkdir $(LIB_DIR)
#
#
