#ifndef DEBUG_H
#define DEBUG_H

#ifdef DEBUG
  #define IFDEBUG  if (1)
#else /*!DEBUG*/
  #define IFDEBUG  if (0)
  #define NDEBUG		// disable assertions
#endif /*DEBUG*/

#include <assert.h>

#define CHECK_TREE(node, edgePtr)               \
    IFDEBUG {                                   \
        if (!(node).checkTree(edgePtr)) {   \
            (node).printTree(0);                 \
            abort();                            \
        }                                       \
    }


#ifdef TRACING
    #define trout cout
    #define TRACE(print_expression) 	print_expression ;
#else
    #define TRACE(print_expression)	 ;
#endif


#ifdef PERF_TRACING
    #define PERF_METER(expr)	expr
#else
    #define PERF_METER(expr) 	;
#endif

#endif /*DEBUG_H*/

