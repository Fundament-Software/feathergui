// Copyright ©2012 Black Sphere Studios
// For conditions of distribution and use, see copyright notice in "feathergui.h"

#ifndef __FG_RENDERABLE_H__
#define __FG_RENDERABLE_H__

#include "feathergui.h"

// Message types for renderables only
enum FG_RENDERMSG
{
  FG_RDRAW,
  FG_RSHOW,
  FG_RADDCHILD,
  FG_RREMOVECHILD,
  FG_RSETAREA,
  FG_RSETELEMENT,
  FG_RSETTEXT,
  FG_RSETUV,
  FG_RSETCOLOR,
  FG_RSETFLAGS,
  FG_RCUSTOM,
};

struct __WINDOW;

// Representation of a renderable, which is implemented by the GUI implementation
typedef struct __RENDERABLE {
  Child element;
  void (FG_FASTCALL *message)(struct __RENDERABLE* self, unsigned char type, void* arg);
  struct __WINDOW* parent;
} Renderable;

extern void FG_FASTCALL Renderable_Init(Renderable* self);
extern void FG_FASTCALL Renderable_Destroy(Renderable* self);
extern void FG_FASTCALL Renderable_Message(Renderable* self, unsigned char type, void* arg);
extern void FG_FASTCALL Renderable_SetWindow(Renderable* self, struct __WINDOW* window);
extern void FG_FASTCALL Renderable_RemoveParent(Renderable* self);

extern Renderable* FG_FASTCALL LoadImage(const char* path);
extern Renderable* FG_FASTCALL LoadImageData(const void* data, size_t length);
extern Renderable* FG_FASTCALL LoadVector(const char* path);
extern Renderable* FG_FASTCALL LoadVectorData(const void* data, size_t length);
extern Renderable* FG_FASTCALL LoadText(const char* text, unsigned int flags);
extern void FG_FASTCALL FreeRenderable(Renderable* p);

#endif