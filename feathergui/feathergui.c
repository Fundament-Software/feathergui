// Copyright ©2012 Black Sphere Studios
// For conditions of distribution and use, see copyright notice in "feathergui.h"

#include "feathergui.h"

AbsVec FG_FASTCALL ResolveVec(fgChild* p, CVec* v)
{
  AbsVec r;
  static fgChild* plast=0; // This uses a simple memoization scheme so that repeated calls using the same child don't recalculate everything
  static AbsRect last;

  if(!p) { r.x=v->x.abs; r.y=v->y.abs; return r; }
  if(plast!=p)
  {
    last=ResolveRect(p->parent,&p->element.area);
    plast=p;
  }

  r.x = lerp(last.left,last.right,v->x.rel);
  r.y = lerp(last.top,last.bottom,v->y.rel);
  return r; 
}

AbsRect FG_FASTCALL ResolveRect(fgChild* p, CRect* v)
{
  static fgChild* plast=0; // This uses a simple memoization scheme so that repeated calls using the same child don't recalculate everything
  static AbsRect last;
  AbsRect r = { v->left.abs, v->top.abs, v->right.abs, v->bottom.abs };

  if(!p)
    return r;
  if(plast!=p)
  {
    last=ResolveRect(p->parent,&p->element.area);
    plast=p;
  }

  r.left += lerp(last.left,last.right,v->left.rel);
  r.top += lerp(last.top,last.bottom,v->top.rel);
  r.right += lerp(last.left,last.right,v->right.rel);
  r.bottom += lerp(last.top,last.bottom,v->bottom.rel);
  return r; 
}

void FG_FASTCALL ResolveRectCache(AbsRect* r, CRect* v, AbsRect* last)
{
  r->left = lerp(last->left,last->right,v->left.rel)+v->left.abs;
  r->top = lerp(last->top,last->bottom,v->top.rel)+v->top.abs;
  r->right = lerp(last->left,last->right,v->right.rel)+v->right.abs;
  r->bottom = lerp(last->top,last->bottom,v->bottom.rel)+v->bottom.abs;
}

char FG_FASTCALL MsgHitAbsRect(FG_Msg* msg, AbsRect* r)
{
  return (msg->x <= r->right) && (msg->x >= r->left) && (msg->y <= r->bottom) && (msg->y >= r->top);
}

char FG_FASTCALL MsgHitCRect(FG_Msg* msg, fgChild* child)
{
  AbsRect r = ResolveRect(child->parent,&child->element.area);
  return MsgHitAbsRect(msg,&r);
}

void FG_FASTCALL LList_Remove(fgChild* self, fgChild** root)
{
	if(self->prev != 0) self->prev->next = self->next;
  else *root = self->next;
	if(self->next != 0) self->next->prev = self->prev;
}

void FG_FASTCALL LList_Add(fgChild* self, fgChild** root)
{
  self->prev=0;
  self->next=*root;
  if((*root)!=0) (*root)->prev=self;
  *root=self;
}

void FG_FASTCALL LList_Insert(fgChild* self, fgChild* target, fgChild** root)
{
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

void FG_FASTCALL fgChild_Init(fgChild* BSS_RESTRICT self, fgChild* BSS_RESTRICT parent) 
{ 
  memset(self,0,sizeof(fgChild));
  self->destroy=&fgChild_Destroy; 
  fgChild_SetParent(self,parent);
}
void FG_FASTCALL fgChild_Destroy(fgChild* self) 
{
  while(self->root) // Destroy all children
    (*self->root->destroy)(self->root);
    //fgChild_SetParent(self->root, 0);
  
  if(self->parent!=0)
    LList_Remove(self,&self->parent->root); // Remove ourselves from our parent
  free(self);
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

void FG_FASTCALL CRect_DoCenter(CRect* cr, unsigned char axis)
{
  FABS adjust;
  if((axis&1)!=0) //x-axis center
  {
    adjust=(cr->left.abs+cr->right.abs)/2;
    cr->left.abs -= adjust;
    cr->right.abs -= adjust;
  }
  if((axis&2)!=0) //y-axis center
  {
    adjust=(cr->top.abs+cr->bottom.abs)/2; 
    cr->top.abs -= adjust;
    cr->bottom.abs -= adjust;
  }
}

char FG_FASTCALL CompareCRects(CRect* l, CRect* r)
{
  assert(l!=0 && r!=0);
  if(l->left.abs!=r->left.abs) return 1; // Optimization to catch a whole ton of move situations where almost all abs coords change.
  return memcmp(l,r,sizeof(CRect))!=0;
}