
include ../site.mk


OPTIONS     = -g
INLINE_ALL  = -DINLINE_NODE -DINLINE_EDGELIST -DINLINE_EDGE -DINLINE_SOLVER
INLINE  = $(INLINE_ALL)
STANDARD_FLAGS = -DCOUNT_LABELS -DTERM_LABELS -DLIFO_BUCKETS
#EXTRA_FLAGS =
CFLAGS  = -I.. -Wall $(OPTIONS) $(INLINE) $(SITE_CFLAGS) \
	  $(STANDARD_FLAGS) $(EXTRA_FLAGS)

LDPROF  =
LDFLAGS = -L.. -lpsa $(LDPROF) $(LOCAL_LIBS)

.C.o: ;		$(CXX) -c $(CFLAGS) $*.C

llps: llps.o ../libpsa.a
	$(CXX) -o llps llps.o $(LDFLAGS)

addchild: addchild.o ../libpsa.a
	$(CXX) -o addchild addchild.o $(LDFLAGS)



addchild.o: ../Node.h ../Edge.h ../buildinfo.c
llps.o: ../Node.h ../Edge.h ../PhaseSolver.h ../buildinfo.c


profile:
	rm -f llps.o
	$(MAKE) OPTIONS="-pg" LDPROF="-pg" INLINE="$(PROFILE_INLINE)" 

optimize:
	rm -f llps.o
	$(MAKE) 

