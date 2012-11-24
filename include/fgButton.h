// Copyright ©2012 Black Sphere Studios
// For conditions of distribution and use, see copyright notice in "feathergui.h"

#ifndef __FG_BUTTON_H__
#define __FG_BUTTON_H__

#include "feathergui.h"

// A button is usually implemented as a simple background design, plus a centering GUI component that takes a renderable and
// displays it in the center of the button. However, this is abstracted out, because most systems have built-in button widgets.
typedef struct {
  Window window;
} fgButton;

extern void __fastcall fgButton_Init(fgButton* self, Renderable* item);
extern void __fastcall fgButton_Message(fgButton* self, FG_Msg* msg);

#endif