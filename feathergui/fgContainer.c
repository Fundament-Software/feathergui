// Copyright ©2012 Black Sphere Studios
// For conditions of distribution and use, see copyright notice in "feathergui.h"

#include "fgContainer.h"

void FG_FASTCALL RegionDestroy(fgChild* self)
{
  while(self->root) // Recursively set all children's parents to 0
    fgChild_SetParent(self->root, 0);

  assert(self!=0);
  if(self->parent!=0)
    LList_Remove(self,&((fgContainer*)self->parent)->regions); // Remove ourselves from our parent
}

void FG_FASTCALL fgContainer_Init(fgContainer* self)
{
  fgWindow_Init(&self->window,0);
  self->window.element.destroy=&fgContainer_Destroy; 
}
void FG_FASTCALL fgContainer_Destroy(fgContainer* self)
{
  while(self->regions!=0)
    fgContainer_RemoveRegion(self,self->regions);
  fgWindow_Destroy(&self->window);
}
fgChild* FG_FASTCALL fgContainer_AddRegion(fgContainer* self, fgElement* region)
{
  fgChild* r = (fgChild*)malloc(sizeof(fgChild));
  memset(r,0,sizeof(fgChild));
  r->destroy=fgChild_Destroy;
  r->parent=&self->window.element;

  LList_Add(r,&self->regions);
  memcpy(&r->element,region,sizeof(fgElement));
  return r;
}
void FG_FASTCALL fgContainer_RemoveRegion(fgContainer* self, fgChild* region)
{
  assert(region->parent == &self->window.element);
  RegionDestroy(region);
  free(region);
}