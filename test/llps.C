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
}

int 
main(int argc, char** argv)
{
    Boolean dumpNodes = FALSE;
    Boolean writeFlow = FALSE;

    // parse arguments
    int ch;
    while ((ch = getopt(argc, argv, "df")) != EOF) {
	switch (ch) {
	case 'd':
	    dumpNodes = TRUE;
	    break;
	case 'f':
	    writeFlow = TRUE;
	    break;
	default:
	    usage();
	    return 1;
	}
    }

    argc -= optind;
    argv += optind;
    
    if (argc != 2) {
	usage();
	return 1;
    }

    const char* instanceName = argv[0];
    const char* outputName = argv[1];

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
	solver.establishInitialTree();
	treeTimer.stop();
	solver.solve();
	solveTimer.stop();
	cout << "done solving: " << solveTimer << endl;
	cout << "time to establish tree: " << treeTimer << endl;

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
