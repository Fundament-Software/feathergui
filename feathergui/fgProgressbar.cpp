// Copyright �2016 Black Sphere Studios
// For conditions of distribution and use, see copyright notice in "feathergui.h"

#include "fgProgressbar.h"
#include "fgSkin.h"
#include "feathercpp.h"

FG_EXTERN void FG_FASTCALL fgProgressbar_Init(fgProgressbar* BSS_RESTRICT self, fgFlag flags, fgElement* BSS_RESTRICT parent, fgElement* BSS_RESTRICT prev, const fgTransform* transform)
{
  fgElement_InternalSetup(*self, flags, parent, prev, transform, (FN_DESTROY)&fgProgressbar_Destroy, (FN_MESSAGE)&fgProgressbar_Message);
}

FG_EXTERN void FG_FASTCALL fgProgressbar_Destroy(fgProgressbar* self)
{
  fgControl_Destroy(&self->control);
}

FG_EXTERN size_t FG_FASTCALL fgProgressbar_Message(fgProgressbar* self, const FG_Msg* msg)
{
  static const fgTransform BAR_ELEMENT = { 0,0,0,0,0,0,0,1.0,0,0,0,0,0 };

  assert(self != 0 && msg != 0);
  switch(msg->type)
  {
  case FG_CONSTRUCT:
    fgControl_Message(&self->control, msg);
    fgElement_Init(&self->bar, FGELEMENT_BACKGROUND | FGELEMENT_IGNORE, *self, 0, &BAR_ELEMENT);
    fgElement_AddPreChild(*self, &self->bar);
    fgText_Init(&self->text, 0, 0, 0, FGELEMENT_EXPAND | FGELEMENT_IGNORE, *self, 0, &fgTransform_CENTER);
    fgElement_AddPreChild(*self, self->text);
    self->value = 0.0f;
    return 1;
  case FG_ADDITEM:
    if(msg->other)
      _sendmsg<FG_ADDCHILD, void*>(&self->bar, msg->other);
    else
      fgElement_Clear(&self->bar);
    return 1;
  case FG_SETSTATE:
    if(msg->otherf != self->value)
    {
      self->value = msg->otherf;
      CRect area = self->bar.transform.area;
      area.right.rel = self->value;
      _sendmsg<FG_SETAREA, void*>(&self->bar, &area);
    }
    return 1;
  case FG_GETSTATE:
    return *reinterpret_cast<size_t*>(&self->value);
  case FG_SETTEXT:
  case FG_SETFONT:
  case FG_SETCOLOR:
  case FG_GETTEXT:
  case FG_GETFONT:
  case FG_GETCOLOR:
    return fgPassMessage(self->text, msg);
  case FG_GETCLASSNAME:
    return (size_t)"fgProgressbar";
  }
  return fgControl_Message(&self->control, msg);
}