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

void __fastcall Element_Init(Element* self) { memset(self,0,sizeof(Element)); }

void __fastcall Child_Init(Child* self) { memset(self,0,sizeof(Child)); self->destroy=&Child_Destroy; }
void __fastcall Child_Destroy(Child* self) {}

void __fastcall Renderable_Init(Renderable* self)
{ 
  assert(self!=0); 
  Child_Init(&self->element); 
  self->draw=&Renderable_Draw; 
}
void __fastcall Renderable_Draw(Renderable* self) { }

void __fastcall Window_Init(Window* self, Window* parent)
{ 
  assert(self!=0);
  memset(self,0,sizeof(Window)); 
  self->element.destroy=&Window_Destroy; 
  self->message=&Window_Message; 
  Window_SetParent(self,parent);
}
void __fastcall Window_Destroy(Window* self)
{
  assert(self!=0);
  Child_Destroy(&self->element);
  if(self->rlist) free(self->rlist);

  while(self->root) // Recursively destroy all children, who will remove themselves from the list until root is NULL.
    Window_Destroy(self->root);

  Window_SetParent(self,0); //Remove ourselves from our parent by setting parent to NULL
}
void __fastcall Window_SetParent(Window* self, Window* parent)
{
  assert(self!=0);
  if(self->element.parent)
  {
		if(self->prev != 0) self->prev->next = self->next;
    else ((Window*)self->element.parent)->root = self->next;
		if(self->next != 0) self->next->prev = self->prev;
  }

  self->element.parent=&parent->element;

  if(parent)
  {
    self->prev=0;
    self->next=parent->root;
    if(parent->root!=0) parent->root->prev=self;
    parent->root=self;
  }
}
void __fastcall Window_Message(Window* self, FG_Msg* msg)
{
  FG_Msg aux;
  assert(self!=0);
  assert(msg!=0);
  assert(msg->type<FG_CUSTOMEVENT);

  switch(msg->type)
  {
  case FG_MOUSEDOWN:
    memset(&aux,0,sizeof(FG_Msg));
    aux.type=FG_GOTFOCUS;
    (*self->message)(self,&aux);
    break;
  case FG_GOTFOCUS:
    if(fgFocusedWindow) // We do this here so you can disable getting focus by blocking this message without messing things up
    {
      memset(&aux,0,sizeof(FG_Msg));
      aux.type=FG_LOSTFOCUS;
      (*self->message)(fgFocusedWindow,&aux);
    }
    fgFocusedWindow=self;
    break;
  case FG_LOSTFOCUS:
    assert(fgFocusedWindow==self);
    fgFocusedWindow=0;
    break;
  }
}
