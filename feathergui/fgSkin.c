// Copyright ©2013 Black Sphere Studios
// For conditions of distribution and use, see copyright notice in "feathergui.h"

#include "fgSkin.h"
#include "fgRoot.h"

void FG_FASTCALL fgSkinSet_Init(fgSkinSet* self)
{
  self->hash=kh_init(skin);
  self->strhash=kh_init(strskin);
}
void FG_FASTCALL fgSkinSet_Destroy(fgSkinSet* self)
{
  fgSkin* hold;
  fgSkin* cur=self->root;
  kh_destroy(skin,self->hash);
  kh_destroy(strskin,self->strhash);
  while(cur=self->root)
  {
    hold=cur;
    cur=cur->next;
    fgSkin_Destroy(hold);
    free(hold);
  }
}
void FG_FASTCALL fgSkin_Destroy(fgSkin* self)
{
  fgStatic* cur;
  while(cur=self->root)
  {
    self->root=(fgStatic*)cur->element.next;
    VirtualFreeChild((fgChild*)cur);
  }
}

void FG_FASTCALL fgSkinSet_Add(fgSkinSet* self, khint32_t id, const char* name)
{
  int r;
  fgSkin* pskin=(fgSkin*)malloc(sizeof(fgSkin*));
  khiter_t iter = kh_put(skin,self->hash,id,&r);
  khiter_t striter = kh_put(strskin,self->strhash,name,&r);
  kh_value(self->hash, iter) = pskin;
  kh_value(self->strhash, striter) = pskin;
  pskin->root=0;
  pskin->last=0;
  pskin->next=self->root;
  self->root=pskin;
}
fgSkin* FG_FASTCALL fgSkinSet_Get(fgSkinSet* self, khint32_t id)
{
  khiter_t iter = kh_get(skin,self->hash,id);
  if(iter==kh_end(self->hash)) return 0;
  return kh_value(self->hash,iter);
}
fgSkin* FG_FASTCALL fgSkinSet_GetStr(fgSkinSet* self, const char* name)
{
  khiter_t iter = kh_get(strskin,self->strhash,name);
  if(iter==kh_end(self->strhash)) return 0;
  return kh_value(self->strhash,iter);
}

void FG_FASTCALL fgSkin_Apply(fgSkin* self, fgWindow* target)
{
  fgStatic* cur=self->root;
  FG_Msg msg;
  msg.type=FG_ADDSTATIC;
  while(cur)
  {
    msg.other=(*cur->clone)(cur);
    msg.otheraux=cur->element.order;
    fgRoot_CallBehavior(target,&msg);
    cur=(fgStatic*)cur->element.next;
  }
}
void FG_FASTCALL fgSkin_Add(fgSkin* self, fgStatic* stat)
{
  LList_Add((fgChild*)stat,(fgChild**)&self->root,(fgChild**)&self->last);
  stat->parent=0;
  stat->element.parent=0;
}