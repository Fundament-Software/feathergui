// Copyright ©2012 Black Sphere Studios
// For conditions of distribution and use, see copyright notice in "feathergui.h"

#include "rtree.h"

extern void __fastcall InitRTree(RTree* self)
{
  memset(self,0,sizeof(RTree));
}
extern void __fastcall DestroyRTree(RTree* self)
{
  Destroy_RTNVect(&self->children);
}
extern RTreeNode* __fastcall InsertRTree(RTree* self, AbsRect* area, void* p)
{
}
extern void* __fastcall GetRTree(RTree* self, AbsVec pt)
{

}