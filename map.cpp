#include "stdafx.h"
#include "errors.h"
#include "ascii.h"
#include "math.h"
#include "vector3.h"
#include "matrix4.h"
#include "model.h"
#include "map.h"

static std::vector<MeshUTF> models;
static std::vector<MeshUTFInstance> instances;

ErrorCode LoadMap(LPCWSTR name)
{
 return EC_SUCCESS;
}

void FreeMap(void)
{
}

void RenderMap(void)
{
}