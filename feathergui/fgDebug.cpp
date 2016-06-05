// Copyright �2016 Black Sphere Studios
// For conditions of distribution and use, see copyright notice in "feathergui.h"

#include "fgDebug.h"
#include "fgRoot.h"
#include "feathercpp.h"
#include "bss-util/cDynArray.h"

fgDebug* fgdebug_instance = nullptr;

void FG_FASTCALL fgDebug_Init(fgDebug* BSS_RESTRICT self, fgElement* BSS_RESTRICT parent, fgElement* BSS_RESTRICT next, const char* name, fgFlag flags, const fgTransform* transform)
{
  if(fgdebug_instance != nullptr)
    VirtualFreeChild(*fgdebug_instance);

  fgdebug_instance = self;
  fgElement_InternalSetup(&self->element, parent, next, name, flags, transform, (fgDestroy)&fgDebug_Destroy, (fgMessage)&fgDebug_Message);
}

FG_EXTERN void FG_FASTCALL fgDebug_ClearLog(fgDebug* self)
{
  for(size_t i = 0; i < self->messagestrings.l; ++i)
    free(self->messagestrings.p[i]);
  self->messagestrings.l = 0;
  self->messagelog.l = 0;
}

fgDebug* FG_FASTCALL fgDebug_Get()
{
  return fgdebug_instance;
}

void FG_FASTCALL fgDebug_Destroy(fgDebug* self)
{
  fgDebug_Hide();
  fgdebug_instance = 0;
  fgDebug_ClearLog(self);
  ((bss_util::cDynArray<char*>&)self->messagestrings).~cDynArray();
  ((bss_util::cDynArray<fgDebugMessage>&)self->messagelog).~cDynArray();
  fgElement_Destroy(&self->element);
}
size_t FG_FASTCALL fgDebug_Message(fgDebug* self, const FG_Msg* msg)
{
  switch(msg->type)
  {
  case FG_CONSTRUCT:
    fgElement_Message(&self->element, msg);
    fgTreeView_Init(&self->elements, *self, 0, "fgDebug:elements", 0, &fgTransform { { -300,1,0,0,0,1,0,1 }, 0, { 0,0,0,0 } });
    fgText_Init(&self->properties, *self, 0, "fgDebug:properties", FGELEMENT_HIDDEN, &fgTransform { { -300,1,-200,1,0,1,0,1 }, 0, { 0,0,0,0 } });
    fgTreeView_Init(&self->messages, *self, 0, "fgDebug:messages", 0, &fgTransform { { 0,0,0,0,200,0,0,1 }, 0, { 0,0,0,0 } });
    fgText_Init(&self->contents, *self, 0, "fgDebug:contents", FGELEMENT_HIDDEN, &fgTransform { { 0,0,-200,1,200,0,0,1 }, 0, { 0,0,0,0 } });
    self->messagelog.p = 0;
    self->messagelog.l = 0;
    self->messagelog.s = 0;
    self->messagestrings.p = 0;
    self->messagestrings.l = 0;
    self->messagestrings.s = 0;
    self->depth = 0;
    self->hover = 0;
    self->behaviorhook = &fgRoot_BehaviorDefault;
    return FG_ACCEPT;
  case FG_DRAW:
    if(self->hover != 0)
    {
      AbsRect r;
      ResolveRect(self->hover, &r);
      AbsRect clientarea = r;
      clientarea.left += self->hover->padding.left;
      clientarea.top += self->hover->padding.top;
      clientarea.right -= self->hover->padding.right;
      clientarea.bottom -= self->hover->padding.bottom;
      AbsRect totalarea = r;
      totalarea.left -= self->hover->margin.left;
      totalarea.top -= self->hover->margin.top;
      totalarea.right += self->hover->margin.right;
      totalarea.bottom += self->hover->margin.bottom;

      fgDrawResource(0, &CRect { 0,0,0,0,0,0,0,0 }, 0x666666FF, 0, 0.0f, &totalarea, 0, &AbsVec { 0,0 }, FGRESOURCE_ROUNDRECT);
      fgDrawResource(0, &CRect { 0,0,0,0,0,0,0,0 }, 0x6666FFFF, 0, 0.0f, &r, 0, &AbsVec { 0,0 }, FGRESOURCE_ROUNDRECT);
      fgDrawResource(0, &CRect { 0,0,0,0,0,0,0,0 }, 0x6666FF66, 0, 0.0f, &clientarea, 0, &AbsVec { 0,0 }, FGRESOURCE_ROUNDRECT);
    }
    break;
  case FG_GETCLASSNAME:
    return (size_t)"fgDebug";
  }
  return fgElement_Message(&self->element, msg);
}

char* fgDebug_CopyText(fgDebug* self, const char* s)
{
  if(!s) return 0;
  size_t len = strlen(s) + 1;
  char* ret = (char*)malloc(len);
  memcpy(ret, s, len);
  ((bss_util::cDynArray<char*>&)self->messagestrings).AddConstruct(ret);
  return ret;
}

size_t FG_FASTCALL fgTreeItem_DebugMessage(fgTreeItem* self, const FG_Msg* msg)
{
  switch(msg->type)
  {
  case FG_MOUSEON:
    fgdebug_instance->hover = (fgElement*)self->control.element.userdata;
    return FG_ACCEPT;
  case FG_MOUSEOFF:
    fgdebug_instance->hover = 0;
    return FG_ACCEPT;
  }

  return fgTreeItem_Message(self, msg);
}

const char* fgDebug_GetElementName(fgDebug* self, fgElement* e)
{
  if(!e)
    return 0;
  const char* r = e->GetName();
  return !r ? e->GetClassName() : fgDebug_CopyText(self, r);
}

size_t FG_FASTCALL fgRoot_BehaviorDebug(fgElement* self, const FG_Msg* msg)
{
  assert(fgdebug_instance->behaviorhook != &fgRoot_BehaviorDebug);
  size_t index = fgDebug_LogMessage(fgdebug_instance, msg, 0, fgdebug_instance->depth);
  ++fgdebug_instance->depth;
  size_t r = (*fgdebug_instance->behaviorhook)(self, msg);
  --fgdebug_instance->depth;
  fgdebug_instance->messagelog.p[index].value = r;
  return r;
}

void FG_FASTCALL fgDebug_TreeInsert(fgElement* parent, fgElement* element, fgElement* treeview)
{
  fgElement* root = parent;

  if(treeview != 0)
  {
    root = fgCreate("fgTreeItem", parent, 0, 0, FGELEMENT_EXPAND, &fgTransform_EMPTY);
    root->message = (fgMessage)&fgTreeItem_DebugMessage;
    root->userdata = element;
  }
  else
    treeview = !fgdebug_instance ? parent : *fgdebug_instance;

  fgElement* text = fgCreate("fgText", root, 0, 0, FGELEMENT_EXPAND, &fgTransform_EMPTY);

  if(element->GetName())
    text->SetText(element->GetName());
  else
    text->SetText(element->GetClassName());

  if(element == treeview)
    return;

  fgElement* cur = element->root;
  while(cur)
  {
    fgDebug_TreeInsert(root, cur, treeview);
    cur = cur->next;
  }
}

void FG_FASTCALL fgDebug_BuildTree(fgElement* treeview)
{
  // Clean out the tree
  fgElement* cur;
  fgElement* hold = treeview->root;
  while(cur = hold)
  {
    hold = hold->next;
    if(!(cur->flags&FGELEMENT_BACKGROUND))
      VirtualFreeChild(cur);
  }

  fgDebug_TreeInsert(treeview, &fgroot_instance->gui.element, 0);
}

FG_EXTERN void FG_FASTCALL fgDebug_Show(float left, float right)
{
  if(!fgdebug_instance)
    fgDebug_Init((fgDebug*)malloc(sizeof(fgDebug)), *fgroot_instance, 0, 0, FGELEMENT_HIDDEN, &fgTransform_DEFAULT);
  if(fgroot_instance->behaviorhook == &fgRoot_BehaviorDebug)
    return; // Prevent an infinite loop

  fgdebug_instance->elements->SetTransform(fgTransform { { -right,1,0,0,0,1,0,1 }, 0, { 0,0,0,0 } });
  fgdebug_instance->messages->SetTransform(fgTransform { { 0,0,0,0,left,0,0,1 }, 0, { 0,0,0,0 } });
  fgdebug_instance->element.SetTransform(fgTransform_DEFAULT);
  fgdebug_instance->behaviorhook = fgroot_instance->behaviorhook;
  fgDebug_BuildTree(fgdebug_instance->elements);
  fgdebug_instance->element.SetFlag(FGELEMENT_HIDDEN, false);
  fgroot_instance->behaviorhook = &fgRoot_BehaviorDebug;
}
FG_EXTERN void FG_FASTCALL fgDebug_Hide()
{
  fgroot_instance->behaviorhook = fgdebug_instance->behaviorhook;
  fgdebug_instance->element.SetFlag(FGELEMENT_HIDDEN, true);
  if(fgdebug_instance->element.flags&FGDEBUG_CLEARONHIDE)
    fgDebug_ClearLog(fgdebug_instance);
}



size_t FG_FASTCALL fgDebug_LogMessage(fgDebug* self, const FG_Msg* msg, unsigned long long time, size_t depth)
{
  fgDebugMessage m = { 0 };
  m.type = msg->type;
  m.subtype = msg->subtype;
  m.time = time;
  m.depth = depth;

  switch(msg->type)
  {
  default:
    m.arg1.p = msg->other;
    m.arg2.p = msg->other2;
    break;
  case FG_DRAG:
  case FG_MOVE:
    m.arg1.element = (fgElement*)msg->other;
    m.arg1.name = fgDebug_GetElementName(self, m.arg1.element);
    m.arg2.p = msg->other2;
    break;
  case FG_SETPARENT:
  case FG_ADDITEM:
  case FG_ADDCHILD:
    m.arg2.element = (fgElement*)msg->other2;
    m.arg2.name = fgDebug_GetElementName(self, m.arg2.element);
  case FG_REMOVEITEM:
  case FG_REMOVECHILD:
  case FG_CLONE:
  case FG_DROP:
  case FG_GETSKIN:
    m.arg1.name = fgDebug_GetElementName(self, m.arg1.element);
    m.arg1.element = (fgElement*)msg->other;
    break;
  case FG_SETUV:
  case FG_SETAREA:
    if(msg->other != nullptr)
      m.arg1.crect = *(CRect*)msg->other;
    break;
  case FG_SETTRANSFORM:
    if(msg->other != nullptr)
      m.arg1.transform = *(fgTransform*)msg->other;
    break;
  case FG_SETMARGIN:
    if(msg->other != nullptr)
      m.arg1.rect = *(AbsRect*)msg->other;
    break;
  case FG_SETPADDING:
    if(msg->other != nullptr)
      m.arg1.rect = *(AbsRect*)msg->other;
    break;
  case FG_LAYOUTFUNCTION:
    m.arg1.message = (FG_Msg*)msg->other;
    if(msg->other2 != nullptr)
      m.arg2.crect = *(CRect*)msg->other2;
    break;
  case FG_SETMAXDIM:
    m.arg1.f = msg->otherf;
    m.arg2.f = msg->otherfaux;
    break;
  case FG_MOUSEDOWN:
  case FG_MOUSEDBLCLICK:
  case FG_MOUSEUP:
    m.mouse.button = msg->button;
    m.mouse.allbtn = msg->allbtn;
  case FG_MOUSEON:
  case FG_MOUSEOFF:
  case FG_MOUSEMOVE:
  case FG_MOUSELEAVE:
  case FG_DRAGGING:
    m.mouse.x = msg->x;
    m.mouse.y = msg->y;
    break;
  case FG_DRAW:
    if(msg->other != nullptr)
      m.arg1.rect = *(AbsRect*)msg->other;
    m.arg2.i = msg->otheraux;
    break;
  case FG_INJECT:
    m.arg1.message = (FG_Msg*)msg->other;
    if(msg->other2 != nullptr)
      m.arg2.rect = *(AbsRect*)msg->other2;
    break;
  case FG_SETSTYLE:
    if(!msg->subtype)
      m.arg1.s = fgDebug_CopyText(self, (const char*)msg->other);
    else
      m.arg1.p = msg->other;
    m.arg2.u = msg->otheraux;
    break;
  case FG_SETUSERDATA:
    m.arg1.p = msg->other;
    m.arg2.s = fgDebug_CopyText(self, (const char*)msg->other2);
    break;
  case FG_GETUSERDATA:
  case FG_SETTEXT:
  case FG_SETNAME:
    m.arg1.s = fgDebug_CopyText(self, (const char*)msg->other);
    break;
  case FG_MOUSESCROLL:
    m.mouse.scrolldelta = msg->scrolldelta;
    m.mouse.scrollhdelta = msg->scrollhdelta;
    m.mouse.x = msg->x;
    m.mouse.y = msg->y;
    break;
  case FG_TOUCHBEGIN:
  case FG_TOUCHEND:
  case FG_TOUCHMOVE:
    m.mouse.touchindex = msg->touchindex;
    m.mouse.x = msg->x;
    m.mouse.y = msg->y;
    break;
  case FG_KEYUP:
  case FG_KEYDOWN:
    m.keys.keycode = msg->keycode;
    m.keys.sigkeys = msg->sigkeys;
    break;
  case FG_KEYCHAR:
    m.keys.keychar = msg->keychar;
    m.keys.sigkeys = msg->sigkeys;
    break;
  }

  return ((bss_util::cDynArray<fgDebugMessage>&)self->messagelog).Add(m);
}

const char* _dbg_getstr(const char* s)
{
  return !s ? "[null]" : s;
}

#define OUTPUT_CRECT(r) r.left.abs,r.left.rel,r.top.abs,r.top.rel,r.right.abs,r.right.rel,r.bottom.abs,r.bottom.rel
#define OUTPUT_CVEC(r) r.x.abs,r.x.rel,r.y.abs,r.y.rel
#define OUTPUT_RECT(r) r.left,r.top,r.right,r.bottom

ptrdiff_t FG_FASTCALL fgDebug_WriteMessage(char* buf, size_t bufsize, fgDebugMessage* msg)
{
  int spaces = (int)(msg->depth * 2);

  switch(msg->type)
  {
  case FG_CONSTRUCT:
    return snprintf(buf, bufsize, "%*sFG_CONSTRUCT()", spaces, "");
  case FG_GETSTYLE:
    return snprintf(buf, bufsize, "%*sFG_GETSTYLE() - %p", spaces, "", msg->valuep);
  case FG_GETDPI:
    return snprintf(buf, bufsize, "%*sFG_GETDPI() - %ti", spaces, "", msg->value);
  case FG_GETCLASSNAME:
    return snprintf(buf, bufsize, "%*sFG_GETCLASSNAME() - %s", spaces, "", (char*)msg->valuep);
  case FG_GETNAME:
    return snprintf(buf, bufsize, "%*sFG_GETNAME() - %s", spaces, "", (char*)msg->valuep);
  case FG_NUETRAL:
    return snprintf(buf, bufsize, "%*sFG_NUETRAL()", spaces, "");
  case FG_HOVER:
    return snprintf(buf, bufsize, "%*sFG_HOVER()", spaces, "");
  case FG_ACTIVE:
    return snprintf(buf, bufsize, "%*sFG_ACTIVE()", spaces, "");
  case FG_ACTION:
    return snprintf(buf, bufsize, "%*sFG_ACTION()", spaces, "");
  case FG_GOTFOCUS:
    return snprintf(buf, bufsize, "%*sFG_GOTFOCUS()", spaces, "");
  case FG_LOSTFOCUS:
    return snprintf(buf, bufsize, "%*sFG_LOSTFOCUS()", spaces, "");
  case FG_GETMAXDIM:
    return snprintf(buf, bufsize, "%*sFG_GETMAXDIM()", spaces, "");
  case FG_GETITEM:
    return snprintf(buf, bufsize, "%*sFG_GETITEM() - 0x%p", spaces, "", msg->valuep);
  case FG_GETRESOURCE:
    return snprintf(buf, bufsize, "%*sFG_GETRESOURCE() - 0x%p", spaces, "", msg->valuep);
  case FG_GETUV:
    //return snprintf(buf, bufsize, "%*sFG_GETUV() - CRect{%f,%f,%f,%f,%f,%f,%f,%f}", spaces, "");
    return snprintf(buf, bufsize, "%*sFG_GETUV() - CRect", spaces, "");
  case FG_GETCOLOR:
    return snprintf(buf, bufsize, "%*sFG_GETCOLOR() - %#zX", spaces, "", msg->value);
  case FG_GETOUTLINE:
    return snprintf(buf, bufsize, "%*sFG_GETOUTLINE() - %f", spaces, "", msg->valuef);
  case FG_GETFONT:
    return snprintf(buf, bufsize, "%*sFG_GETFONT() - 0x%p", spaces, "", msg->valuep);
  case FG_GETLINEHEIGHT:
    return snprintf(buf, bufsize, "%*sFG_GETLINEHEIGHT() - %f", spaces, "", msg->valuef);
  case FG_GETLETTERSPACING:
    return snprintf(buf, bufsize, "%*sFG_GETLETTERSPACING() - %f", spaces, "", msg->valuef);
  case FG_MOVE:
    return snprintf(buf, bufsize, "%*sFG_MOVE:%hhu(%s [0x%p], %zu)", spaces, "", msg->subtype, _dbg_getstr(msg->arg1.name), msg->arg1.element, msg->arg2.u);
  case FG_SETALPHA:
    return snprintf(buf, bufsize, "%*sFG_SETALPHA(%f)", spaces, "", msg->arg1.f);
  case FG_SETAREA:
    return snprintf(buf, bufsize, "%*sFG_SETAREA(CRect{%f,%f,%f,%f,%f,%f,%f,%f})", spaces, "", OUTPUT_CRECT(msg->arg1.crect));
  case FG_SETTRANSFORM:
    return snprintf(buf, bufsize, "%*sFG_SETTRANSFORM(fgTransform{{%f,%f,%f,%f,%f,%f,%f,%f}, %f, {%f,%f,%f,%f})", spaces, "",
      OUTPUT_CRECT(msg->arg1.transform.area),
      msg->arg1.transform.rotation,
      OUTPUT_CVEC(msg->arg1.transform.center));
  case FG_SETFLAG:
    return snprintf(buf, bufsize, "%*sFG_SETFLAG(%#zX, %s)", spaces, "", msg->arg1.u, msg->arg2.u ? "true" : "false");
  case FG_SETFLAGS:
    return snprintf(buf, bufsize, "%*sFG_SETFLAGS(%#zX)", spaces, "", msg->arg1.u);
  case FG_SETMARGIN:
    return snprintf(buf, bufsize, "%*sFG_SETMARGIN(AbsRect{%f, %f, %f, %f})", spaces, "", OUTPUT_RECT(msg->arg1.rect));
  case FG_SETPADDING:
    return snprintf(buf, bufsize, "%*sFG_SETPADDING(AbsRect{%f, %f, %f, %f})", spaces, "", OUTPUT_RECT(msg->arg1.rect));
  case FG_LAYOUTCHANGE:
    return snprintf(buf, bufsize, "%*sFG_LAYOUTCHANGE:%hhu(0x%p, 0x%p)", spaces, "", msg->subtype, msg->arg1.p, msg->arg2.p);
  case FG_SETPARENT:
    return snprintf(buf, bufsize, "%*sFG_SETPARENT(%s [0x%p], %s [0x%p])", spaces, "", _dbg_getstr(msg->arg1.name), msg->arg1.element, _dbg_getstr(msg->arg2.name), msg->arg2.element);
  case FG_ADDCHILD:
    return snprintf(buf, bufsize, "%*sFG_ADDCHILD(%s [0x%p], %s [0x%p])", spaces, "", _dbg_getstr(msg->arg1.name), msg->arg1.element, _dbg_getstr(msg->arg2.name), msg->arg2.element);
  case FG_CLONE:
    return snprintf(buf, bufsize, "%*sFG_CLONE(%s [0x%p]) - 0x%p", spaces, "", _dbg_getstr(msg->arg1.name), msg->arg1.element, msg->valuep);
  case FG_REMOVECHILD:
    return snprintf(buf, bufsize, "%*sFG_REMOVECHILD(%s [0x%p])", spaces, "", _dbg_getstr(msg->arg1.name), msg->arg1.element);
  case FG_GETSKIN:
    return snprintf(buf, bufsize, "%*sFG_GETSKIN(%s [0x%p]) - 0x%p", spaces, "", _dbg_getstr(msg->arg1.name), msg->arg1.element, msg->valuep);
  case FG_LAYOUTFUNCTION:
    return snprintf(buf, bufsize, "%*sFG_LAYOUTFUNCTION(0x%p, CRect{%f,%f,%f,%f,%f,%f,%f,%f})", spaces, "", msg->arg1.p, OUTPUT_CRECT(msg->arg2.crect));
  case FG_LAYOUTLOAD:
    return snprintf(buf, bufsize, "%*sFG_LAYOUTLOAD(0x%p)", spaces, "", msg->arg1.p);
  case FG_SETRESOURCE:
    return snprintf(buf, bufsize, "%*sFG_SETRESOURCE(0x%p)", spaces, "", msg->arg1.p);
  case FG_SETFONT:
    return snprintf(buf, bufsize, "%*sFG_SETFONT(0x%p)", spaces, "", msg->arg1.p);
  case FG_DRAG:
    return snprintf(buf, bufsize, "%*sFG_DRAG(%s [0x%p], 0x%p)", spaces, "", _dbg_getstr(msg->arg1.name), msg->arg1.element, msg->arg2.p);
  case FG_MOUSEDOWN:
    return snprintf(buf, bufsize, "%*sFG_MOUSEDOWN(x:%i, y:%i, %#hhX, %#hhX)", spaces, "", msg->mouse.x, msg->mouse.y, msg->mouse.button, msg->mouse.allbtn);
  case FG_MOUSEDBLCLICK:
    return snprintf(buf, bufsize, "%*sFG_MOUSEDBLCLICK(x:%i, y:%i, %#hhX, %#hhX)", spaces, "", msg->mouse.x, msg->mouse.y, msg->mouse.button, msg->mouse.allbtn);
  case FG_MOUSEUP:
    return snprintf(buf, bufsize, "%*sFG_MOUSEUP(x:%i, y:%i, %#hhX, %#hhX)", spaces, "", msg->mouse.x, msg->mouse.y, msg->mouse.button, msg->mouse.allbtn);
  case FG_MOUSEON:
    return snprintf(buf, bufsize, "%*sFG_MOUSEON(x:%i, y:%i)", spaces, "", msg->mouse.x, msg->mouse.y);
  case FG_MOUSEOFF:
    return snprintf(buf, bufsize, "%*sFG_MOUSEOFF(x:%i, y:%i)", spaces, "", msg->mouse.x, msg->mouse.y);
  case FG_MOUSEMOVE:
    return snprintf(buf, bufsize, "%*sFG_MOUSEMOVE(x:%i, y:%i)", spaces, "", msg->mouse.x, msg->mouse.y);
  case FG_MOUSELEAVE:
    return snprintf(buf, bufsize, "%*sFG_MOUSELEAVE(x:%i, y:%i)", spaces, "", msg->mouse.x, msg->mouse.y);
  case FG_DRAGGING:
    return snprintf(buf, bufsize, "%*sFG_DRAGGING(x:%i, y:%i)", spaces, "", msg->mouse.x, msg->mouse.y);
  case FG_DROP:
    return snprintf(buf, bufsize, "%*sFG_SETFONT(%s [0x%p])", spaces, "", _dbg_getstr(msg->arg1.name), msg->arg1.element);
  case FG_DRAW:
    return snprintf(buf, bufsize, "%*sFG_DRAW:%hhu(AbsRect{%f, %f, %f, %f}, 0x%p)", spaces, "", msg->subtype, OUTPUT_RECT(msg->arg1.rect), msg->arg2.p);
  case FG_INJECT:
    return snprintf(buf, bufsize, "%*sFG_INJECT(0x%p, AbsRect{%f, %f, %f, %f})", spaces, "", msg->arg1.p, OUTPUT_RECT(msg->arg2.rect));
  case FG_SETSKIN:
    return snprintf(buf, bufsize, "%*sFG_SETSKIN(0x%p)", spaces, "", msg->arg1.p);
  case FG_SETSTYLE:
    if(!msg->subtype)
      return snprintf(buf, bufsize, "%*sFG_SETSTYLE(%s, %zu)", spaces, "", _dbg_getstr(msg->arg1.s), msg->arg2.u);
    else if(msg->subtype == 1)
      return snprintf(buf, bufsize, "%*sFG_SETSTYLE:1(%zu, %zu)", spaces, "", msg->arg1.u, msg->arg2.u);
    return snprintf(buf, bufsize, "%*sFG_SETSTYLE:%hhu(0x%p, %zu)", spaces, "", msg->subtype, msg->arg1.p, msg->arg2.u);
  case FG_GETSTATE:
    return snprintf(buf, bufsize, "%*sFG_GETSTATE() - %ti", spaces, "", msg->value);
  case FG_GETSELECTEDITEM:
    return snprintf(buf, bufsize, "%*sFG_GETSELECTEDITEM(%zu) - 0x%p", spaces, "", msg->arg1.u, msg->valuep);
  case FG_SETDPI:
    return snprintf(buf, bufsize, "%*sFG_SETDPI(%ti)", spaces, "", msg->arg1.i);
  case FG_SETCOLOR:
    return snprintf(buf, bufsize, "%*sFG_SETCOLOR(%#zX)", spaces, "", msg->arg1.u);
  case FG_SETUSERDATA:
    return snprintf(buf, bufsize, "%*sFG_SETUSERDATA(0x%p, %s)", spaces, "", msg->arg1.p, _dbg_getstr(msg->arg2.s));
  case FG_GETUSERDATA:
    return snprintf(buf, bufsize, "%*sFG_GETUSERDATA(%s) - 0x%p", spaces, "", _dbg_getstr(msg->arg1.s), msg->valuep);
  case FG_SETTEXT:
    return snprintf(buf, bufsize, "%*sFG_SETTEXT(%s)", spaces, "", _dbg_getstr(msg->arg1.s));
  case FG_SETNAME:
    return snprintf(buf, bufsize, "%*sFG_SETNAME(%s)", spaces, "", _dbg_getstr(msg->arg1.s));
  case FG_MOUSESCROLL:
    return snprintf(buf, bufsize, "%*sFG_MOUSESCROLL(x:%i, y:%i, v:%#hi, h:%#hi)", spaces, "", msg->mouse.x, msg->mouse.y, msg->mouse.scrolldelta, msg->mouse.scrollhdelta);
  case FG_TOUCHBEGIN:
    return snprintf(buf, bufsize, "%*sFG_TOUCHBEGIN(x:%i, y:%i, %hi)", spaces, "", msg->mouse.x, msg->mouse.y, msg->mouse.touchindex);
  case FG_TOUCHEND:
    return snprintf(buf, bufsize, "%*sFG_TOUCHEND(x:%i, y:%i, %hi)", spaces, "", msg->mouse.x, msg->mouse.y, msg->mouse.touchindex);
  case FG_TOUCHMOVE:
    return snprintf(buf, bufsize, "%*sFG_TOUCHMOVE(x:%i, y:%i, %hi)", spaces, "", msg->mouse.x, msg->mouse.y, msg->mouse.touchindex);
  case FG_KEYUP:
    return snprintf(buf, bufsize, "%*sFG_KEYUP(%#hhX, %#hhX)", spaces, "", msg->keys.keycode, msg->keys.sigkeys);
  case FG_KEYDOWN:
    return snprintf(buf, bufsize, "%*sFG_KEYDOWN(%#hhX, %#hhX)", spaces, "", msg->keys.keycode, msg->keys.sigkeys);
  case FG_KEYCHAR:
    return snprintf(buf, bufsize, "%*sFG_KEYCHAR(%c (%u), %#hhX)", spaces, "", (char)msg->keys.keychar, msg->keys.keychar, msg->keys.sigkeys);
  case FG_JOYBUTTONDOWN:
    return snprintf(buf, bufsize, "%*sFG_JOYBUTTONDOWN(%hi, %s)", spaces, "", msg->joybutton, msg->joydown ? "true" : "false");
  case FG_JOYBUTTONUP:
    return snprintf(buf, bufsize, "%*sFG_JOYBUTTONUP(%hi, %s)", spaces, "", msg->joybutton, msg->joydown ? "true" : "false");
  case FG_JOYAXIS:
    return snprintf(buf, bufsize, "%*sFG_JOYAXIS(%hi, %f)", spaces, "", msg->joyaxis, msg->joyvalue);
  case FG_SETMAXDIM:
    return snprintf(buf, bufsize, "%*sFG_SETMAXDIM(%f, %f)", spaces, "", msg->arg1.f, msg->arg2.f);
  case FG_SETSTATE:
    return snprintf(buf, bufsize, "%*sFG_SETSTATE(%ti, %zu)", spaces, "", msg->arg1.i, msg->arg2.u);
  case FG_SETUV:
    return snprintf(buf, bufsize, "%*sFG_SETUV(CRect{%f,%f,%f,%f,%f,%f,%f,%f})", spaces, "", OUTPUT_CRECT(msg->arg2.crect));
  case FG_SETLETTERSPACING:
    return snprintf(buf, bufsize, "%*sFG_SETLETTERSPACING(%.2g)", spaces, "", msg->arg1.f);
  case FG_SETLINEHEIGHT:
    return snprintf(buf, bufsize, "%*sFG_SETLINEHEIGHT(%.2g)", spaces, "", msg->arg1.f);
  case FG_SETOUTLINE:
    return snprintf(buf, bufsize, "%*sFG_SETOUTLINE(%.2g)", spaces, "", msg->arg1.f);
  }

  return 0;
}