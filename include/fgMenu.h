// Copyright ©2012 Black Sphere Studios
// For conditions of distribution and use, see copyright notice in "feathergui.h"

#ifndef __FG_MENU_H__
#define __FG_MENU_H__

#include "fgGrid.h"

#ifdef  __cplusplus
extern "C" {
#endif

struct FG_MENUITEM;

// A Menu is either a window menu or a context menu. Turns into a menubar if made the child of a top-level window
typedef struct {
  fgGrid grid;
  struct FG_MENUITEM* items; // Linked list of menu items
  struct FG_MENUITEM* expanded; // Stores the current item that has an expanded submenu, if any exist.
  fgStatic* skin[4]; // index 0 is the background to the menu, 1 is the highlighter, 2 is the seperator, 3 is the submenu arrow
  long long prevtime;
} fgMenu;

struct FG_MENUITEM {
  fgStatic* render; // Pointer to the renderable containing the 3 or 4 elements of a menu item.
  fgMenu* submenu;
  struct FG_MENUITEM* next;
  struct FG_MENUITEM* prev;
};

FG_EXTERN fgMenu* FG_FASTCALL fgMenu_Create();
FG_EXTERN void FG_FASTCALL fgMenu_Init(fgMenu* self);
FG_EXTERN char FG_FASTCALL fgMenu_Message(fgWindow* self, const FG_Msg* msg);

#ifdef  __cplusplus
}
#endif

#endif
