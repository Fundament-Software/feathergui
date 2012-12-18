// Copyright ©2012 Black Sphere Studios
// For conditions of distribution and use, see copyright notice in "feathergui.h"

#include "rtree.h"

extern void FG_FASTCALL InitRTree(RTree* self)
{
  memset(self,0,sizeof(RTree));
}
extern void FG_FASTCALL DestroyRTree(RTree* self)
{
  Destroy_RTNVect(&self->children);
}
extern RTreeNode* FG_FASTCALL InsertRTree(RTree* self, AbsRect* area, void* p)
{
}
extern void* FG_FASTCALL GetRTree(RTree* self, AbsVec pt)
{

}