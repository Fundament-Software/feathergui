// Copyright ©2012 Black Sphere Studios
// For conditions of distribution and use, see copyright notice in "feathergui.h"

#include "fgWinAPI.h"
#include "win32_includes.h"

extern WinAPIfgRoot* _fgroot;

fgStatic* FG_FASTCALL fgLoadImage(const char* path)
{
  return 0;
}
fgStatic* FG_FASTCALL fgLoadImageData(const void* data, size_t length)
{
  return 0;
}

void FG_FASTCALL WinAPIfgImage_Destroy(WinAPIfgImage* self)
{
  switch(self->st.render.flags&0x18) // We store what type it is here
  {
  case 8:
    DestroyCursor(self->imghandle);
    break;
  case 16:
    DestroyIcon(self->imghandle);
    break;
  default:
    DeleteObject(self->imghandle);
    break;
  }
  fgStatic_Destroy((fgStatic*)self);
}
