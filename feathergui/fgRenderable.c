// Copyright ©2012 Black Sphere Studios
// For conditions of distribution and use, see copyright notice in "feathergui.h"

#include "fgWindow.h"

void FG_FASTCALL Renderable_Init(Renderable* self)
{ 
  assert(self!=0); 
  Child_Init(&self->element,0); 
  self->message=&Renderable_Message; 
  self->element.destroy=&Renderable_Destroy; 
  self->parent=0;
}
void FG_FASTCALL Renderable_Destroy(Renderable* self)
{
  assert(self!=0);
  Renderable_RemoveParent(self);
  
  while(self->element.root) // Instead of child's default behavior of removing our children, we destroy our children
    (self->element.root->destroy)(self->element.root);
  
  FreeRenderable(self);
}
void FG_FASTCALL Renderable_RemoveParent(Renderable* self)
{
  if(self->parent!=0 && (&self->parent->element)==self->element.parent)
    LList_Remove(&self->element, (Child**)&self->parent->rlist); // Remove ourselves from our window parent 
  else if(self->element.root->parent!=0) // Otherwise remove ourselves from our renderable parent, which has a different root
    LList_Remove(&self->element,&self->element.parent->root);
}
void FG_FASTCALL Renderable_Message(Renderable* self, unsigned char type, void* arg)
{
  assert(self!=0);

  switch(type)
  {
  case FG_RADDCHILD:
    Child_SetParent(&((Renderable*)arg)->element,&self->element);
    break;
  case FG_RREMOVECHILD:
    assert(((Renderable*)arg)->element.parent==&self->element);
    Child_SetParent((Child*)arg,0);
    break;
  case FG_RSETAREA:
    memcpy(&self->element.element.area,arg,sizeof(CRect));
    break;
  case FG_RSETELEMENT:
    memcpy(&self->element.element,arg,sizeof(Element));
    break;
  }
}
void FG_FASTCALL Renderable_SetWindow(Renderable* self, struct __WINDOW* window)
{
  Child* cur=self->element.root;
  self->parent=window;
  while(cur)
  {
    Renderable_SetWindow((Renderable*)cur,window);
    cur=cur->next;
  }
}