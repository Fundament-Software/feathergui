// Copyright ©2012 Black Sphere Studios
// FgNull - A null-implementation of Feather GUI
// For conditions of distribution and use, see copyright notice in "feathergui.h"

#ifndef __FG_NULL_H__
#define __FG_NULL_H__

#include "fgStatic.h"
#include "fgRoot.h"
#include "fgButton.h"
#include "fgMenu.h"
#include "fgTopWindow.h"

#ifdef  __cplusplus
extern "C" {
#endif

FG_EXTERN fgRoot* FG_FASTCALL fgInitialize();
FG_EXTERN void FG_FASTCALL fgTerminate(fgRoot* root);

FG_EXTERN void FG_FASTCALL fgButton_Create(fgStatic* item);
FG_EXTERN void FG_FASTCALL fgMenu_Create();
FG_EXTERN void FG_FASTCALL fgTopWindow_Create(fgRoot* root);

#ifdef  __cplusplus
}
#endif

#endif