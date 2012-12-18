// Copyright ©2012 Black Sphere Studios
// For conditions of distribution and use, see copyright notice in "feathergui.h"

#include "feathergui.h"

void (FG_FASTCALL *keymsghook)(FG_Msg* msg)=0;

AbsVec FG_FASTCALL ResolveVec(Child* p, CVec* v)
{
  AbsVec r;
  static Child* plast=0; // This uses a simple memoization scheme so that repeated calls using the same child don't recalculate everything
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

AbsRect FG_FASTCALL ResolveRect(Child* p, CRect* v)
{
  AbsRect r;
  static Child* plast=0; // This uses a simple memoization scheme so that repeated calls using the same child don't recalculate everything
  static AbsRect last;

  if(!p) { r.left=v->left.abs; r.top=v->top.abs; r.right=v->right.abs; r.bottom=v->bottom.abs; return r; }
  if(plast!=p)
  {
    last=ResolveRect(p->parent,&p->element.area);
    plast=p;
  }

  r.left = lerp(last.left,last.right,v->left.rel);
  r.top = lerp(last.top,last.bottom,v->top.rel);
  r.right = lerp(last.left,last.right,v->right.rel);
  r.bottom = lerp(last.top,last.bottom,v->bottom.rel);
  return r; 
}

void FG_FASTCALL LList_Remove(Child* self, Child** root)
{
	if(self->prev != 0) self->prev->next = self->next;
  else *root = self->next;
	if(self->next != 0) self->next->prev = self->prev;
}

void FG_FASTCALL LList_Add(Child* self, Child** root)
{
  self->prev=0;
  self->next=*root;
  if((*root)!=0) (*root)->prev=self;
  *root=self;
}

void FG_FASTCALL Child_Init(Child* BSS_RESTRICT self, Child* BSS_RESTRICT parent) 
{ 
  memset(self,0,sizeof(Child));
  self->destroy=&Child_Destroy; 
  Child_SetParent(self,parent);
}
void FG_FASTCALL Child_Destroy(Child* self) 
{
  while(self->root) // Recursively set all children's parents to 0
    Child_SetParent(self->root, 0);
  
  if(self->parent!=0)
    LList_Remove(self,&self->parent->root); // Remove ourselves from our parent
}

void FG_FASTCALL Child_SetParent(Child* BSS_RESTRICT self, Child* BSS_RESTRICT parent)
{
  assert(self!=0);
  if(self->parent!=0)
    LList_Remove(self,&self->parent->root); // Remove ourselves from our parent

  self->parent=parent;

  if(parent)
    LList_Add(self,&parent->root); // Add ourselves to our new parent
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