// Copyright ©2013 Black Sphere Studios
// For conditions of distribution and use, see copyright notice in "feathergui.h"

#include "fgContainer.h"

void FG_FASTCALL RegionDestroy(fgChild* self)
{
  fgContainer* parent = (fgContainer*)self->parent;
  assert(self!=0);
  while(self->root) // Recursively set all children's parents to 0
    fgChild_SetParent(self->root, 0);

  if(parent!=0)
    LList_Remove(self,(fgChild**)&parent->regions,(fgChild**)&parent->regionslast); // Remove ourselves from our parent
}

void FG_FASTCALL fgContainer_Init(fgContainer* self, fgWindow* parent, const fgElement* element, FG_UINT id, fgFlag flags)
{
  assert(self!=0);
  fgWindow_Init(&self->window,parent,element,id,flags);
  self->window.element.destroy=&fgContainer_Destroy; 
}
void FG_FASTCALL fgContainer_Destroy(fgContainer* self)
{
  assert(self!=0);
  while(self->regions!=0)
    fgContainer_RemoveRegion(self,self->regions);
  fgWindow_Destroy(&self->window);
}
fgWindow* FG_FASTCALL fgContainer_AddRegion(fgContainer* self, fgElement* region)
{
  fgWindow* r = (fgWindow*)malloc(sizeof(fgWindow));
  assert(self!=0);
  memset(r,0,sizeof(fgWindow));
  r->element.destroy=fgWindow_Destroy;
  r->element.parent=&self->window.element;

  LList_Add((fgChild*)r,(fgChild**)&self->regions,(fgChild**)&self->regionslast);
  memcpy(&r->element.element,region,sizeof(fgElement));
  return r;
}
void FG_FASTCALL fgContainer_RemoveRegion(fgContainer* self, fgWindow* region)
{
  assert(self!=0);
  assert(region->element.parent == &self->window.element);
  RegionDestroy((fgChild*)region);
  free(region);
}