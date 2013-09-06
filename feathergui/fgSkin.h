// Copyright ©2012 Black Sphere Studios
// For conditions of distribution and use, see copyright notice in "feathergui.h"

#ifndef __FG_SKIN_H__
#define __FG_SKIN_H__

#include "fgWindow.h"
#include "khash.h"

#ifdef  __cplusplus
extern "C" {
#endif
  /*
KHASH_MAP_INIT_INT(skin, struct FG_WINDOWSKIN*);
KHASH_MAP_INIT_STRINS(strskin, struct FG_WINDOWSKIN*);

typedef struct {
  struct FG_WINDOWSKIN* root;
  kh_skin_t* hash;
  kh_strskin_t* strhash;
} fgSkinSet;

FG_EXTERN void FG_FASTCALL fgSkinSet_Init(fgSkinSet* self);
FG_EXTERN void FG_FASTCALL fgSkinSet_Destroy(fgSkinSet* self);
FG_EXTERN void FG_FASTCALL fgSkinSet_Add(fgSkinSet* self, khint32_t id, const char* name);
FG_EXTERN struct FG_WINDOWSKIN* FG_FASTCALL fgSkinSet_Get(fgSkinSet* self, khint32_t id);
FG_EXTERN struct FG_WINDOWSKIN* FG_FASTCALL fgSkinSet_GetStr(fgSkinSet* self, const char* name);

FG_EXTERN void FG_FASTCALL fgSkin_Add(struct FG_WINDOWSKIN* self, fgStatic* stat);
FG_EXTERN void FG_FASTCALL fgSkin_Destroy(struct FG_WINDOWSKIN* self);*/

#ifdef  __cplusplus
}
#endif

#endif