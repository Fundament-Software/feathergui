// Copyright ©2013 Black Sphere Studios
// For conditions of distribution and use, see copyright notice in "feathergui.h"

#include "fgGrid.h"

void FG_FASTCALL fgGrid_Init(fgGrid* self)
{
  assert(self!=0);
  fgWindow_Init(&self->window,0);
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
    if(msg->other!=0) // If it doesn't come from us, reject it.
      return 1;
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
  AbsVec mov={0};
  FABS dx,dy;
  if(p->next) // If there is a p->next (there will be if its not the last one), initialize values from it.
  {
    rt=&p->next->element.area;
    mov.x=rt->right.abs+self->margins.right;
    mov.y=rt->bottom.abs+self->margins.bottom;
  }
  rt=&p->element.area;

  while(p)
  {
    mov.x+=self->margins.left; // TODO: Put in a check here on the appropriate axis to wrap around if necessary
    mov.y+=self->margins.top;
    dx=rt->right.abs-rt->left.abs;
    dy=rt->bottom.abs-rt->top.abs;
    rt->left.abs=mov.x;
    rt->top.abs=mov.y;
    rt->right.abs=mov.x+dx;
    rt->right.abs=mov.y+dy;
    mov.x+=dx+self->margins.right;
    mov.y+=dy+self->margins.bottom;
    p=p->prev;
  }
  // Then we run through the the image's prev values until we hit the root, repositioning everything

  // Using the root element, we can then expand the fgGrid object as necessary

  fgWindow_BasicMessage(self,FG_MOVE); // Now we send an FG_MOVE message to ourselves, which will propagate upwards when appropriate.
}

fgChild* FG_FASTCALL fgGrid_HitElement(fgGrid* self, FABS x, FABS y)
{
  // This gets optimized depending on if TILEX, TILEY, or both are active.
  return 0;
}
