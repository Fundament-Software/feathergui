// Copyright ©2012 Black Sphere Studios
// For conditions of distribution and use, see copyright notice in "feathergui.h"

#include "fgContainer.h"

void FG_FASTCALL RegionDestroy(Child* self)
{
  while(self->root) // Recursively set all children's parents to 0
    Child_SetParent(self->root, 0);

  assert(self!=0);
  if(self->parent!=0)
  { // Remove ourselves from our parent
		if(self->prev != 0) self->prev->next = self->next;
    else ((fgContainer*)self->parent)->regions = self->next;
		if(self->next != 0) self->next->prev = self->prev;
  }
}

void FG_FASTCALL fgContainer_Init(fgContainer* self)
{
  Window_Init(&self->window,0);
  self->window.element.destroy=&fgContainer_Destroy; 
}
void FG_FASTCALL fgContainer_Destroy(fgContainer* self)
{
  while(self->regions!=0)
    fgContainer_RemoveRegion(self,self->regions);
  Window_Destroy(&self->window);
}
Child* FG_FASTCALL fgContainer_AddRegion(fgContainer* self, Element* region)
{
  Child* r = (Child*)malloc(sizeof(Child));
  memset(r,0,sizeof(Child));
  r->destroy=&RegionDestroy;
  r->parent=&self->window.element;

  r->prev=0;
  r->next=self->regions;
  if(self->regions!=0) self->regions->prev=r;
  self->regions=r;

  memcpy(&r->element,region,sizeof(Element));
  return r;
}
void FG_FASTCALL fgContainer_RemoveRegion(fgContainer* self, Child* region)
{
  assert(region->parent == &self->window.element);
  (*region->destroy)(region);
  free(region);
}