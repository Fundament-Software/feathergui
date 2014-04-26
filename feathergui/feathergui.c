// Copyright ©2013 Black Sphere Studios
// For conditions of distribution and use, see copyright notice in "feathergui.h"

#include "feathergui.h"
#include <intrin.h>
#include <limits.h>

void FG_FASTCALL fgVector_Init(fgVector* self)
{
  memset(self,0,sizeof(fgVector));
}
void FG_FASTCALL fgVector_Destroy(fgVector* self)
{
  if(self->p) free(self->p);
}
// TODO: Speed this up using a custom simplistic heap allocator
void FG_FASTCALL fgVector_SetSize(fgVector* self, FG_UINT length, FG_UINT size)
{
  self->s=length*size;
  self->p=realloc(self->p,self->s);
  if(self->l>length) self->l = length;
}
void FG_FASTCALL fgVector_CheckSize(fgVector* self, FG_UINT size)
{
  if((self->l*size)>=self->s)
    fgVector_SetSize(self,fbnext(self->l),size);
  assert((self->l*size)<self->s);
}
void FG_FASTCALL fgVector_Remove(fgVector* self, FG_UINT index, FG_UINT size)
{
  assert(index<self->l);
  memmove(((char*)self->p)+(index*size),((char*)self->p)+((index+1)*size),((--self->l)-index)*size);
}

AbsVec FG_FASTCALL ResolveVec(const CVec* v, const AbsRect* last)
{
  AbsVec r = { v->x.abs, v->y.abs };
  assert(last!=0);
  r.x += lerp(last->left,last->right,v->x.rel);
  r.y += lerp(last->top,last->bottom,v->y.rel);
  return r; 
}

void FG_FASTCALL ResolveRect(const fgChild* self, AbsRect* out)
{
  //static const fgChild* plast=0; // This uses a simple memoization scheme so that repeated calls using the same child don't recalculate everything
  //static AbsRect last;
  AbsRect last;
  AbsVec center = { self->element.center.x.abs,self->element.center.y.abs };
  const CRect* v=&self->element.area;
  assert(out!=0);
  out->left = v->left.abs;
  out->top = v->top.abs;
  out->right = v->right.abs;
  out->bottom = v->bottom.abs;

  if(!self->parent)
    return;
  //if(plast!=self->parent)
  //{
    ResolveRect(self->parent,&last);
  //  plast=self->parent;
  //}

  out->left += lerp(last.left,last.right,v->left.rel);
  out->top += lerp(last.top,last.bottom,v->top.rel);
  out->right += lerp(last.left,last.right,v->right.rel);
  out->bottom += lerp(last.top,last.bottom,v->bottom.rel);
  
  center.x += (v->right.abs-v->left.abs)*self->element.center.x.rel;
  center.y += (v->bottom.abs-v->top.abs)*self->element.center.y.rel;
  //center = ResolveVec(&,r); // We can't use this because the center is relative to the DIMENSIONS, not the actual position.
  out->left -= center.x;
  out->top -= center.y;
  out->right -= center.x;
  out->bottom -= center.y;
  out->left += self->margin.left;
  out->top += self->margin.top;
  out->right += self->margin.right;
  out->bottom += self->margin.bottom;
}

void FG_FASTCALL ResolveRectCache(AbsRect* BSS_RESTRICT out, const fgChild* elem, const AbsRect* BSS_RESTRICT last)
{
  AbsVec center = { elem->element.center.x.abs, elem->element.center.y.abs };
  const CRect* v=&elem->element.area;
  assert(out!=0 && elem!=0 && last!=0);
  out->left = lerp(last->left, last->right, v->left.rel)+v->left.abs;
  out->top = lerp(last->top, last->bottom, v->top.rel)+v->top.abs;
  out->right = lerp(last->left, last->right, v->right.rel)+v->right.abs;
  out->bottom = lerp(last->top, last->bottom, v->bottom.rel)+v->bottom.abs;
  
  center.x += (v->right.abs-v->left.abs)*elem->element.center.x.rel;
  center.y += (v->bottom.abs-v->top.abs)*elem->element.center.y.rel;
  //center = ResolveVec(&,r); // We can't use this because the center is relative to the DIMENSIONS, not the actual position.
  out->left -= center.x;
  out->top -= center.y;
  out->right -= center.x;
  out->bottom -= center.y;
  out->left += elem->margin.left;
  out->top += elem->margin.top;
  out->right += elem->margin.right;
  out->bottom += elem->margin.bottom;
}

// This uses a standard inclusive-exclusive rectangle interpretation.
char FG_FASTCALL HitAbsRect(const AbsRect* r, FABS x, FABS y)
{
  assert(r!=0);
  return (x < r->right) && (x >= r->left) && (y < r->bottom) && (y >= r->top);
}

char FG_FASTCALL MsgHitAbsRect(const FG_Msg* msg, const AbsRect* r)
{
  assert(msg!=0 && r!=0);
  return HitAbsRect(r,(FABS)msg->x,(FABS)msg->y);
}

char FG_FASTCALL MsgHitCRect(const FG_Msg* msg, const fgChild* child)
{
  AbsRect r;
  assert(msg!=0 && child!=0);
  ResolveRect(child,&r);
  return MsgHitAbsRect(msg,&r);
}

void FG_FASTCALL LList_Remove(fgChild* self, fgChild** root, fgChild** last)
{
  assert(self!=0);
	if(self->prev != 0) self->prev->next = self->next;
  else *root = self->next;
	if(self->next != 0) self->next->prev = self->prev;
  else *last = self->prev;
}

void FG_FASTCALL LList_ChangeOrder(fgChild* self, fgChild** root, fgChild** last, fgFlag flag)
{
  fgChild* cur = self->next;
  fgChild* prev = self->prev;
  while(cur != 0 && (((self->flags&flag) < (cur->flags&flag)) || ((self->order < cur->order) && (self->flags&flag) == (cur->flags&flag))))
  {
      prev = cur;
      cur = cur->next;
  }
  while(prev != 0 && !(((self->flags&flag) < (prev->flags&flag)) || ((self->order < prev->order) && (self->flags&flag) == (prev->flags&flag))))
  {
      cur = prev;
      prev = prev->prev;
  }
  
  if(cur==self->next) { assert(prev==self->prev); return; } // we didn't move anywhere
  LList_Remove(self,root,last);
  self->next = cur;
  self->prev = prev;
  if(prev) prev->next=self;
  else *root=self; // Prev is only null if we're inserting before the root, which means we must reassign the root.
  if(cur) cur->prev=self; 
  else *last=self; // Cur is null if we are at the end of the list, so update last
}

//me cur
//y  y  | order<order
//y  n  | false
//n  y  | true
//n  n  | order<order

void FG_FASTCALL LList_Add(fgChild* self, fgChild** root, fgChild** last, fgFlag flag)
{
  fgChild* cur = *root;
  fgChild* prev = 0; // Sadly the elegant pointer to pointer method doesn't work for doubly linked lists.
  assert(self!=0 && root!=0);
  if(!cur) // We do this check up here because we'd have to do it for the end append check below anyway so we might as well utilize it!
    (*last)=(*root)=self;
  //else if(self->order<(*last)->order) { // shortcut for appending to the end of the list
  else if(((self->flags&flag) < ((*last)->flags&flag)) || ((self->order < (*last)->order) && (self->flags&flag) == ((*last)->flags&flag))) {
    cur=0;
    prev=*last;
  } else {
    while(cur != 0 && (((self->flags&flag) < (cur->flags&flag)) || ((self->order < cur->order) && (self->flags&flag) == (cur->flags&flag))))
    {
       prev = cur;
       cur = cur->next;
    }
  }
  self->next = cur;
  self->prev = prev;
  if(prev) prev->next=self;
  else *root=self; // Prev is only null if we're inserting before the root, which means we must reassign the root.
  if(cur) cur->prev=self; 
  else *last=self; // Cur is null if we are at the end of the list, so update last
}

void FG_FASTCALL fgChild_Init(fgChild* self) 
{ 
  assert(self!=0);
  memset(self,0,sizeof(fgChild));
  self->destroy=&fgChild_Destroy; 
  self->free=&free; 
}
void FG_FASTCALL fgChild_Destroy(fgChild* self) 
{
  assert(self!=0);
  while(self->root) // Destroy all children
    VirtualFreeChild(self->root);
    //fgChild_SetParent(self->root, 0);
  
  if(self->parent!=0)
    LList_Remove(self,&self->parent->root,&self->parent->last); // Remove ourselves from our parent
}

void FG_FASTCALL fgChild_SetParent(fgChild* BSS_RESTRICT self, fgChild* BSS_RESTRICT parent, fgFlag flag)
{
  assert(self!=0);
  if(self->parent==parent) { // If this is true, we just need to make sure the ordering is valid
    LList_ChangeOrder(self,&parent->root,&parent->last,flag);
    return;
  }

  if(self->parent!=0)
    LList_Remove(self,&self->parent->root,&self->parent->last); // Remove ourselves from our parent

  self->next=0;
  self->prev=0;
  self->parent=parent;

  if(parent)
    LList_Add(self,&parent->root,&parent->last,flag);
}

//void FG_FASTCALL fgChild_ExpandX(fgChild* self, fgElement* elem)
//{
//  CRect* area=(CRect*)elem; // we can do this because CRect is on top of the fgElement struct and therefore fgElement "inherits" it.
//  CRect* selfarea=(CRect*)self;
//  FABS d = selfarea->right.abs-selfarea->left.abs;
//  if(area->right.rel==0.0 && area->right.abs>d) // Only expand on stuff that isn't relatively positioned for obvious reasons.
//    selfarea->right.abs=selfarea->left.abs+area->right.abs;
//}
//void FG_FASTCALL fgChild_ExpandY(fgChild* self, fgElement* elem)
//{
//  CRect* area=(CRect*)elem; 
//  CRect* selfarea=(CRect*)self;
//  FABS d = selfarea->bottom.abs-selfarea->top.abs;
//  if(area->bottom.rel==0.0 && area->bottom.abs>d)
//    selfarea->bottom.abs=selfarea->top.abs+area->bottom.abs;
//}

char FG_FASTCALL CompareCRects(const CRect* l, const CRect* r)
{
  assert(l!=0 && r!=0);
  if(l->left.abs!=r->left.abs) return 1; // Optimization to catch a whole ton of move situations where almost all abs coords change.
  return memcmp(l,r,sizeof(CRect))!=0;
}
void FG_FASTCALL MoveCRect(AbsVec v, CRect* r)
{
  FABS dx,dy;
  dx=r->right.abs-r->left.abs;
  dy=r->bottom.abs-r->top.abs;
  r->left.abs=v.x;
  r->top.abs=v.y;
  r->right.abs=v.x+dx;
  r->right.abs=v.y+dy;
}

/*char FG_FASTCALL CompChildOrder(const fgChild* l, const fgChild* r)
{
  const fgChild* cur;
  unsigned int ldepth=0,rdepth=0,diff;
  assert(l!=0 && r!=0);
  if(l==r) return 0;
  cur=l;
  while((cur=cur->parent)!=0) ++ldepth;
  cur=r;
  while((cur=cur->parent)!=0) ++rdepth;

  diff=(ldepth<rdepth) - (ldepth>rdepth);
  while(ldepth<rdepth) { r=r->parent; --rdepth; }
  while(ldepth>rdepth) { l=l->parent; --ldepth; }

  if(l==r) return diff;

  while(l->parent!=r->parent)
  {
    r=r->parent;
    l=l->parent;
  }

  return (l->order>r->order) - (l->order<r->order);
}*/

void FG_FASTCALL VirtualFreeChild(fgChild* self)
{
  assert(self!=0);
  (*self->destroy)(self);
  (*self->free)(self);
}
//void FG_FASTCALL ToIntAbsRect(const AbsRect* r, int target[static 4]) // Gotta love VC++ not supporting C99 which is 14 FUCKING YEARS OLD
void FG_FASTCALL ToIntAbsRect(const AbsRect* r, int target[4])
{
  _mm_storeu_si128((__m128i*)target,_mm_cvttps_epi32(_mm_loadu_ps(&r->left)));
}

void FG_FASTCALL ToLongAbsRect(const AbsRect* r, long target[4])
{
#if ULONG_MAX==UINT_MAX
  _mm_storeu_si128((__m128i*)target,_mm_cvttps_epi32(_mm_loadu_ps(&r->left)));
#else
  int hold[4];
  _mm_storeu_si128((__m128i*)hold,_mm_cvtps_epi32(_mm_loadu_ps(&r->left)));
  target[0]=hold[0];
  target[1]=hold[1];
  target[2]=hold[2];
  target[3]=hold[3];
#endif

}
