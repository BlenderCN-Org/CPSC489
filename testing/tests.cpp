#include "../stdafx.h"
#include "../stdres.h"
#include "../errors.h"
#include "../win.h"
#include "../gfx.h"
#include "tests.h"

#include "sk_axes.h"

typedef BOOL (*InitFunc)(void);
typedef void (*FreeFunc)(void);
typedef void (*DrawFunc)(void);

static int active_test = -1;
static InitFunc init_func = nullptr;
static FreeFunc free_func = nullptr;
static DrawFunc draw_func = nullptr;

BOOL BeginTest(int cmd)
{
 // end previous test
 EndTest();

 // set test
 if(cmd == CM_SKELETON_AXES_TEST) {
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

 return TRUE;
}

void EndTest(void)
{
 if(free_func) (*free_func)();
 if(active_test != -1) CheckMenuItem(GetMenu(GetMainWindow()), active_test, MF_BYCOMMAND | MF_UNCHECKED);
 active_test = -1;
 init_func = nullptr;
 free_func = nullptr;
 draw_func = nullptr;
}

void RenderTest(void)
{
 if(draw_func) (*draw_func)();
}

BOOL IsTestActive(void)
{
 return (active_test == -1 ? FALSE : TRUE);
}

int GetActiveTest(void)
{
 return active_test;
}