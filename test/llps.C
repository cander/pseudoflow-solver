#include "PhaseSolver.h"
#include "Timer.h"

#include <iostream.h>
#include <fstream.h>
#include <time.h>
#include <stdlib.h>

void 
usage()
{
    cerr << "Usage: llps [-d] [-f] input-graph flow-output" << endl;
    cerr << "\t -d   dump the final disposition of each node" << endl;
    cerr << "\t -f   write the flow values for each arc" << endl;
    cerr << "\t -n   use delayed normalization solver" << endl;
    cerr << "\t -p   use the greedyPushTree method to initialize" << endl;
    cerr << "\t -s   use the saturateAll method to initialize" << endl;
}

int 
main(int argc, char** argv)
{
    Boolean dumpNodes = FALSE;
    Boolean writeFlow = FALSE;
    void (PhaseSolver::* initFunc)() = &PhaseSolver::buildSimpleTree;
    void (PhaseSolver::* solverFunc)(AddBranchPtr) = &PhaseSolver::solve;
    AddBranchPtr addBranchFunc = &PhaseSolver::addBranchLifo;

    // parse arguments
    int ch;
    while ((ch = getopt(argc, argv, "dfnps")) != EOF) {
	switch (ch) {
	case 'd':
	    dumpNodes = TRUE;
	    break;
	case 'f':
	    writeFlow = TRUE;
	    break;
	case 'n':
	    solverFunc = &PhaseSolver::delayedNormalizeSolve;
	    break;
	case 'p':
	    initFunc = &PhaseSolver::buildBlockingPathTree;
	    break;
	case 's':
	    initFunc = &PhaseSolver::saturateAllArcs;
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

    PhaseSolver solver;
    Timer initTimer;
    initTimer.start();
    Boolean readOK =  solver.readDimacsInstance(instanceName);
    initTimer.stop();

    if (readOK) {
	cout << "read problem instance OK" << endl;
	Timer treeTimer;
	Timer solveTimer;
	solveTimer.start();

	treeTimer.start();
	(solver.*initFunc)();
	treeTimer.stop();

	(solver.*solverFunc)(addBranchFunc);
	solveTimer.stop();

	cout << "time to establish tree: " << treeTimer << endl;
	cout << "done solving: " << solveTimer << endl;

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
	dout << "c  argv: ";
	for (char** ap = argv; *ap != nil; ap++) {
	    dout << *ap << " ";
	}
	dout << endl;

	if (writeFlow) {
	    solver.writeDimacsFlow(dout);
	}
	if (dumpNodes) {
	    solver.dumpNodes(dout);
	}

	dout.close();
    }

    return 0;
}
