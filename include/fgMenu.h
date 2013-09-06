// Copyright ©2013 Black Sphere Studios
// For conditions of distribution and use, see copyright notice in "feathergui.h"

#ifndef __FG_MENU_H__
#define __FG_MENU_H__

#include "fgGrid.h"
#include "fgRoot.h"

#ifdef  __cplusplus
extern "C" {
#endif

// A Menu is either a window menu or a context menu. Turns into a menubar if made the child of a top-level window
typedef struct FG_MENU {
  fgWindow window; 
  fgGrid grid; // This grid is a child of this menu's window and contains 3 other grids for each column, plus one for submenus
  struct FG_MENU* expanded; // Stores the current item that has an expanded submenu, if any exist.
  fgStatic* highlight;
  fgStatic* seperator;
  fgStatic* arrow;
  fgDeferAction* dropdown; // Keeps track of our dropdown action in fgRoot
} fgMenu;

struct FG_MENUITEM {
  fgStatic render; // FG_MENUITEM is an empty static containing the 3 or 4 child statics that make up a menu item.
  fgMenu* submenu; // submenu's parent is the menu that this static is a child of.
}; // By default a menu will only render text, but FGMENU_IMAGES will render a column of images on the left side for items that have them. This is automatically enabled when an item with an image is added, but can be manually turned on via SETFLAG

struct FG_MENUSKIN {
  struct FG_GRIDSKIN base;
  fgStatic* highlight;
  fgStatic* seperator;
  fgStatic* arrow;
};

FG_EXTERN fgWindow* FG_FASTCALL fgMenu_Create(fgWindow* parent, const fgElement* element, FG_UINT id, fgFlag flags);
FG_EXTERN void FG_FASTCALL fgMenu_Init(fgMenu* self, fgWindow* parent, const fgElement* element, FG_UINT id, fgFlag flags);
FG_EXTERN void FG_FASTCALL fgMenu_Destroy(fgMenu* self);
FG_EXTERN char FG_FASTCALL fgMenu_Message(fgMenu* self, const FG_Msg* msg);
FG_EXTERN char FG_FASTCALL fgMenu_DoDropdown(fgMenu* self);

#ifdef  __cplusplus
}
#endif

#endif
