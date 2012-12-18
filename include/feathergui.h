/* Feather - Lightweight GUI Abstraction Layer
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
typedef float FABS; // We seperate both of these types because then you don't have to guess if a float is relative or absolute
typedef unsigned int FG_UINT;

#define FGUI_VERSION_MAJOR 0
#define FGUI_VERSION_MINOR 1
#define FGUI_VERSION_REVISION 0
#define FG_FASTCALL BSS_COMPILER_FASTCALL

// A unified coordinate specifies things in terms of absolute and relative positions.
typedef struct {
  FABS abs; // Absolute coordinates are added to relative coordinates, which specify a point from a linear interpolation of the parent's dimensions
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

static BSS_FORCEINLINE FG_UINT FG_FASTCALL fbnext(FG_UINT in)
{
  return in + 1 + (in>>1) + (in>>3) - (in>>7);
}
static BSS_FORCEINLINE FABS FG_FASTCALL lerp(FABS a, FABS b, FREL amt)
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
      n = (_T##*)realloc(self->p,self->s*sizeof(_T)); \
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
  void (FG_FASTCALL *destroy)(void* self); // virtual destructor
  struct __CHILD* parent;
  struct __CHILD* root; // children list root
  struct __CHILD* next;
  struct __CHILD* prev;
} Child;


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
  FG_MOVE, // Passed when any change is made to an element
  FG_ADDCHILD, // Pass an FG_Msg with this type and set the other pointer to the child that should be added.
  FG_SETPARENT,
  FG_ADDRENDERABLE, // Send a Renderable in other with an optional ID for its skin index in otheraux.
  FG_REMOVERENDERABLE,
  FG_SETCLIP, // Send an otherint equal to 0 to disable cliping, otherwise its clipped by parent
  FG_SETCENTERED, // Removes centering if otherint is 0, centers x-axis on 1, y-axis on 2, both on 3
  FG_ADDITEM,
  FG_SHOW, // Send an otherint equal to 0 to hide, otherwise its made visible
  FG_SETTILE, // fgGrid only, 16 tiles on x-axis, 32 tiles on y-axis, 48 both.
  FG_CUSTOMEVENT
};

// General message structure which contains the message type and then various kinds of information depending on the type.
typedef struct __FG_MSG {
  unsigned char type;

  union {
    struct { int x; int y; unsigned char button; unsigned char allbtn; }; // Mouse events
    struct { short scrolldelta; }; // Mouse scroll
    struct {  // Keys
        unsigned char keycode; //only used by KEYDOWN/KEYUP, represents an actual keycode, not a character
        char sigkeys;
        char keydown;
        int keychar; //Only used by KEYCHAR, represents a utf32 character
    };
    struct { float joyvalue; short joyaxis; }; // JOYAXIS
    struct { char joydown; short joybutton; }; // JOYBUTTON
    struct { void* other; unsigned char otheraux; }; // Used by any generic messages (FG_ADDCHILD, FG_SETPARENT, etc.)
    int otherint; // Used by any generic message that want an int (FG_SETCLIP, FG_SETCENTERED, etc.)
  };
} FG_Msg;


extern void (FG_FASTCALL *keymsghook)(FG_Msg* msg);

extern void FG_FASTCALL Child_Init(Child* BSS_RESTRICT self, Child* BSS_RESTRICT parent);
extern void FG_FASTCALL Child_Destroy(Child* self);
extern void FG_FASTCALL Child_SetParent(Child* BSS_RESTRICT self, Child* BSS_RESTRICT parent);

extern AbsVec FG_FASTCALL ResolveVec(Child* p, CVec* v);
extern AbsRect FG_FASTCALL ResolveRect(Child* p, CRect* v);
extern char FG_FASTCALL CompareCRects(CRect* l, CRect* r); // Returns 0 if both are the same or 1 otherwise
extern void FG_FASTCALL CRect_DoCenter(CRect* self, unsigned char axis);
extern void FG_FASTCALL LList_Remove(Child* self, Child** root);
extern void FG_FASTCALL LList_Add(Child* self, Child** root);

#endif
