// Copyright ©2012 Black Sphere Studios
// For conditions of distribution and use, see copyright notice in "feathergui.h"

#include "fgButton.h"

void FG_FASTCALL fgButton_Init(fgButton* self)
{
  assert(self!=0);
  fgWindow_Init(&self->window,0);
  self->window.message=&fgButton_Message;
}
char FG_FASTCALL fgButton_Message(fgButton* self, const FG_Msg* msg)
{
  assert(self!=0 && msg!=0);
  switch(msg->type)
  {
  case FG_MOUSEON:
    SetSkinArray(self->skin,4,2);
    break;
  case FG_MOUSEUP:
    if(fgFocusedWindow==(fgWindow*)self && MsgHitCRect(msg,&self->window.element)) // Does this happen while we have focus AND the event is inside our control?
      fgWindow_BasicMessage((fgWindow*)self,FG_CLICKED); // Fire off a message
  case FG_MOUSEOFF: // FG_MOUSEUP falls through
    SetSkinArray(self->skin,4,1);
    break;
  case FG_MOUSEDOWN:
    SetSkinArray(self->skin,4,3);
    break;
  case FG_ADDSTATIC:
    if(msg->otheraux<4)
      DoSkinCheck((fgWindow*)self,self->skin,msg);
    break;
  }
  return fgWindow_Message(&self->window,msg);
}