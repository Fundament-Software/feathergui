// Copyright ©2013 Black Sphere Studios
// For conditions of distribution and use, see copyright notice in "feathergui.h"

#include "fgSkin.h"
#include "khash.h" // Note: minor change made in khash.h to define __kh_##name##_t so the struct isn't unnamed

KHASH_MAP_INIT_STRINS(skin, struct FG_WINDOWSKIN*);

FG_EXTERN kh_skin_t* FG_FASTCALL fgSkin_Create()
{
  return kh_init_skin();
}
FG_EXTERN void FG_FASTCALL fgSkin_Destroy(kh_skin_t* self)
{
  kh_destroy_skin(self);
}

FG_EXTERN void FG_FASTCALL fgSkin_Insert(kh_skin_t* self, struct FG_WINDOWSKIN* skin, const char* id)
{
  int ret;
  kh_put_skin(self,id,&ret);
  kh_val(self,kh_get_skin(self,id))=skin;
}
FG_EXTERN struct FG_WINDOWSKIN* FG_FASTCALL fgSkin_Get(kh_skin_t* self, const char* id)
{
  khint_t i;
  if(!self || !id) return 0;
  i = kh_get_skin(self,id);
  if(i == kh_end(self)) return 0;
  return kh_val(self,i);
}
FG_EXTERN struct FG_WINDOWSKIN* FG_FASTCALL fgSkin_Remove(kh_skin_t* self, const char* id)
{
  struct FG_WINDOWSKIN* r;
  khint_t i;
  if(!self || !id) return 0;
  i = kh_get_skin(self,id);
  if(i==kh_end(self)) return 0;
  r = kh_val(self,i);
  kh_del_skin(self,i);
  return r;
}
FG_EXTERN char FG_FASTCALL fgSkin_Apply(kh_skin_t* self, fgWindow* p)
{
  const char* name=0;
  if(!self || !p) return 1;
  fgWindow_VoidMessage(p,FG_GETCLASSNAME,&name);
  if(!name) return 1;
  return fgWindow_VoidMessage(p,FG_APPLYSKIN,fgSkin_Get(self,name));
}