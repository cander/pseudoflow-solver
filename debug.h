#ifndef DEBUG_H
#define DEBUG_H

#ifdef DEBUG
  extern bool checkTree;
  #define IFCHECK  if (checkTree)
  #define IFDEBUG(code) { code; }
#else /*!DEBUG*/
  #define IFCHECK  if (0)
  #define NDEBUG		// disable assertions
  #define IFDEBUG(code) { }
#endif /*DEBUG*/

#include <assert.h>

#define CHECK_TREE(node, edgePtr) 		\
    IFCHECK {                                   \
	assert((node).checkTree(edgePtr));	\
    }

#define CHECK_TREE2(node, edgePtr)               \
    IFCHECK {                                   \
        if (!(node).checkTree(edgePtr)) {   \
            (node).printTree(0);                 \
            abort();                            \
        }                                       \
    }


#ifdef TRACING
    extern bool tracingEnabled;
    #define trout cout
    #define IFTRACE(expr)		expr;
    #define TRACE(print_expression) 	if(tracingEnabled) { print_expression ; }
#else
    #define IFTRACE(expr)		;
    #define TRACE(print_expression)	 ;
#endif


#ifdef STATS_TRACING
    #define STATS(expr)	expr
#else
    #define STATS(expr) 
#endif

#endif /*DEBUG_H*/

