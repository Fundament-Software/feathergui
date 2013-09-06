// Copyright ©2013 Black Sphere Studios
// For conditions of distribution and use, see copyright notice in "feathergui.h"

#ifndef __FG_GRID_H__
#define __FG_GRID_H__

#include "fgWindow.h"

#ifdef  __cplusplus
extern "C" {
#endif

// A grid arranges statics according to a tiling and margin system.
typedef struct {
  fgWindow window; // Note that the window flags consider 16 to be tile on x-axis, 32 to tile on y-axis, 48 both.
  AbsVec margins;
  AbsRect padding;
} fgGrid;

struct FG_GRIDSKIN {
  struct FG_WINDOWSKIN base;
  AbsVec margins;
  AbsRect padding;
};

FG_EXTERN void FG_FASTCALL fgGrid_Init(fgGrid* self, fgWindow* parent, const fgElement* element, FG_UINT id, fgFlag flags);
FG_EXTERN char FG_FASTCALL fgGrid_Message(fgGrid* self, const FG_Msg* msg);
FG_EXTERN fgChild* FG_FASTCALL fgGrid_HitElement(fgGrid* self, FABS x, FABS y);
FG_EXTERN void FG_FASTCALL fgGrid_Reposition(fgGrid* self, fgChild* element);

#ifdef  __cplusplus
}
#endif

#endif
