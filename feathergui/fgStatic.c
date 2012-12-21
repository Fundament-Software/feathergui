// Copyright ©2012 Black Sphere Studios
// For conditions of distribution and use, see copyright notice in "feathergui.h"

#include "fgWindow.h"

void FG_FASTCALL fgStatic_Init(fgStatic* self)
{ 
  assert(self!=0); 
  fgChild_Init(&self->element,0); 
  self->message=&fgStatic_Message; 
  self->element.destroy=&fgStatic_Destroy; 
  self->parent=0;
}
void FG_FASTCALL fgStatic_Destroy(fgStatic* self)
{
  assert(self!=0);
  fgStatic_RemoveParent(self);
  
  while(self->element.root) 
    (self->element.root->destroy)(self->element.root);
  
  FreeStatic(self);
}
void FG_FASTCALL fgStatic_RemoveParent(fgStatic* self)
{
  if(self->parent!=0 && (&self->parent->element)==self->element.parent)
    LList_Remove(&self->element, (fgChild**)&self->parent->rlist); // Remove ourselves from our window parent 
  else if(self->element.parent!=0) // Otherwise remove ourselves from our static parent, which has a different root
    LList_Remove(&self->element,&self->element.parent->root);
}
void FG_FASTCALL fgStatic_Message(fgStatic* self, unsigned char type, void* arg)
{
  assert(self!=0);

  switch(type)
  {
  case FG_RADDCHILD:
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
      fgWindow_VoidMessage(self->parent,FG_ADDRENDERABLE,self);
    else if(self->element.root->parent!=0)
      fgChild_SetParent(&self->element,self->element.root->parent);
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

void FG_FASTCALL SetSkinArray(fgStatic** pp, unsigned char size, unsigned char index)
{
  unsigned char i;
  for(i = 0; i < size; ++i)
    if(pp[i])
      (*pp[i]->message)(pp[i],FG_RSHOW,0);
  
  if(pp[index])
    (*pp[index]->message)(pp[index],FG_RSHOW,(void*)1);
}

