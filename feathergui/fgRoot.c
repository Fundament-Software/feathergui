// Copyright ©2012 Black Sphere Studios
// For conditions of distribution and use, see copyright notice in "feathergui.h"

#include "fgRoot.h"

// Recursive event injection function
char fgRoot_RInject(fgChild* self, FG_Msg* msg, AbsRect* area)
{
  AbsRect curarea;
  fgChild* cur = self->root;
  ResolveRectCache(&curarea,&self->element.area,area);
  if(!MsgHitAbsRect(msg,&curarea)) //If the event completely misses our area we must reject it
    return 1;

  while(cur) // Try to inject to any children we have
  {
    if(!fgRoot_RInject(cur,msg,&curarea)) // If the message is NOT rejected, return 0 immediately.
      return 0;
    cur=cur->next; // Otherwise the child rejected the message.
  } // If we get this far either we have no children, the event missed them all, or they all rejected the event.

  if(self->destroy!=fgChild_Destroy) // We can only call the window message function if it's actually a window, which won't have fgChild_Destroy as the destructor
    // TODO: Compare this with as many nonclipping objects as possible
    return (*((fgWindow*)self)->message)((fgWindow*)self,msg);
  return 1; // If we have no window function we must reject the message
}

void FG_FASTCALL fgRoot_Inject(fgRoot* self, FG_Msg* msg)
{
  static AbsRect absarea = { 0,0,0,0 };

  switch(msg->type)
  {
  case FG_KEYCHAR:
  case FG_KEYUP:
  case FG_KEYDOWN:
  case FG_MOUSESCROLL:
    if(!(*self->keymsghook)(msg))
      return;
    if(fgFocusedWindow)
      (*fgFocusedWindow->message)(fgFocusedWindow,msg);
    break;
  case FG_MOUSEDOWN:
  case FG_MOUSEUP:
  case FG_MOUSEMOVE:
    if(fgFocusedWindow)
      if(!(*fgFocusedWindow->message)(fgFocusedWindow,msg))
        return;

    // TODO: Go through all nonclipping windows and add ones that intersect the event to an array
    // TODO: Sort array by order using heapsort

    if(!fgRoot_RInject(&self->gui,msg,&absarea))
      return;

    // TODO: loop through any remaining nonclipping windows in the array and try to get one to accept the event
    
    if(msg->type==FG_MOUSEMOVE && fgLastHover!=0) // If we STILL haven't accepted a mousemove event, send a MOUSEOFF message if lasthover exists
    {
      fgWindow_BasicMessage(fgLastHover,FG_MOUSEOFF);
      fgLastHover=0;
    }
    break;
  }
}

char FG_FASTCALL fgRoot_BehaviorDefault(fgWindow* self, FG_Msg* msg)
{
  return (*self->message)(self,msg);
}

void FG_FASTCALL fgTerminate(fgRoot* root)
{
  (*root->gui.destroy)(root);
}

void FG_FASTCALL fgRoot_RListRender(fgStatic* self, AbsRect* area)
{
  AbsRect curarea;
  ResolveRectCache(&curarea,&self->element.element.area,area);
  while(self)
  {
    (*self->message)(self,FG_RDRAW,&curarea);
    if(self->element.root)
      fgRoot_RListRender(self->element.root,&curarea);
    self=(fgStatic*)self->element.next;
  }
}

void FG_FASTCALL fgRoot_WinRender(fgChild* self, AbsRect* area)
{
  AbsRect curarea;
  fgChild* cur = self->root;
  ResolveRectCache(&curarea,&self->element.area,area);

  if(self->destroy!=fgChild_Destroy) // We only have statics if we're a window
  {
    if(((fgWindow*)self)->flags&8) // If we aren't visible, none of our children are either, so bail out.
      return;
    if(((fgWindow*)self)->rlist)
      fgRoot_RListRender(((fgWindow*)self)->rlist,&curarea);
  }
  
  while(cur) // Render all our children
  {
    fgRoot_WinRender(cur,&curarea);
    cur=cur->next; 
  } 
}

FG_EXTERN void FG_FASTCALL fgRoot_Render(fgRoot* self)
{
  static AbsRect absarea = { 0,0,0,0 };

  fgRoot_WinRender(self, &absarea);
}

