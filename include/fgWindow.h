// Copyright ©2012 Black Sphere Studios
// For conditions of distribution and use, see copyright notice in "feathergui.h"

#ifndef __FG_WINDOW_H__
#define __FG_WINDOW_H__

#include "fgRenderable.h"

// Defines the base GUI element, a window. This is not an actual top level window.
typedef struct __WINDOW {
  Child element;
  void (FG_FASTCALL *message)(struct __WINDOW* self, FG_Msg* msg);
  FG_UINT id;
  FG_UINT order; // order relative to other windows
  unsigned char flags; // 1 is x-axis centering, 2 is y-axis, 3 is both, 4 is clipping disabled, 8 is not visible
  Renderable* rlist;
  struct __WINDOW* contextmenu;
} Window;

extern Window* fgFocusedWindow;
extern void (FG_FASTCALL *behaviorhook)(struct __WINDOW* self, FG_Msg* msg);

extern void FG_FASTCALL Window_Init(Window* BSS_RESTRICT self, Child* BSS_RESTRICT parent);
extern void FG_FASTCALL Window_Destroy(Window* self);
extern void FG_FASTCALL Window_Message(Window* self, FG_Msg* msg);
extern void FG_FASTCALL Window_SetElement(Window* self, Element* element);
extern void FG_FASTCALL Window_SetArea(Window* self, CRect* area);
extern void FG_FASTCALL Window_BasicMessage(Window* self, unsigned char type); // Shortcut for sending type messages with no data
extern void FG_FASTCALL Window_VoidMessage(Window* self, unsigned char type, void* data); // Shortcut for sending void* messages
extern void FG_FASTCALL Window_IntMessage(Window* self, unsigned char type, int data); // Shortcut for sending int messages


#endif