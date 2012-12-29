// Copyright ©2012 Black Sphere Studios
// For conditions of distribution and use, see copyright notice in "feathergui.h"

#include "feathergui.h"

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
  static const fgChild* plast=0; // This uses a simple memoization scheme so that repeated calls using the same child don't recalculate everything
  static AbsRect last;
  AbsVec center;
  const CRect* v=&self->element.area;
  assert(out!=0);
  out->left = v->left.abs;
  out->top = v->top.abs;
  out->right = v->right.abs;
  out->bottom = v->bottom.abs;

  if(!self->parent)
    return;
  if(plast!=self->parent)
  {
    ResolveRect(self->parent,&last);
    plast=self->parent;
  }

  out->left += lerp(last.left,last.right,v->left.rel);
  out->top += lerp(last.top,last.bottom,v->top.rel);
  out->right += lerp(last.left,last.right,v->right.rel);
  out->bottom += lerp(last.top,last.bottom,v->bottom.rel);
  
  center = ResolveVec(&self->element.center,out);
  out->left -= center.x;
  out->top -= center.y;
  out->right -= center.x;
  out->bottom -= center.y;
}

void FG_FASTCALL ResolveRectCache(AbsRect* r, const CRect* v, const AbsRect* last)
{
  assert(r!=0 && v!=0 && last!=0);
  r->left = lerp(last->left,last->right,v->left.rel)+v->left.abs;
  r->top = lerp(last->top,last->bottom,v->top.rel)+v->top.abs;
  r->right = lerp(last->left,last->right,v->right.rel)+v->right.abs;
  r->bottom = lerp(last->top,last->bottom,v->bottom.rel)+v->bottom.abs;
}

char FG_FASTCALL MsgHitAbsRect(const FG_Msg* msg, const AbsRect* r)
{
  assert(msg!=0 && r!=0);
  return (msg->x <= r->right) && (msg->x >= r->left) && (msg->y <= r->bottom) && (msg->y >= r->top);
}

char FG_FASTCALL MsgHitCRect(const FG_Msg* msg, const fgChild* child)
{
  AbsRect r;
  assert(msg!=0 && child!=0);
  ResolveRect(child,&r);
  return MsgHitAbsRect(msg,&r);
}

void FG_FASTCALL LList_Remove(fgChild* self, fgChild** root)
{
  assert(self!=0);
	if(self->prev != 0) self->prev->next = self->next;
  else *root = self->next;
	if(self->next != 0) self->next->prev = self->prev;
}

void FG_FASTCALL LList_Add(fgChild* self, fgChild** root)
{
  assert(self!=0);
  assert(root!=0); // While root cannot be zero, the pointer root is pointing to can be
  self->prev=0;
  self->next=*root;
  if((*root)!=0) (*root)->prev=self;
  *root=self;
}

void FG_FASTCALL LList_Insert(fgChild* self, fgChild* target, fgChild** root)
{
  assert(self!=0);
  if(!target) 
  {
    *root=self;
    return;
  }

	self->prev = target->prev;
	self->next = target;
	if(target->prev != 0) target->prev->next = self;
	else *root = self;
	target->prev = self;
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
    LList_Remove(self,&self->parent->root); // Remove ourselves from our parent
}

void FG_FASTCALL fgChild_SetParent(fgChild* BSS_RESTRICT self, fgChild* BSS_RESTRICT parent)
{
  fgChild* cur;
  assert(self!=0);
  if(self->parent!=0)
    LList_Remove(self,&self->parent->root); // Remove ourselves from our parent

  self->parent=parent;

  if(parent)
  {
    cur=parent->root;
    while(cur!=0 && self->order<cur->order)
      cur=cur->next;
    LList_Insert(self,cur,&parent->root); // Add ourselves to our new parent
  }
}

char FG_FASTCALL CompareCRects(const CRect* l, const CRect* r)
{
  assert(l!=0 && r!=0);
  if(l->left.abs!=r->left.abs) return 1; // Optimization to catch a whole ton of move situations where almost all abs coords change.
  return memcmp(l,r,sizeof(CRect))!=0;
}

char FG_FASTCALL CompChildOrder(const fgChild* l, const fgChild* r)
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
}
void FG_FASTCALL VirtualFreeChild(fgChild* self)
{
  assert(self!=0);
  (*self->destroy)(self);
  (*self->free)(self);
}
