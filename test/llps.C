#include "PhaseSolver.h"
#include "Timer.h"

#include <iostream.h>
#include <fstream.h>
#include <time.h>


int main(int argc, char** argv)
{
    if (argc != 3) {
	cerr << "Usage: llps input-graph flow-output" << endl;
	return 1;
    }

    const char* instanceName = argv[1];
    const char* outputName = argv[2];

    PhaseSolver solver;
    Timer initTimer;
    initTimer.start();
    Boolean readOK =  solver.readDimacsInstance(instanceName);
    initTimer.stop();

    if (readOK) {
	cout << "read problem instance OK" << endl;
	Timer solveTimer;
	solveTimer.start();
	solver.establishInitialTree();
	solver.solve();
	solveTimer.stop();
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

	solver.writeDimacsFlow(dout);
	//solver.dumpNodes(dout);
	dout.close();
    }

    return 0;
}
