// Copyright ©2012 Black Sphere Studios
// For conditions of distribution and use, see copyright notice in "feathergui.h"

#ifndef __FG_SCROLLBAR_H__
#define __FG_SCROLLBAR_H__

#include "fgWindow.h"
#include "fgButton.h"

#ifdef  __cplusplus
extern "C" {
#endif

// A Scrollbar area acts as a clipping area that has optional scrollbars to navigate the internal area.
typedef struct {
  fgWindow window;
  fgWindow area; // this is an internal window that actually contains all the children, so we can move them around efficiently.
  fgButton* btn[6]; // 0 - up arrow, 1 - down arrow, 2 - vertical slider, 3 - right arrow, 4 - left arrow, 6 - horz slider
  fgStatic* skin[2]; // 0 - vertical background, 1 - horizontal background
} fgScrollbar;

FG_EXTERN void FG_FASTCALL fgScrollbar_Init(fgScrollbar* self, fgWindow* parent, const fgElement* element, FG_UINT id, fgFlag flags);
FG_EXTERN char FG_FASTCALL fgScrollbar_Message(fgScrollbar* self, const FG_Msg* msg);

#ifdef  __cplusplus
}
#endif

#endif