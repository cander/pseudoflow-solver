#ifndef DEBUG_H
#define DEBUG_H

#include <assert.h>

#ifdef DEBUG
#define IFDEBUG  if (1)
#else /*!DEBUG*/
#define IFDEBUG  if (0)
#endif /*DEBUG*/


#define CHECK_TREE(node, edgePtr)               \
    IFDEBUG {                                   \
        if (!(node).checkTree(edgePtr)) {   \
            (node).printTree(0);                 \
            abort();                            \
        }                                       \
    }
#endif /*DEBUG_H*/
