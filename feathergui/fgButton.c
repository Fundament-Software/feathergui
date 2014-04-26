// Copyright ©2013 Black Sphere Studios
// For conditions of distribution and use, see copyright notice in "feathergui.h"

#include "fgButton.h"

void FG_FASTCALL fgButton_Init(fgButton* self, fgWindow* parent, const fgElement* element, FG_UINT id, fgFlag flags)
{
  assert(self!=0);
  memset(self,0,sizeof(fgButton));
  fgWindow_Init(&self->window,parent,element,id,flags);
  self->window.message=&fgButton_Message;
}
char FG_FASTCALL fgButton_Message(fgButton* self, const FG_Msg* msg)
{
  static fgElement defelem = { 0,0,0,0,0,0,0,0,0,0,0.5f,0,0.5f };

  assert(self!=0 && msg!=0);
  switch(msg->type)
  {
  case FG_SETCAPTION:
    if(!self->item) {
      self->item=fgLoadDefaultText(msg->other);
      fgWindow_VoidMessage((fgWindow*)self,FG_ADDCHILD,self->item);
    }
    else
      (*self->item->message)(self->item,FG_RSETTEXT,msg->other,0);
    return 0;
  case FG_ADDITEM:
    if(self->item)
      VirtualFreeChild((fgChild*)self->item);
    self->item=fgLoadDef(msg->other,!self->window.skin?&defelem:&((struct FG_BUTTONSKIN*)self->window.skin)->item,1);
    fgWindow_VoidMessage((fgWindow*)self,FG_ADDCHILD,self->item);
    return 0;
  case FG_NUETRAL:
    STATIC_SET_ENABLE3(0,neutral,hover,active);
    return 0;
  case FG_HOVER:
    STATIC_SET_ENABLE3(1,neutral,hover,active);
    return 0;
  case FG_ACTIVE:
    STATIC_SET_ENABLE3(2,neutral,hover,active);
    return 0;
  case FG_GETCLASSNAME:
    (*(const char**)msg->other) = "fgButton";
    return 0;
  }
  return fgWindow_HoverProcess(&self->window,msg);
}