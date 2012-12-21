// Copyright ©2012 Black Sphere Studios
// For conditions of distribution and use, see copyright notice in "feathergui.h"

#include "fgWindow.h"

fgWindow* fgFocusedWindow = 0;
fgWindow* fgLastHover = 0;
VectWindow fgNonClipping = {0};

void FG_FASTCALL fgWindow_Init(fgWindow* BSS_RESTRICT self, fgChild* BSS_RESTRICT parent)
{ 
  assert(self!=0);
  memset(self,0,sizeof(fgWindow)); 
  fgChild_Init(&self->element,parent);
  self->element.destroy=&fgWindow_Destroy; 
  self->message=&fgWindow_Message; 
}
void FG_FASTCALL fgWindow_Destroy(fgWindow* self)
{
  fgStatic* cur;
  assert(self!=0);
  while((cur=self->rlist)!=0) 
    (*cur->element.destroy)(cur); // Removes static from rlist

  fgChild_Destroy(&self->element);
}

char FG_FASTCALL fgWindow_Message(fgWindow* self, FG_Msg* msg)
{
  //FG_Msg aux;
  FG_UINT i;
  char flip;
  CRect* cr;
  assert(self!=0);
  assert(msg!=0);
  assert(msg->type<FG_CUSTOMEVENT);

  switch(msg->type)
  {
  case FG_MOUSEMOVE:
    if(fgLastHover!=self)
    {
      if(fgLastHover!=0)
        fgWindow_BasicMessage(fgLastHover,FG_MOUSEOFF);
      fgLastHover=self;
      fgWindow_BasicMessage(self,FG_MOUSEON);
    }
    break;
  case FG_MOUSEDOWN:
    if(fgFocusedWindow != self)
      fgWindow_BasicMessage(self,FG_GOTFOCUS);
    if(msg->button == 2 && self->contextmenu!=0)
      fgWindow_BasicMessage(self->contextmenu,FG_GOTFOCUS);
    break;
  case FG_GOTFOCUS:
    if(fgFocusedWindow) // We do this here so you can disable getting focus by blocking this message without messing things up
      fgWindow_BasicMessage(fgFocusedWindow,FG_LOSTFOCUS);
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
    fgChild_SetParent((fgChild*)msg->other,&self->element);
    break;
  case FG_REMOVECHILD:
    fgChild_SetParent((fgChild*)msg->other,0);
    break;
  case FG_SETPARENT:
    fgChild_SetParent(&self->element,(fgChild*)msg->other);
    break;
  case FG_ADDRENDERABLE:
    fgStatic_RemoveParent((fgStatic*)msg->other);
    LList_Add((fgChild*)msg->other,(fgChild**)&self->rlist);
    fgStatic_SetWindow((fgStatic*)msg->other,self);
    break;
  case FG_REMOVERENDERABLE:
    assert(((fgStatic*)msg->other)->parent==self);
    fgStatic_RemoveParent((fgStatic*)msg->other);
    ((fgStatic*)msg->other)->element.parent=0;
    fgStatic_SetWindow((fgStatic*)msg->other,0);
    break;
  case FG_SETCLIP: // If 0 is sent in, disable clipping, otherwise enable. Our internal flag is 1 if clipping disabled, 0 otherwise.
    flip = ((msg->otherint==0)<<2)^(self->flags&4);
    if(msg->otherint!=0 && flip!=0) // Enable clipping, remove from nonclipping array
    {
      for(i=0; i<fgNonClipping.l; ++i)
        if(fgNonClipping.p[i]==self)
        {
          Remove_VectWindow(&fgNonClipping,i);
          break;
        }
    }
    else if(msg->otherint==0 && flip!=0) // Disable clipping, add to nonclipping array
      Add_VectWindow(&fgNonClipping,self); //DON'T sort this; if any parent window changed its zorder everything would explode
    self->flags^=flip;
    break;
  case FG_SHOW: // If 0 is sent in, hide, otherwise show. Our internal flag is 1 if hidden, 0 if shown.
    flip = ((msg->otherint==0)<<3)^(self->flags&8);
    self->flags^=flip;
    break;
  case FG_SETCENTERED:
    cr = &self->element.element.area;
    flip = (msg->otherint&3)^(self->flags&3);
    self->flags^=flip;
    cr->left.rel=(FREL)(((self->flags&1)!=0)?0.5:0.0); //x-axis center
    cr->top.rel=(FREL)(((self->flags&2)!=0)?0.5:0.0); //y-axis center
    CRect_DoCenter(cr,self->flags);
    break;
  case FG_SETORDER:
    self->element.order=msg->otherint;
    fgChild_SetParent(&self->element,self->element.parent);
    break;
  }
  return 0;
}

void FG_FASTCALL fgWindow_SetElement(fgWindow* self, fgElement* element)
{
  char moved;
  fgElement* e = &self->element.element;
  moved = memcmp(e,element,sizeof(fgElement))!=0;
  if(moved)
  {
    memcpy(e,element,sizeof(fgElement));
    fgWindow_BasicMessage(self,FG_MOVE);
  }
}
void FG_FASTCALL fgWindow_SetArea(fgWindow* self, CRect* area)
{
  char moved;
  CRect* l = &self->element.element.area;
  moved = CompareCRects(l,area);
  if(moved)
  {
    memcpy(l,area,sizeof(CRect));
    fgWindow_BasicMessage(self,FG_MOVE);
  }
}

void FG_FASTCALL fgWindow_BasicMessage(fgWindow* self, unsigned char type)
{
  FG_Msg aux = {0};
  aux.type=type;
  assert(self!=0);
  (*self->message)(self,&aux);
}

void FG_FASTCALL fgWindow_VoidMessage(fgWindow* self, unsigned char type, void* data)
{
  FG_Msg aux = {0};
  aux.type=type;
  aux.other=data;
  assert(self!=0);
  (*self->message)(self,&aux);
}

void FG_FASTCALL fgWindow_IntMessage(fgWindow* self, unsigned char type, int data)
{
  FG_Msg aux = {0};
  aux.type=type;
  aux.otherint=data;
  assert(self!=0);
  (*self->message)(self,&aux);
}