// Copyright ©2012 Black Sphere Studios
// For conditions of distribution and use, see copyright notice in "feathergui.h"

#include "fgGrid.h"

void FG_FASTCALL fgGrid_Init(fgGrid* self)
{
  fgWindow_Init(&self->window,0);
  memset(&self->margins,0,sizeof(AbsRect));
  memset(&self->dimensions,0,sizeof(AbsRect));
}
char FG_FASTCALL fgGrid_Message(fgGrid* self, FG_Msg* msg)
{
  switch(msg->type)
  {
  case FG_ADDCHILD:
  case FG_REMOVECHILD:
  case FG_ADDRENDERABLE:
    
    break;
  case FG_REMOVERENDERABLE:

    break;
  }
  return fgWindow_Message(self,msg);
}