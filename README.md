# Pseudoflow Solver Software

## Introduction
In this directory, you will find the source code for an implementation of
the pseudoflow algorithm for the maximum flow problem. In addition to the
C++ code that is compiled into the solver programs, there is also a
[Literate Program](http://www.literateprogramming.com/) which explains in
great detail how it all works. 

[solver-lit-prog.pdf](./solver-lit-prog.pdf) is a PDF of the complete literate program generated in 2023.

*Note: I developed this code over 20 years ago when I was working on my
dissertation.  The code originally compiled under g++ 2.7. Later, I
updated it for version 3.2, and in 2023, I made more changes to compile
under g++ 4.4. I've included a `Dockerfile` for the environment I built to
compile the code and generate documentation under Ubuntu 10.04 (very old).
However, I haven't run the code in nearly 20 years - I don't
know if it still works.
I've open-sourced it in case it's of use to someone else.*

## Prerequisites
The code is built with the following tools:

* <a href="http://gcc.gnu.org"> g++</a>: This compiles under version 4.4 of g++ on Ubuntu 10.04.
* <a href="http://www.gnu.org/software/make/make.html"> gmake</a> - other make tools will probably work, but I've only used gmake</li>
* <a href="http://www.eecs.harvard.edu/~nr/noweb/">noweb</a>: The code is written using the noweb literate programming tool. noweb generates the C++ source files and the Latex documentation.
* [Latex:](http://www.latex-project.org/) To format the literate program documentation

All of these are available in Docker using the `Dockerfile` provided.
Although it creates an Intel/AMD image, it runs fine on an M1 Mac.

## Installation Instructions
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
Last Updated: 8 May 2023<br>
Last Real Update: September 2003
