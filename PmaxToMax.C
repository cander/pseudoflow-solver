#include "PhaseSolver.h"

#include <iostream.h>
#include <fstream.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#ifdef NEED_GETOPT
#include <getopt.h>
#endif /*NEED_GETOPT*/

// both of these are only needed for linking
Boolean checkTree = FALSE;
Boolean tracingEnabled = FALSE;

void 
usage()
{
    cerr << "Usage: PmaxToMax -V lambda input-graph output-graph" << endl;
}


int 
main(int argc, char** argv)
{
    double lambdaVal = 1.0;

    // parse arguments
    int ch;
    while ((ch = getopt(argc, argv, "V:")) != EOF) {
	switch (ch) {
	case 'V':
	    lambdaVal = atof(optarg);
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
	assert(0);
	return 1;
    }

    const char* instanceName = argv[optind];
    const char* outputName = argv[optind + 1];

    PhaseSolver* solver = new PhaseSolver();
    Boolean readOK =  solver->readDimacsInstance(instanceName);

    if (readOK == FALSE) {
	cout << "CANNOT READ INSTANCE" << endl;
	exit(-1);
	return 1;
    }

    cout << "read problem instance OK" << endl;

    ofstream dout(outputName, ios::out);
    if (dout == nil) {
	cerr << "Unable to open output file: " << outputName << endl;
	return 1;
    }

    time_t now = time(0);
    dout << "c" << endl;
    dout << "c  instance: " << instanceName << endl;
    dout << "c  convertedTime: " << ctime(&now);
    dout << "c  by PmaxToMax.C" << endl;
    dout << "c  lambda: " << lambdaVal << endl;

    dout << "c  argv: ";
    for (char** ap = argv; *ap != nil; ap++) {
	dout << *ap << " ";
    }
    dout << endl;

    solver->setInitialCapacity(lambdaVal);
    solver->writeInstance(dout);

    dout.close();

    return 0;
}
