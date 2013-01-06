// Copyright ©2013 Black Sphere Studios
// For conditions of distribution and use, see copyright notice in "feathergui.h"

#include "fgRoot.h"
#include <stdlib.h>

VectWindow fgNonClipHit = {0};

// Recursive event injection function
char fgRoot_RInject(fgWindow* self, const FG_Msg* msg, AbsRect* area)
{
  AbsRect curarea;
  fgChild* cur = self->element.root;
  assert(msg!=0 && area!=0);
  ResolveRectCache(&curarea,&self->element.element,area);
  if((self->flags&FGWIN_HIDDEN)!=0 || !MsgHitAbsRect(msg,&curarea)) //If the event completely misses us, or we're hidden, we must reject it
    return 1;

  while(cur) // Try to inject to any children we have
  {
    //if((((fgWindow*)cur)->flags&FGWIN_HIDDEN)==0)
      if(!fgRoot_RInject((fgWindow*)cur,msg,&curarea)) // If the message is NOT rejected, return 0 immediately.
        return 0;
    cur=cur->next; // Otherwise the child rejected the message.
  } // If we get this far either we have no children, the event missed them all, or they all rejected the event.

  while(fgNonClipHit.l>0 && CompChildOrder((fgChild*)fgNonClipHit.p[0],(fgChild*)self)>0)
  { // If there's a nonclipping element that has an order larger than ours, try that first
    if(!(*fgNonClipHit.p[0]->message)(fgNonClipHit.p[0],msg)) return 0; // If its accepted, we return that
    Remove_VectWindow(&fgNonClipHit,0); // Otherwise, we remove the element from fgNonClipHit and repeat
  }
  return (*((fgWindow*)self)->message)((fgWindow*)self,msg);
}
int fgwincomp(const void* l,const void* r)
{
  return CompChildOrder(l,r);
}

char FG_FASTCALL fgRoot_Inject(fgRoot* self, const FG_Msg* msg)
{
  static AbsRect absarea = { 0,0,0,0 };
  CRect* mousearea;
  FG_UINT i;
  assert(self!=0);

  switch(msg->type)
  {
  case FG_KEYCHAR:
  case FG_KEYUP:
  case FG_KEYDOWN:
  case FG_MOUSESCROLL:
    if(!(*self->keymsghook)(msg))
      return 0;
    if(fgFocusedWindow)
      return (*fgFocusedWindow->message)(fgFocusedWindow,msg);
  case FG_MOUSEDOWN:
  case FG_MOUSEUP:
  case FG_MOUSEMOVE:
    mousearea=&self->mouse.element.element.area; // Update our mouse-tracking static
    mousearea->left.abs=mousearea->right.abs=(FABS)msg->x;
    mousearea->top.abs=mousearea->bottom.abs=(FABS)msg->y;

    if(fgFocusedWindow)
      if(!(*fgFocusedWindow->message)(fgFocusedWindow,msg))
        return 1;

    for(i=0; i < fgNonClipping.l; ++i) // Make a list of all nonclipping windows this event intersects
      if((fgNonClipping.p[i]->flags&FGWIN_HIDDEN)==0 && MsgHitCRect(msg,(fgChild*)fgNonClipping.p[i]))
        Add_VectWindow(&fgNonClipHit,fgNonClipping.p[i]);
    if(fgNonClipHit.l>0) // Sort them in top-down order
      qsort(fgNonClipHit.p,fgNonClipHit.l,sizeof(fgWindow*),&fgwincomp); 

    if(!fgRoot_RInject((fgWindow*)self,msg,&absarea))
      break; // it's important to break, not return, because we need to reset nonclip

    for(i=0; i < fgNonClipHit.l; ++i) // loop through remaining nonclipping windows and try to get one to accept the event
      if(!(*fgNonClipHit.p[i]->message)(fgNonClipHit.p[i],msg))
        break;
    
    if(msg->type==FG_MOUSEMOVE && fgLastHover!=0) // If we STILL haven't accepted a mousemove event, send a MOUSEOFF message if lasthover exists
    {
      fgWindow_BasicMessage(fgLastHover,FG_MOUSEOFF);
      fgLastHover=0;
    }
    fgNonClipHit.l=0;
    return 1;
  }
  fgNonClipHit.l=0;
  return 0;
}

char FG_FASTCALL fgRoot_BehaviorDefault(fgWindow* self, const FG_Msg* msg)
{
  assert(self!=0);
  return (*self->message)(self,msg);
}

void FG_FASTCALL fgTerminate(fgRoot* root)
{
  VirtualFreeChild((fgChild*)root);
}

void FG_FASTCALL fgRoot_RListRender(fgStatic* self, AbsRect* area)
{
  AbsRect curarea;
  assert(self!=0 && area!=0);
  ResolveRectCache(&curarea,&self->element.element,area);
  while(self)
  {
    (*self->message)(self,FG_RDRAW,&curarea);
    if(self->element.last) // Render everything backwards
      fgRoot_RListRender((fgStatic*)self->element.last,&curarea);
    self=(fgStatic*)self->element.prev;
  }
}

void FG_FASTCALL fgRoot_WinRender(fgWindow* self, AbsRect* area)
{
  AbsRect curarea;
  fgChild* cur = self->element.last;
  assert(area!=0);
  ResolveRectCache(&curarea,&self->element.element,area);

  if(self->flags&FGWIN_HIDDEN) // If we aren't visible, none of our children are either, so bail out.
    return;
  if(self->rlast)
    fgRoot_RListRender(self->rlast,&curarea);
  
  while(cur) // Render all our children (backwards)
  {
    fgRoot_WinRender((fgWindow*)cur,&curarea);
    cur=cur->prev; 
  } 
}

FG_EXTERN void FG_FASTCALL fgRoot_Render(fgRoot* self)
{
  static AbsRect absarea = { 0,0,0,0 };

  assert(self!=0);
  (*self->winrender)((fgWindow*)self, &absarea);
}

