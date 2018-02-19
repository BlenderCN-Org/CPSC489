#include "stdafx.h"
#include "errors.h"
#include "app.h"
#include "win.h"
#include "gfx.h"

int Run(void)
{

 // window initialization
 SetLanguageCode(LC_ENGLISH);
 InitErrorStrings();
 if(!CreateMainWindow()) return -1;

 // Direct3D initialization
 ErrorCode code = InitD3D();
 if(code != EC_SUCCESS) {
    FreeD3D();
    Error(code);
    return -1;
   }

 // run program
 return MessagePump(RenderFrame);
}

int WINAPI WinMain(HINSTANCE basead, HINSTANCE unused, LPSTR cmdline, int cmdshow)
{
 // set base address and initialize COM
 SetInstance(basead);
 CoInitializeEx(0, COINIT_MULTITHREADED);

 // run program
 int retval = Run();
 CoUninitialize();

 // free base address and free COM
 SetInstance(NULL);
 return retval;
}