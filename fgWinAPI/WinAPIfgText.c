// Copyright ©2012 Black Sphere Studios
// For conditions of distribution and use, see copyright notice in "feathergui.h"

#include "fgWinAPI.h"
#include "win32_includes.h"

extern WinAPIfgRoot* _fgroot;

void FG_FASTCALL WinAPIfgText_Destroy(WinAPIfgText* self)
{
  if(self->text) free(self->text);
  fgStatic_Destroy((fgStatic*)self);
}

void FG_FASTCALL WinAPIfgText_Message(fgStatic* self, unsigned char type, void* arg)
{
  size_t len;
  char* str;
  WinAPIfgText* s=(WinAPIfgText*)self;
  assert(self!=0);

  switch(type)
  {
  case FG_RSETTEXT:
    if(s->text) free(s->text);
    len=strlen((const char*)arg)+1;
    str=malloc(len);
    strncpy(str,(const char*)arg,len);
    s->text=str;
    break;
  case FG_RCLONE:
    fgStatic_Clone((fgStatic*)arg,self); // this copies over the destroy function
    ((WinAPIfgText*)arg)->st.handle=0; //do handle duplication
    len=strlen(s->text)+1;
    str=malloc(len);
    strncpy(str,s->text,len);
    ((WinAPIfgText*)arg)->text=str;
    return;
  }

  fgStatic_Message(self,type,arg);
}

fgStatic* FG_FASTCALL fgLoadText(const char* text, unsigned int flags)
{
  size_t len=strlen(text)+1;
  char* str=(char*)malloc(len);
  WinAPIfgText* r = (WinAPIfgText*)malloc(sizeof(WinAPIfgText));
  fgStatic_Init((fgStatic*)r);
  r->st.render.flags=flags;
  r->st.render.element.destroy=WinAPIfgText_Destroy;
  strncpy(str,text,len);
  r->text=str;
  return (fgStatic*)r;
}