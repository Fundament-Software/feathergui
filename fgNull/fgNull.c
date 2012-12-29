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

void (FG_FASTCALL *debugmsghook)(fgWindow* self, const FG_Msg* msg)=0;

#define BUILDDEBUGMSG(TYPE) char FG_FASTCALL fgdebug_Message_##TYPE##(fgWindow* self, const FG_Msg* msg) \
{ \
  if(debugmsghook!=0) (*debugmsghook)((fgWindow*)self,msg); \
  return TYPE##_Message(self,msg); \
}

BUILDDEBUGMSG(fgButton)
BUILDDEBUGMSG(fgMenu)
BUILDDEBUGMSG(fgTopWindow)
BUILDDEBUGMSG(fgWindow)

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
fgButton* FG_FASTCALL fgButton_Create(fgStatic* item)
{
  fgButton* r = (fgButton*)malloc(sizeof(fgButton));
  r->window.message=&fgdebug_Message_fgButton;
  fgButton_Init(r);
  return r;
}
fgMenu* FG_FASTCALL fgMenu_Create()
{
  fgMenu* r = (fgMenu*)malloc(sizeof(fgMenu));
  r->grid.window.message=&fgdebug_Message_fgMenu;
  fgMenu_Init(r);
  return r;
}
fgTopWindow* FG_FASTCALL fgTopWindow_Create(fgRoot* root)
{
  fgTopWindow* r = (fgTopWindow*)malloc(sizeof(fgTopWindow));
  fgTopWindow_Init(r);
  r->window.message=&fgdebug_Message_fgTopWindow;
  r->region.message=&fgdebug_Message_fgWindow;
  fgWindow_VoidMessage(r,FG_SETPARENT,root);
  return r;
}

void FG_FASTCALL fgRoot_destroy(fgRoot* self)
{
  (*self->mouse.element.destroy)(&self->mouse);
  fgWindow_Destroy(self);
}

fgRoot* FG_FASTCALL fgInitialize()
{
  fgRoot* r = (fgRoot*)malloc(sizeof(fgRoot));
  fgWindow_Init((fgWindow*)r,0);
  r->gui.element.destroy=&fgRoot_destroy;
  r->behaviorhook=&fgRoot_BehaviorDefault;
  r->keymsghook=0;
  r->winrender=&fgRoot_WinRender;
  fgWindow_Init(&r->mouse);
  return r;
}