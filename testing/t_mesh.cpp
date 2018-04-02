#include "../stdafx.h"
#include "../stdgfx.h"
#include "../errors.h"
#include "../app.h"
#include "../math.h"
#include "../vector3.h"
#include "../matrix4.h"
#include "../model_v2.h"
#include "../meshinst.h"
#include "../camera.h"
#include "../xinput.h"
#include "../gfx.h"
#include "../axes.h"

#include "tests.h"
#include "t_mesh.h"

static MeshData mesh;
static std::unique_ptr<MeshInstance> instance;

BOOL InitMeshTest(void)
{
 auto code = mesh.LoadMeshUTF(L"models/room.txt");
 if(Fail(code)) {
    Error(code);
    return FALSE;
   }
 instance = std::make_unique<MeshInstance>(&mesh);
 instance->InitInstance();

 return TRUE;
}

void FreeMeshTest(void)
{
 instance.reset();
 mesh.Free();
}

void RenderMeshTest(real32 dt)
{
 if(instance) instance->RenderModel();
}