/* Feather - Lightweight GUI Standard
   Copyright ©2012 Black Sphere Studios

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
*/


#ifndef __FEATHER_GUI_H__
#define __FEATHER_GUI_H__

#include <assert.h>
#include <string.h> // memcpy,memset
#include <malloc.h> 
#include "bss_compiler.h"

typedef float FREL; // Change this to double for double precision (why on earth you would need that for a GUI, however, is beyond me)
typedef float FABS;
typedef unsigned int FG_UINT;

#define FGUI_VERSION_MAJOR 0
#define FGUI_VERSION_MINOR 1
#define FGUI_VERSION_REVISION 0

// A unified coordinate specifies things in terms of absolute and relative positions.
typedef struct {
  FABS abs; // Absolute coordinates are always added to the point specified by relative coordinates.
  FREL rel;
} Coord;

#define MAKE_VEC(_T,_N) typedef struct { _T x; _T y; } _N

MAKE_VEC(FREL,RelVec);
MAKE_VEC(FABS,AbsVec);
// A coordinate vector specifies a point by unified coordinates
MAKE_VEC(Coord,CVec);

#define MAKE_RECT(_T,_T2,_N) typedef struct { \
  union { \
    struct { \
      _T top; \
      _T left; \
      _T right; \
      _T bottom; \
    }; \
    struct { \
      _T2 topleft; \
      _T2 bottomright; \
    }; \
  }; \
} _N

MAKE_RECT(FREL,RelVec,RelRect);
MAKE_RECT(FABS,AbsVec,AbsRect);
// A coordinate rect specifies its topleft and bottomright corners in terms of coordinate vectors.
MAKE_RECT(Coord,CVec,CRect);

static BSS_FORCEINLINE FG_UINT __fastcall fbnext(FG_UINT in)
{
  return in + 1 + (in>>1) + (in>>3) - (in>>7);
}
static BSS_FORCEINLINE FABS __fastcall lerp(FABS a, FABS b, FREL amt)
{
	return a+((FABS)((b-a)*amt));
}

#define MAKE_VECTOR(_T,_N) typedef struct { _T *p; FG_UINT s; FG_UINT l; } _N; \
  static FG_UINT Add_##_N##(_N *self, _T item) \
  { \
    _T *n; \
    if(self->l==self->s) \
    { \
      self->s=fbnext(self->l); \
      n = (_T##*)malloc(self->s*sizeof(_T)); \
      if(self->p!=0) { \
        memcpy((void*)n,(void*)self->p,self->l*sizeof(_T)); \
        free(self->p); \
      } \
      self->p=n; \
    } \
    assert(self->l<self->s); \
    self->p[self->l]=item; \
    return self->l++; \
  } \
  static void Remove_##_N##(_N *self, FG_UINT index) \
  { \
    assert(index<self->l); \
    memmove(self->p+index,self->p+index+1,((--self->l)-index)*sizeof(_T)); \
  } \
  static void Init_##_N##(_N *self) { memset(self,0,sizeof(_N)); } \
  static void Destroy_##_N##(_N *self) { if(self->p!=0) free(self->p); }

typedef struct {
  CRect area;
  FABS rotation;
  CVec center;
} Element;

typedef struct __CHILD {
  Element element;
  void (__fastcall *destroy)(void* self); // virtual destructor
  struct __CHILD* parent;
} Child;

typedef struct __RENDERABLE {
  Child element;
  void (__fastcall *draw)(struct __RENDERABLE* self);
} Renderable;

enum FG_MSGTYPE
{
  FG_MOUSEDOWN,
  //FG_MOUSEDBLCLICK,
  FG_MOUSEUP, // Sent to focused window
  FG_MOUSEON,
  FG_MOUSEOFF,
  FG_MOUSEMOVE, // Sent to focused window
  FG_MOUSESCROLL, // Sent to focused window
  FG_KEYUP,
  FG_KEYDOWN,
  FG_KEYCHAR, //Passed in conjunction with keydown/up to differentiate a typed character from other keys.
  FG_JOYBUTTONDOWN,
  FG_JOYBUTTONUP,
  FG_JOYAXIS,
  FG_GOTFOCUS,
  FG_LOSTFOCUS,
  FG_CUSTOMEVENT
};

typedef struct __FG_MSG {
  unsigned char type;

  union {
    struct { int x; int y; unsigned char _button; unsigned char _allbtn; }; // Mouse events
    struct { short scrolldelta; }; // Mouse scroll
    struct {  // Keys
        unsigned char keycode; //only used by KEYDOWN/KEYUP, represents an actual keycode, not a character
        char sigkeys;
        char keydown;
        int keychar; //Only used by KEYCHAR
    };
    struct { float joyvalue; short joyaxis; }; // JOYAXIS
    struct { char joydown; short joybutton; }; // JOYBUTTON
  };
} FG_Msg;

struct __WINDOW;
MAKE_VECTOR(struct __WINDOW*,VectWindow)

// Defines the base GUI element, a window. This is not an actual top level window.
typedef struct __WINDOW {
  Child element;
  void (__fastcall *message)(struct __WINDOW* self, FG_Msg* msg);
  FG_UINT id;
  FG_UINT order; // order relative to other windows
  FG_UINT maxorder; // Largest order number in this window's group of children
  Renderable* rlist;
  FG_UINT rnum;
  struct __WINDOW* root; // children list root
  struct __WINDOW* next;
  struct __WINDOW* prev;
} Window;

extern Window* fgFocusedWindow;
extern void (__fastcall *keymsghook)(FG_Msg* msg);

extern void __fastcall Element_Init(Element* self);

extern void __fastcall Child_Init(Child* self);
extern void __fastcall Child_Destroy(Child* self);

extern void __fastcall Renderable_Init(Renderable* self);
extern void __fastcall Renderable_Draw(Renderable* self);

extern void __fastcall Window_Init(Window* self, Window* parent);
extern void __fastcall Window_Destroy(Window* self);
extern void __fastcall Window_Message(Window* self, FG_Msg* msg);
extern void __fastcall Window_SetParent(Window* self, Window* parent);
extern void __fastcall Window_SetClip(Window* self, char clip);
extern char __fastcall AddRenderable(Window* self, Renderable* r);

extern AbsVec __fastcall ResolveVec(Child* p, CVec* v);
extern AbsRect __fastcall ResolveRect(Child* p, CRect* v);

extern Renderable* __fastcall LoadImage(const char* path);
//extern Renderable* __fastcall LoadBitmap(const void* data);
extern Renderable* __fastcall LoadVector(const char* text);
extern Renderable* __fastcall LoadText(const char* text, int flags);
extern void __fastcall FreeRenderable(Renderable* p);

extern char __fastcall EditText(Renderable* text, int flags);

#endif
