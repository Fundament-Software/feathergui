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
  assert(self!=0 && msg!=0);
  switch(msg->type)
  {
  case FG_ADDCHILD:
  case FG_REMOVECHILD:
  case FG_ADDSTATIC:
    
    break;
  case FG_REMOVESTATIC:

    break;
  }
  return fgWindow_Message((fgWindow*)self,msg);
}

fgChild* FG_FASTCALL fgGrid_HitElement(fgGrid* self, FABS x, FABS y)
{
  return 0;
}
