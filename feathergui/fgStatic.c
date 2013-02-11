// Copyright ©2013 Black Sphere Studios
// For conditions of distribution and use, see copyright notice in "feathergui.h"

#include "fgWindow.h"

void FG_FASTCALL fgStatic_Init(fgStatic* self)
{ 
  assert(self!=0); 
  fgChild_Init(&self->element); 
  self->message=&fgStatic_Message; 
  self->element.destroy=&fgStatic_Destroy; 
  self->parent=0;
  self->flags=0;
}

void FG_FASTCALL fgStatic_Destroy(fgStatic* self)
{
  assert(self!=0);
  fgStatic_NotifyParent(self);
  
  while(self->element.root)
    VirtualFreeChild(self->element.root);
}
void FG_FASTCALL fgStatic_RemoveParent(fgStatic* self)
{
  assert(self!=0);
  if(self->parent!=0 && (&self->parent->element)==self->element.parent)
    LList_Remove(&self->element, (fgChild**)&self->parent->rlist, (fgChild**)&self->parent->rlast); // Remove ourselves from our window parent 
  else if(self->element.parent!=0) // Otherwise remove ourselves from our static parent, which has a different root
    LList_Remove(&self->element,&self->element.parent->root,&self->element.parent->last);
  self->element.next=0;
  self->element.prev=0;
}
void FG_FASTCALL fgStatic_NotifyParent(fgStatic* self)
{
  assert(self!=0);
  if(self->parent!=0 && (&self->parent->element)==self->element.parent) // Notify parent window of removal
    fgWindow_VoidMessage(self->parent,FG_REMOVESTATIC,self);
  else if(self->element.parent!=0) // Otherwise notify parent static of removal
    (*((fgStatic*)self->element.parent)->message)((fgStatic*)self->element.parent,FG_RREMOVECHILD,self);
  assert(self->element.parent==0);
}
void FG_FASTCALL fgStatic_Message(fgStatic* self, unsigned char type, void* arg)
{
  assert(self!=0);

  switch(type)
  {
  case FG_RADDCHILD:
    fgStatic_NotifyParent((fgStatic*)arg);
    fgChild_SetParent(&((fgStatic*)arg)->element,&self->element);
    break;
  case FG_RREMOVECHILD:
    assert(((fgStatic*)arg)->element.parent==&self->element);
    fgChild_SetParent((fgChild*)arg,0);
    break;
  case FG_RSETAREA:
    memcpy(&self->element.element.area,arg,sizeof(CRect));
    break;
  case FG_RSETELEMENT:
    memcpy(&self->element.element,arg,sizeof(fgElement));
    break;
  case FG_RSETORDER:
    self->element.order=(FG_UINT)arg;
    fgStatic_RemoveParent(self);
    if(self->parent!=0 && (&self->parent->element)==self->element.parent)
      fgWindow_VoidMessage(self->parent,FG_ADDSTATIC,self);
    else if(self->element.parent!=0)
      fgChild_SetParent(&self->element,self->element.parent);
    break;
  case FG_RSETFLAGS:
    self->flags=(fgFlag)arg;
    break;
  case FG_RCLONE:
    assert(arg!=0);
    fgStatic_Clone(arg,self);  
    break;
  }
}
void FG_FASTCALL fgStatic_MessageEmpty(fgStatic* self, unsigned char type, void* arg)
{
  assert(self!=0);

  fgStatic_Message(self,type,arg);

  switch(type)
  {
  case FG_RADDCHILD:
  case FG_RREMOVECHILD:
  case FG_RMOVE:
    if(self->flags&(FGWIN_EXPANDX|FGWIN_EXPANDY)) // Should we expand?
    {
      if(self->flags&FGSTATIC_EXPANDX) fgChild_ExpandX((fgChild*)self,arg);
      if(self->flags&FGSTATIC_EXPANDY) fgChild_ExpandY((fgChild*)self,arg);
      if(self->parent!=0 && (&self->parent->element)==self->element.parent) // If our parent is a window, pass it an FG_MOVE message
        fgWindow_VoidMessage(self->parent,FG_MOVE,self);
      else if(self->element.parent!=0) // Otherwise if our parent exists, its an fgStatic, so pass it an FG_RMOVE message.
        (*((fgStatic*)self->element.parent)->message)((fgStatic*)self->element.parent,FG_RMOVE,self);
    }
    break;
  }
}

void FG_FASTCALL fgStatic_SetWindow(fgStatic* self, struct __WINDOW* window)
{
  fgChild* cur=self->element.root;
  self->parent=window;
  while(cur)
  {
    fgStatic_SetWindow((fgStatic*)cur,window);
    cur=cur->next;
  }
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
    (*self->message)(self,FG_RADDCHILD,(*((fgStatic*)cur)->clone)((fgStatic*)cur));
    cur=cur->next;
  }
}

void FG_FASTCALL SetSkinArray(fgStatic** pp, unsigned char size, unsigned char index)
{
  unsigned char i;
  assert(pp!=0);
  for(i = 0; i < size; ++i)
    if(pp[i])
      (*pp[i]->message)(pp[i],FG_RSHOW,0);
  
  if(pp[index])
    (*pp[index]->message)(pp[index],FG_RSHOW,(void*)1);
}

