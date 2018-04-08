#include "stdafx.h"
#include "stdres.h"
#include "errors.h"
#include "app.h"
#include "win.h"
#include "camera.h"
#include "orbit.h"
#include "viewport.h"
#include "gfx.h"

// testing
#include "testing/tests.h"

// Window Variables
static HWND handle = NULL;
static LPWSTR classname = L"MainWindow";
static LPWSTR title = L"CSCP 489 (Direct3D 11)";

// Mouse Tracking Variables
static bool is_tracking = false;
static uint32 track_viewport = 0xFFFFFFFFul;
static sint32 track_flags = 0;
static sint32 track_x0, track_y0;
static sint32 track_x1, track_y1;
static sint32 track_dx = 0;
static sint32 track_dy = 0;

// Mouse Tracking Functions
static void BeginMouseTracking(HWND window, WPARAM wparam, LPARAM lparam);
static void EndMouseTracking(void);

// Window Messages
static WINDOW_PROCEDURE(MainWindowProc);
static WINDOW_MESSAGE(EvNCCreate);
static WINDOW_MESSAGE(EvCreate);
static WINDOW_MESSAGE(EvDestroy);
static WINDOW_MESSAGE(EvNCDestroy);
static WINDOW_MESSAGE(EvSize);
static WINDOW_MESSAGE(EvPaint);
static WINDOW_MESSAGE(EvClose);
static WINDOW_MESSAGE(EvLButtonDown);
static WINDOW_MESSAGE(EvLButtonUp);
static WINDOW_MESSAGE(EvMButtonDown);
static WINDOW_MESSAGE(EvMButtonUp);
static WINDOW_MESSAGE(EvRButtonDown);
static WINDOW_MESSAGE(EvRButtonUp);
static WINDOW_MESSAGE(EvMouseMove);
static WINDOW_MESSAGE(EvMouseWheel);
static WINDOW_MESSAGE(EvCaptureChanged);
static WINDOW_MESSAGE(EvCancelMode);
static WINDOW_MESSAGE(EvKeyDown);
static WINDOW_MESSAGE(EvSysKeyDown);
static WINDOW_MESSAGE(EvCommand);

// Window Commands
static WINDOW_COMMAND(CmFlybyTest);
static WINDOW_COMMAND(CmPortalTest);
static WINDOW_COMMAND(CmSkeletonAxesTest);
static WINDOW_COMMAND(CmAABBTest);
static WINDOW_COMMAND(CmAABBMinMaxTest);
static WINDOW_COMMAND(CmMapTest);
// Window Commands: General Tests
static WINDOW_COMMAND(CmMeshTest);
static WINDOW_COMMAND(CmSoundTest);


#pragma region WINDOW_FUNCTIONS

BOOL CreateMainWindow(void)
{
 // error lambda
 auto e_lambda = [](ErrorCode code) { Error(code); return FALSE; };

 // already exists, doesn't hurt to call again
 if(::handle && IsWindow(::handle))
    return TRUE;

 // register window class
 WNDCLASSEX wcx;
 if(GetClassInfoEx(GetInstance(), classname, &wcx) == FALSE) {
    wcx.cbSize = sizeof(WNDCLASSEX);
    wcx.style = CS_DBLCLKS;
    wcx.lpfnWndProc = (WNDPROC)MainWindowProc;
    wcx.cbClsExtra = 0;
    wcx.cbWndExtra = 0;
    wcx.hInstance = GetInstance();
    wcx.hIcon = NULL;
    wcx.hCursor = LoadCursor(NULL, IDC_ARROW);
    wcx.hbrBackground = (HBRUSH)GetStockObject(NULL_BRUSH);
    wcx.lpszMenuName = L"DEVMENU"; // NULL;
    wcx.lpszClassName = classname;
    wcx.hIconSm = NULL;
    if(RegisterClassEx(&wcx) == FALSE) return e_lambda(EC_WIN32_REGISTER_WINDOW);
   }

 // create window
 DWORD styleEx = 0;
 DWORD style = WS_OVERLAPPEDWINDOW;
 int x = CW_USEDEFAULT; int y = CW_USEDEFAULT;
 int w = CW_USEDEFAULT; int h = CW_USEDEFAULT;
 HWND hwnd = CreateWindowEx(styleEx, classname, title, style, x, y, w, h, NULL, NULL, GetInstance(), NULL);
 if(!hwnd) return e_lambda(EC_WIN32_MAIN_WINDOW);
 ShowWindow(hwnd, SW_SHOW);
 UpdateWindow(hwnd);

 return TRUE;
}

HWND GetMainWindow(void)
{
 return ::handle;
}

LPWSTR GetMainWindowTitle(void)
{
 return ::title;
}

#pragma endregion WINDOW_FUNCTIONS

#pragma region MOUSETRACKING_FUNCTIONS

void BeginMouseTracking(HWND window, WPARAM wparam, LPARAM lparam)
{
 // track only if we are in an active viewport
 int xpos = GET_X_LPARAM(lparam);
 int ypos = GET_Y_LPARAM(lparam);
 uint32 viewport = GetViewportIndexFromPosition(xpos, ypos);
 if(!is_tracking && viewport != 0xFFFFFFFFul)
   {
    SetCapture(window);
    is_tracking = true;
    track_viewport = viewport;
    track_flags = (sint32)wparam;
    track_x0 = track_x1 = xpos;
    track_y0 = track_y1 = ypos;
    track_dx = 0;
    track_dy = 0;
   }
}

void EndMouseTracking(void)
{
 if(is_tracking) {
    ReleaseCapture();
    is_tracking = false;
    track_viewport = 0xFFFFFFFFul;
    track_flags = 0;
    track_x0 = track_y0 = -1;
    track_x1 = track_y1 = -1;
    track_dx = 0;
    track_dy = 0;
   }
}

#pragma endregion MOUSETRACKING_FUNCTIONS

WINDOW_PROCEDURE(MainWindowProc)
{
 BEGIN_MESSAGE_HANDLER
  WINDOW_MESSAGE_HANDLER(WM_NCCREATE, EvNCCreate);
  WINDOW_MESSAGE_HANDLER(WM_CREATE, EvCreate);
  WINDOW_MESSAGE_HANDLER(WM_DESTROY, EvDestroy);
  WINDOW_MESSAGE_HANDLER(WM_NCDESTROY, EvNCDestroy);
  WINDOW_MESSAGE_HANDLER(WM_SIZE, EvSize);
  WINDOW_MESSAGE_HANDLER(WM_PAINT, EvPaint);
  WINDOW_MESSAGE_HANDLER(WM_CLOSE, EvClose);
  WINDOW_MESSAGE_HANDLER(WM_LBUTTONDOWN, EvLButtonDown);
  WINDOW_MESSAGE_HANDLER(WM_LBUTTONUP, EvLButtonUp);
  WINDOW_MESSAGE_HANDLER(WM_MBUTTONDOWN, EvMButtonDown);
  WINDOW_MESSAGE_HANDLER(WM_MBUTTONUP, EvMButtonUp);
  WINDOW_MESSAGE_HANDLER(WM_RBUTTONDOWN, EvRButtonDown);
  WINDOW_MESSAGE_HANDLER(WM_RBUTTONUP, EvRButtonUp);
  WINDOW_MESSAGE_HANDLER(WM_CAPTURECHANGED, EvCaptureChanged);
  WINDOW_MESSAGE_HANDLER(WM_CANCELMODE, EvCancelMode);
  WINDOW_MESSAGE_HANDLER(WM_MOUSEMOVE, EvMouseMove);
  WINDOW_MESSAGE_HANDLER(WM_MOUSEWHEEL, EvMouseWheel);
  WINDOW_MESSAGE_HANDLER(WM_KEYDOWN, EvKeyDown);
  WINDOW_MESSAGE_HANDLER(WM_SYSKEYDOWN, EvSysKeyDown);
  WINDOW_MESSAGE_HANDLER(WM_COMMAND, EvCommand);
  WINDOW_MESSAGE_DEFAULT;
 END_MESSAGE_HANDLER
}

#pragma region WINDOW_MESSAGES

WINDOW_MESSAGE(EvNCCreate)
{
 ::handle = window;
 return DefWindowProc(window, WM_NCCREATE, wparam, lparam);
}

WINDOW_MESSAGE(EvCreate)
{
 LRESULT retval = DefWindowProc(window, WM_CREATE, wparam, lparam);
 if(retval == -1) return -1;
 return retval;
}

WINDOW_MESSAGE(EvDestroy)
{
 FreeD3D();
 return DefWindowProc(window, WM_DESTROY, wparam, lparam);
}

WINDOW_MESSAGE(EvNCDestroy)
{
 LRESULT retval = DefWindowProc(window, WM_NCDESTROY, wparam, lparam);
 ::handle = 0;
 PostQuitMessage(0);
 return retval;
}

WINDOW_MESSAGE(EvSize)
{
 LRESULT retval = DefWindowProc(window, WM_SIZE, wparam, lparam);
 int dx = LOWORD(lparam);
 int dy = HIWORD(lparam);
 if(dx < 1) dx = 1;
 if(dy < 1) dy = 1;
 if(GetD3DDeviceContext()) {
    ErrorCode code = ResetD3D((UINT)dx, (UINT)dy);
    if(Fail(code)) {
       FreeD3D();
       Error(code);
       return retval;
      }
   }
 return retval;
}

WINDOW_MESSAGE(EvPaint)
{
 // Let Windows know we finished painting. If you don't call ValidateRect,
 // the system continues to generate infinite WM_PAINT messages.
 ValidateRect(window, NULL);
 return 0;
}

WINDOW_MESSAGE(EvClose)
{
 // if there are any tests active, end them here
 if(IsTestActive()) EndTest();
 return DefWindowProc(window, WM_CLOSE, wparam, lparam);
}

WINDOW_MESSAGE(EvLButtonDown)
{
 BeginMouseTracking(window, wparam, lparam);
 return 0;
}

WINDOW_MESSAGE(EvLButtonUp)
{
 EndMouseTracking();
 return 0;
}

WINDOW_MESSAGE(EvMButtonDown)
{
 BeginMouseTracking(window, wparam, lparam);
 return 0;
}

WINDOW_MESSAGE(EvMButtonUp)
{
 EndMouseTracking();
 return 0;
}

WINDOW_MESSAGE(EvRButtonDown)
{
 BeginMouseTracking(window, wparam, lparam);
 return 0;
}

WINDOW_MESSAGE(EvRButtonUp)
{
 EndMouseTracking();
 return 0;
}

WINDOW_MESSAGE(EvMouseMove)
{
 // if tracking
 if(is_tracking)
   {
    // MK_LBUTTON dragging = orbit
    // MK_LBUTTON dragging + CONTROL = position model on ground
    // MK_LBUTTON dragging + SHIFT = panning
    if(track_flags & MK_LBUTTON)
      {
       // pan
       if(GetAsyncKeyState(VK_SHIFT) & 0x8000)
         {
          track_flags = (sint32)wparam;
          track_x0 = track_x1;
          track_y0 = track_y1;
          track_x1 = GET_X_LPARAM(lparam);
          track_y1 = GET_Y_LPARAM(lparam);
          GetViewportCamera(track_viewport)->Pan(track_x0, track_y0, track_x1, track_y1);
          UpdateViewportCamera(track_viewport);
         }
       // orbit
       else
         {
          track_flags = (sint32)wparam;
          track_x0 = track_x1;
          track_y0 = track_y1;
          track_x1 = GET_X_LPARAM(lparam);
          track_y1 = GET_Y_LPARAM(lparam);
          GetViewportCamera(track_viewport)->Orbit(track_x0, track_y0, track_x1, track_y1);
          UpdateViewportCamera(track_viewport);
         }
      }
    // MK_MBUTTON dragging
    else if(track_flags & MK_MBUTTON)
      {
      }
    // MK_RBUTTON dragging
    else if(track_flags & MK_RBUTTON)
      {
       // dolly
       track_flags = (sint32)wparam;
       track_x0 = track_x1;
       track_y0 = track_y1;
       track_x1 = GET_X_LPARAM(lparam);
       track_y1 = GET_Y_LPARAM(lparam);
       int speed = 1;
       if(GetAsyncKeyState(VK_SHIFT) & 0x8000) speed = 2;
       GetViewportCamera(track_viewport)->Dolly(speed*(track_y1 - track_y0));
       UpdateViewportCamera(track_viewport);
      }
    // nothing is pressed
    else
       EndMouseTracking();
   }

 return 0;
}

WINDOW_MESSAGE(EvMouseWheel)
{
 // can't be dragging
 if(is_tracking) return 0;

 // mouse position
 int mx = GET_X_LPARAM(lparam);
 int my = GET_Y_LPARAM(lparam);

 // dolly camera if viewport is enabled
 uint32 viewport = GetViewportIndexFromPosition(mx, my);
 if(viewport != 0xFFFFFFFFul) {
    int speed = 1;
    if(GetAsyncKeyState(VK_SHIFT) & 0x8000) speed = 2;
    int delta = GET_WHEEL_DELTA_WPARAM(wparam); // 30, 60, 90, 120
    GetViewportCamera(viewport)->Dolly(-(speed*delta)/15);
    UpdateViewportCamera(viewport);
   }

 return 0;
}

WINDOW_MESSAGE(EvCaptureChanged)
{
 // disable mouse-tracking
 if(is_tracking) EndMouseTracking();
 return DefWindowProc(window, WM_CAPTURECHANGED, wparam, lparam);
}

WINDOW_MESSAGE(EvCancelMode)
{
 EndMouseTracking();
 return 0;
}

WINDOW_MESSAGE(EvKeyDown)
{
 return 0;
}

WINDOW_MESSAGE(EvSysKeyDown)
{
 return 0;
}

WINDOW_MESSAGE(EvCommand)
{
 BEGIN_COMMAND_HANDLER
  WINDOW_COMMAND_HANDLER(CM_FLYBY_TEST, CmFlybyTest);
  WINDOW_COMMAND_HANDLER(CM_PORTAL_TEST, CmPortalTest);
  WINDOW_COMMAND_HANDLER(CM_SKELETON_AXES_TEST, CmSkeletonAxesTest);
  WINDOW_COMMAND_HANDLER(CM_AABB_TEST, CmAABBTest);
  WINDOW_COMMAND_HANDLER(CM_AABB_MINMAX_TEST, CmAABBMinMaxTest);
  WINDOW_COMMAND_HANDLER(CM_MAP_TEST, CmMapTest);
  WINDOW_COMMAND_HANDLER(CM_MESH_TEST, CmMeshTest);
  WINDOW_COMMAND_HANDLER(CM_SOUND_TEST, CmSoundTest);
  WINDOW_COMMAND_DEFAULT;
 END_COMMAND_HANDLER
 return 0;
}

#pragma endregion WINDOW_MESSAGES

#pragma region WINDOW_COMMANDS

WINDOW_COMMAND(CmFlybyTest)
{
 // camera flyby test
 if(GetActiveTest() != CM_FLYBY_TEST) BeginTest(CM_FLYBY_TEST);
 else if(GetActiveTest() == CM_FLYBY_TEST) EndTest();
 return 0;
}

WINDOW_COMMAND(CmPortalTest)
{
 // camera flyby test
 if(GetActiveTest() != CM_PORTAL_TEST) BeginTest(CM_PORTAL_TEST);
 else if(GetActiveTest() == CM_PORTAL_TEST) EndTest();
 return 0;
}

WINDOW_COMMAND(CmSkeletonAxesTest)
{
 // skeleton axes test
 if(GetActiveTest() != CM_SKELETON_AXES_TEST) BeginTest(CM_SKELETON_AXES_TEST);
 else if(GetActiveTest() == CM_SKELETON_AXES_TEST) EndTest();
 return 0;
}

WINDOW_COMMAND(CmAABBTest)
{
 // skeleton axes test
 if(GetActiveTest() != CM_AABB_TEST) BeginTest(CM_AABB_TEST);
 else if(GetActiveTest() == CM_AABB_TEST) EndTest();
 return 0;
}

WINDOW_COMMAND(CmAABBMinMaxTest)
{
 // skeleton axes test
 if(GetActiveTest() != CM_AABB_MINMAX_TEST) BeginTest(CM_AABB_MINMAX_TEST);
 else if(GetActiveTest() == CM_AABB_MINMAX_TEST) EndTest();
 return 0;
}

WINDOW_COMMAND(CmMapTest)
{
 if(GetActiveTest() != CM_MAP_TEST) BeginTest(CM_MAP_TEST);
 else if(GetActiveTest() == CM_MAP_TEST) EndTest();
 return 0;
}

WINDOW_COMMAND(CmMeshTest)
{
 if(GetActiveTest() != CM_MESH_TEST) BeginTest(CM_MESH_TEST);
 else if(GetActiveTest() == CM_MESH_TEST) EndTest();
 return 0;
}

WINDOW_COMMAND(CmSoundTest)
{
 if(GetActiveTest() != CM_SOUND_TEST) BeginTest(CM_SOUND_TEST);
 else if(GetActiveTest() == CM_SOUND_TEST) EndTest();
 return 0;
}

#pragma endregion WINDOW_COMMANDS