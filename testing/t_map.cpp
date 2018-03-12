// non-testing includes
#include "../stdafx.h"
#include "../stdgfx.h"
#include "../errors.h"
#include "../math.h"
#include "../vector3.h"
#include "../matrix4.h"
#include "../model.h"
#include "../camera.h"
#include "../xinput.h"
#include "../gfx.h"
#include "../map.h"

// testing includes
#include "tests.h"
#include "t_map.h"

BOOL InitMapTest(void)
{
 // get singleton map pointer
 Map* map = GetMap();
 ErrorCode code = map->LoadMap(L"models\\map_cube.txt");
 if(Fail(code)) return FALSE;

 return TRUE;
}

void FreeMapTest(void)
{
 // unload map
 GetMap()->FreeMap();
}

void RenderMapTest(real32 dt)
{
}