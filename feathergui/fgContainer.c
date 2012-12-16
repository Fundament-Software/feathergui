// Copyright ©2012 Black Sphere Studios
// For conditions of distribution and use, see copyright notice in "feathergui.h"

#include "fgContainer.h"

void __fastcall RegionDestroy(Child* self)
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

void __fastcall fgContainer_Init(fgContainer* self)
{
  Window_Init(&self->window,0);
  self->window.element.destroy=&fgContainer_Destroy; 
}
void __fastcall fgContainer_Destroy(fgContainer* self)
{
  while(self->regions!=0)
    fgContainer_RemoveRegion(self,self->regions);
  Window_Destroy(&self->window);
}
Child* __fastcall fgContainer_AddRegion(fgContainer* self, Element* region)
{
  Child* r = (Child*)malloc(sizeof(Child));
  memset(r,0,sizeof(Child));
  r->destroy=&RegionDestroy;
  r->parent=(Child*)self;

  r->prev=0;
  r->next=self->regions;
  if(self->regions!=0) self->regions->prev=r;
  self->regions=r;

  memcpy(&r->element,region,sizeof(Element));
}
void __fastcall fgContainer_RemoveRegion(fgContainer* self, Child* region)
{
  assert(region->parent == self);
  (*region->destroy)(region);
  free(region);
}