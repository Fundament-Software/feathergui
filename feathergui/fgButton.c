// Copyright ©2012 Black Sphere Studios
// For conditions of distribution and use, see copyright notice in "feathergui.h"

#include "fgButton.h"

void FG_FASTCALL fgButton_Init(fgButton* self, Renderable* item)
{
  Window_Init(&self->window,0);
}
void FG_FASTCALL fgButton_Message(fgButton* self, FG_Msg* msg)
{
  switch(msg->type)
  {
  case FG_ADDRENDERABLE:
    if(msg->otheraux<4)
    {
      if(self->skin[msg->otheraux]!=0)
        Window_VoidMessage(&self->window,FG_REMOVERENDERABLE,self->skin[msg->otheraux]);
      self->skin[msg->otheraux]=(Renderable*)msg->other;
    }
    break;
  }
  Window_Message(&self->window,msg);
}