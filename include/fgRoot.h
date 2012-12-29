// Copyright ©2012 Black Sphere Studios
// For conditions of distribution and use, see copyright notice in "feathergui.h"

#ifndef __FG_ROOT_H__
#define __FG_ROOT_H__

#include "fgWindow.h"

#ifdef  __cplusplus
extern "C" {
#endif

// Defines the root interface to the GUI. This object should be returned by the implementation at some point
typedef struct {
  fgWindow gui;
  void (FG_FASTCALL *winrender)(fgWindow* self, AbsRect* area);
  char (FG_FASTCALL *keymsghook)(const FG_Msg* msg);
  char (FG_FASTCALL *behaviorhook)(struct __WINDOW* self, const FG_Msg* msg);
  fgWindow mouse; // Add children to this to have them follow the mouse around.
} fgRoot;

FG_EXTERN fgRoot* FG_FASTCALL fgInitialize();
FG_EXTERN void FG_FASTCALL fgTerminate(fgRoot* root);
FG_EXTERN void FG_FASTCALL fgRoot_Inject(fgRoot* self, const FG_Msg* msg);
FG_EXTERN char FG_FASTCALL fgRoot_BehaviorDefault(fgWindow* self, const FG_Msg* msg);
FG_EXTERN void FG_FASTCALL fgRoot_Render(fgRoot* self);
FG_EXTERN void FG_FASTCALL fgRoot_RListRender(fgStatic* self, AbsRect* area);
FG_EXTERN void FG_FASTCALL fgRoot_WinRender(fgWindow* self, AbsRect* area);

#ifdef  __cplusplus
}
#endif

#endif
