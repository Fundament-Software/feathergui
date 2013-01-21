// Copyright ©2012 Black Sphere Studios
// For conditions of distribution and use, see copyright notice in "feathergui.h"

#ifndef __FG_SKIN_H__
#define __FG_SKIN_H__

#include "fgWindow.h"
#include "khash.h"

#ifdef  __cplusplus
extern "C" {
#endif

// A skin is just a collection of fgStatics. These are applied to an fgWindow by calling AddStatic with increasing skin IDs.
typedef struct _FG_SKIN {
  fgStatic* root; // Holds all the fgStatic's that make up this skin. The order of the static is used ast the skin ID.
  fgStatic* last;
  struct _FG_SKIN* next; // holds this skin's location in the skin set
} fgSkin;

KHASH_MAP_INIT_INT(skin, fgSkin*);
KHASH_MAP_INIT_STRINS(strskin, fgSkin*);

typedef struct {
  fgSkin* root;
  kh_skin_t* hash;
  kh_strskin_t* strhash;
} fgSkinSet;

FG_EXTERN void FG_FASTCALL fgSkinSet_Init(fgSkinSet* self);
FG_EXTERN void FG_FASTCALL fgSkinSet_Destroy(fgSkinSet* self);
FG_EXTERN void FG_FASTCALL fgSkinSet_Add(fgSkinSet* self, khint32_t id, const char* name);
FG_EXTERN fgSkin* FG_FASTCALL fgSkinSet_Get(fgSkinSet* self, khint32_t id);
FG_EXTERN fgSkin* FG_FASTCALL fgSkinSet_GetStr(fgSkinSet* self, const char* name);

FG_EXTERN void FG_FASTCALL fgSkin_Apply(fgSkin* self, fgWindow* target);
FG_EXTERN void FG_FASTCALL fgSkin_Add(fgSkin* self, fgStatic* stat);
FG_EXTERN void FG_FASTCALL fgSkin_Destroy(fgSkin* self);

#ifdef  __cplusplus
}
#endif

#endif