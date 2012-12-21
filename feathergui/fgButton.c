// Copyright ©2012 Black Sphere Studios
// For conditions of distribution and use, see copyright notice in "feathergui.h"

#include "fgButton.h"

void FG_FASTCALL fgButton_Init(fgButton* self, fgStatic* item)
{
  fgWindow_Init(&self->window,0);
}
char FG_FASTCALL fgButton_Message(fgButton* self, FG_Msg* msg)
{
  switch(msg->type)
  {
  case FG_MOUSEON:
    SetSkinArray(self->skin,4,2);
    break;
  case FG_MOUSEUP:
    if(fgFocusedWindow==(fgWindow*)self && MsgHitCRect(msg,&self->window.element)) // Does this happen while we have focus AND the event is inside our control?
      fgWindow_BasicMessage((fgWindow*)self,FG_CLICKED); // Fire off a message
  case FG_MOUSEOFF:
    SetSkinArray(self->skin,4,1);
    break;
  case FG_MOUSEDOWN:
    SetSkinArray(self->skin,4,3);
    break;
  case FG_ADDRENDERABLE:
    if(msg->otheraux<4)
    {
      if(self->skin[msg->otheraux]!=0)
        fgWindow_VoidMessage(&self->window,FG_REMOVERENDERABLE,self->skin[msg->otheraux]);
      self->skin[msg->otheraux]=(fgStatic*)msg->other;
    }
    break;
  }
  return fgWindow_Message(&self->window,msg);
}