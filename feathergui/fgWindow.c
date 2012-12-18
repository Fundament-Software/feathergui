// Copyright ©2012 Black Sphere Studios
// For conditions of distribution and use, see copyright notice in "feathergui.h"

#include "fgWindow.h"

void FG_FASTCALL Behavior_Default(Window* self, FG_Msg* msg)
{
  (*self->message)(self,msg);
}

Window* fgFocusedWindow=0;
void (FG_FASTCALL *behaviorhook)(struct __WINDOW* self, FG_Msg* msg)=&Behavior_Default;

void FG_FASTCALL Window_Init(Window* BSS_RESTRICT self, Child* BSS_RESTRICT parent)
{ 
  assert(self!=0);
  memset(self,0,sizeof(Window)); 
  Child_Init(&self->element,parent);
  self->element.destroy=&Window_Destroy; 
  self->message=&Window_Message; 
}
void FG_FASTCALL Window_Destroy(Window* self)
{
  Renderable* cur;
  assert(self!=0);
  while((cur=self->rlist)!=0) 
    (*cur->element.destroy)(cur); // Removes renderable from rlist

  Child_Destroy(&self->element);
}

void FG_FASTCALL Window_AddRenderable(Window* BSS_RESTRICT self, Renderable* BSS_RESTRICT rend, Child* BSS_RESTRICT parent)
{
  Child_SetParent(&rend->element,0); //Removes this renderable from its parent
  rend->element.parent=parent; // Adds this renderable to this window as a child of parent
  rend->parent=self;
  rend->element.prev=0;
  rend->element.next=&self->rlist->element;
  if(self->rlist!=0) self->rlist->element.prev=&rend->element;
  self->rlist=rend;

  while(rend->element.root!=0) // Goes through this renderables children and adds them all to this window
    Window_AddRenderable(self,(Renderable*)rend->element.root,&rend->element);
}

void FG_FASTCALL Window_Message(Window* self, FG_Msg* msg)
{
  //FG_Msg aux;
  CRect* cr;
  assert(self!=0);
  assert(msg!=0);
  assert(msg->type<FG_CUSTOMEVENT);

  switch(msg->type)
  {
  case FG_MOUSEDOWN:
    if(fgFocusedWindow != self)
      Window_BasicMessage(self,FG_GOTFOCUS);
    if(msg->button == 2 && self->contextmenu!=0)
      Window_BasicMessage(self->contextmenu,FG_GOTFOCUS);
    break;
  case FG_GOTFOCUS:
    if(fgFocusedWindow) // We do this here so you can disable getting focus by blocking this message without messing things up
      Window_BasicMessage(fgFocusedWindow,FG_LOSTFOCUS);
    fgFocusedWindow=self;
    break;
  case FG_LOSTFOCUS:
    assert(fgFocusedWindow==self);
    fgFocusedWindow=0;
    break;
  case FG_MOVE:
    if(self->flags&3)
      CRect_DoCenter(&self->element.element.area,self->flags);
    break;
  case FG_ADDCHILD:
    Child_SetParent((Child*)msg->other,&self->element);
    break;
  case FG_SETPARENT:
    Child_SetParent(&self->element,(Child*)msg->other);
    break;
  case FG_ADDRENDERABLE:
    Window_AddRenderable(self,(Renderable*)msg->other,&self->element);
    break;
  case FG_REMOVERENDERABLE:

    break;
  case FG_SETCLIP:
    self->flags|=((msg->otherint==0)<<2);
    break;
  case FG_SHOW:
    self->flags|=((msg->otherint==0)<<3);
    break;
  case FG_SETCENTERED:
    cr = &self->element.element.area;
    self->flags|=(msg->otherint&3);
    cr->left.rel=(FREL)(((self->flags&1)!=0)?0.5:0.0); //x-axis center
    cr->top.rel=(FREL)(((self->flags&2)!=0)?0.5:0.0); //y-axis center
    CRect_DoCenter(cr,self->flags);
    break;
  }
}

void FG_FASTCALL Window_SetElement(Window* self, Element* element)
{
  char moved;
  Element* e = &self->element.element;
  moved = memcmp(e,element,sizeof(Element))!=0;
  if(moved)
  {
    memcpy(e,element,sizeof(Element));
    Window_BasicMessage(self,FG_MOVE);
  }
}
void FG_FASTCALL Window_SetArea(Window* self, CRect* area)
{
  char moved;
  CRect* l = &self->element.element.area;
  moved = CompareCRects(l,area);
  if(moved)
  {
    memcpy(l,area,sizeof(CRect));
    Window_BasicMessage(self,FG_MOVE);
  }
}

void FG_FASTCALL Window_BasicMessage(Window* self, unsigned char type)
{
  FG_Msg aux = {0};
  aux.type=type;
  assert(self!=0);
  (*self->message)(self,&aux);
}

void FG_FASTCALL Window_VoidMessage(Window* self, unsigned char type, void* data)
{
  FG_Msg aux = {0};
  aux.type=type;
  aux.other=data;
  assert(self!=0);
  (*self->message)(self,&aux);
}

void FG_FASTCALL Window_IntMessage(Window* self, unsigned char type, int data)
{
  FG_Msg aux = {0};
  aux.type=type;
  aux.otherint=data;
  assert(self!=0);
  (*self->message)(self,&aux);
}