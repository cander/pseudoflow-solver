#include "PhaseSolver.h"
#include "Timer.h"

#include <iostream.h>
#include <fstream.h>
#include <time.h>
#include <stdlib.h>

void 
usage()
{
    cerr << "Usage: llps [-d] [-f] [-I init] [-N norm] [-B branch] input-graph flow-output" << endl;
    cerr << "\t -d   dump the final disposition of each node" << endl;
    cerr << "\t -f   write the flow values for each arc" << endl;
    cerr << "\t -I   specify initialization function" << endl;
    cerr << "\t -B   specify strong bucket management" << endl;
    cerr << "\t -M   specify merger function" << endl;
    cerr << "\t -N   specify normalization method" << endl;
}

int 
main(int argc, char** argv)
{
    Boolean dumpNodes = FALSE;
    Boolean writeFlow = FALSE;
    void (PhaseSolver::* initFunc)() = &PhaseSolver::buildSimpleTree;
    void (PhaseSolver::* solverFunc)(AddBranchPtr) = &PhaseSolver::solve;
    AddBranchPtr addBranchFunc = &PhaseSolver::addBranchLifo;
    PhaseSolver* solver = new PhaseSolver();

    // parse arguments
    int ch;
    while ((ch = getopt(argc, argv, "dfI:M:N:B:")) != EOF) {
	switch (ch) {
	case 'd':
	    dumpNodes = TRUE;
	    break;
	case 'f':
	    writeFlow = TRUE;
	    break;
	case 'I':
	    if (strcmp(optarg, "simple") == 0) {
		initFunc = &PhaseSolver::buildSimpleTree;
	    } else if (strcmp(optarg, "path") == 0) {
		initFunc = &PhaseSolver::buildBlockingPathTree;
	    } else if (strcmp(optarg, "saturate") == 0) {
		initFunc = &PhaseSolver::saturateAllArcs;
	    } else {
		cerr << "Invalid initialization option " << optarg << endl;
		usage();
		return 1;
	    }
	    break;

	case 'M':
	    if (strcmp(optarg, "simplex") == 0) {
		solver = new SimplexSolver();
	    } else if (strcmp(optarg, "normal") == 0) {
		solver = new PhaseSolver();
	    } else {
		cerr << "Invalid merge function option " << optarg << endl;
		usage();
		return 1;
	    }
	    break;

	case 'N':
	    if (strcmp(optarg, "delayed") == 0) {
		solverFunc = &PhaseSolver::delayedNormalizeSolve;
	    } else if (strcmp(optarg, "normal") == 0) {
		solverFunc = &PhaseSolver::solve;
	    } else {
		cerr << "Invalid normalization option " << optarg << endl;
		usage();
		return 1;
	    }
	    break;

	case 'B':
	    if (strcmp(optarg, "fifo") == 0) {
		addBranchFunc = &PhaseSolver::addBranchFifo;
	    } else if (strcmp(optarg, "lifo") == 0) {
		addBranchFunc = &PhaseSolver::addBranchLifo;
	    } else if (strcmp(optarg, "wave") == 0) {
		addBranchFunc = &PhaseSolver::addBranchWave;
	    } else {
		cerr << "Invalid branch-management option " << optarg << endl;
		usage();
		return 1;
	    }
	    break;

	default:
	    usage();
	    return 1;
	}
    }

    argc -= optind;
    //argv += optind;
    
    if (argc != 2) {
	usage();
	return 1;
    }

    const char* instanceName = argv[optind];
    const char* outputName = argv[optind + 1];

    Timer initTimer;
    initTimer.start();
    Boolean readOK =  solver->readDimacsInstance(instanceName);
    initTimer.stop();

    if (readOK) {
	cout << "read problem instance OK" << endl;
	Timer treeTimer;
	Timer solveTimer;
	solveTimer.start();

	treeTimer.start();
	(solver->*initFunc)();
	treeTimer.stop();

	(solver->*solverFunc)(addBranchFunc);
	solveTimer.stop();

	Timer convertTimer;
	convertTimer.start();
	solver->convertToFlow();
	convertTimer.stop();

	cout << "time to establish tree: " << treeTimer << endl;
	cout << "done solving: " << solveTimer << endl;
	cout << "time to convert flow: " << convertTimer << endl;

	ofstream dout(outputName, ios::out);
	if (dout == nil) {
	    cerr << "Unable to open output file: " << outputName << endl;
	    return 1;
	}

	dout << "c" << endl;
	dout << "c  instance: " << instanceName << endl;
	time_t now = time(0);
	dout << "c  runTime: " << ctime(&now);

	extern char* buildFlags;
	extern char* buildDate;
	dout << "c  buildFlags: " << buildFlags << endl;
	dout << "c  buildDate: " << buildDate << endl;
	dout << "c  timeToInitialize: " << initTimer << endl;
	dout << "c  timeToSolve: " << solveTimer << endl;
	dout << "c  timeToFlow: " << convertTimer << endl;
	dout << "c  argv: ";
	for (char** ap = argv; *ap != nil; ap++) {
	    dout << *ap << " ";
	}
	dout << endl;

	if (writeFlow) {
	    solver->writeDimacsFlow(dout);
	}
	if (dumpNodes) {
	    solver->dumpNodes(dout);
	}

	dout.close();
    }

    return 0;
}
