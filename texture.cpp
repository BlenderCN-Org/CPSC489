#include "stdafx.h"
#include "stdwin.h"
#include "errors.h"
#include "app.h"
#include "gfx.h"
#include "texture.h"

// format includes
#include "bmp.h"
#include "png.h"
#include "tga.h"
#include "stc.h"

// texture variables
struct TextureResource {
 ID3D11ShaderResourceView* data;
 DWORD refs;
};
typedef std::unordered_map<std::wstring, TextureResource, WideStringHash, WideStringInsensitiveEqual> hashmap_type;
static hashmap_type hashmap;

ErrorCode LoadImage(LPCWSTR filename, TextureData* xlid)
{
 // validate
 if(!filename) return DebugErrorCode(EC_INVALID_ARG, __LINE__, __FILE__);
 if(!xlid) return DebugErrorCode(EC_INVALID_ARG, __LINE__, __FILE__);

 // split pathname
 auto pathname = GetPathnameFromFilenameW(filename);
 auto shrtname = GetShortFilenameWithoutExtensionW(filename);
 auto xtension = GetExtensionFromFilenameW(filename);
 if(!shrtname.length()) return DebugErrorCode(EC_FILE_FILENAME, __LINE__, __FILE__);
 if(!xtension.length()) return DebugErrorCode(EC_FILE_EXTENSION, __LINE__, __FILE__);

 // BMP (note that GetExtensionFromFilename extracts the dot)
 bool is_bmp = (_wcsicmp(xtension.c_str(), L".bmp") == 0);
 if(is_bmp) return LoadBMP(filename, xlid);

 // PNG (note that GetExtensionFromFilename extracts the dot)
 bool is_png = (_wcsicmp(xtension.c_str(), L".png") == 0);
 if(is_png) return LoadPNG(filename, xlid);

 // TGA (note that GetExtensionFromFilename extracts the dot)
 bool is_tga = (_wcsicmp(xtension.c_str(), L".tga") == 0);
 if(is_tga) return LoadTGA(filename, xlid);

 // STC (note that GetExtensionFromFilename extracts the dot)
 bool is_stc = (_wcsicmp(xtension.c_str(), L".stc") == 0);
 if(is_stc) return LoadSTC(filename, xlid);

 return DebugErrorCode(EC_IMAGE_FORMAT, __LINE__, __FILE__);
}

ErrorCode LoadTexture(LPCWSTR filename, ID3D11ShaderResourceView** srv)
{
 // must have device
 ID3D11Device* device = GetD3DDevice();
 if(!device) return DebugErrorCode(EC_D3D_DEVICE, __LINE__, __FILE__);

 // must have context
 ID3D11DeviceContext* context = GetD3DDeviceContext();
 if(!context) return DebugErrorCode(EC_D3D_DEVICE_CONTEXT, __LINE__, __FILE__);

 // lookup filename in hash table
 if(!filename) return DebugErrorCode(EC_INVALID_ARG, __LINE__, __FILE__);
 if(!srv) return DebugErrorCode(EC_INVALID_ARG, __LINE__, __FILE__);
 auto entry = hashmap.find(filename);

 // filename is not in hash table, create new texture
 if(entry == std::end(hashmap))
   {
    // step #1: read image
    TextureData xid;
    ErrorCode code = LoadImage(filename, &xid);
    if(code != EC_SUCCESS) return code;
    if(!xid.dx || xid.dx > D3D11_REQ_TEXTURE2D_U_OR_V_DIMENSION) return DebugErrorCode(EC_D3D_TEXTURE_DIMENSIONS, __LINE__, __FILE__);
    if(!xid.dy || xid.dy > D3D11_REQ_TEXTURE2D_U_OR_V_DIMENSION) return DebugErrorCode(EC_D3D_TEXTURE_DIMENSIONS, __LINE__, __FILE__);

    // step #2: compute number of mip levels
    UINT miplevels = 1;
    UINT dx = xid.dx;
    UINT dy = xid.dy;
    while((dx > 1) || (dy > 1)) {
          dx = std::max(dx/2, 1u);
          dy = std::max(dy/2, 1u);
          miplevels++;
         }

    // step #3: initialize subresource data
    std::unique_ptr<D3D11_SUBRESOURCE_DATA[]> srd(new D3D11_SUBRESOURCE_DATA[miplevels]);
    for(uint32 i = 0; i < miplevels; i++) ZeroMemory(&srd[i], sizeof(srd[i]));

    // step #4: fill out subresource data
    dx = xid.dx;
    dy = xid.dy;
    for(uint32 j = 0; j < miplevels; j++) {
        // set pitch
        srd[j].pSysMem = (LPCVOID)xid.data.get();
        if(xid.format == DXGI_FORMAT_B8G8R8X8_UNORM) srd[j].SysMemPitch = 4 * dx;
        else if(xid.format == DXGI_FORMAT_B8G8R8A8_UNORM) srd[j].SysMemPitch = 4 * dx;
        else if(xid.format == DXGI_FORMAT_R8G8B8A8_UNORM) srd[j].SysMemPitch = 4 * dx;
        else srd[j].SysMemPitch = 0;
        srd[j].SysMemSlicePitch = 0;
        // set next mip level dimensions
        dx = std::max(dx/2, 1u);
        dy = std::max(dy/2, 1u);
       }

    // step #5: fill out texture descriptor
    D3D11_TEXTURE2D_DESC t2dd;
    ZeroMemory(&t2dd, sizeof(t2dd));
    t2dd.Width = (UINT)xid.dx;
    t2dd.Height = (UINT)xid.dy;
    t2dd.MipLevels = 0;
    t2dd.ArraySize = 1;
    t2dd.Format = xid.format;
    t2dd.SampleDesc.Count = 1;
    t2dd.SampleDesc.Quality = 0;
    t2dd.Usage = D3D11_USAGE_DEFAULT;
    t2dd.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE; // D3D11_BIND_RENDER_TARGET is necessary for mipmap generation
    t2dd.CPUAccessFlags = 0;
    t2dd.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;

    // step #6: create texture
    CComPtr<ID3D11Texture2D> texture;
    HRESULT result = device->CreateTexture2D(&t2dd, srd.get(), &texture);
    if(FAILED(result)) return DebugErrorCode(EC_D3D_CREATE_TEXTURE2D, __LINE__, __FILE__);

    // step #7: fill out shader resource view descriptor
    D3D11_SHADER_RESOURCE_VIEW_DESC srvd;
    ZeroMemory(&srvd, sizeof(srvd));
    srvd.Format = t2dd.Format;
    srvd.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    srvd.Texture2D.MostDetailedMip = 0; // should always be 0, unless you want force rendering with lower-quality mip
    srvd.Texture2D.MipLevels = (UINT)-1; // use all mipmaps

    // step #8: create shader resource
    ID3D11ShaderResourceView* resource = NULL;
    result = device->CreateShaderResourceView(texture, &srvd, &resource);
    if(FAILED(result)) return DebugErrorCode(EC_D3D_CREATE_SHADER_RESOURCE, __LINE__, __FILE__);

    // step #9: generate mipmaps
    context->GenerateMips(resource);

    // step #10: insert resource into hash map
    TextureResource entry;
    entry.data = resource;
    entry.refs = 1;
    auto pairiter = hashmap.insert(hashmap_type::value_type(filename, entry));
    if(pairiter.second == false) {
       resource->Release();
       resource = nullptr;
       return DebugErrorCode(EC_D3D_INSERT_SHADER_RESOURCE, __LINE__, __FILE__);
      }

    // step #11: set resource
    *srv = resource;
   }
 // reuse texture
 else {
    TextureResource& res = entry->second;
    if(res.refs == 0) return DebugErrorCode(EC_D3D_SHADER_RESOURCE_REFERENCE_COUNT, __LINE__, __FILE__);
    res.refs++;
    *srv = res.data;
   }

 return EC_SUCCESS;
}

ErrorCode FreeTexture(LPCWSTR filename)
{
 // lookup filename in hash table
 auto entry = hashmap.find(filename);
 if(entry == std::end(hashmap)) return DebugErrorCode(EC_D3D_SHADER_RESOURCE, __LINE__, __FILE__);

 // a free request should NEVER have a reference count of zero
 TextureResource& resource = entry->second;
 if(resource.refs == 0) return DebugErrorCode(EC_D3D_SHADER_RESOURCE_REFERENCE_COUNT, __LINE__, __FILE__);

 // delete texture if no longer referenced
 resource.refs--;
 if(resource.refs == 0) {
    if(resource.data) resource.data->Release();
    resource.data = NULL;
    resource.refs = 0;
    hashmap.erase(entry);
   }

 return EC_SUCCESS;
}

ID3D11ShaderResourceView* FindTexture(LPCWSTR filename)
{
 auto entry = hashmap.find(filename);
 if(entry == std::end(hashmap)) return NULL;
 return entry->second.data;
}