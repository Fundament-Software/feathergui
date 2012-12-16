// Copyright ©2012 Black Sphere Studios
// For conditions of distribution and use, see copyright notice in "feathergui.h"

#include "feathergui.h"

Window* fgFocusedWindow=0;
void (__fastcall *keymsghook)(FG_Msg* msg)=0;

AbsVec __fastcall ResolveVec(Child* p, CVec* v)
{
  AbsVec r;
  static Child* plast=0; // This uses a simple memoization scheme so that repeated calls using the same child don't recalculate everything
  static AbsRect last;

  if(!p) { r.x=v->x.abs; r.y=v->y.abs; return r; }
  if(plast!=p)
  {
    last=ResolveRect(p->parent,&p->element.area);
    plast=p;
  }

  r.x = lerp(last.left,last.right,v->x.rel);
  r.y = lerp(last.top,last.bottom,v->y.rel);
  return r; 
}

AbsRect __fastcall ResolveRect(Child* p, CRect* v)
{
  AbsRect r;
  static Child* plast=0; // This uses a simple memoization scheme so that repeated calls using the same child don't recalculate everything
  static AbsRect last;

  if(!p) { r.left=v->left.abs; r.top=v->top.abs; r.right=v->right.abs; r.bottom=v->bottom.abs; return r; }
  if(plast!=p)
  {
    last=ResolveRect(p->parent,&p->element.area);
    plast=p;
  }

  r.left = lerp(last.left,last.right,v->left.rel);
  r.top = lerp(last.top,last.bottom,v->top.rel);
  r.right = lerp(last.left,last.right,v->right.rel);
  r.bottom = lerp(last.top,last.bottom,v->bottom.rel);
  return r; 
}

void __fastcall Child_Init(Child* BSS_RESTRICT self, Child* BSS_RESTRICT parent) 
{ 
  memset(self,0,sizeof(Child));
  self->destroy=&Child_Destroy; 
  Child_SetParent(self,parent);
}
void __fastcall Child_Destroy(Child* self) 
{
  while(self->root) // Recursively set all children's parents to 0
    Child_SetParent(self->root, 0);

  Child_SetParent(self,0); // Remove ourselves from our parent
}

void __fastcall Child_SetParent(Child* BSS_RESTRICT self, Child* BSS_RESTRICT parent)
{
  assert(self!=0);
  if(self->parent!=0)
  { // Remove ourselves from our parent
		if(self->prev != 0) self->prev->next = self->next;
    else self->parent->root = self->next;
		if(self->next != 0) self->next->prev = self->prev;
  }

  self->parent=parent;

  if(parent)
  { // Add ourselves to our new parent
    self->prev=0;
    self->next=parent->root;
    if(parent->root!=0) parent->root->prev=self;
    parent->root=self;
  }
}

void __fastcall Renderable_Init(Renderable* self)
{ 
  assert(self!=0); 
  Child_Init(&self->element,0); 
  self->draw=&Renderable_Draw; 
  self->element.destroy=&Renderable_Destroy; 
  self->ptop=self;
}
void __fastcall Renderable_Draw(Renderable* self) { }
void __fastcall Renderable_Destroy(Renderable* self)
{
  assert(self!=0);
  if(self->parent!=0)
  { // Remove ourselves from our window parent 
		if(self->element.prev != 0) self->element.prev->next = self->element.next;
    else self->parent->rlist = (Renderable*)self->element.next;
		if(self->element.next != 0) self->element.next->prev = self->element.prev;
  } // Because we don't care about the child parent (and don't want it trying to do anything) we don't call Child_Destroy here
}

void __fastcall Window_Init(Window* BSS_RESTRICT self, Child* BSS_RESTRICT parent)
{ 
  assert(self!=0);
  memset(self,0,sizeof(Window)); 
  Child_Init((Child*)self,parent);
  self->element.destroy=&Window_Destroy; 
  self->message=&Window_Message; 
}
void __fastcall Window_Destroy(Window* self)
{
  Renderable* cur;
  assert(self!=0);
  while((cur=self->rlist)!=0) 
  {
    (*cur->element.destroy)(cur->ptop); // Removes renderable from rlist
    free(cur);
  }

  Child_Destroy(&self->element);
}
void __fastcall Window_Message(Window* self, FG_Msg* msg)
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
    if(self->centeraxis!=0)
      CRect_DoCenter(&self->element.element.area,self->centeraxis);
    break;
  case FG_ADDCHILD:
    Child_SetParent((Child*)msg->other,(Child*)self);
    break;
  case FG_SETPARENT:
    Child_SetParent((Child*)self,(Child*)msg->other);
    break;
  case FG_ADDRENDERABLE:
    break;
  case FG_SETCLIP:
    break;
  case FG_SETCENTERED:
    cr = &self->element.element.area;
    self->centeraxis=msg->otherint;
    cr->left.rel=(FREL)(((self->centeraxis&1)!=0)?0.5:0.0); //x-axis center
    cr->top.rel=(FREL)(((self->centeraxis&2)!=0)?0.5:0.0); //y-axis center
    CRect_DoCenter(cr,self->centeraxis);
    break;
  }
}

void __fastcall Window_SetElement(Window* self, Element* element)
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
void __fastcall Window_SetArea(Window* self, CRect* area)
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

void __fastcall Window_BasicMessage(Window* self, unsigned char type)
{
  FG_Msg aux = {0};
  aux.type=type;
  assert(self!=0);
  (*self->message)(self,&aux);
}

void __fastcall Window_VoidMessage(Window* self, unsigned char type, void* data)
{
  FG_Msg aux = {0};
  aux.type=type;
  aux.other=data;
  assert(self!=0);
  (*self->message)(self,&aux);
}

void __fastcall Window_IntMessage(Window* self, unsigned char type, int data)
{
  FG_Msg aux = {0};
  aux.type=type;
  aux.otherint=data;
  assert(self!=0);
  (*self->message)(self,&aux);
}

void __fastcall CRect_DoCenter(CRect* cr, unsigned char axis)
{
  FABS adjust;
  if((axis&1)!=0) //x-axis center
  {
    adjust=(cr->left.abs+cr->right.abs)/2;
    cr->left.abs -= adjust;
    cr->right.abs -= adjust;
  }
  if((axis&2)!=0) //y-axis center
  {
    adjust=(cr->top.abs+cr->bottom.abs)/2; 
    cr->top.abs -= adjust;
    cr->bottom.abs -= adjust;
  }
}

char __fastcall CompareCRects(CRect* l, CRect* r)
{
  assert(l!=0 && r!=0);
  if(l->left.abs!=r->left.abs) return 1; // Optimization to catch a whole ton of move situations where almost all abs coords change.
  return memcmp(l,r,sizeof(CRect))!=0;
}