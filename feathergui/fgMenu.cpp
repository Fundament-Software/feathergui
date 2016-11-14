// Copyright �2016 Black Sphere Studios
// For conditions of distribution and use, see copyright notice in "feathergui.h"

#include "fgMenu.h"
#include "feathercpp.h"

static const char* SUBMENU_NAME = "Submenu";
static const char* MENU_NAME = "Menu";

void FG_FASTCALL fgMenu_Init(fgMenu* self, fgElement* BSS_RESTRICT parent, fgElement* BSS_RESTRICT next, const char* name, fgFlag flags, const fgTransform* transform, unsigned short units)
{
  assert(self != 0);
  fgElement_InternalSetup(*self, parent, next, name, (!flags ? (FGELEMENT_EXPANDY | FGBOX_TILEX) : flags), (!transform ? &fgTransform { { 0, 0, 0, 0, 0, 1.0, 0, 0 }, 0, {0,0,0,0} } : transform), units, (fgDestroy)&fgMenu_Destroy, (fgMessage)&fgMenu_Message);
}

void FG_FASTCALL fgMenu_Destroy(fgMenu* self)
{
  fgElement_Destroy(&self->arrow); // Both of these must be manually destroyed because they are ghost entities not actually attached to the menu.
  fgElement_Destroy(&self->seperator);
  fgBox_Destroy(&self->box); 
  //fgRoot_DeallocAction(fgSingleton(),self->dropdown);
}

inline void FG_FASTCALL fgMenu_Show(fgMenu* self, bool show)
{
  assert(self != 0);
  fgFlag set = show ? (self->box->flags & (~FGELEMENT_HIDDEN)) : (self->box->flags | FGELEMENT_HIDDEN);
  fgIntMessage(*self, FG_SETFLAGS, set, 0);

  fgMenu* submenu = (fgMenu*)self->box->GetSelectedItem();
  if(submenu)
  {
    if(!show)
      self->expanded = 0;
    fgMenu_Show(submenu, show);
  }
}

inline fgMenu* fgMenu_ExpandMenu(fgMenu* self, fgMenu* submenu)
{
  if(submenu != self->expanded)
  {
    if(self->expanded)
      fgMenu_Show(self->expanded, false);
    self->expanded = submenu;
    if(self->expanded)
      fgMenu_Show(self->expanded, true);
  }

  return submenu;
}

size_t FG_FASTCALL fgMenu_Message(fgMenu* self, const FG_Msg* msg)
{
  assert(self != 0 && msg != 0);

  switch(msg->type)
  {
  case FG_CONSTRUCT:
    fgBox_Message(&self->box, msg);
    self->expanded = 0;
    fgElement_Init(&self->arrow, 0, 0, "Menu$arrow", FGELEMENT_IGNORE | FGELEMENT_BACKGROUND | FGELEMENT_EXPAND, &fgTransform { {0,0,0,0.5,0,0,0,0.5}, 0, {1.0,0.5} }, 0);
    fgElement_Init(&self->seperator, 0, 0, "Menu$seperator", FGELEMENT_IGNORE | FGELEMENT_EXPAND, &fgTransform { { 0,0,0,0,0,0,0,0 }, 0, { 0,0 } }, 0);
    return FG_ACCEPT;
  case FG_MOUSEUP:
    if(fgCaptureWindow == *self)
    {
      AbsRect cache;
      fgElement* child = fgElement_GetChildUnderMouse(*self, msg->x, msg->y, &cache);
      if(!MsgHitAbsRect(msg, &cache))
        fgCaptureWindow = 0;
      else if(child != 0 && !fgMenu_ExpandMenu(self, (fgMenu*)child->GetSelectedItem()))
      {
        _sendmsg<FG_ACTION, void*>(*self, child);
        fgCaptureWindow = 0;
      }
    }
    return fgControl_Message((fgControl*)self, msg);
  case FG_MOUSEMOVE:
    if(fgCaptureWindow != *self)
      return fgControl_Message((fgControl*)self, msg);
    break;
  case FG_MOUSEDOWN:
  {
    AbsRect cache;
    fgElement* child = fgElement_GetChildUnderMouse(*self, msg->x, msg->y, &cache);
    if(fgCaptureWindow == *self)
    {
      if(self->expanded)
        fgMenu_Show(self->expanded, false);
      self->expanded = 0;
      fgCaptureWindow = 0;
      return fgControl_Message((fgControl*)self, msg);
    }
    fgCaptureWindow = *self;
    if(child != 0)
      fgMenu_ExpandMenu(self, (fgMenu*)child->GetSelectedItem());
  }
    return fgControl_Message((fgControl*)self, msg);
  case FG_ADDITEM:
    switch(msg->subtype)
    {
    case FGADDITEM_DEFAULT:
      break;
    case FGADDITEM_ELEMENT:
    {
      fgElement* menuitem = fgroot_instance->backend.fgCreate("MenuItem", *self, 0, 0, 0, 0, 0);
      fgPassMessage(menuitem, msg);
      return (size_t)menuitem;
    }
    case FGADDITEM_TEXT:
    {
      fgElement* menuitem = fgroot_instance->backend.fgCreate("MenuItem", *self, 0, 0, 0, 0, 0);
      menuitem->SetText((const char*)msg->other);
      return (size_t)menuitem;
    }
    }
    return 0;
  case FG_GETCLASSNAME:
    return (size_t)MENU_NAME;
  }
  return fgSubmenu_Message(self, msg);
}

void FG_FASTCALL fgSubmenu_Init(fgMenu* self, fgElement* BSS_RESTRICT parent, fgElement* BSS_RESTRICT next, const char* name, fgFlag flags, const fgTransform* transform, unsigned short units)
{
  assert(self != 0);
  if(!transform) transform = (parent != 0 && parent->parent != 0 && parent->parent->GetClassName() == MENU_NAME) ? &fgTransform { { 0, 0, 0, 1.0, 0, 0, 0, 1.0 }, 0, { 0,0,0,0 } } : &fgTransform { { 0, 1.0, 0, 0, 0, 1.0, 0, 0 }, 0, { 0,0,0,0 } };
  fgElement_InternalSetup(*self, parent, next, name, (!flags ? (FGELEMENT_NOCLIP | FGELEMENT_BACKGROUND | FGELEMENT_HIDDEN | FGBOX_TILEY | FGELEMENT_EXPAND) : flags), transform, units, (fgDestroy)&fgMenu_Destroy, (fgMessage)&fgSubmenu_Message);
}

size_t FG_FASTCALL fgSubmenu_Message(fgMenu* self, const FG_Msg* msg)
{
  switch(msg->type)
  {
  case FG_CONSTRUCT:
    fgBox_Message(&self->box, msg);
    self->expanded = 0;
    fgElement_Init(&self->arrow, 0, 0, "Submenu$arrow", FGELEMENT_IGNORE | FGELEMENT_BACKGROUND | FGELEMENT_EXPAND, &fgTransform { { 0,0,0,0.5,0,0,0,0.5 }, 0, { 1.0,0.5 } }, 0);
    fgElement_Init(&self->seperator, 0, 0, "Submenu$seperator", FGELEMENT_IGNORE | FGELEMENT_EXPAND, &fgTransform { { 0,0,0,0,0,1.0,0,0 }, 0, { 0,0 } }, 0);
    return FG_ACCEPT;
  case FG_MOUSEUP:
  case FG_MOUSEDOWN:
  {
    AbsRect cache;
    fgElement* child = fgElement_GetChildUnderMouse(*self, msg->x, msg->y, &cache);
    if(MsgHitAbsRect(msg, &cache))
    {
      if(child != 0 && !fgMenu_ExpandMenu(self, (fgMenu*)child->GetSelectedItem()) && msg->type == FG_MOUSEUP)
        _sendmsg<FG_ACTION, void*>(*self, child);
      else
        return FG_ACCEPT;
    }
    if(fgCaptureWindow == *self || fgCaptureWindow->GetClassName() == MENU_NAME)
    {
      fgMenu* menu = reinterpret_cast<fgMenu*>(fgCaptureWindow);
      if(menu->expanded)
        fgMenu_Show(menu->expanded, false);
      menu->expanded = 0;
      fgCaptureWindow = 0;
    }
  }
  return FG_ACCEPT;
  case FG_MOUSEMOVE:
  {
    AbsRect cache;
    fgElement* child = fgElement_GetChildUnderMouse(*self, msg->x, msg->y, &cache);
    if(child != 0)
      fgMenu_ExpandMenu(self, (fgMenu*)child->GetSelectedItem());
  }
  break;
  case FG_ADDITEM:
    if(!msg->other)
    {
      fgElement* item = self->seperator.Clone(0);
      item->SetParent(*self);
      return (size_t)item;
    }
    switch(msg->subtype)
    {
    case FGADDITEM_DEFAULT:
      break;
    case FGADDITEM_ELEMENT:
    {
      fgElement* menuitem = fgroot_instance->backend.fgCreate("MenuItem", *self, 0, 0, 0, 0, 0);
      fgPassMessage(menuitem, msg);
      return (size_t)menuitem;
    }
    case FGADDITEM_TEXT:
    {
      fgElement* menuitem = fgroot_instance->backend.fgCreate("MenuItem", *self, 0, 0, 0, 0, 0);
      menuitem->SetText((const char*)msg->other);
      return (size_t)menuitem;
    }
    }
    return 0;
  case FG_DRAW:
    fgBox_Message(&self->box, msg);
    if(!(msg->subtype & 1))
    {
      fgElement* cur = self->box->root;
      while(cur)
      {
        if(!(cur->flags & FGELEMENT_BACKGROUND) && cur->GetSelectedItem())
        {
          AbsRect rect;
          ResolveRectCache(cur, &rect, (const AbsRect*)msg->other, &self->box->padding);
          self->arrow.Draw(&rect, msg->otherint);
        }
        cur = cur->next;
      }
    }
    return FG_ACCEPT;
  case FG_GETSELECTEDITEM:
    return (size_t)self->expanded;
  case FG_GETCLASSNAME:
    return (size_t)SUBMENU_NAME; // This allows you to properly differentiate a top level menu glued to the top of a window from a submenu, like a context menu.
  }
  return fgBox_Message(&self->box, msg);
}

void FG_FASTCALL fgMenuItem_Init(fgMenuItem* self, fgElement* BSS_RESTRICT parent, fgElement* BSS_RESTRICT next, const char* name, fgFlag flags, const fgTransform* transform, unsigned short units)
{
  static const fgTransform MENU_TRANSFORM = fgTransform { { 0,0,0,0,0,1,0,0 }, 0, { 0,0,0,0 } };
  if(!transform) transform = (parent != 0 && parent->GetClassName() == MENU_NAME) ? &fgTransform_EMPTY : &MENU_TRANSFORM;
  if(!flags) flags = FGELEMENT_EXPAND | FGELEMENT_NOCLIP;
  assert(self != 0);
  fgElement_InternalSetup(&self->element, parent, next, name, flags, transform, units, (fgDestroy)&fgElement_Destroy, (fgMessage)&fgMenuItem_Message);
}

size_t FG_FASTCALL fgMenuItem_Message(fgMenuItem* self, const FG_Msg* msg)
{
  switch(msg->type)
  {
  case FG_CONSTRUCT:
    fgElement_Message(&self->element, msg);
    fgText_Init(&self->text, &self->element, 0, "MenuItem$text", FGELEMENT_EXPAND, 0, 0);
    self->submenu = 0;
    return FG_ACCEPT;
  case FG_ADDCHILD:
  {
    size_t r = fgElement_Message(&self->element, msg);
    if(r != 0 && ((fgElement*)msg->other)->GetClassName() == SUBMENU_NAME)
      self->submenu = (fgMenu*)msg->other;
    return r;
  }
  case FG_GETSELECTEDITEM:
    return (size_t)self->submenu;
  case FG_SETTEXT:
  case FG_SETFONT:
  case FG_SETLINEHEIGHT:
  case FG_SETLETTERSPACING:
  case FG_SETCOLOR:
  case FG_GETTEXT:
  case FG_GETFONT:
  case FG_GETLINEHEIGHT:
  case FG_GETLETTERSPACING:
  case FG_GETCOLOR:
    return fgPassMessage(self->text, msg);
  case FG_GETCLASSNAME:
    return (size_t)"MenuItem";
  }
  return fgElement_Message(&self->element, msg);
}
