// Copyright ©2012 Black Sphere Studios
// For conditions of distribution and use, see copyright notice in "feathergui.h"

#ifndef __R_TREE_H__
#define __R_TREE_H__

#include "feathergui.h"

// Implementation of an R-tree used to efficiently calculate mouse events
typedef struct __RTREE_NODE {
  AbsRect area;
  struct __RTREE_NODE* parent;
  Window* p; // May (but does not have to) store a pointer to a window
  FG_UINT nchild; // Number of additional pointers in the memory after the struct. A null pointer signifies the end of the used space.
} RTreeNode;

MAKE_VECTOR(RTreeNode,RTNVect)

typedef struct {
   RTNVect children;
} RTree;

extern void __fastcall InitRTree(RTree* self);
extern void __fastcall DestroyRTree(RTree* self);
extern RTreeNode* __fastcall InsertRTree(RTree* self, AbsRect* area, void* p);
extern void* __fastcall GetRTree(RTree* self, AbsVec pt);

#endif