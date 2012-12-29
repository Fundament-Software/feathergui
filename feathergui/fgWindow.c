// Copyright ©2012 Black Sphere Studios
// For conditions of distribution and use, see copyright notice in "feathergui.h"

#include "fgWindow.h"

fgWindow* fgFocusedWindow = 0;
fgWindow* fgLastHover = 0;
VectWindow fgNonClipping = {0};

void FG_FASTCALL fgWindow_Init(fgWindow* BSS_RESTRICT self, fgWindow* BSS_RESTRICT parent)
{ 
  assert(self!=0);
  memset(self,0,sizeof(fgWindow)); 
  self->element.destroy=&fgWindow_Destroy; 
  self->element.free=&free;
  self->message=&fgWindow_Message;
  if(parent) fgWindow_VoidMessage(parent,FG_ADDCHILD,self);
}
void FG_FASTCALL fgWindow_Destroy(fgWindow* self)
{
  fgStatic* cur;
  assert(self!=0);
  fgWindow_VoidMessage(self,FG_SETPARENT,0);
  while((cur=self->rlist)!=0)
    VirtualFreeChild((fgChild*)cur); // Removes static from rlist

  fgChild_Destroy(&self->element);
}

char FG_FASTCALL fgWindow_Message(fgWindow* self, const FG_Msg* msg)
{
  //FG_Msg aux;
  FG_UINT i;
  char flip,flag,active;
  CRect* cr;
  fgChild* hold;
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
  case FG_ADDCHILD:
    hold=(fgChild*)msg->other;
    assert(hold!=0 && hold->destroy!=fgChild_Destroy);
    assert(hold->parent==0 || hold->parent->destroy!=fgChild_Destroy);
    if(hold->parent!=0)
      fgWindow_VoidMessage((fgWindow*)hold->parent,FG_REMOVECHILD,hold);
    fgChild_SetParent((fgChild*)msg->other,&self->element);
    break;
  case FG_REMOVECHILD:
    assert(((fgChild*)msg->other)->parent==(fgChild*)self);
    fgChild_SetParent((fgChild*)msg->other,0);
    break;
  case FG_SETPARENT:
    hold=(fgChild*)msg->other;
    if(hold==self->element.parent) break;
    assert(hold==0 || hold->destroy!=fgChild_Destroy);
    if(hold!=0)
      fgWindow_VoidMessage((fgWindow*)hold,FG_ADDCHILD,self); // This will remove us from our parent if we have one.
    else // If msg->other is not zero, then our parent MUST be nonzero or we wouldn't have reached this code
      fgWindow_VoidMessage((fgWindow*)self->element.parent,FG_REMOVECHILD,self);
    break;
  case FG_ADDSTATIC:
    hold=(fgChild*)msg->other;
    fgStatic_NotifyParent((fgStatic*)hold);
    hold->parent=(fgChild*)self;
    LList_Add(hold,(fgChild**)&self->rlist);
    fgStatic_SetWindow((fgStatic*)hold,self);
    break;
  case FG_REMOVESTATIC:
    assert(((fgStatic*)msg->other)->parent==self);
    fgStatic_RemoveParent((fgStatic*)msg->other);
    ((fgStatic*)msg->other)->element.parent=0;
    fgStatic_SetWindow((fgStatic*)msg->other,0);
    break;
  case FG_SHOW: // If 0 is sent in, hide, otherwise show. Our internal flag is 1 if hidden, 0 if shown.
  case FG_SETFLAG: // If 0 is sent in, disable the flag, otherwise enable. Our internal flag is 1 if clipping disabled, 0 otherwise.
    if(msg->type==FG_SHOW) {
      flag=FGWIN_HIDDEN;
      active=(msg->otherint==0);
    } else {
      flag=msg->otherint;
      active=(msg->otherintaux!=0);
    }
    flip = ((-active)&flag)^(self->flags&flag);
    switch(flag)
    {
    case FGWIN_NOCLIP:
      if(active && flip!=0) // Enable clipping, remove from nonclipping array
      {
        for(i=0; i<fgNonClipping.l; ++i)
          if(fgNonClipping.p[i]==self)
          {
            Remove_VectWindow(&fgNonClipping,i);
            break;
          }
      }
      else if(!active && flip!=0) // Disable clipping, add to nonclipping array
        Add_VectWindow(&fgNonClipping,self); //DON'T sort this; if any parent window changed its zorder everything would explode
      break;
    case FGWIN_HIDDEN:
      hold=self->element.parent;
      if(active && flip!=0) // Hide window
      {
        fgWindow_VoidMessage(self,FG_SETPARENT,0);
        self->element.parent=hold;
      }
      else if(!active && flip!=0) // Show window
      {
        self->element.parent=0; // Trick parent into thinking we were never on it
        fgWindow_VoidMessage(self,FG_SETPARENT,hold);
      }
      break;
    }
    self->flags^=flip;
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

void FG_FASTCALL DoSkinCheck(fgWindow* self, fgStatic** skins, const FG_Msg* msg)
{
  if(skins[msg->otheraux]!=0)
    fgWindow_VoidMessage(self,FG_REMOVESTATIC,skins[msg->otheraux]);
  skins[msg->otheraux]=(fgStatic*)msg->other;
}