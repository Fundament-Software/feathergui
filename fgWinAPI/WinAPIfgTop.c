// Copyright ©2012 Black Sphere Studios
// For conditions of distribution and use, see copyright notice in "feathergui.h"

#include "fgWinAPI.h"
#include "win32_includes.h"

extern WinAPIfgRoot* _fgroot;

//typedef HRESULT (STDAPICALLTYPE *DWMCOMPENABLE)(BOOL*);
//typedef HRESULT (STDAPICALLTYPE *DWMEXTENDFRAME)(HWND,const MARGINS*);
//DWMEXTENDFRAME dwmextend=0;
HWND TopWndCreate(HINSTANCE instance, fgWindow* wn, const wchar_t* name, long style, INT x, INT y, INT cx, INT cy)
{
	HWND hWnd;
  HINSTANCE hInstance = !instance?GetModuleHandleW(0):instance;

  //if(icon)
	//  wcex.hIcon = (HICON)LoadImageW(hInstance, icon, IMAGE_ICON, 0,0, LR_LOADFROMFILE); 
	
  //if(composite!=0) 
  //  hWnd = CreateWindowExW((composite==2||composite==4)?WS_EX_TRANSPARENT|WS_EX_COMPOSITED|WS_EX_LAYERED:WS_EX_COMPOSITED, 
  //                          L"FeatherWindow", L"", style, INT(wleft), INT(wtop), INT(rwidth), INT(rheight), NULL, NULL, hInstance, NULL);
  //else 
  hWnd = CreateWindowExW(0,L"FeatherWindow", name, style, x, y, cx ,cy, NULL, NULL, hInstance, NULL);
  SetWindowLongPtr(hWnd,GWLP_USERDATA,(LONG_PTR)wn); // Cast to LONG_PTR so it works in x64, not just x86
  
	UpdateWindow(hWnd);
  //SetWindowPos(hWnd,composite==2||composite==3?HWND_TOPMOST:HWND_TOP, INT(wleft), INT(wtop), INT(rwidth), INT(rheight),
  //              SWP_NOCOPYBITS|SWP_NOMOVE|SWP_NOACTIVATE);
  
 // if(composite==2||composite==4) SetLayeredWindowAttributes(hWnd, 0, 0xFF, LWA_ALPHA);
	//else 
 // {
 //   SetActiveWindow(hWnd);
	//  SetForegroundWindow(hWnd);
 // }
  SetCursor(LoadCursor(NULL, IDC_ARROW));

  return hWnd;
}

char FG_FASTCALL WinAPIfgTop_Message(fgTopWindow* self, const FG_Msg* msg)
{
  switch(msg->type)
  {
  case FG_REMOVESTATIC:
  case FG_REMOVECHILD:
    if(((fgChild*)msg->other)->parent==(fgChild*)self)
      return fgWindow_Message(&self->window,msg);
    return fgWindow_Message(&self->region,msg);
  case FG_ADDSTATIC:
  case FG_ADDCHILD:
    if(msg->otheraux==1)
      return fgWindow_Message(&self->window,msg);
    return fgWindow_Message(&self->region,msg);
  }

  return fgWindow_Message((fgWindow*)self,msg);
}

fgWindow* FG_FASTCALL fgTopWindow_Create(const char* caption, const fgElement* element, FG_UINT id, fgFlag flags)
{
  AbsRect absr;
  RECT rsize = { CW_USEDEFAULT,CW_USEDEFAULT,CW_USEDEFAULT,CW_USEDEFAULT };
  wchar_t* buf=0;
  long style=WS_OVERLAPPED | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_VISIBLE;
  WinAPIfgTop* r = (WinAPIfgTop*)malloc(sizeof(WinAPIfgTop));
  fgWindow_Init(&r->wn.window,(fgWindow*)_fgroot,element,id,flags);
  if(flags&FGTOPWINDOW_MINIMIZE) style|=WS_MINIMIZEBOX;
  if(flags&FGTOPWINDOW_RESTORE) style|=WS_MAXIMIZEBOX;
  if(!(flags&FGTOPWINDOW_NOTITLEBAR)) style|=WS_CAPTION|WS_SYSMENU;
  if(flags&FGTOPWINDOW_RESIZABLE) style|=WS_THICKFRAME;
  else
  {
    if(element!=0) style|=WS_POPUP;
    if(!(flags&FGTOPWINDOW_NOTITLEBAR)) style|=WS_BORDER;
  }

  if(element!=0) 
  {
    ResolveRect((fgChild*)r,&absr); //Get size
	  rsize.left = (LONG)absr.left;
	  rsize.top = (LONG)absr.top;
	  rsize.right = (LONG)absr.right;
	  rsize.bottom = (LONG)absr.bottom;

	  AdjustWindowRect(&rsize, style, FALSE);
	  rsize.right = (rsize.right - rsize.left);
	  rsize.bottom = (rsize.bottom - rsize.top);
  }

  r->wn.window.message=&WinAPIfgTop_Message;
  r->wn.window.element.destroy=&WinAPIfgWindow_Destroy;
  fgWindow_VoidMessage((fgWindow*)r,FG_SETPARENT,_fgroot);
  
  if(caption!=0)
    WinAPIutf8to16(&buf,caption);

  r->wn.handle=TopWndCreate(_fgroot->instance,&r->wn.window, buf, style, rsize.left, rsize.top, rsize.right, rsize.bottom);
  if(buf) free(buf);
  return (fgWindow*)r;
}