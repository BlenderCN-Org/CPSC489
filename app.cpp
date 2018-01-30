#include "stdafx.h"
#include "errors.h"
#include "app.h"
#include "win.h"

// Application Variables
static HINSTANCE basead = NULL;

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

int MessagePump(BOOL (*function)(void))
{
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
     else if(function() == FALSE)
        break;
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