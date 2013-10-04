// Copyright ©2013 Black Sphere Studios
// For conditions of distribution and use, see copyright notice in "feathergui.h"

#include "fgGrid.h"
#include "bss_defines.h"

void FG_FASTCALL fgGrid_Init(fgGrid* self, fgWindow* parent, const fgElement* element, FG_UINT id, fgFlag flags)
{
  assert(self!=0);
  fgWindow_Init(&self->window,parent,element,id,flags);
  self->window.message=&fgGrid_Message;
  memset(&self->margins,0,sizeof(AbsRect));
}
char FG_FASTCALL fgGrid_Message(fgGrid* self, const FG_Msg* msg)
{
  static char skip=0;
  char b;
  fgChild* hold;
  assert(self!=0 && msg!=0);

  switch(msg->type)
  {
  case FG_MOVE:
    if(msg->other!=0) { // If its external we do a reposition, which will then send an internal move message that will propagate
      if(skip) return 1;
      skip=1;
      if(msg->otheraux<0) // This is a size propagation coming down from our parent, so we have to re-do everything.
        fgGrid_Reposition(self,self->window.element.root);
      else // Otherwise it's coming up from a child, so reposition only it. Do this even if it's dimensions didn't change, because it could have been added.
        fgGrid_Reposition(self,(fgChild*)msg->other);
      skip=0;
    }
    break; // Otherwise let the default handler deal with it
  case FG_ADDITEM:
    hold=(fgChild*)msg->other;
    hold->order=msg->otheraux;
    if(hold->order<0 || hold->order>self->items.l) hold->order=self->items.l;
    fgVector_Insert(self->items,hold,hold->order,fgChild*);
    {
      fgChild** root = &self->window.element.root;
      fgChild** last = &self->window.element.last;
      fgFlag flag = FGWIN_NOCLIP;
      if(hold->flags&FGSTATIC_MARKER)
      {
        root = &self->window.rlist;
        last = &self->window.rlast;
        flag = 0;
      }

      for(int i = self->items.l; (--i) > hold->order;) { // This works because self->items.l must be at least 1, and hold->order must be at least 0
        fgVector_Get(self->items,i,fgChild*)->order=i;
        LList_ChangeOrder(fgVector_Get(self->items,i,fgChild*),root,last,flag);
      }
      FG_Msg aux = *msg;
      aux.type=(hold->flags&FGSTATIC_MARKER)?FG_ADDSTATIC:FG_ADDCHILD;
      aux.otheraux=0;
      return fgWindow_Message((fgWindow*)self,msg); // Have the window insert the item. This will trigger an FG_MOVE that gets propagated up
    }
  case FG_REMOVEITEM:
    hold=(fgChild*)msg->other;
    b = hold->element.area.bottom.abs>(self->window.element.element.area.bottom.abs-self->padding.bottom-1) || hold->element.area.right.abs>(self->window.element.element.area.right.abs-self->padding.right-1);
    hold=hold->next; // First we save the image afterwards
    if(!fgWindow_Message((fgWindow*)self,msg)) // Then we have the window remove the image
    {
      skip=1;
      // When not tiling, if one of the removed windows borders are exactly equal to the bounds, then we have to re-calculate everything
      if((self->window.element.flags&(FGGRID_TILEX|FGGRID_TILEY))==0 && b)
        fgGrid_Reposition(self,0); 
      else if(hold!=0) // Otherwise we just reposition the image afterwards, if there is one, which will fix the grid.
        fgGrid_Reposition(self,hold);
      skip=0;
      return 0;
    }
    return 1;
  case FG_GETCLASSNAME:
    (*(const char**)msg->other) = "fgGrid";
    return 0;
  }
  return fgWindow_Message((fgWindow*)self,msg);
}
FG_EXTERN void FG_FASTCALL fgGrid_Reposition(fgGrid* self, fgChild* p)
{
  CRect* hold=&self->window.element.element.area;
  AbsVec dim = { hold->right.abs-hold->left.abs, hold->bottom.abs-hold->top.abs }; // this is NOT the resolved rect because it's only used when we're expanding
  AbsRect area;
  CRect narea;
  fgFlag flags = self->window.element.flags;
  AbsVec cur = { self->padding.left,self->padding.top };
  fgChild** items = (fgChild**)self->items.p;
  int i = p->order;
  if(p!=fgVector_Get(self->items,p->order,fgChild*)) return; // If false, this isn't actually an item

  if((flags&FGGRID_TILEX) && (flags&FGGRID_TILEY)) {
    ResolveRect((fgChild*)self,&area);
    if(self->window.element.flags&FGGRID_EXPANDX) { // If we're expanding on the X axis we must grow downward and then loop around.
      if(i>0)
      {
        cur.x=items[i-1]->element.area.left.abs-self->margins.x;
        cur.y=items[i-1]->element.area.bottom.abs+self->margins.y;
      }
      for(; i < self->items.l; ++i)
      {
        hold=&items[i]->element.area;
        if(hold->bottom.abs-hold->top.abs+cur.y+self->padding.bottom>area.bottom)
        {
          if(cur.y>dim.y) dim.y=cur.y;
          cur.y=self->padding.top;
          cur.x=dim.x+self->margins.x;
        }
        MoveCRect(cur,hold);
        cur.y=hold->bottom.abs+self->margins.y;
        if(hold->right.abs>dim.x) dim.x=hold->right.abs;
      }
      if(cur.y>dim.y) dim.y=cur.y;
    } else { // EXPANDY is the default even if its not specified. If we expand on the Y axis, we must grow rightward.
      if(i>0)
      {
        cur.x=items[i-1]->element.area.right.abs+self->margins.x;
        cur.y=items[i-1]->element.area.top.abs-self->margins.y;
      }
      for(; i < self->items.l; ++i)
      {
        hold=&items[i]->element.area;
        if(hold->right.abs-hold->left.abs+cur.x+self->padding.right>area.right)
        {
          if(cur.x>dim.x) dim.x=cur.x;
          cur.x=self->padding.left;
          cur.y=dim.y+self->margins.y;
        }
        MoveCRect(cur,hold);
        cur.x=hold->right.abs+self->margins.x;
        if(hold->bottom.abs>dim.y) dim.y=hold->bottom.abs;
      }
      if(cur.x>dim.x) dim.x=cur.x;
    }
  } else if(flags&FGGRID_TILEX) {
    if(i>0) cur.x=items[i-1]->element.area.right.abs+self->margins.x;
    for(; i < self->items.l; ++i)
    {
      hold=&items[i]->element.area;
      MoveCRect(cur,hold);
      cur.x=hold->right.abs+self->margins.x;
      if(hold->bottom.abs>dim.y) dim.y=hold->bottom.abs;
      p=p->next;
    }
    dim.x=cur.x-self->margins.x;
  } else if(flags&FGGRID_TILEY) {
    if(i>0) cur.y=items[i-1]->element.area.bottom.abs+self->margins.y;
    for(; i < self->items.l; ++i)
    {
      MoveCRect(cur,&items[i]->element.area);
      cur.y=p->element.area.bottom.abs+self->margins.y;
      if(p->element.area.right.abs>dim.x) dim.x=p->element.area.right.abs;
    }
    dim.y=cur.y-self->margins.y;
  } else { // No grid flags are set, so we don't reposition any elements, we only grow in size.
    if(!p) // If null, do ALL of them because we removed something on the border
    {
      for(i=0; i < self->items.l; ++i)
      {
        hold=&items[i]->element.area;
        if(hold->right.rel==0.0 && hold->right.abs>dim.x) dim.x=hold->right.abs;
        if(hold->bottom.rel==0.0 && hold->bottom.abs>dim.y) dim.y=hold->bottom.abs;
      }
    }
    else // If not null we're inserting something so just check that one.
    {
      hold=&p->element.area;
      if(hold->right.rel==0.0 && hold->right.abs>dim.x) dim.x=hold->right.abs;
      if(hold->bottom.rel==0.0 && hold->bottom.abs>dim.y) dim.y=hold->bottom.abs;
    }
  }
  
  narea = self->window.element.element.area; // at this point, dim is always equal to the total area regardless of whether we wanted it.
  if(flags&FGGRID_EXPANDX) narea.right.abs=narea.left.abs+dim.x+self->padding.right;
  if(flags&FGGRID_EXPANDY) narea.bottom.abs=narea.top.abs+dim.y+self->padding.bottom;
  fgWindow_SetArea((fgWindow*)self,&narea); // This sends an FG_MOVE message to ourselves, which will propagate upwards when appropriate.
}

/*
fgChild* FG_FASTCALL fgGrid_HitElement(fgGrid* self, FABS x, FABS y)
{
  AbsRect dim;
  //AbsRect chdim;
  //char lasthit=0;
  fgChild* cur=self->window.element.root;
  if(!cur) cur=(fgChild*)self->window.rlist;
  ResolveRect((fgChild*)self,&dim);

  // This doesn't work
  if((self->window.element.flags&FGGRID_TILEX)!=0 == (self->window.element.flags&FGGRID_TILEY)!=0) { // Either TILEX and TILEY are both on or both off
    while(cur!=0 && (
      (x >= dim.right+cur->element.area.right.abs) || 
      (x < dim.left+cur->element.area.left.abs) || 
      (y >= dim.bottom+cur->element.area.bottom.abs) || 
      (y < dim.top+cur->element.area.top.abs))) 
      cur=cur->next;
    //while(cur!=0 && !lasthit)
    //{
    //  ResolveRectCache(&chdim,(fgElement*)cur,&dim);
    //  lasthit=HitAbsRect(&chdim,x,y);
    //  cur=cur->next;
    //}
  } else if((self->window.element.flags&FGGRID_TILEX)!=0) { // Only TILEX is active
    while(cur!=0 && ((x >= dim.right+cur->element.area.right.abs) || (x < dim.left+cur->element.area.left.abs)))
      cur=cur->next;
  } else { // only TILEY is active
    while(cur!=0 && ((y >= dim.bottom+cur->element.area.bottom.abs) || (y < dim.top+cur->element.area.top.abs)))
      cur=cur->next;
  }
  return cur;
}*/
