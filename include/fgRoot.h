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
  fgChild gui;
  char (FG_FASTCALL *keymsghook)(FG_Msg* msg);
  char (FG_FASTCALL *behaviorhook)(struct __WINDOW* self, FG_Msg* msg);
} fgRoot;

FG_EXTERN fgRoot* FG_FASTCALL fgInitialize();
FG_EXTERN void FG_FASTCALL fgTerminate(fgRoot* root);
FG_EXTERN void FG_FASTCALL fgRoot_Inject(fgRoot* self, FG_Msg* msg);
FG_EXTERN char FG_FASTCALL fgRoot_BehaviorDefault(fgWindow* self, FG_Msg* msg);
FG_EXTERN void FG_FASTCALL fgRoot_Render(fgRoot* self);

#ifdef  __cplusplus
}
#endif

#endif