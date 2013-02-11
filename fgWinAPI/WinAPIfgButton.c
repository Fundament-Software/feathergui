// Copyright ©2012 Black Sphere Studios
// For conditions of distribution and use, see copyright notice in "feathergui.h"

#include "fgWinAPI.h"
#include "win32_includes.h"
#include <CommCtrl.h>

extern WinAPIfgRoot* _fgroot;
extern void FG_FASTCALL WinAPIfgImage_Destroy(WinAPIfgImage*);
extern void FG_FASTCALL WinAPIfgText_Destroy(WinAPIfgText*);

fgWindow* FG_FASTCALL fgButton_Create(fgStatic* item, fgWindow* parent, const fgElement* element, FG_UINT id, fgFlag flags)
{
  AbsRect absr;
  RECT rsize = { CW_USEDEFAULT,CW_USEDEFAULT,CW_USEDEFAULT,CW_USEDEFAULT };
  DWORD style=WS_CHILD;
  wchar_t* txt=0;
  WinAPIfgButton* r = (WinAPIfgButton*)malloc(sizeof(WinAPIfgButton));
  //r->window.message=&fgdebug_Message_fgButton;
  fgButton_Init((fgButton*)r,parent,element,id,flags);
  if(item->element.destroy==&WinAPIfgText_Destroy) { // If true its text-based
    WinAPIutf8to16(&txt,((WinAPIfgText*)item)->text);
    style=BS_TEXT|BS_VCENTER|BS_CENTER;
  } else if(item->element.destroy==&WinAPIfgImage_Destroy) {
    style=BS_BITMAP;
  }
  assert(parent!=0);

  ResolveRect((fgChild*)r,&absr); //Get size
	rsize.left = (LONG)absr.left;
	rsize.top = (LONG)absr.top;
	rsize.right = (LONG)absr.right - rsize.left;
	rsize.bottom = (LONG)absr.bottom - rsize.top;
  if(!rsize.right) rsize.right=CW_USEDEFAULT;
  if(!rsize.bottom) rsize.bottom=CW_USEDEFAULT;

  r->wn.handle=CreateWindowExW(0,WC_BUTTON,txt,style,rsize.left,rsize.top,rsize.right,rsize.bottom,((WinAPIfgWindow*)parent)->handle,0,_fgroot->instance,0);
  if(txt) free(txt);
  return (fgWindow*)r;
}