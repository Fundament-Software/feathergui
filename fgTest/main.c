// Copyright ©2012 Black Sphere Studios
// For conditions of distribution and use, see copyright notice in "feathergui.h"

#include "fgTest.h"
#include "../fgNull/fgNull.h"
#include "bss_defines.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#if defined(BSS_DEBUG) && defined(BSS_CPU_x86_64)
#pragma comment(lib, "../bin/feathergui64_d.lib")
#pragma comment(lib, "../bin/fgNull64_d.lib")
#elif defined(BSS_CPU_x86_64)
#pragma comment(lib, "../bin/feathergui64.lib")
#pragma comment(lib, "../bin/fgNull64.lib")
#elif defined(BSS_DEBUG)
#pragma comment(lib, "../bin/feathergui_d.lib")
#pragma comment(lib, "../bin/fgNull_d.lib")
#else
#pragma comment(lib, "../bin/feathergui.lib")
#pragma comment(lib, "../bin/fgNull.lib")
#endif

typedef struct {
  size_t first;
  size_t second;
} RETPAIR;

typedef struct
{
  const char* NAME;
  RETPAIR (*FUNC)();
} TESTDEF;

static FILE* failedtests=0;

#define BEGINTEST RETPAIR __testret = {0,0}
#define ENDTEST return __testret
#define FAILEDTEST(t) fprintf(failedtests, "Test #%u Failed  < %s >",  __testret.first,MAKESTRING(t))
#define TEST(t) { ++__testret.first; if(t) ++__testret.second; else FAILEDTEST(t); }
//#define TESTARRAY(t,f) _ITERFUNC(__testret,t,[&](uint i) -> bool { f });
//#define TESTALL(t,f) _ITERALL(__testret,t,[&](uint i) -> bool { f });
//#define TESTCOUNT(c,t) { for(uint i = 0; i < c; ++i) TEST(t) }
//#define TESTCOUNTALL(c,t) { bool __val=true; for(uint i = 0; i < c; ++i) __val=__val&&(t); TEST(__val); }

static fgRoot* gui=0;

RETPAIR test_feathergui()
{
  BEGINTEST;
  fgChild ch;
  fgChild pch;
  fgElement zeroelement;

  TEST(fbnext(0)==1);
  TEST(fbnext(1)==2);
  TEST(fbnext(2)==4);
  TEST(lerp(0.0f,1.0f,0.5f)==0.5f);
  TEST(lerp(-1.0f,1.0f,0.5f)==0.0f);
  TEST(lerp(0.0f,1.0f,1.0f)==1.0f);
  TEST(lerp(-1.0f,1.0f,0.0f)==-1.0f);
  
  fgChild_Init(&ch);
  memset(&zeroelement,0,sizeof(fgElement));
  TEST(!memcmp(&ch.element,&zeroelement,sizeof(fgElement)));
  TEST(ch.destroy==&fgChild_Destroy);
  TEST(ch.free==&free);
  TEST(ch.next==0);
  TEST(ch.order==0);
  TEST(ch.parent==0);
  TEST(ch.prev==0);
  TEST(ch.root==0);

  fgChild_Init(&pch);
  fgChild_SetParent(&ch,&pch);
  TEST(ch.next==0);
  TEST(ch.prev==0);
  TEST(ch.parent==&pch);
  TEST(pch.parent==0);
  TEST(pch.root==&ch);
  fgChild_Destroy(&ch);
  TEST(pch.root==0);

  /*

FG_EXTERN AbsVec FG_FASTCALL ResolveVec(const CVec* v, const AbsRect* last);
FG_EXTERN void FG_FASTCALL ResolveRect(const fgChild* self, AbsRect* out);
FG_EXTERN void FG_FASTCALL ResolveRectCache(AbsRect* r, const CRect* v, const AbsRect* last);
FG_EXTERN char FG_FASTCALL CompareCRects(const CRect* l, const CRect* r); // Returns 0 if both are the same or 1 otherwise
FG_EXTERN char FG_FASTCALL CompChildOrder(const fgChild* l, const fgChild* r);
FG_EXTERN char FG_FASTCALL MsgHitAbsRect(const FG_Msg* msg, const AbsRect* r);
FG_EXTERN char FG_FASTCALL MsgHitCRect(const FG_Msg* msg, const fgChild* child);
FG_EXTERN void FG_FASTCALL LList_Remove(fgChild* self, fgChild** root);
FG_EXTERN void FG_FASTCALL LList_Add(fgChild* self, fgChild** root);
FG_EXTERN void FG_FASTCALL LList_Insert(fgChild* self, fgChild* target, fgChild** root);
FG_EXTERN void FG_FASTCALL VirtualFreeChild(fgChild* self);
*/
  ENDTEST;
}

RETPAIR test_Static()
{
  BEGINTEST;
  ENDTEST;
}

RETPAIR test_Window()
{
  BEGINTEST;
  ENDTEST;
}

RETPAIR test_TopWindow()
{
  BEGINTEST;
  ENDTEST;
}

RETPAIR test_Root()
{
  BEGINTEST;
  fgTopWindow* top;
  gui = fgInitialize();
  top = fgTopWindow_Create(gui);

  fgWindow_VoidMessage(top,FG_ADDCHILD,fgButton_Create(fgLoadImage("fake")));
  fgWindow_VoidMessage(top->region.element.root,FG_ADDCHILD,fgMenu_Create());
  fgWindow_VoidMessage(top,FG_ADDCHILD,top->region.element.root->root);
  fgWindow_VoidMessage(top,FG_ADDCHILD,fgButton_Create(fgLoadText("fake",0)));
  fgWindow_VoidMessage(top,FG_ADDCHILD,fgMenu_Create());
  fgWindow_VoidMessage(top,FG_ADDSTATIC,fgLoadImage("fake"));
  fgStatic_Message(top->region.rlist,FG_RADDCHILD,fgLoadText("fake",0));
  fgWindow_VoidMessage(top,FG_ADDSTATIC,top->region.rlist->element.root);
  fgRoot_Render(gui);
  ENDTEST;
}

RETPAIR test_Grid()
{
  BEGINTEST;
  ENDTEST;
}

RETPAIR test_Container()
{
  BEGINTEST;
  ENDTEST;
}

RETPAIR test_Button()
{
  BEGINTEST;
  ENDTEST;
}

RETPAIR test_Menu()
{
  BEGINTEST;
  ENDTEST;
}

RETPAIR test_TERMINATE()
{
  BEGINTEST;
  fgTerminate(gui);
  ENDTEST;
}

int main(int argc, char** argv)
{
  static const int COLUMNS[3] = { 24, 11, 8 };
  static TESTDEF tests[] = {
    { "feathergui.h", &test_feathergui },
    { "fgStatic.h", &test_Static },
    { "fgWindow.h", &test_Window },
    { "fgTopWindow.h", &test_TopWindow },
    { "fgRoot.h", &test_Root },
    { "fgGrid.h", &test_Grid },
    { "fgContainer.h", &test_Container },
    { "fgButton.h", &test_Button },
    { "fgMenu.h", &test_Menu },
    { "TERMINATE", &test_TERMINATE },
  };

  static const size_t NUMTESTS=sizeof(tests)/sizeof(TESTDEF);
  RETPAIR numpassed;
  unsigned int i;
  char buf[44]; // 21 + 21 + 1 + null terminator
  unsigned int failures[sizeof(tests)/sizeof(TESTDEF)];
  unsigned int nfail=0;

  srand(time(NULL));
  failedtests=fopen("failedtests.txt","wb");

  printf("Feather GUI Abstraction Layer v%i.%i.%i: Unit Tests\nCopyright (c)2012 Black Sphere Studios\n\n",FGUI_VERSION_MAJOR,FGUI_VERSION_MINOR,FGUI_VERSION_REVISION);
  printf("%-*s %-*s %-*s\n",COLUMNS[0],"Test Name", COLUMNS[1],"Subtests", COLUMNS[2],"Pass/Fail");

  for(i = 0; i < NUMTESTS; ++i)
  {
    numpassed=tests[i].FUNC(); //First is total, second is succeeded
    if(numpassed.first!=numpassed.second) failures[nfail++]=i;
    sprintf(buf,"%u/%u",numpassed.second,numpassed.first);
    printf("%-*s %*s %-*s\n",COLUMNS[0],tests[i].NAME, COLUMNS[1],buf, COLUMNS[2],(numpassed.first==numpassed.second)?"PASS":"FAIL");
  }

  if(!nfail)
    printf("\nAll tests passed successfully!\n");
  else
  {
    printf("\nThe following tests failed: \n");
    for (i = 0; i < nfail; i++)
      printf("  %s\n", tests[failures[i]].NAME);
    printf("\nThese failures indicate either a misconfiguration on your system, or a potential bug. Please report all bugs to http://code.google.com/p/bss-util/issues/list\n\nA detailed list of failed tests was written to failedtests.txt\n");
  }

  printf("\nPress Enter to exit the program.");
  getc(stdin);
}

struct HINSTANCE__;

// WinMain function, simply a catcher that calls the main function
int __stdcall WinMain(struct HINSTANCE__* hInstance, struct HINSTANCE__* hPrevInstance, char* lpCmdLine, int nShowCmd)
{
  main(0,(char**)hInstance);
}