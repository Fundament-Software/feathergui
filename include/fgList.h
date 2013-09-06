// Copyright ©2012 Black Sphere Studios
// For conditions of distribution and use, see copyright notice in "feathergui.h"

#ifndef __FG_LIST_H__
#define __FG_LIST_H__

#include "fgGrid.h"
#include "fgScrollbar.h"

#ifdef  __cplusplus
extern "C" {
#endif

// A List is a list of items that can be sorted into any number of columns and optionally have column headers.
typedef struct {
  fgGrid window; // Buttons are directly added as children to the list.
  fgScrollbar list; // Actual list scrollbar area. Contains one or more fgGrids and nothing else.
  fgStatic* selected; // Stores current selected item.
  fgStatic* selector;
  fgStatic* highlighter;
} fgList;

struct FG_LISTSKIN {
  struct FG_GRIDSKIN base;
  fgStatic* selected; // Stores current selected item.
  fgStatic* selector;
  fgStatic* highlighter;
};

FG_EXTERN fgWindow* FG_FASTCALL fgList_Create(fgWindow* parent, const fgElement* element, FG_UINT id, fgFlag flags);
FG_EXTERN void FG_FASTCALL fgList_Init(fgList* self, fgWindow* parent, const fgElement* element, FG_UINT id, fgFlag flags);
FG_EXTERN char FG_FASTCALL fgList_Message(fgList* self, const FG_Msg* msg);

#ifdef  __cplusplus
}
#endif

#endif