// Copyright ©2012 Black Sphere Studios
// For conditions of distribution and use, see copyright notice in "feathergui.h"

#ifndef __FG_TOPWINDOW_H__
#define __FG_TOPWINDOW_H__

#include "fgContainer.h"

// A top-level window is an actual window with a titlebar that can be dragged and resized.
typedef struct {
  fgContainer window;
} fgTopWindow;

extern void FG_FASTCALL fgTopWindow_Create();
extern void FG_FASTCALL fgTopWindow_Init(fgTopWindow* self);
extern void FG_FASTCALL fgTopWindow_Destroy(fgTopWindow* self);


#endif