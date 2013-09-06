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
  assert(self!=0 && msg!=0);
  switch(msg->type)
  {
  case FG_MOUSEON:
    STATIC_SET_ENABLE3(1,neutral,hover,active);
    break;
  case FG_MOUSEUP:
    if(fgFocusedWindow==(fgWindow*)self && MsgHitCRect(msg,&self->window.element)) // Does this happen while we have focus AND the event is inside our control?
      fgWindow_BasicMessage((fgWindow*)self,FG_CLICKED); // Fire off a message
  case FG_MOUSEOFF: // FG_MOUSEUP falls through
    STATIC_SET_ENABLE3(0,neutral,hover,active);
    break;
  case FG_MOUSEDOWN:
    STATIC_SET_ENABLE3(2,neutral,hover,active);
    break;
  }
  return fgWindow_Message(&self->window,msg);
}