// Copyright ©2012 Black Sphere Studios
// For conditions of distribution and use, see copyright notice in "feathergui.h"

#include "fgButton.h"

void __fastcall fgButton_Init(fgButton* self, Renderable* item)
{
  Window_Init(&self->window,0);
}
void __fastcall fgButton_Message(fgButton* self, FG_Msg* msg)
{
  Window_Message(&self->window,msg);
}