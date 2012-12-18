// Copyright ©2012 Black Sphere Studios
// For conditions of distribution and use, see copyright notice in "feathergui.h"

#include "fgWindow.h"

void FG_FASTCALL Renderable_Init(Renderable* self)
{ 
  assert(self!=0); 
  Child_Init(&self->element,0); 
  self->message=&Renderable_Message; 
  self->element.destroy=&Renderable_Destroy; 
}
void FG_FASTCALL Renderable_Destroy(Renderable* self)
{
  assert(self!=0);
  if(self->parent!=0)
  { // Remove ourselves from our window parent 
		if(self->element.prev != 0) self->element.prev->next = self->element.next;
    else self->parent->rlist = (Renderable*)self->element.next;
		if(self->element.next != 0) self->element.next->prev = self->element.prev;
  } // Because we don't care about the child parent (and don't want it trying to do anything) we don't call Child_Destroy here
  FreeRenderable(self);
}
void FG_FASTCALL Renderable_Message(Renderable* self, unsigned char type, void* arg)
{
  Renderable* r;
  assert(self!=0);
  switch(type)
  {
  case FG_RADDCHILD:
    r=(Renderable*)arg;
    if(self->parent!=0)
    {
      Window_VoidMessage(self->parent,FG_ADDRENDERABLE,arg);
      r->element.parent=&self->element;
    }
    else
      Child_SetParent(&r->element,&self->element);
    break;
  case FG_RREMOVECHILD:
    r=(Renderable*)arg;
    assert(r->element.parent==&self->element);
    if(self->parent!=0)
      Window_VoidMessage(self->parent,FG_REMOVERENDERABLE,arg);
    else
      Child_SetParent(arg,0);
    (*r->element.destroy)(r);
    break;
  case FG_RSETAREA:
    memcpy(&self->element.element.area,arg,sizeof(CRect));
    break;
  case FG_RSETELEMENT:
    memcpy(&self->element.element,arg,sizeof(Element));
    break;
  }
}
