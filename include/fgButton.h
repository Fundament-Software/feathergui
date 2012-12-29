// Copyright ©2012 Black Sphere Studios
// For conditions of distribution and use, see copyright notice in "feathergui.h"

#ifndef __FG_BUTTON_H__
#define __FG_BUTTON_H__

#include "fgWindow.h"

#ifdef  __cplusplus
extern "C" {
#endif

// A button is usually implemented as a simple background design, plus a centering GUI component that takes a static and
// displays it in the center of the button.
typedef struct _FG_BUTTON {
  fgWindow window;
  fgStatic* skin[4]; // index 0 is the item, 1 is the skin, 2 is hover, 3 is active
} fgButton;

FG_EXTERN fgButton* FG_FASTCALL fgButton_Create(fgStatic* item);
FG_EXTERN void FG_FASTCALL fgButton_Init(fgButton* self);
FG_EXTERN char FG_FASTCALL fgButton_Message(fgWindow* self, const FG_Msg* msg);

#ifdef  __cplusplus
}
#endif

#endif
