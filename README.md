# Pseudoflow Solver Software

## Introduction
In this directory, you will find the source code for an implementation of the pseudoflow algorithm for the maximum flow problem. In addition to the C++ code that is compiled into the solver programs, there is also a [Literate Program](http://www.literateprogramming.com/) which explains in great detail how it all works.

*Note: I developed this code over 10 years ago when I was working on my dissertation.  The tools used to build this may or may not still be available.  I don't know if the code still compiles under more modern versions of C++.  I'm just open-sourcing it in case it's of use to someone else.*

## Prerequisites
The code is built with the following tools:

* <a href="http://gcc.gnu.org"> g++</a>: This compiles under versions 2.7-2.9 of g++ on <a
  href="http://www.freebsd.org">FreeBSD</a>,  <a href="http://www.sun.com">Solaris</a>, and <a href="http://sources.redhat.com/cygwin"> Cygwin</a>.
* <a href="http://www.gnu.org/software/make/make.html"> gmake</a> - other make tools will probably work, but I've only used gmake</li>
* <a href="http://www.eecs.harvard.edu/~nr/noweb/">noweb</a>: The code is written using the noweb literate programming tool. noweb generates the C++ source files and the Latex documentation.  *(Unfortunuately, I don't have generated versions of these files available, so you'll have to get noweb running.)*
* [Latex:](http://www.latex-project.org/) To format the literate program documentation

## Installation Instructions
After extracting the files from the tar-archive, gmake will build the binaries.
Edit <tt>site.mk</tt> to adjust the compile flags as needed as explained in the
file. Some of the available make targets are:

* default/all: build all the programs with debugging enabled
* optimize: build the programs without debugging and with compiler optimizations enabled
* doc.dvi: build the literate program version of the Latex code

The programs that get built are:

* llps - the basic solver for single instances of the max-flow problem.
* pllps - the parametric solver for parametric max-flow problems
* gpps - the warm-start solver for non-parametric sequences of problems.

## Running the programs
The programs all accept arguments in the same basic format:
<tt>program [options] input-file(s) output-file</tt>
Invoking the programs without arguments will cause a usage message to be printed which explains all of the available options.
The input and output files are in the [Dimacs format](http://lpsolve.sourceforge.net/5.5/DIMACS_maxf.htm).

---
Last Updated: 1 October 2013
Last Real Update: September 2003
