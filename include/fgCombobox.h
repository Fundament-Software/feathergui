// Copyright ©2013 Black Sphere Studios
// For conditions of distribution and use, see copyright notice in "feathergui.h"

#ifndef __FG_COMBOBOX_H__
#define __FG_COMBOBOX_H__

#include "fgTextbox.h"
#include "fgButton.h"

#ifdef  __cplusplus
extern "C" {
#endif

// A combobox is a dropdown that can optionally let the user enter their own choice into the box.
typedef struct {
  fgTextbox box; // The textbox's context menu is the actual dropdown list, so elements are added to that (this is done automatically)
  fgButton button;
} fgCombobox;

FG_EXTERN fgCombobox* FG_FASTCALL fgCombobox_Create(fgTriplet* item);
FG_EXTERN void FG_FASTCALL fgCombobox_Init(fgCombobox* self);
FG_EXTERN char FG_FASTCALL fgCombobox_Message(fgCombobox* self, const FG_Msg* msg);

#ifdef  __cplusplus
}
#endif

#endif
