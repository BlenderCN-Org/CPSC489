#include "stdafx.h"
#include "stdwin.h"
#include "errors.h"
#include "app.h"
#include "win.h"
#include "xinput.h"

// Application Variables
static HINSTANCE basead = NULL;
static __int64 n_frames = 0;
static __int64 last_frame_ticks = 0;
static __int64 dt = 0;

//
// Application Functions
//

HINSTANCE GetInstance(void)
{
 return basead;
}

HINSTANCE SetInstance(HINSTANCE handle)
{
 HINSTANCE retval = basead;
 basead = handle;
 return retval;
}

//
// Message Loops
//

int MessageLoop(void)
{
 MSG msg;
 while(GetMessage(&msg, 0, 0, 0)) {
       TranslateMessage(&msg);
       DispatchMessage(&msg);
      }
 return (int)msg.wParam;
}

int MessagePump(BOOL (*function)(real32))
{
 PerformanceCounter hpc;
 MSG msg;
 for(;;) {
     if(PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE)) {
        BOOL status = GetMessage(&msg, NULL, 0, 0);
        if(status == -1) return -1;
        else if(status != 0) {
           TranslateMessage(&msg);
           DispatchMessage(&msg);
          }
        else
           break;
       }
     else {
        // render frame
        real32 last_frame_time = static_cast<real32>(hpc.microseconds(last_frame_ticks))/1000000.0f;
        hpc.begin();
        BOOL result = function(last_frame_time);
        hpc.end();
        n_frames++;
        last_frame_ticks = hpc.ticks();
        dt += last_frame_ticks;
        if(result == FALSE) break;
        // track most recent time and reset after one second
        if(double elapsed = hpc.seconds(dt) > 1.0) {
           double fps = static_cast<double>(n_frames)/elapsed;
           WCHAR buffer[1024];
           swprintf(buffer, 1023, L"%s - %.2f frame per second", GetMainWindowTitle(), fps);
           SetWindowText(GetMainWindow(), buffer);
           n_frames = 0ll;
           dt = 0ll;
          }
       }
    }
 return (int)msg.wParam;
}

//
// Error and Warning Messages
//

BOOL ErrorBox(LPCTSTR message)
{
 MessageBox(GetMainWindow(), message, TEXT("Error"), MB_ICONSTOP);
 return FALSE;
}

BOOL ErrorBox(LPCTSTR message, LPCTSTR title)
{
 MessageBox(GetMainWindow(), message, title, MB_ICONSTOP);
 return FALSE;
}

BOOL ErrorBox(HWND window, LPCTSTR message)
{
 MessageBox(window, message, TEXT("Error"), MB_ICONSTOP);
 return FALSE;
}

BOOL ErrorBox(HWND window, LPCTSTR message, LPCTSTR title)
{
 MessageBox(window, message, title, MB_ICONSTOP);
 return FALSE;
}