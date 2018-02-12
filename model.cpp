#include "stdafx.h"
#include "stdgfx.h"
#include "errors.h"
#include "gfx.h"
#include "ascii.h"
#include "model.h"

MeshUTF::MeshUTF()
{
}

MeshUTF::~MeshUTF()
{
}

ErrorCode MeshUTF::LoadModel(const wchar_t* filename)
{
 // must have device
 ID3D11Device* device = GetD3DDevice();
 if(!device) return EC_D3D_DEVICE;

 // load lines
 std::deque<std::string> linelist;
 ErrorCode code = ASCIIParseFile(filename, linelist);
 if(Fail(code)) return code;

 return EC_SUCCESS;
}

void MeshUTF::FreeModel(void)
{
 for(size_t i = 0; i < buffers.size(); i++) {
     buffers[i].jd_buffer->Release(); buffers[i].jd_buffer = nullptr;
     buffers[i].jv_buffer->Release(); buffers[i].jv_buffer = nullptr;
     buffers[i].ji_buffer->Release(); buffers[i].ji_buffer = nullptr;
     buffers[i].jc_buffer->Release(); buffers[i].jc_buffer = nullptr;
     buffers[i].mm_buffer->Release(); buffers[i].mm_buffer = nullptr;
    }
 buffers.clear();
 animations.clear();
 meshes.clear();
 joints.clear();
}

ErrorCode MeshUTF::RenderSkeleton(void)
{
 return EC_SUCCESS;
}

ErrorCode MeshUTF::RenderMeshList(void)
{
 return EC_SUCCESS;
}

ErrorCode MeshUTF::RenderModel(void)
{
 return EC_SUCCESS;
}
