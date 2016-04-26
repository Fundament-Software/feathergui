// Copyright �2016 Black Sphere Studios
// For conditions of distribution and use, see copyright notice in "feathergui.h"

#include "fgSlider.h"
#include "bss-util\bss_util.h"
#include "feathercpp.h"

void FG_FASTCALL fgSlider_Init(fgSlider* BSS_RESTRICT self, size_t range, fgFlag flags, fgElement* BSS_RESTRICT parent, fgElement* BSS_RESTRICT prev, const fgTransform* transform)
{
  fgElement_InternalSetup(*self, flags, parent, prev, transform, (FN_DESTROY)&fgSlider_Destroy, (FN_MESSAGE)&fgSlider_Message);
  self->range = range;
}
void FG_FASTCALL fgSlider_Destroy(fgSlider* self)
{
  fgControl_Destroy(&self->control);
}
size_t FG_FASTCALL fgSlider_Message(fgSlider* self, const FG_Msg* msg)
{
  assert(self != 0 && msg != 0);
  switch(msg->type)
  {
  case FG_CONSTRUCT:
    fgControl_Message(&self->control, msg);
    fgElement_Init(&self->slider, FGELEMENT_EXPAND | FGELEMENT_IGNORE, *self, 0, &fgTransform_CENTER);
    fgElement_AddPreChild(*self, &self->slider);
    self->value = 0;
    self->range = 0;
    return 1;
  case FG_SETSTATE:
  {
    if(msg->otheraux)
    {
      if(self->range == msg->otherint)
        return 1;
      self->range = msg->otherint;
    }
    else
    {
      if(self->value == msg->otherint)
        return 1;
      self->value = msg->otherint;
    }
    CRect area = self->slider.transform.area;
    area.left.rel = area.right.rel = !self->range ? 0.0 : ((FREL)self->value / (FREL)self->range);
    _sendmsg<FG_SETAREA, void*>(&self->slider, &area);
    return 1;
  }
  case FG_MOUSEMOVE:
  case FG_MOUSEUP:
    if(fgCaptureWindow != *self)
      break;
  case FG_MOUSEDOWN:
  {
    AbsRect out;
    ResolveRect(*self, &out);
    out.left += self->control.element.padding.left;
    out.top += self->control.element.padding.top;
    out.right -= self->control.element.padding.right;
    out.bottom -= self->control.element.padding.bottom;

    double x = (msg->x - out.left) / (out.right - out.left); // we need all the precision in a double here
    size_t value = bss_util::fFastRound(bssclamp(x, 0.0, 1.0)*self->range); // Clamp to [0,1], multiply into [0, range], then round to nearest integer.
    fgIntMessage(*self, FG_SETSTATE, value, 0);
  }
    break;
  case FG_GETSTATE:
    return msg->otherint ? self->range : self->value;
  case FG_GETCLASSNAME:
    return (size_t)"fgSlider";
  }
  return fgControl_HoverMessage(&self->control, msg);
}