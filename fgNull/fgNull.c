// Copyright ©2012 Black Sphere Studios
// For conditions of distribution and use, see copyright notice in "feathergui.h"

#include "fgNull.h"
#include "bss_defines.h"

#if defined(BSS_DEBUG) && defined(BSS_CPU_x86_64)
#pragma comment(lib, "../bin/feathergui64_d.lib")
#elif defined(BSS_CPU_x86_64)
#pragma comment(lib, "../bin/feathergui64.lib")
#elif defined(BSS_DEBUG)
#pragma comment(lib, "../bin/feathergui_d.lib")
#else
#pragma comment(lib, "../bin/feathergui.lib")
#endif

fgStatic* NullRenderable()
{
  fgStatic* r = (fgStatic*)malloc(sizeof(fgStatic));
  fgStatic_Init(r);
  return r;
}
fgStatic* FG_FASTCALL fgLoadImage(const char* path)
{
  return NullRenderable();
}
fgStatic* FG_FASTCALL fgLoadImageData(const void* data, size_t length)
{
  return NullRenderable();
}
fgStatic* FG_FASTCALL fgLoadVector(const char* path)
{
  return NullRenderable();
}
fgStatic* FG_FASTCALL fgLoadVectorData(const void* data, size_t length)
{
  return NullRenderable();
}
fgStatic* FG_FASTCALL fgLoadText(const char* text, unsigned int flags)
{
  return NullRenderable();
}
void FG_FASTCALL FreeStatic(fgStatic* p)
{
  free(p);
}
fgRoot* FG_FASTCALL fgInitialize()
{
  fgRoot* r = (fgRoot*)malloc(sizeof(fgRoot));
  fgChild_Init((fgChild*)r,0);
  r->behaviorhook=fgRoot_BehaviorDefault;
  r->keymsghook=0;
  return r;
}