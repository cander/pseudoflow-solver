#include "PhaseSolver.h"
#include "Timer.h"

#include <iostream.h>
#include <fstream.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#ifdef NEED_GETOPT
#include <getopt.h>
#endif /*NEED_GETOPT*/

// set to TRUE to cause (time consuming) tree checks to take place
Boolean checkTree = FALSE;
// set to TRUE to print verbose tracing messages
Boolean tracingEnabled = FALSE;

void 
usage()
{
    cerr << "Usage: pllps [-x] [-g freq] [-t] [-d] [-f] -V lam1,lam2,... [-I init] [-N norm] [-B branch] input-graph flow-output" << endl;
    cerr << "\t -d   dump the final disposition of each node" << endl;
    cerr << "\t -f   write the flow values for each arc" << endl;
    cerr << "\t -t   perform checkTree operations frequently" << endl;
    cerr << "\t -x   trace execution on console" << endl;
    cerr << "\t -g   specify the global relabel frequency" << endl;

    cerr << "\t -V   specify values for lambda" << endl;
    cerr << "\t -L   inital labeling method: const, sink, deficit" << endl;
    cerr << "\t -I   initialization function: simple, path, saturate, greedy " << endl;
    cerr << "\t -s   specify the number of splits for path init" << endl;
    cerr << "\t -B   strong bucket management: fifo, lifo, wave" << endl;
    cerr << "\t -M   merger function: pseudo, simplex" << endl;
    cerr << "\t -N   normalization method: immed, delayed" << endl;
    cerr << "\t -O   search order: pre, post" << endl;
}

double*
getLambdas(int& numLambdas, char* str)
{
    // figure out how many elements - find each comma and replace
    // it with a null.
    int numCommas = 0;
    char* curr = index(str, ',');
    while (curr != nil) {
	numCommas++;
	*curr = '\0';
	curr++;
	curr = index(curr, ',');
    }
    numLambdas = numCommas + 1;
    double* result = new double[numLambdas];

    // parse each element with atof
    curr = str;
    for (int i = 0; i < numLambdas; i++) {
	result[i] = atof(curr);
	curr += strlen(curr) + 1;
    }

    return result;
}

int 
main(int argc, char** argv)
{
    Boolean dumpNodes = FALSE;
    Boolean writeFlow = FALSE;
    void (PhaseSolver::* initFunc)(LabelMethod) = &PhaseSolver::buildSimpleTree;
    void (PhaseSolver::* solverFunc)(AddBranchPtr) = &PhaseSolver::solve;
    AddBranchPtr addBranchFunc = &PhaseSolver::addBranchLifo;
    PhaseSolver* solver = new PhaseSolver();
    int numSplits = -1;
    Boolean postOrder = FALSE;
    double* lambdaValues = nil;
    int numLambdas = 0;
    float relabelFreq = 0.0;
    LabelMethod labelMethod = LABELS_CONSTANT;

    // parse arguments
    int ch;
    while ((ch = getopt(argc, argv, "dfg:txV:L:I:s:M:N:B:O:")) != EOF) {
	switch (ch) {
	case 'd':
	    dumpNodes = TRUE;
	    break;
	case 'f':
	    writeFlow = TRUE;
	    break;
	case 'g':
	    relabelFreq = atof(optarg);
	    break;
	case 't':
	    checkTree = TRUE;
	    break;
	case 'x':
	    tracingEnabled = TRUE;
	    break;
	case 'V':
	    lambdaValues = getLambdas(numLambdas, optarg);
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

	case 'L':
	    if (strcmp(optarg, "const") == 0) {
		labelMethod = LABELS_CONSTANT;
	    } else if (strcmp(optarg, "sink") == 0) {
		labelMethod = LABELS_SINK_DIST;
	    } else if (strcmp(optarg, "deficit") == 0) {
		labelMethod = LABELS_DEFICIT_DIST;
	    } else {
		cerr << "Invalid inital labeling option " << optarg << endl;
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
    
    if ((argc != 2) || (numLambdas < 1)) {
	usage();
	assert(0);
	return 1;
    }

    const char* instanceName = argv[optind];
    const char* outputName = argv[optind + 1];

    ofstream dout(outputName, ios::out);
    if (dout == nil) {
	cerr << "Unable to open output file: " << outputName << endl;
	return 1;
    }

    extern char* buildFlags;
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
    if (relabelFreq >= 0.0) {
	solver->relabelFrequency = relabelFreq;
    }
    solver->postOrderSearch = postOrder;


    time_t beginTime = time(0);
    Timer initTimer;
    initTimer.start();
    Boolean readOK =  solver->readDimacsInstance(instanceName);
    initTimer.stop();

    if (readOK == FALSE) {
	dout << "c  CANNOT READ INSTANCE" << endl;
	return 1;
    }

    cout << "read problem instance OK" << endl;
    Timer totalTimer;
    totalTimer.start();

    solver->setInitialCapacity(lambdaValues[0]);
    // build initial tree
    Timer treeTimer;
    treeTimer.start();
    (solver->*initFunc)(labelMethod);
    treeTimer.stop();

    // solve
    Timer solveTimer;
    solveTimer.start();
    Timer* lambdaTimers = new Timer[numLambdas];

    for (int i = 0; i < numLambdas; i++) {
	lambdaTimers[i].start();
	if (i > 0) {
	    solver->setNextCapacityParameter(lambdaValues[i-1], lambdaValues[i]);
	}

	(solver->*solverFunc)(addBranchFunc);
	lambdaTimers[i].stop();
    }

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

    cout << "time to readInstance: " << initTimer << endl;
    cout << "time to establish tree: " << treeTimer << endl;
    cout << "done solving: " << solveTimer << endl;
    cout << "time to convert flow: " << convertTimer << endl;
    cout << "total time: " << totalTimer << endl;
    for (int i = 0; i < numLambdas; i++) {
	cout << "lambda[" << i << "]: " << lambdaTimers[i] << endl;
    }

    time_t endTime = time(0);
    dout << "c  beginRun: " << ctime(&beginTime);
    dout << "c  endRun: " << ctime(&endTime);

    dout << "c  timeToRead: " << initTimer << endl;
    dout << "c  timeToInitialize: " << treeTimer << endl;
    dout << "c  timeToSolve: " << solveTimer << endl;
    dout << "c  timeToFlow: " << convertTimer << endl;
    dout << "c  totalTime: " << totalTimer << endl;
    for (int i = 0; i < numLambdas; i++) {
	dout << "c  lambda[" << i << "]: " << lambdaTimers[i] << endl;
    }

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
