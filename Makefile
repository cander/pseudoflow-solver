
include site.mk

OPTIONS	= -g 

INLINE_ALL  = -DINLINE_NODE -DINLINE_EDGELIST -DINLINE_EDGE -DINLINE_SOLVER
INLINE  = $(INLINE_ALL)
PROFILE_INLINE = $(INLINE_ALL)
STANDARD_FLAGS = -DCOUNT_LABELS -DTERM_LABELS -DLIFO_BUCKETS
#EXTRA_FLAGS =

CFLAGS	= -Wall $(OPTIONS) $(INLINE) $(SITE_CFLAGS) $(STANDARD_FLAGS) $(EXTRA_FLAGS)


# after installation, make doc.dvi for literate version

# the order of these files is the order they show up in the latex doc
WEBFILES = PhaseSolver.nw Node.nw Edge.nw EdgeList.nw Timer.nw

SRCS     = Node.h Node.C Edge.h Edge.C EdgeList.C EdgeList.h \
	   PhaseSolver.C PhaseSolver.h Timer.C Timer.h buildinfo.c

OBJECTS  = Node.o Edge.o EdgeList.o PhaseSolver.o Timer.o buildinfo.o

LIBNAME	 = libpsa.a
DOCFILES = doc.dvi doc.ps doc.aux doc.log allcode.tex doc.tex doc.toc

NOTANGLE=notangle
SHELL=/bin/sh
# set this for CPIF and then distribute tools with bad timestamps...
#CPIF=| cpif
CPIF=>

.SUFFIXES: .nw .tex .dvi .h
.nw.tex: ;	noweave $*.nw > $*.tex
.nw.c: ;	$(NOTANGLE) -L $*.nw > $*.c
.nw.C: ;	$(NOTANGLE) -L $*.nw > $*.C
.nw.o: ;	$(NOTANGLE) -L $*.nw > $*.c
		$(CC) $(CFLAGS) -c $*.c
.nw.h: ;	$(NOTANGLE) -L -Rheader $*.nw $(CPIF) $*.h
.C.o: ;		$(CXX) -c $(CFLAGS) $*.C




all: $(LIBNAME) 


$(LIBNAME): $(OBJECTS)
	ar rv $(LIBNAME) $(OBJECTS)
	ranlib $(LIBNAME)


clean: 
	rm -f $(DOCFILES) $(OBJECTS)

clobber:	clean
		rm -f $(SRCS)


buildinfo.c: .
	echo 'char* buildFlags = "'$(CFLAGS)'";' > buildinfo.c
	echo 'char* buildDate = "'`date`'";' >> buildinfo.c


# special build version for various purposes
profile:
	rm -f $(OBJECTS)
	$(MAKE) OPTIONS="-pg -O" INLINE="$(PROFILE_INLINE)" all

check:
	rm -f $(OBJECTS)
	$(MAKE) OPTIONS="-g -DDEBUG -DTRACING"

trace:
	rm -f $(OBJECTS)
	$(MAKE) OPTIONS="-g -DTRACING"

optimize:
	rm -f $(OBJECTS)
	$(MAKE) OPTIONS="-O2" all

# strip out all the references to the noweb files to create 'pure' C/C++
purec: $(SRCS)
	for f in $(SRCS) ; do \
		sed '/^#line/d' $$f > $$f.pure; \
		mv $$f.pure $$f ; done

# rules to build documentation

doc.tex:	doc.nw
	cp doc.nw doc.tex

allcode.tex: $(WEBFILES)
	noweave -n -index $(WEBFILES) > allcode.tex

doc.dvi:	doc.tex allcode.tex
	latex doc
	latex doc

doc.ps: doc.dvi
	dvips -o doc.ps doc.dvi

twoup.ps: doc.ps
	# psselect -r doc.ps | psnup -2 -pletter > twoup.ps
	cat doc.ps | psnup -2 -pletter > twoup.ps

cvsignore:
	echo "*.doc *.dvi *.ps *.log *.toc *.tex *.aux" > .cvsignore
	echo $(SRCS) .cvsignore >> .cvsignore

# dependencies - these should be done automatically

Node.o: Node.C Node.h types.h debug.h Edge.h EdgeList.h
EdgeList.o: EdgeList.C EdgeList.h types.h debug.h
Edge.o: Edge.C Edge.h Node.h types.h debug.h
PhaseSolver.o: PhaseSolver.h PhaseSolver.C Node.h types.h debug.h Edge.h EdgeList.h
Timer.o: Timer.h
