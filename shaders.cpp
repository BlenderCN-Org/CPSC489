#include "stdafx.h"
#include "errors.h"
#include "app.h"
#include "gfx.h"
#include "shaders.h"

// Shader Variables
typedef std::pair<std::basic_string<wchar_t>, DWORD> shader_list_type;
typedef std::map<DWORD, VertexShader> vs_map_type;
typedef std::map<DWORD, PixelShader> ps_map_type;
static std::deque<shader_list_type> vslist;
static std::deque<shader_list_type> pslist;
static vs_map_type vsm;
static ps_map_type psm;

ErrorCode CreateVertexShader(LPCTSTR filename, VertexShader* shader)
{
 // must have device
 ID3D11Device* device = GetD3DDevice();
 if(!device) return EC_D3D_DEVICE;

 // open file
 HANDLE handle = CreateFile(filename, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
 if(!handle) return EC_D3D_VERTEX_SHADER_FILE_NOT_FOUND;

 // get filesize
 LARGE_INTEGER filesize;
 if(!GetFileSizeEx(handle, &filesize)) {
    CloseHandle(handle);
    return EC_D3D_VERTEX_SHADER_FILE_INVALID;
   }
 DWORD size = static_cast<DWORD>(filesize.QuadPart);
 if(!size || (size > 1048576)) {
    CloseHandle(handle);
    return EC_D3D_VERTEX_SHADER_FILE_INVALID;
   }

 // read vertex shader data
 std::shared_ptr<char[]> data(new char[size]);
 DWORD bytes_read = 0;
 if(!ReadFile(handle, (LPVOID)data.get(), size, &bytes_read, NULL)) {
    CloseHandle(handle);
    return EC_FILE_READ;
   }
 if(bytes_read != size) {
    CloseHandle(handle);
    return EC_FILE_READ;
   }

 // close file handle
 CloseHandle(handle);
 handle = NULL;

 // create vertex shader
 HRESULT result = device->CreateVertexShader((LPCVOID)data.get(), size, NULL, &shader->shader);
 if(FAILED(result)) return EC_D3D_CREATE_VERTEX_SHADER;
 shader->code = std::move(data);
 shader->size = size;

 return EC_SUCCESS;
}

ErrorCode CreatePixelShader(LPCTSTR filename, PixelShader* shader)
{
 // must have device
 ID3D11Device* device = GetD3DDevice();
 if(!device) return EC_D3D_DEVICE;

 // open file
 HANDLE handle = CreateFile(filename, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
 if(!handle) return EC_D3D_PIXEL_SHADER_FILE_NOT_FOUND;

 // get filesize
 LARGE_INTEGER filesize;
 if(!GetFileSizeEx(handle, &filesize)) {
    CloseHandle(handle);
    return EC_D3D_PIXEL_SHADER_FILE_INVALID;
   }
 DWORD size = static_cast<DWORD>(filesize.QuadPart);
 if(!size || (size > 1048576)) {
    CloseHandle(handle);
    return EC_D3D_PIXEL_SHADER_FILE_INVALID;
   }

 // read pixel shader data
 std::shared_ptr<char[]> data(new char[size]);
 DWORD bytes_read = 0;
 if(!ReadFile(handle, (LPVOID)data.get(), size, &bytes_read, NULL)) {
    CloseHandle(handle);
    return EC_FILE_READ;
   }
 if(bytes_read != size) {
    CloseHandle(handle);
    return EC_FILE_READ;
   }

 // close file handle
 CloseHandle(handle);
 handle = NULL;

 // create pixel shader
 HRESULT result = device->CreatePixelShader((LPCVOID)data.get(), size, NULL, &shader->shader);
 if(FAILED(result)) return EC_D3D_CREATE_VERTEX_SHADER;
 shader->code = std::move(data);
 shader->size = size;

 return EC_SUCCESS;
}

ErrorCode InitVertexShaders(void)
{
 // free previous
 FreeVertexShaders();

 // vertex shader associations
 STDSTRINGW path = L"";//GetModulePathnameW();
 vslist.push_back(shader_list_type(path + std::basic_string<wchar_t>(TEXT("shaders\\VS\\VS_000.cso")), 0));
 vslist.push_back(shader_list_type(path + std::basic_string<wchar_t>(TEXT("shaders\\VS\\VS_001.cso")), 1));
 vslist.push_back(shader_list_type(path + std::basic_string<wchar_t>(TEXT("shaders\\VS\\VS_002.cso")), 2));
 // vslist.push_back(shader_list_type(path + std::basic_string<wchar_t>(TEXT("shaders\\VS\\VS_003.cso")), 3));
 // vslist.push_back(shader_list_type(path + std::basic_string<wchar_t>(TEXT("shaders\\VS\\VS_004.cso")), 4));
 // vslist.push_back(shader_list_type(path + std::basic_string<wchar_t>(TEXT("shaders\\VS\\VS_005.cso")), 5));
 // vslist.push_back(shader_list_type(path + std::basic_string<wchar_t>(TEXT("shaders\\VS\\VS_006.cso")), 6));
 // vslist.push_back(shader_list_type(path + std::basic_string<wchar_t>(TEXT("shaders\\VS\\VS_007.cso")), 7));
 // vslist.push_back(shader_list_type(path + std::basic_string<wchar_t>(TEXT("shaders\\VS\\VS_008.cso")), 8));
 // vslist.push_back(shader_list_type(path + std::basic_string<wchar_t>(TEXT("shaders\\VS\\VS_009.cso")), 9));
 // vslist.push_back(shader_list_type(path + std::basic_string<wchar_t>(TEXT("shaders\\VS\\VS_010.cso")), 10));
 // vslist.push_back(shader_list_type(path + std::basic_string<wchar_t>(TEXT("shaders\\VS\\VS_011.cso")), 11));
 // vslist.push_back(shader_list_type(path + std::basic_string<wchar_t>(TEXT("shaders\\VS\\VS_012.cso")), 12));
 // vslist.push_back(shader_list_type(path + std::basic_string<wchar_t>(TEXT("shaders\\VS\\VS_013.cso")), 13));
 // vslist.push_back(shader_list_type(path + std::basic_string<wchar_t>(TEXT("shaders\\VS\\VS_014.cso")), 14));
 // vslist.push_back(shader_list_type(path + std::basic_string<wchar_t>(TEXT("shaders\\VS\\VS_015.cso")), 15));

 // load vertex shaders
 for(size_t i = 0; i < vslist.size(); i++) {
     VertexShader shader;
     ErrorCode code = CreateVertexShader(vslist[i].first.c_str(), &shader);
     if(Fail(code)) return code;
     vs_map_type::value_type item;
     vsm.insert(vs_map_type::value_type(vslist[i].second, shader));
    }

 return EC_SUCCESS;
}

void FreeVertexShaders(void)
{
 for(auto iter = vsm.begin(); iter != vsm.end(); iter++) {
     iter->second.shader->Release();
     iter->second.shader = 0;
    }
 vsm.erase(vsm.begin(), vsm.end());
 vsm.clear();
 vslist.clear();
}

VertexShader* GetVertexShader(DWORD id)
{
 auto iter = vsm.find(id);
 if(iter == vsm.end()) return nullptr;
 return &(iter->second);
}

ErrorCode SetVertexShader(DWORD id)
{
 // must have device context
 ID3D11DeviceContext* context = GetD3DDeviceContext();
 if(!context) return EC_D3D_DEVICE_CONTEXT;

 // look up shader
 auto iter = vsm.find(id);
 if(iter == vsm.end()) return EC_D3D_SET_VERTEX_SHADER;

 // set shader
 if(!iter->second.shader) return EC_D3D_SET_VERTEX_SHADER;
 context->VSSetShader(iter->second.shader, NULL, 0);

 // success
 return EC_SUCCESS;
}

ErrorCode InitPixelShaders(void)
{
 // free previous
 FreePixelShaders();

 // vertex shader associations
 STDSTRINGW path = L"";//GetModulePathnameW();
 pslist.push_back(shader_list_type(path + std::basic_string<wchar_t>(TEXT("shaders\\PS\\PS_000.cso")), 0));
 pslist.push_back(shader_list_type(path + std::basic_string<wchar_t>(TEXT("shaders\\PS\\PS_001.cso")), 1));
 pslist.push_back(shader_list_type(path + std::basic_string<wchar_t>(TEXT("shaders\\PS\\PS_002.cso")), 2));
 // pslist.push_back(shader_list_type(path + std::basic_string<wchar_t>(TEXT("shaders\\PS\\PS_003.cso")), 3));
 // pslist.push_back(shader_list_type(path + std::basic_string<wchar_t>(TEXT("shaders\\PS\\PS_004.cso")), 4));
 // pslist.push_back(shader_list_type(path + std::basic_string<wchar_t>(TEXT("shaders\\PS\\PS_005.cso")), 5));
 // pslist.push_back(shader_list_type(path + std::basic_string<wchar_t>(TEXT("shaders\\PS\\PS_006.cso")), 6));
 // pslist.push_back(shader_list_type(path + std::basic_string<wchar_t>(TEXT("shaders\\PS\\PS_007.cso")), 7));
 // pslist.push_back(shader_list_type(path + std::basic_string<wchar_t>(TEXT("shaders\\PS\\PS_008.cso")), 8));
 // pslist.push_back(shader_list_type(path + std::basic_string<wchar_t>(TEXT("shaders\\PS\\PS_009.cso")), 9));
 // pslist.push_back(shader_list_type(path + std::basic_string<wchar_t>(TEXT("shaders\\PS\\PS_010.cso")), 10));
 // pslist.push_back(shader_list_type(path + std::basic_string<wchar_t>(TEXT("shaders\\PS\\PS_011.cso")), 11));
 // pslist.push_back(shader_list_type(path + std::basic_string<wchar_t>(TEXT("shaders\\PS\\PS_012.cso")), 12));
 // pslist.push_back(shader_list_type(path + std::basic_string<wchar_t>(TEXT("shaders\\PS\\PS_013.cso")), 13));
 // pslist.push_back(shader_list_type(path + std::basic_string<wchar_t>(TEXT("shaders\\PS\\PS_014.cso")), 14));
 // pslist.push_back(shader_list_type(path + std::basic_string<wchar_t>(TEXT("shaders\\PS\\PS_015.cso")), 15));

 // load vertex shaders
 for(size_t i = 0; i < pslist.size(); i++) {
     PixelShader shader;
     ErrorCode code = CreatePixelShader(pslist[i].first.c_str(), &shader);
     if(Fail(code)) return code;
     psm.insert(ps_map_type::value_type(pslist[i].second, shader));
    }

 return EC_SUCCESS;
}

void FreePixelShaders(void)
{
 for(auto iter = psm.begin(); iter != psm.end(); iter++) {
     iter->second.shader->Release();
     iter->second.shader = 0;
    }
 psm.erase(psm.begin(), psm.end());
 psm.clear();
 pslist.clear();
}

PixelShader* GetPixelShader(DWORD id)
{
 auto iter = psm.find(id);
 if(iter == psm.end()) return nullptr;
 return &(iter->second);
}

ErrorCode SetPixelShader(DWORD id)
{
 // must have device context
 ID3D11DeviceContext* context = GetD3DDeviceContext();
 if(!context) return EC_D3D_DEVICE_CONTEXT;

 // look up shader
 auto iter = psm.find(id);
 if(iter == psm.end()) return EC_D3D_SET_PIXEL_SHADER;

 // set shader
 if(!iter->second.shader) return EC_D3D_SET_PIXEL_SHADER;
 context->PSSetShader(iter->second.shader, NULL, 0);

 // success
 return EC_SUCCESS;
}