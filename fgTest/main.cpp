// Copyright ©2012 Black Sphere Studios
// For conditions of distribution and use, see copyright notice in "feathergui.h"

#include "../fgNull/fgNull.h"
#include "bss_defines.h"

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

int main(int argc, char** argv)
{
  fgRoot* gui = fgInitialize();
  fgStatic* r1 = fgLoadImage("blah");
  (*r1->element.destroy)(r1);


  fgTerminate(gui);
}

struct HINSTANCE__;

// WinMain function, simply a catcher that calls the main function
int __stdcall WinMain(struct HINSTANCE__* hInstance, struct HINSTANCE__* hPrevInstance, char* lpCmdLine, int nShowCmd)
{
  main(0,(char**)hInstance);
}