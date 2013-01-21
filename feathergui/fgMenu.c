// Copyright ©2013 Black Sphere Studios
// For conditions of distribution and use, see copyright notice in "feathergui.h"

#include "fgMenu.h"
#include <time.h>

static const double DROPDOWN_TIME=0.5; // 500 milliseconds for dropdown

void FG_FASTCALL fgMenu_Init(fgMenu* self)
{
  FG_Msg msg = {0};
  assert(self!=0);
  fgWindow_Init(&self->window,0);
  self->window.element.destroy=&fgMenu_Destroy;
  self->dropdown=fgRoot_AllocAction(&fgMenu_DoDropdown,0,0);
  fgGrid_Init(&self->grid);
  fgWindow_VoidMessage(&self->grid,FG_SETPARENT,self);
  fgWindow_Init(&self->overlay,self);
  self->expanded=0;
  self->grid.window.message=&fgMenu_Message;
  msg.type=FG_SETFLAG;
  msg.otherint=FGWIN_NOCLIP;
  msg.otherintaux=1;
  fgMenu_Message((fgWindow*)self,&msg); // All menus are nonclipping by necessity unless it morphs into a menubar
}
void FG_FASTCALL fgMenu_Destroy(fgMenu* self)
{
  fgRoot_DeallocAction(fgSingleton(),self->dropdown);
  fgWindow_Destroy(&self->overlay);
  fgWindow_Destroy(&self->grid);
  fgWindow_Destroy(&self->window);
}

char FG_FASTCALL fgMenu_Message(fgMenu* self, const FG_Msg* msg)
{
  long long curtime;
  struct FG_MENUITEM* item;
  assert(self!=0 && msg!=0);

  switch(msg->type)
  {
  case FG_MOUSEMOVE:
    if(self->expanded!=0) break;
    curtime = clock()/(CLOCKS_PER_SEC/1000);
    item=(struct FG_MENUITEM*)fgGrid_HitElement(&self->grid,msg->x,msg->y);
    if(!item) // if it didn't hit anything, hide skin[1]
      (*self->skin[1]->message)(self->skin[1],FG_RSHOW,0);
    else // Otherwise reposition skin[1] properly
    {
      if(self->dropdown->arg!=item) // If position actually changed, update dropdown
      {
        self->dropdown->arg=item;
        self->dropdown->time=fgSingleton()->time+DROPDOWN_TIME;
        fgRoot_ModifyAction(fgSingleton(),self->dropdown);
      }
      (*self->skin[1]->message)(self->skin[1],FG_RSHOW,(void*)1);
    }
    break;
  case FG_MOUSEOFF:
    if(self->expanded!=0) break;
    (*self->skin[1]->message)(self->skin[1],FG_RSHOW,0);
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

FG_EXTERN void FG_FASTCALL fgMenu_DoDropdown(fgMenu* self)
{
  fgWindow_BasicMessage((fgWindow*)self,FG_GOTFOCUS); // By transfering focus to our child menu it will show itself.
}
