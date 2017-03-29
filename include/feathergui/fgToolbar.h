// Copyright �2017 Black Sphere Studios
// For conditions of distribution and use, see copyright notice in "feathergui.h"

#ifndef __FG_TOOLBAR_H__
#define __FG_TOOLBAR_H__

#include "fgList.h"

#ifdef  __cplusplus
extern "C" {
#endif

enum FGTOOLBAR_FLAGS
{
  FGTOOLBAR_LOCKED = (FGCONTROL_DISABLE << 1), // Locks the toolgroups so they can't be moved or re-arranged.
};

// A toolbar is simply a list of lists containing buttons or other controls.
typedef struct _FG_TOOLBAR {
  fgList list;
  fgElement seperator; // cloned to replace a null value inserted into the list. This allows a style to control the size and appearence of the seperator.
#ifdef  __cplusplus
  inline operator fgElement*() { return &list.box.scroll.control.element; }
  inline fgElement* operator->() { return operator fgElement*(); }
#endif
} fgToolbar;

FG_EXTERN void fgToolbar_Init(fgToolbar* BSS_RESTRICT self, fgElement* BSS_RESTRICT parent, fgElement* BSS_RESTRICT next, const char* name, fgFlag flags, const fgTransform* transform, unsigned short units);
FG_EXTERN void fgToolbar_Destroy(fgToolbar* self);
FG_EXTERN size_t fgToolbar_Message(fgToolbar* self, const FG_Msg* msg);

FG_EXTERN size_t fgToolGroup_Message(fgList* self, const FG_Msg* msg);

#ifdef  __cplusplus
}
#endif

#endif