// Copyright ©2012 Black Sphere Studios
// For conditions of distribution and use, see copyright notice in "feathergui.h"

#ifndef __FG_CONTAINER_H__
#define __FG_CONTAINER_H__

#include "feathergui.h"

// A container is a collection of Child objects that divide a window into areas.
typedef struct {
  Window window;
  Child* regions;
} fgContainer;

extern void __fastcall fgContainer_Create();
extern void __fastcall fgContainer_Init(fgContainer* self);
extern void __fastcall fgContainer_Destroy(fgContainer* self);
extern Child* __fastcall fgContainer_AddRegion(fgContainer* self, Element* region);
extern void __fastcall fgContainer_RemoveRegion(fgContainer* self, Child* region);

#endif