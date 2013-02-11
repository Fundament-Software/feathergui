// Copyright ©2013 Black Sphere Studios
// For conditions of distribution and use, see copyright notice in "feathergui.h"

#ifndef __FG_CONTAINER_H__
#define __FG_CONTAINER_H__

#include "fgWindow.h"

#ifdef  __cplusplus
extern "C" {
#endif

// A container is a collection of fgChild objects that divide a window into areas.
typedef struct {
  fgWindow window;
  fgWindow* regions;
  fgWindow* regionslast;
} fgContainer;

FG_EXTERN void FG_FASTCALL fgContainer_Init(fgContainer* self, fgWindow* parent, const fgElement* element, FG_UINT id, fgFlag flags);
FG_EXTERN void FG_FASTCALL fgContainer_Destroy(fgContainer* self);
FG_EXTERN fgWindow* FG_FASTCALL fgContainer_AddRegion(fgContainer* self, fgElement* region);
FG_EXTERN void FG_FASTCALL fgContainer_RemoveRegion(fgContainer* self, fgWindow* region);

#ifdef  __cplusplus
}
#endif

#endif
