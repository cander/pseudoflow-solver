#include "PhaseSolver.h"

#include <stdio.h>
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

    FILE* in = fopen(instanceName, "r");
    if (in == NULL) {
	perror("Cannot open input instance.");
	exit(1);
    }
    FILE* out = fopen(outputName, "w");
    if (out == NULL) {
	perror("Cannot open output instance.");
	exit(1);
    }
    fprintf(out, "c  converted from %s with lambda=%f\n", instanceName, lambdaVal);

    char line[1024];
    int sourceId = -1;
    int sinkId = -1;

    for (;;) {
	char* str = fgets(line, 1024, in);
	if (str == NULL) {
	    break;
	}

	int ch = *str;
	char c1, c2;
	int id, head, tail, aParam;
	int ntoks;
	double bParam;

	switch(ch) {
	case 'a':
	    ntoks = sscanf(line, "%c %d %d %d %lf", &c1, &tail, &head, &aParam, &bParam);
	    if (ntoks == 5) {
		int cap = aParam;
		// parametric line
		if (bParam != 0) {
		    cap = (int)(aParam + bParam * lambdaVal);
		    if (head == sinkId) {
			if (bParam > 0) {
			    // it's a mining arc
			    if (cap < 0) {
				// leave it at the sink - but negate the cap
				cap = -cap;
			    } else {
				// move it to the source
				head = tail;
				tail = sourceId;
			    }
			} else {
			    // "normal" graph
			    if (cap < 0) {
				// move it to source with postive cap
				head = tail;
				tail = sourceId;
				cap = -cap;
			    }
			}
		    } else if (tail == sourceId) {
			// source-arc, b is always > 0
			if (cap < 0) {
			    // move it to the sink
			    tail = head;
			    head = sinkId;
			    cap = -cap;
			}
		    }
		}  else {
		    // have a zero b value
		    if ((head == sinkId) && (cap < 0)) {
			// mining block
			cap = -cap;
		    }
		}
		assert(cap >= 0);
		fprintf(out, "a %d %d %d\n", tail, head, cap);
		continue;
	    }
	    break;
	case 'n':
	    sscanf(line, "%c %d %c", &c1, &id, &c2);
	    if (c2 == 's') {
		sourceId = id;
	    } else if (c2 == 't') {
		sinkId = id;
	    }
	    break;
	}

	// just copy the line to the output
	fprintf(out, line);
    }

    fclose(out);



    return 0;
}
