// Copyright ©2012 Black Sphere Studios
// For conditions of distribution and use, see copyright notice in "feathergui.h"

#ifndef __FG_MENU_H__
#define __FG_MENU_H__

#include "fgGrid.h"

// A Menu is either a window menu or a context menu. Turns into a menubar if made the child of a top-level window
typedef struct {
  fgGrid grid;
} fgMenu;

extern void FG_FASTCALL fgMenu_Create();
extern void FG_FASTCALL fgMenu_Init(fgMenu* self);
extern void FG_FASTCALL fgMenu_Message(fgMenu* self, FG_Msg* msg);

#endif