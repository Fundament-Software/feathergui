// Copyright ©2012 Black Sphere Studios
// For conditions of distribution and use, see copyright notice in "feathergui.h"

#ifndef __FG_TOPWINDOW_H__
#define __FG_TOPWINDOW_H__

#include "fgContainer.h"
#include "fgRoot.h"

#ifdef  __cplusplus
extern "C" {
#endif

// A top-level window is an actual window with a titlebar that can be dragged and resized.
typedef struct {
  fgContainer window;
} fgTopWindow;

FG_EXTERN void FG_FASTCALL fgTopfgWindow_Create(fgRoot* root);
FG_EXTERN void FG_FASTCALL fgTopfgWindow_Init(fgTopWindow* self);
FG_EXTERN void FG_FASTCALL fgTopfgWindow_Destroy(fgTopWindow* self);
FG_EXTERN void FG_FASTCALL fgTopfgWindow_Message(fgWindow* self, FG_Msg* msg);

#ifdef  __cplusplus
}
#endif

#endif