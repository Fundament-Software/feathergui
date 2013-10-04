// Copyright ©2012 Black Sphere Studios
// For conditions of distribution and use, see copyright notice in "feathergui.h"

#ifndef __FG_SKIN_H__
#define __FG_SKIN_H__

#include "fgWindow.h"

#ifdef  __cplusplus
extern "C" {
#endif

struct __kh_skin_t;

FG_EXTERN struct __kh_skin_t* FG_FASTCALL fgSkin_Create();
FG_EXTERN void FG_FASTCALL fgSkin_Destroy(struct __kh_skin_t* self);
FG_EXTERN void FG_FASTCALL fgSkin_Insert(struct __kh_skin_t* self, struct FG_WINDOWSKIN* skin, const char* id);
FG_EXTERN struct FG_WINDOWSKIN* FG_FASTCALL fgSkin_Get(struct __kh_skin_t* self, const char* id);
FG_EXTERN struct FG_WINDOWSKIN* FG_FASTCALL fgSkin_Remove(struct __kh_skin_t* self, const char* id);
FG_EXTERN char FG_FASTCALL fgSkin_Apply(struct __kh_skin_t* self, fgWindow* p);

#ifdef  __cplusplus
}
#endif

#endif