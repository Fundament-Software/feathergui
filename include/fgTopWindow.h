// Copyright ©2013 Black Sphere Studios
// For conditions of distribution and use, see copyright notice in "feathergui.h"

#ifndef __FG_TOPWINDOW_H__
#define __FG_TOPWINDOW_H__

#include "fgWindow.h"
#include "fgRoot.h"

#ifdef  __cplusplus
extern "C" {
#endif

struct _FG_BUTTON;

enum FGTOPWINDOW_MSGTYPE {
  FGTOPWINDOW_SETMARGIN=FG_CUSTOMEVENT,
  FGTOPWINDOW_SETCAPTION,
};

// A top-level window is an actual window with a titlebar that can be dragged and resized.
typedef struct {
  fgWindow window;
  fgWindow region;
  struct _FG_BUTTON* controls[3]; // 0 is the close button, 1 is the maximize/restore button, 2 is the minimize button
  AbsRect prevrect; // Stores where the window was before being maximized
} fgTopWindow;

FG_EXTERN fgWindow* FG_FASTCALL fgTopWindow_Create(const char* caption, const fgElement* element, FG_UINT id, fgFlag flags);
FG_EXTERN void FG_FASTCALL fgTopWindow_Init(fgTopWindow* self, const fgElement* element, FG_UINT id, fgFlag flags);
FG_EXTERN void FG_FASTCALL fgTopWindow_Destroy(fgTopWindow* self);
FG_EXTERN char FG_FASTCALL fgTopWindow_Message(fgTopWindow* self, const FG_Msg* msg);

#ifdef  __cplusplus
}
#endif

#endif
