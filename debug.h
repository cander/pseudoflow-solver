#ifndef DEBUG_H
#define DEBUG_H

#ifdef DEBUG
  extern Boolean checkTree;
  #define IFCHECK  if (checkTree)
  #define IFDEBUG(code) { code; }
#else /*!DEBUG*/
  #define IFCHECK  if (0)
  #define NDEBUG		// disable assertions
  #define IFDEBUG(code) { }
#endif /*DEBUG*/

#include <assert.h>

#define CHECK_TREE(node, edgePtr)               \
    IFCHECK {                                   \
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


#ifdef STATS_TRACING
    #define STATS(expr)	expr
#else
    #define STATS(expr) 	;
#endif

#endif /*DEBUG_H*/

