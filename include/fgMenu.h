// Copyright ©2012 Black Sphere Studios
// For conditions of distribution and use, see copyright notice in "feathergui.h"

#ifndef __FG_MENU_H__
#define __FG_MENU_H__

#include "fgContainer.h"

// A Menu is either a window menu or a context menu. Turns into a menubar if made the child of a top-level window
typedef struct {
} fgMenu;

extern void __fastcall fgMenu_Create();
extern void __fastcall fgMenu_Init(fgMenu* self);
extern void __fastcall fgMenu_Destroy(fgMenu* self);

#endif