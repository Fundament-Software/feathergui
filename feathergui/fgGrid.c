// Copyright ©2013 Black Sphere Studios
// For conditions of distribution and use, see copyright notice in "feathergui.h"

#include "fgGrid.h"

void FG_FASTCALL fgGrid_Init(fgGrid* self, fgWindow* parent, const fgElement* element, FG_UINT id, fgFlag flags)
{
  assert(self!=0);
  fgWindow_Init(&self->window,parent,element,id,flags);
  self->window.message=&fgGrid_Message;
  memset(&self->margins,0,sizeof(AbsRect));
}
char FG_FASTCALL fgGrid_Message(fgGrid* self, const FG_Msg* msg)
{
  fgChild* hold;
  assert(self!=0 && msg!=0);

  switch(msg->type)
  {
  case FG_MOVE:
    hold=(fgChild*)self;
    if(msg->other!=0) // If its external we only propagate if we are expanding an axis in response.
    {
      if(hold->flags&(FGGRID_EXPANDX|FGGRID_EXPANDY)) // Should we expand?
      {
        if(hold->flags&FGGRID_EXPANDX) fgChild_ExpandX((fgChild*)self,msg->other); // Expand X axis if needed
        if(hold->flags&FGGRID_EXPANDY) fgChild_ExpandY((fgChild*)self,msg->other); // Expand Y axis if needed
        if(hold->parent!=0) // Propagate if we have a parent to propagate to.
          fgWindow_VoidMessage((fgWindow*)hold->parent,FG_MOVE,self);
      }
      return 0;
    }
    break; // Otherwise let the default handler deal with it
  case FG_ADDCHILD:
  case FG_ADDSTATIC:
    if(!fgWindow_Message((fgWindow*)self,msg)) // First we have the window insert the image
      fgGrid_Reposition(self,(fgChild*)msg->other); // Then reposition the image
    else // If it was rejected, propagate the rejection upwards
      return 1;
    return 0;
  case FG_REMOVECHILD:
  case FG_REMOVESTATIC:
    hold=((fgChild*)msg->other)->prev; // First we save the image afterwards
    if(!fgWindow_Message((fgWindow*)self,msg)) // Then we have the window remove the image
    {
      if(hold!=0)
        fgGrid_Reposition(self,hold); // Then reposition the image that was right after it, if there is one, which will fix the grid.
      return 0;
    }
    return 1;
  }
  return fgWindow_Message((fgWindow*)self,msg);
}
FG_EXTERN void FG_FASTCALL fgGrid_Reposition(fgGrid* self, fgChild* p)
{
  CRect* rt;
  CRect narea;
  AbsVec mov={0};
  FABS dx,dy,dim;
  if(p->next) // If there is a p->next (there will be if its not the last one), initialize values from it.
  {
    rt=&p->next->element.area;
    mov.x=rt->right.abs+self->margins.x;
    mov.y=rt->bottom.abs+self->margins.y;
  } // Then we run through the the image's prev values until we hit the root, repositioning everything
  
  if((self->window.element.flags&FGGRID_TILEX)!=0 == (self->window.element.flags&FGGRID_TILEY)!=0) { // Either TILEX and TILEY are both on or both off
    if(self->window.element.flags&FGGRID_EXPANDX) { // If we're expanding on the X axis we must grow downward and then loop around.
      dim=self->window.element.element.area.bottom.abs-self->window.element.element.area.top.abs-self->margins.y;
      mov.x=self->padding.left;
      while(p)
      {
        mov.y+=self->padding.top;
        if(mov.y>dim) mov.x=self->padding.left+self->margins.x+rt->right.abs;
        rt=&p->element.area; // set rt down here so we use the previous value above
        dx=rt->right.abs-rt->left.abs;
        dy=rt->bottom.abs-rt->top.abs;
        rt->left.abs=mov.x;
        rt->top.abs=mov.y;
        rt->right.abs=mov.x+dx;
        rt->bottom.abs=mov.y+dy;
        mov.y+=dy+self->margins.y;
        p=p->prev;
      }
    } else { // EXPANDY is the default even if its not specified. If we expand on the Y axis, we must grow rightward.
      dim=self->window.element.element.area.right.abs-self->window.element.element.area.left.abs-self->margins.x;
      mov.y=self->padding.top;
      while(p)
      {
        mov.x+=self->padding.left;
        if(mov.x>dim) mov.y=self->padding.top+self->margins.y+rt->bottom.abs;
        rt=&p->element.area; // set rt down here so we use the previous value above
        dx=rt->right.abs-rt->left.abs;
        dy=rt->bottom.abs-rt->top.abs;
        rt->left.abs=mov.x;
        rt->top.abs=mov.y;
        rt->right.abs=mov.x+dx;
        rt->bottom.abs=mov.y+dy;
        mov.x+=dx+self->margins.x;
        p=p->prev;
      }
    }
  } else if((self->window.element.flags&FGGRID_TILEX)!=0) { // Only TILEX is active
    while(p)
    {
      rt=&p->element.area;
      mov.x+=self->padding.left;
      dx=rt->right.abs-rt->left.abs;
      rt->left.abs=mov.x;
      rt->right.abs=mov.x+dx;
      mov.x+=dx+self->margins.x;
      p=p->prev;
    }
  } else { // only TILEY is active
    while(p)
    {
      rt=&p->element.area;
      mov.y+=self->padding.top;
      dy=rt->bottom.abs-rt->top.abs;
      rt->top.abs=mov.y;
      rt->bottom.abs=mov.y+dy;
      mov.y+=dy+self->margins.y;
      p=p->prev;
    }
  }

  narea = self->window.element.element.area; // At this point, mov is equal to the dimensions we want to set ourselves to.
  if(self->window.element.flags&FGGRID_EXPANDX) narea.right.abs=narea.left.abs+mov.x;
  if(self->window.element.flags&FGGRID_EXPANDY) narea.bottom.abs=narea.top.abs+mov.y;
  fgWindow_SetArea((fgWindow*)self,&narea); // This sends an FG_MOVE message to ourselves, which will propagate upwards when appropriate.
}

fgChild* FG_FASTCALL fgGrid_HitElement(fgGrid* self, FABS x, FABS y)
{
  AbsRect dim;
  //AbsRect chdim;
  //char lasthit=0;
  fgChild* cur=self->window.element.root;
  if(!cur) cur=(fgChild*)self->window.rlist;
  ResolveRect((fgChild*)self,&dim);

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
}
