// Copyright ©2013 Black Sphere Studios
// For conditions of distribution and use, see copyright notice in "feathergui.h"

#include "fgWindow.h"
#include "fgRoot.h"

void FG_FASTCALL fgStatic_Init(fgStatic* self)
{ 
  assert(self!=0); 
  memset(self,0,sizeof(fgStatic));
  fgChild_Init(&self->element); 
  self->message=&fgStatic_Message; 
  self->element.destroy=&fgStatic_Destroy;
  self->element.flags|=FGSTATIC_MARKER;
}

void FG_FASTCALL fgStatic_Destroy(fgStatic* self)
{
  assert(self!=0);
  (*self->message)(self,FG_RSETPARENT,0,0);
  fgStatic_NotifyParent(self);
  fgChild_Destroy(&self->element);
}
void FG_FASTCALL fgStatic_NotifyParent(fgStatic* self)
{
  assert(self!=0);
  if(self->element.parent->flags&FGSTATIC_MARKER) // Is our parent a static?
    (*((fgStatic*)self->element.parent)->message)((fgStatic*)self->element.parent,FG_RREMOVECHILD,self,0);
  else // If not, notify as a window
    fgWindow_VoidMessage((fgWindow*)self->element.parent,FG_REMOVECHILD,self);
}
void FG_FASTCALL fgStatic_Message(fgStatic* self, unsigned char type, void* arg, int other)
{
  fgStatic* hold=(fgStatic*)arg;
  assert(self!=0);

  switch(type)
  {
  case FG_RADDCHILD:
    if(!hold) break; 
    assert(hold->element.flags&FGSTATIC_MARKER); // can't add a window to a static
    if(hold->element.parent!=0)
      fgStatic_NotifyParent(hold);
    fgStatic_SetParent(hold,(fgChild*)self);
    break;
  case FG_RREMOVECHILD:
    if(!hold) break;
    assert(hold->element.parent==(fgChild*)self);
    fgStatic_SetParent(hold,0);
    break;
  case FG_SETPARENT:
    if(((fgChild*)hold)==self->element.parent) break;
    if(hold!=0) {
      if(self->element.parent->flags&FGSTATIC_MARKER) // Is our parent a static?
        (*((fgStatic*)self->element.parent)->message)((fgStatic*)self->element.parent,FG_RADDCHILD,self,0);
      else // If not, notify as a window
        fgWindow_VoidMessage((fgWindow*)self->element.parent,FG_ADDCHILD,self);
    } else // If hold is zero, then our parent MUST be nonzero, or they'd both be 0 and we wouldn't have reached this code.
      fgStatic_NotifyParent(self);
    break;
  case FG_RSETAREA:
    memcpy(&self->element.element.area,arg,sizeof(CRect));
    (self->message)(self,FG_RMOVE,0,0);
    break;
  case FG_RSETELEMENT:
    memcpy(&self->element.element,arg,sizeof(fgElement));
    (self->message)(self,FG_RMOVE,0,0);
    break;
  case FG_RSETORDER:
    if(self->element.parent)
    {
      int old = self->element.order;
      self->element.order=(int)arg;
      if(!(self->element.parent->flags&FGSTATIC_MARKER)) // if our parent is a window, notify them.
      {
        fgWindow* parent=(fgWindow*)self->element.parent;
        FG_Msg aux = {0};
        LList_ChangeOrder((fgChild*)self,(fgChild**)&parent->rlist,(fgChild**)&parent->rlast,0);
        aux.type=FG_SETORDER;
        aux.otherint=old;
        aux.other2=self;
        assert(self!=0);
        (*fgSingleton()->behaviorhook)(parent,&aux);
      }
      else
        LList_ChangeOrder((fgChild*)self,&self->element.parent->root,&self->element.parent->last,0);
    }
    break;
  case FG_RSETFLAGS:
    self->element.flags=(fgFlag)arg;
    break;
  case FG_RMOVE:
    if(self->element.parent!=0 && !(self->element.parent->flags&FGSTATIC_MARKER)) // If our parent is a window, pass it an FG_MOVE message
      fgWindow_VoidMessage((fgWindow*)self->element.parent,FG_MOVE,self);
    break;
  case FG_RSHOW:
    self->element.flags=T_SETBIT(self->element.flags,FGSTATIC_HIDDEN,!((char)arg));
    break;
  }
}

void FG_FASTCALL fgStatic_SetParent(fgStatic* BSS_RESTRICT selfp, fgChild* BSS_RESTRICT parent)
{
  fgChild* self = (fgChild*)selfp;
  fgWindow* win = (fgWindow*)self->parent;

  if(self->parent==parent) { // If this is true, we just need to make sure the ordering is valid
    if(parent->flags&FGSTATIC_MARKER)
      LList_ChangeOrder(self,&parent->root,&parent->last,0);
    else
      LList_ChangeOrder(self,(fgChild**)&win->rlist,(fgChild**)&win->rlast,0);
    return;
  }

  if(self->parent!=0) { // Remove ourselves from our parent
    if(self->parent->flags&FGSTATIC_MARKER)
      LList_Remove(self,&self->parent->root,&self->parent->last);
    else
      LList_Remove(self,(fgChild**)&win->rlist,(fgChild**)&win->rlast);
  }

  self->next=0;
  self->prev=0;
  self->parent=parent;
  win = (fgWindow*)parent;

  if(parent) {
    if(parent->flags&FGSTATIC_MARKER)
      LList_Add(self,&parent->root,&parent->last,0);
    else
      LList_Add(self,(fgChild**)&win->rlist,(fgChild**)&win->rlast,0);
  }

  (*selfp->message)(selfp,FG_RMOVE,0,0);
}
void FG_FASTCALL fgStatic_Clone(fgStatic* self, fgStatic* from)
{
  fgChild* cur=from->element.root;
  memcpy(self,from,sizeof(fgStatic));
  self->element.root=0;
  self->element.last=0;
  self->element.parent=0;

  while(cur)
  {
    (*self->message)(self,FG_RADDCHILD,(*((fgStatic*)cur)->clone)((fgStatic*)cur),0);
    cur=cur->next;
  }
}

FG_EXTERN fgStatic* FG_FASTCALL fgLoadImage(const char* path)
{
  return fgLoadImageData(path,0,0);
}


//void FG_FASTCALL fgStatic_SetWindow(fgStatic* self, struct __WINDOW* window)
//{
//  fgChild* cur=self->element.root;
//  self->parent=window;
//  while(cur)
//  {
//    fgStatic_SetWindow((fgStatic*)cur,window);
//    cur=cur->next;
//  }
//}