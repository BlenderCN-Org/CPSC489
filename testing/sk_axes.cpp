#include "../stdafx.h"
#include "../stdgfx.h"
#include "../errors.h"
#include "../model.h"
#include "../gfx.h"
#include "sk_axes.h"

static MeshUTF model;

BOOL InitSkeletonAxesTest(void)
{
 auto code = model.LoadModel(L"bl_TestVertexGroups.txt");
 if(Fail(code)) {
    Error(code);
    return FALSE;
   }
 return TRUE;
}

void FreeSkeletonAxesTest(void)
{
 model.FreeModel();
}