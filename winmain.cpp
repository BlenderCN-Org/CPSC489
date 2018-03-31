#include "stdafx.h"
#include "errors.h"
#include "app.h"
#include "win.h"
#include "gfx.h"
#include "xaudio.h"
#include "xinput.h"

ErrorCode AppInit(void);
void AppFree(void);

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

 // run program
 code = AppInit();
 if(Fail(code)) {
    FreeD3D();
    Error(code);
    return -1;
   }
 int retval = MessagePump(RenderFrame);
 AppFree();
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

ErrorCode AppInit(void)
{
 // audio system
 ErrorCode code = InitAudio();
 if(Fail(code)) return code;

 // initialize controllers and run program
 code = InitControllers();
 if(Fail(code)) {
    AppFree();
    return code;
   }

 return EC_SUCCESS;
}

void AppFree(void)
{
 FreeAudio();
 FreeControllers();
}