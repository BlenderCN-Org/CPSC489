#include "../stdafx.h"
#include "../stdgfx.h"
#include "../errors.h"
#include "../app.h"
#include "../math.h"
#include "../vector3.h"
#include "../matrix4.h"
#include "../model_v2.h"
#include "../camera.h"
#include "../xinput.h"
#include "../gfx.h"
#include "../axes.h"

#include "tests.h"
#include "t_mesh.h"

static MeshData mesh;

BOOL InitMeshTest(void)
{
 auto code = mesh.LoadMeshUTF(L"models/room_test.txt");
 if(Fail(code)) {
    Error(code);
    return FALSE;
   }

 return TRUE;
}

void FreeMeshTest(void)
{
 mesh.Free();
}

void RenderMeshTest(real32 dt)
{
}