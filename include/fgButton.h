// Copyright ©2012 Black Sphere Studios
// For conditions of distribution and use, see copyright notice in "feathergui.h"

#ifndef __FG_BUTTON_H__
#define __FG_BUTTON_H__

#include "fgWindow.h"

// A button is usually implemented as a simple background design, plus a centering GUI component that takes a renderable and
// displays it in the center of the button.
typedef struct {
  Window window;
  Renderable* skin[4]; // index 0 is the item, 1 is the skin, 2 is hover, 3 is active
} fgButton;

extern void FG_FASTCALL fgButton_Create(Renderable* item);
extern void FG_FASTCALL fgButton_Init(fgButton* self, Renderable* item);
extern void FG_FASTCALL fgButton_Message(fgButton* self, FG_Msg* msg);

#endif