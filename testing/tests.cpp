#include "../stdafx.h"
#include "../stdres.h"
#include "../errors.h"
#include "../win.h"
#include "../gfx.h"
#include "tests.h"

// tests
#include "flyby.h"
#include "t_map.h"
#include "t_portal.h"
#include "sk_axes.h"
#include "t_aabb.h"
#include "t_minmax.h"

typedef BOOL (*InitFunc)(void);
typedef void (*FreeFunc)(void);
typedef void (*DrawFunc)(real32 dt);

static int active_test = -1;
static InitFunc init_func = nullptr;
static FreeFunc free_func = nullptr;
static DrawFunc draw_func = nullptr;

BOOL BeginTest(int cmd)
{
 // end previous test
 EndTest();

 // set test
 if(cmd == CM_FLYBY_TEST) {
    init_func = InitFlybyTest;
    free_func = FreeFlybyTest;
    draw_func = RenderFlybyTest;
    if((*init_func)()) {
       active_test = cmd;
       CheckMenuItem(GetMenu(GetMainWindow()), active_test, MF_BYCOMMAND | MF_CHECKED);
       return TRUE;
      }
    else {
       (*free_func)();
       return FALSE;
      }
   }
 // set test
 else if(cmd == CM_PORTAL_TEST) {
    init_func = InitPortalTest;
    free_func = FreePortalTest;
    draw_func = RenderPortalTest;
    if((*init_func)()) {
       active_test = cmd;
       CheckMenuItem(GetMenu(GetMainWindow()), active_test, MF_BYCOMMAND | MF_CHECKED);
       return TRUE;
      }
    else {
       (*free_func)();
       return FALSE;
      }
   }
 // set test
 else if(cmd == CM_SKELETON_AXES_TEST) {
    init_func = InitSkeletonAxesTest;
    free_func = FreeSkeletonAxesTest;
    draw_func = RenderSkeletonAxesTest;
    if((*init_func)()) {
       active_test = cmd;
       CheckMenuItem(GetMenu(GetMainWindow()), active_test, MF_BYCOMMAND | MF_CHECKED);
       return TRUE;
      }
    else {
       (*free_func)();
       return FALSE;
      }
   }
 // set test
 else if(cmd == CM_AABB_TEST) {
    init_func = InitAABBTest;
    free_func = FreeAABBTest;
    draw_func = RenderAABBTest;
    if((*init_func)()) {
       active_test = cmd;
       CheckMenuItem(GetMenu(GetMainWindow()), active_test, MF_BYCOMMAND | MF_CHECKED);
       return TRUE;
      }
    else {
       (*free_func)();
       return FALSE;
      }
   }
 // set test
 else if(cmd == CM_AABB_MINMAX_TEST) {
    init_func = InitAABBMinMaxTest;
    free_func = FreeAABBMinMaxTest;
    draw_func = RenderAABBMinMaxTest;
    if((*init_func)()) {
       active_test = cmd;
       CheckMenuItem(GetMenu(GetMainWindow()), active_test, MF_BYCOMMAND | MF_CHECKED);
       return TRUE;
      }
    else {
       (*free_func)();
       return FALSE;
      }
   }
 // set test
 else if(cmd == CM_MAP_TEST) {
    init_func = InitMapTest;
    free_func = FreeMapTest;
    draw_func = RenderMapTest;
    if((*init_func)()) {
       active_test = cmd;
       CheckMenuItem(GetMenu(GetMainWindow()), active_test, MF_BYCOMMAND | MF_CHECKED);
       return TRUE;
      }
    else {
       (*free_func)();
       return FALSE;
      }
   }

 return TRUE;
}

void EndTest(void)
{
 // free test and uncheck menu
 if(IsTestActive()) {
    if(free_func) (*free_func)();
    CheckMenuItem(GetMenu(GetMainWindow()), active_test, MF_BYCOMMAND | MF_UNCHECKED);
   }

 // reset
 active_test = -1;
 init_func = nullptr;
 free_func = nullptr;
 draw_func = nullptr;
}

void RenderTest(real32 dt)
{
 if(IsTestActive() && draw_func) (*draw_func)(dt);
}

BOOL IsTestActive(void)
{
 return (active_test == -1 ? FALSE : TRUE);
}

int GetActiveTest(void)
{
 return active_test;
}