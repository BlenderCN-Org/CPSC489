#include "../stdafx.h"
#include "../stdres.h"
#include "../errors.h"
#include "../win.h"
#include "../gfx.h"
#include "tests.h"

#include "sk_axes.h"

typedef BOOL (*InitFunc)(void);
typedef void (*FreeFunc)(void);

static int active_test = -1;
static InitFunc init_func = nullptr;
static FreeFunc free_func = nullptr;

BOOL BeginTest(int cmd)
{
 // end previous test
 EndTest();

 // set test
 if(cmd == CM_SKELETON_AXES_TEST) {
    init_func = InitSkeletonAxesTest;
    free_func = FreeSkeletonAxesTest;
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
}

BOOL IsTestActive(void)
{
 return (active_test == -1 ? FALSE : TRUE);
}

int GetActiveTest(void)
{
 return active_test;
}