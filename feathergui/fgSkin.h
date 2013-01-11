// Copyright ©2012 Black Sphere Studios
// For conditions of distribution and use, see copyright notice in "feathergui.h"

#ifndef __FG_SKIN_H__
#define __FG_SKIN_H__

#include "fgWindow.h"
#include "khash.h"

#ifdef  __cplusplus
extern "C" {
#endif
  
KHASH_MAP_INIT_INT(skin, fgSkin*);
KHASH_MAP_INIT_STRINS(strskin, fgSkin*);

// A skin is just a collection of fgStatics. These are applied to an fgWindow by calling AddStatic with increasing skin IDs.
typedef struct {
  fgStatic* root; // Holds all the fgStatic's that make up this skin
  fgStatic* last;
  fgSkin* next; // holds this skin's location in the skin set
  fgSkin* prev;
} fgSkin;

typedef struct {
  fgSkin* root;
  fgSkin* last;
  kh_skin_t* hash;
  kh_strskin_t* strhash;
} fgSkinSet;

FG_EXTERN void FG_FASTCALL fgSkinSet_Init(fgSkinSet* self);
FG_EXTERN void FG_FASTCALL fgSkinSet_Add(fgSkinSet* self, fgSkin* skin);
FG_EXTERN fgSkin* FG_FASTCALL fgSkinSet_Get(fgSkinSet* self, khint32_t id);
FG_EXTERN fgSkin* FG_FASTCALL fgSkinSet_GetStr(fgSkinSet* self, const char* name);

FG_EXTERN void FG_FASTCALL fgSkin_Apply(fgSkin* self, fgWindow* target);
FG_EXTERN void FG_FASTCALL fgSkin_Add(fgSkin* self, fgStatic* stat);

#ifdef  __cplusplus
}
#endif

#endif