#ifndef TYPES_H
#define TYPES_H
#define nil 0
enum Boolean { FALSE = 0, TRUE };

typedef int NodeId;

typedef int NodeLabel;

// excess and flow have to be in the same units
typedef int  FlowAmount;
typedef FlowAmount NodeExcess;

class Node;
class Edge;
#endif /*TYPES_H*/
