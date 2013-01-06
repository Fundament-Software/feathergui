// Copyright ©2013 Black Sphere Studios
// For conditions of distribution and use, see copyright notice in "feathergui.h"

#ifndef __FG_RENDERABLE_H__
#define __FG_RENDERABLE_H__

#include "feathergui.h"

#ifdef  __cplusplus
extern "C" {
#endif

// Message types for statics only
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
  FG_RSETORDER,
  FG_RSETFONT,
  FG_RCLONE,
  FG_RCUSTOM
};

enum FG_TEXTFLAGS
{
  FGTEXT_CHARWRAP=1, // Wraps lines that go past the edge of the container by character
  FGTEXT_WORDWRAP=2, // Wraps lines that go past the edge of the container by word (the definition of a "word" is implementation specific)
  FGTEXT_ELLIPSES=4, // Lines that go past the bounderies of the text object are cut off with an ellipses (...)
  FGTEXT_RTL=8, // Forces right-to-left text rendering.
  FGTEXT_RIGHTALIGN=16,
  FGTEXT_CENTER=32, // Text horizontal centering behaves differently, because it centers each individual line.
};

enum FG_IMAGEFLAGS
{
  FGIMAGE_STRETCH=1, // Stretches the image instead of tiling it
  FGIMAGE_TILEALIGNRIGHT=2, // When tiling an image the image's origin is on the right instead of the left.
  FGIMAGE_TILEALIGNBOTTOM=4, // The image origin is on the bottom instead of the top.
};

struct __WINDOW;

// Representation of a static, which is implemented by the GUI implementation
typedef struct __RENDERABLE {
  fgChild element;
  void (FG_FASTCALL *message)(struct __RENDERABLE* self, unsigned char type, void* arg);
  struct __WINDOW* parent;
} fgStatic;

FG_EXTERN void FG_FASTCALL fgStatic_Init(fgStatic* self);
FG_EXTERN void FG_FASTCALL fgStatic_Destroy(fgStatic* self);
FG_EXTERN void FG_FASTCALL fgStatic_Message(fgStatic* self, unsigned char type, void* arg);
FG_EXTERN void FG_FASTCALL fgStatic_SetWindow(fgStatic* self, struct __WINDOW* window);
FG_EXTERN void FG_FASTCALL fgStatic_RemoveParent(fgStatic* self);
FG_EXTERN void FG_FASTCALL fgStatic_NotifyParent(fgStatic* self);

FG_EXTERN fgStatic* FG_FASTCALL fgLoadImage(const char* path);
FG_EXTERN fgStatic* FG_FASTCALL fgLoadImageData(const void* data, size_t length);
FG_EXTERN fgStatic* FG_FASTCALL fgLoadVector(const char* path);
FG_EXTERN fgStatic* FG_FASTCALL fgLoadVectorData(const void* data, size_t length);
FG_EXTERN fgStatic* FG_FASTCALL fgLoadText(const char* text, unsigned int flags);
FG_EXTERN void FG_FASTCALL SetSkinArray(fgStatic** pp, unsigned char size, unsigned char index);

// An item represented by both text and an image is extremely common, so we define this helper struct to make things simpler
typedef struct {
  fgStatic* image; // This is the image displayed alongside the item
  const char* text; // This is the item text itself
  const char* auxtext; // This represents either the shortcut key (for menu items) or a tooltip, or it may be ignored.
} fgTriplet;

#ifdef  __cplusplus
}
#endif

#endif
