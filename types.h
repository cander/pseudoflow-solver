#ifndef TYPES_H
#define TYPES_H
#define nil 0
enum Boolean { FALSE = 0, TRUE };

typedef int NodeId;

typedef int NodeLabel;
const NodeLabel MAX_LABEL = 999999999;
const int MAXINT = 2147483647;		// 2^31 - 1

// excess and flow have to be in the same units
typedef int  FlowAmount;
typedef FlowAmount NodeExcess;

class Node;
typedef Node* NodePtr;
class Edge;
typedef Edge* EdgePtr;

inline int min(int x, int y) { return (x < y) ? x : y; }
inline int max(int x, int y) { return (x > y) ? x : y; }

#endif /*TYPES_H*/
