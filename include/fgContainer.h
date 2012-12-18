// Copyright ©2012 Black Sphere Studios
// For conditions of distribution and use, see copyright notice in "feathergui.h"

#ifndef __FG_CONTAINER_H__
#define __FG_CONTAINER_H__

#include "fgWindow.h"

// A container is a collection of Child objects that divide a window into areas.
typedef struct {
  Window window;
  Child* regions;
} fgContainer;

extern void FG_FASTCALL fgContainer_Create();
extern void FG_FASTCALL fgContainer_Init(fgContainer* self);
extern void FG_FASTCALL fgContainer_Destroy(fgContainer* self);
extern Child* FG_FASTCALL fgContainer_AddRegion(fgContainer* self, Element* region);
extern void FG_FASTCALL fgContainer_RemoveRegion(fgContainer* self, Child* region);

#endif