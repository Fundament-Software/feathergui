// Copyright ©2012 Black Sphere Studios
// For conditions of distribution and use, see copyright notice in "feathergui.h"

#ifndef __FG_TEXTBOX_H__
#define __FG_TEXTBOX_H__

#include "fgScrollbar.h"

#ifdef  __cplusplus
extern "C" {
#endif

// A Textbox is really just a text static inside an optional Scrollbar. It can be single or multi-line.
typedef struct {
  fgScrollbar window; // Each tab is a button added as a non-region child to the container
  fgStatic* text;
  fgStatic* skin[2]; // 0 - background, 1 - cursor
} fgTextbox;

FG_EXTERN fgTextbox* FG_FASTCALL fgTextbox_Create();
FG_EXTERN void FG_FASTCALL fgTextbox_Init(fgTextbox* self);
FG_EXTERN char FG_FASTCALL fgTextbox_Message(fgTextbox* self, const FG_Msg* msg);

#ifdef  __cplusplus
}
#endif

#endif