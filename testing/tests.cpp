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

// General Tests
#include "t_mesh.h"
#include "t_sounds.h"

typedef BOOL (*InitFunc)(void);
typedef void (*FreeFunc)(void);
typedef void (*UpdateFunc)(real32 dt);
typedef void (*RenderFunc)(void);

static int active_test = -1;
static InitFunc init_func = nullptr;
static FreeFunc free_func = nullptr;
static UpdateFunc update_func = nullptr;
static RenderFunc render_func = nullptr;

BOOL BeginTest(int cmd)
{
 // end previous test
 EndTest();

 // set test
 if(cmd == CM_FLYBY_TEST) {
    init_func = InitFlybyTest;
    free_func = FreeFlybyTest;
    update_func = UpdateFlybyTest;
    render_func = RenderFlybyTest;
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
    update_func = UpdatePortalTest;
    render_func = RenderPortalTest;
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
    update_func = UpdateSkeletonAxesTest;
    render_func = RenderSkeletonAxesTest;
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
    update_func = UpdateAABBTest;
    render_func = RenderAABBTest;
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
    update_func = UpdateAABBMinMaxTest;
    render_func = RenderAABBMinMaxTest;
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
    update_func = UpdateMapTest;
    render_func = RenderMapTest;
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

 //
 // GENERAL TESTS
 //
 else if(cmd == CM_MESH_TEST) {
    init_func = InitMeshTest;
    free_func = FreeMeshTest;
    update_func = UpdateMeshTest;
    render_func = RenderMeshTest;
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
 else if(cmd == CM_SOUND_TEST) {
    init_func = InitSoundTest;
    free_func = FreeSoundTest;
    update_func = UpdateSoundTest;
    render_func = RenderSoundTest;
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
 update_func = nullptr;
 render_func = nullptr;
}

void UpdateTest(real32 dt)
{
 if(IsTestActive() && update_func) (*update_func)(dt);
}

void RenderTest(real32 dt)
{
 if(IsTestActive() && render_func) (*render_func)();
}

BOOL IsTestActive(void)
{
 return (active_test == -1 ? FALSE : TRUE);
}

int GetActiveTest(void)
{
 return active_test;
}