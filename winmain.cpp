#include "stdafx.h"
#include "errors.h"
#include "app.h"
#include "win.h"
#include "gfx.h"
#include "xinput.h"

int Run(void)
{
 // errors and debugging
 SetLanguageCode(LC_ENGLISH);
 InitErrorStrings();
 EnableErrorDebugging(true);

 // window initialization
 if(!CreateMainWindow()) return -1;

 // Direct3D initialization
 ErrorCode code = InitD3D();
 if(code != EC_SUCCESS) {
    FreeD3D();
    Error(code);
    return -1;
   }

 // initialize controllers and run program
 InitControllers();
 int retval = MessagePump(RenderFrame);
 FreeControllers();
 return retval;
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