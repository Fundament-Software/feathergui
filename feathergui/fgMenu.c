// Copyright ©2012 Black Sphere Studios
// For conditions of distribution and use, see copyright notice in "feathergui.h"

#include "fgMenu.h"
#include <time.h>

void FG_FASTCALL fgMenu_Init(fgMenu* self)
{
  FG_Msg msg = {0};
  assert(self!=0);
  fgGrid_Init(&self->grid);
  self->grid.window.message=&fgMenu_Message;
  msg.type=FG_SETFLAG;
  msg.otherint=FGWIN_NOCLIP;
  msg.otherintaux=1;
  fgMenu_Message((fgWindow*)self,&msg); // All menus are nonclipping by necessity unless it morphs into a menubar
}
char FG_FASTCALL fgMenu_Message(fgMenu* self, const FG_Msg* msg)
{
  long long curtime;
  AbsRect r;
  assert(self!=0 && msg!=0);

  switch(msg->type)
  {
  case FG_MOUSEMOVE:
    if(self->expanded!=0) break;
    curtime = clock()/(CLOCKS_PER_SEC/1000);
    ResolveRect((fgChild*)self,&r);
    // Reposition skin[1] properly
    // If position actually changed, change prevtime
    fgStatic_Message(self->skin[1],FG_RSHOW,(void*)1);
    break;
  case FG_MOUSEOFF:
    if(self->expanded!=0) break;
    fgStatic_Message(self->skin[1],FG_RSHOW,0);
    break;
  case FG_ADDITEM:
    return 0;
  case FG_GOTFOCUS:
    fgWindow_IntMessage((fgWindow*)self,FG_SHOW,1);
    break;
  case FG_LOSTFOCUS:
    fgWindow_IntMessage((fgWindow*)self,FG_SHOW,0);
    break;
  case FG_ADDSTATIC:
    if(msg->otheraux<4)
      DoSkinCheck((fgWindow*)self,self->skin,msg);
    break;
  }
  return fgGrid_Message((fgWindow*)self,msg);
}