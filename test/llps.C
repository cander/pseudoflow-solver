#include "PhaseSolver.h"
#include "Timer.h"

#include <iostream.h>
#include <fstream.h>
#include <time.h>
#include <stdlib.h>

extern char* buildFlags;

// set to TRUE to cause (time consuming) tree checks to take place
Boolean checkTree = FALSE;

void 
usage()
{
    cerr << "Usage: llps [-d] [-f] [-I init] [-N norm] [-B branch] input-graph flow-output" << endl;
    cerr << "\t -d   dump the final disposition of each node" << endl;
    cerr << "\t -f   write the flow values for each arc" << endl;
    cerr << "\t -t   perform checkTree operations frequently" << endl;
    cerr << "\t -I   initialization function: simple, path, saturate, greedy " << endl;
    cerr << "\t -s   specify the number of splits for path init" << endl;
    cerr << "\t -B   strong bucket management: fifo, lifo, wave" << endl;
    cerr << "\t -M   merger function: pseudo, simplex" << endl;
    cerr << "\t -N   normalization method: immed, delayed" << endl;
    cerr << "\t -O   search order: pre, post" << endl;
    cerr << "buildFlags: " << buildFlags << endl;
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
    int numSplits = -1;
    Boolean postOrder = FALSE;

    // parse arguments
    int ch;
    while ((ch = getopt(argc, argv, "dftI:s:M:N:B:O:")) != EOF) {
	switch (ch) {
	case 'd':
	    dumpNodes = TRUE;
	    break;
	case 'f':
	    writeFlow = TRUE;
	    break;
	case 't':
	    checkTree = TRUE;
	    break;
	case 'I':
	    if (strcmp(optarg, "simple") == 0) {
		initFunc = &PhaseSolver::buildSimpleTree;
	    } else if (strcmp(optarg, "path") == 0) {
		initFunc = &PhaseSolver::buildBlockingPathTree;
	    } else if (strcmp(optarg, "saturate") == 0) {
		initFunc = &PhaseSolver::saturateAllArcs;
	    } else if (strcmp(optarg, "greedy") == 0) {
		initFunc = &PhaseSolver::buildGreedyPathTree;
		// should also support greedy:N where N is numSplits
	    } else if (strcmp(optarg, "shortest") == 0) {
		initFunc = &PhaseSolver::buildSpTree;
	    } else {
		cerr << "Invalid initialization option " << optarg << endl;
		usage();
		return 1;
	    }
	    break;

	case 's':
	    numSplits = atoi(optarg);
	    break;

	case 'M':
	    if (strcmp(optarg, "simplex") == 0) {
		solver = new SimplexSolver();
	    } else if (strcmp(optarg, "pseudo") == 0) {
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
	    } else if (strcmp(optarg, "immed") == 0) {
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

	case 'O':
	    if (strcmp(optarg, "post") == 0) {
		postOrder = TRUE;
	    } else if (strcmp(optarg, "pre") == 0) {
		postOrder = FALSE;
	    } else {
		cerr << "Invalid pre/post order option " << optarg << endl;
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

    ofstream dout(outputName, ios::out);
    if (dout == nil) {
	cerr << "Unable to open output file: " << outputName << endl;
	return 1;
    }

    extern char* buildDate;
    dout << "c" << endl;
    dout << "c  instance: " << instanceName << endl;
    dout << "c  buildFlags: " << buildFlags << endl;
    dout << "c  buildDate: " << buildDate << endl;
    dout << "c  argv: ";
    for (char** ap = argv; *ap != nil; ap++) {
	dout << *ap << " ";
    }
    dout << endl;

    if (numSplits >= 0) {
	solver->maxSplits = numSplits;
    }
    solver->postOrderSearch = postOrder;

    time_t beginTime = time(0);
    Timer readTimer;
    readTimer.start();
    Boolean readOK =  solver->readDimacsInstance(instanceName);
    readTimer.stop();

    if (readOK == FALSE) {
	dout << "c  CANNOT READ INSTANCE" << endl;
	return 1;
    }

    cout << "read problem instance OK" << endl;

    Timer totalTimer;
    totalTimer.start();			// total time is everything except I/O

    // build initial tree
    Timer treeTimer;
    Timer solveTimer;
    treeTimer.start();
    solveTimer.start();			// tree time is part of solve
    (solver->*initFunc)();
    treeTimer.stop();

    // solve
    (solver->*solverFunc)(addBranchFunc);
    solveTimer.stop();

    // convert to flow
    Timer convertTimer;
    convertTimer.start();
    if (!dumpNodes) {
	// don't convert to flow if we're dumping the nodes,
	// which only makes sense after phase I before conversion
	solver->convertToFlow();
    }
    convertTimer.stop();

    totalTimer.stop();

    cout << "time to readInstance: "	<< readTimer << endl;
    cout << "time to establish tree: "	<< treeTimer << endl;
    cout << "done solving: "		<< solveTimer << endl;
    cout << "time to convert flow: "	<< convertTimer << endl;
    cout << "total time: "		<< totalTimer << endl;

    time_t endTime = time(0);
    dout << "c  beginRun: "		<< ctime(&beginTime);
    dout << "c  endRun: "		<< ctime(&endTime);

    dout << "c  timeToRead: "		<< readTimer << endl;
    dout << "c  timeToInitialize: "	<< treeTimer << endl;
    dout << "c  timeToSolve: "		<< solveTimer << endl;
    dout << "c  timeToFlow: "		<< convertTimer << endl;
    dout << "c  totalTime: "		<< totalTimer << endl;

    solver->writeStats(dout);

    if (dumpNodes) {
	solver->dumpNodes(dout);
	solver->writeDimacsFlow(dout);
    } else if (writeFlow) {
	solver->writeDimacsFlow(dout);
    }

    dout.close();

    return 0;
}
