#include "stdafx.h"
#include "app.h"
#include "errors.h"

// error variables
typedef std::map<LanguageCode, STDSTRINGW> language_map_t;
typedef std::map<ErrorCode, language_map_t> codemap_t;
static codemap_t codemap;
static LanguageCode language = LC_ENGLISH;

void InsertErrorString(ErrorCode code, LanguageCode language, LPCWSTR error)
{
 typedef std::pair<codemap_t::iterator, bool> result_type;
 result_type result;
 result = codemap.insert(codemap_t::value_type(code, language_map_t()));
 result.first->second.insert(language_map_t::value_type(language, error));
}

void InitErrorStrings(void)
{
 // free pre-existing data
 FreeErrorStrings();

 // Common Errors
 InsertErrorString(EC_SUCCESS, LC_ENGLISH, L"");
 InsertErrorString(EC_FILE_OPEN, LC_ENGLISH, L"Failed to open file.");
 InsertErrorString(EC_FILE_READ, LC_ENGLISH, L"Failed to read file.");
 InsertErrorString(EC_FILE_WRITE, LC_ENGLISH, L"Failed to write to file.");
 InsertErrorString(EC_FILE_EOF, LC_ENGLISH, L"Read past EOF (end-of-file).");
 InsertErrorString(EC_FILE_PARSE, LC_ENGLISH, L"Parsed file data is not in the expected or correct format.");

 // Windows Errors
 InsertErrorString(EC_WIN32_REGISTER_WINDOW, LC_ENGLISH, L"Failed to register window class.");
 InsertErrorString(EC_WIN32_MAIN_WINDOW, LC_ENGLISH, L"Invalid main window.");

 // Direct3D: General Errors
 InsertErrorString(EC_D3D_CREATE_DEVICE, LC_ENGLISH, L"Failed to create Direct3D device.");
 InsertErrorString(EC_D3D_DEVICE, LC_ENGLISH, L"Must have a valid Direct3D device for this operation.");
 InsertErrorString(EC_D3D_DEVICE_CONTEXT, LC_ENGLISH, L"Must have a valid Direct3D device context for this operation.");
 InsertErrorString(EC_D3D_GET_BACKBUFFER, LC_ENGLISH, L"IDXGISwapChain::GetBuffer failed.");
 InsertErrorString(EC_D3D_CREATE_RENDER_TARGET_VIEW, LC_ENGLISH, L"ID3D11Device::CreateRenderTargetView failed.");
 InsertErrorString(EC_D3D_CREATE_TEXTURE2D, LC_ENGLISH, L"ID3D11Device::CreateTexture2D failed.");
 InsertErrorString(EC_D3D_CREATE_DEPTH_STENCIL_VIEW, LC_ENGLISH, L"ID3D11Device::CreateDepthStencilView failed.");
 InsertErrorString(EC_D3D_RESIZE_BUFFERS, LC_ENGLISH, L"IDXGISwapChain::ResizeBuffers failed.");
 InsertErrorString(EC_D3D_SET_INPUT_LAYOUT, LC_ENGLISH, L"Direct3D input layout not found."); // failed to set input layout (couldn't find it that's why)
 InsertErrorString(EC_D3D_CREATE_BUFFER, LC_ENGLISH, L"Failed to create Direct3D buffer.");
 InsertErrorString(EC_D3D_MAP_RESOURCE, LC_ENGLISH, L"Failed to map Direct3D resource.");

 // Direct3D: Rasterizer State Errors
 InsertErrorString(EC_D3D_CREATE_RASTERIZER_STATE, LC_ENGLISH, L"Failed to create Direct3D rasterizer state.");
 InsertErrorString(EC_D3D_RASTERIZER_STATE, LC_ENGLISH, L"Invalid rasterizer state index.");

 // Direct3D: Input Layout Errors
 InsertErrorString(EC_D3D_INPUT_LAYOUT_INDEX, LC_ENGLISH, L"Input layout index does not exist.");
 InsertErrorString(EC_D3D_INPUT_LAYOUT_SHADER_LOOKUP, LC_ENGLISH, L"Input layout index is not associated with a vertex shader.");
 InsertErrorString(EC_D3D_INPUT_LAYOUT_SHADER_SIGNATURE, LC_ENGLISH, L"Input layout index is not associated with a compiled vertex shader object.");
 InsertErrorString(EC_D3D_CREATE_INPUT_LAYOUT, LC_ENGLISH, L"Failed to create Direct3D input layout object.");

 // Direct3D: Shader Errors
 InsertErrorString(EC_D3D_VERTEX_SHADER_FILE_NOT_FOUND, LC_ENGLISH, L"Vertex shader file not found.");
 InsertErrorString(EC_D3D_VERTEX_SHADER_FILE_INVALID, LC_ENGLISH, L"Invalid vertex shader file.");
 InsertErrorString(EC_D3D_CREATE_VERTEX_SHADER, LC_ENGLISH, L"Failed to create Direct3D vertex shader.");
 InsertErrorString(EC_D3D_SET_VERTEX_SHADER, LC_ENGLISH, L"Failed to set Direct3D vertex shader.");
 InsertErrorString(EC_D3D_PIXEL_SHADER_FILE_NOT_FOUND, LC_ENGLISH, L"Pixel shader file not found.");
 InsertErrorString(EC_D3D_PIXEL_SHADER_FILE_INVALID, LC_ENGLISH, L"Invalid pixel shader file.");
 InsertErrorString(EC_D3D_CREATE_PIXEL_SHADER, LC_ENGLISH, L"Failed to create Direct3D pixel shader.");
 InsertErrorString(EC_D3D_SET_PIXEL_SHADER, LC_ENGLISH, L"Failed to set Direct3D pixel shader.");

 // Model Errors
 InsertErrorString(EC_MODEL_BONENAME, LC_ENGLISH, L"Invalid bone name.");
 InsertErrorString(EC_MODEL_BONE_LOOKUP, LC_ENGLISH, L"Bone lookup failed.");
 InsertErrorString(EC_MODEL_ANIMATION_NAME, LC_ENGLISH, L"Invalid animation name.");
 InsertErrorString(EC_MODEL_KEYFRAMED_BONES, LC_ENGLISH, L"Animations must have at least one keyframed bone.");
 InsertErrorString(EC_MODEL_KEYFRAMES, LC_ENGLISH, L"Keyframed bones must have at least one keyframe.");
 InsertErrorString(EC_MODEL_MESH, LC_ENGLISH, L"Model contains no meshes.");
 InsertErrorString(EC_MODEL_MESHNAME, LC_ENGLISH, L"Invalid model mesh name.");
 InsertErrorString(EC_MODEL_VERTICES, LC_ENGLISH, L"Invalid number of vertices.");
 InsertErrorString(EC_MODEL_UV_CHANNELS, LC_ENGLISH, L"Invalid number of UV channels.");
 InsertErrorString(EC_MODEL_COLOR_CHANNELS, LC_ENGLISH, L"Invalid number of color channels.");
 InsertErrorString(EC_MODEL_TEXTURES, LC_ENGLISH, L"Invalid number of textures.");
 InsertErrorString(EC_MODEL_TEXTURE_SEMANTIC, LC_ENGLISH, L"Invalid texture semantic (e.g. diffuse, normal).");
 InsertErrorString(EC_MODEL_TEXTURE_CHANNEL, LC_ENGLISH, L"Texture reference out of bounds.");
 InsertErrorString(EC_MODEL_TEXTURE_FILENAME, LC_ENGLISH, L"Invalid texture filename.");

 // Animation Errors
 InsertErrorString(EC_ANIM_INDEX, LC_ENGLISH, L"Animation index out of bounds.");
}

void FreeErrorStrings(void)
{
 codemap.clear();
}

STDSTRINGW FindError(ErrorCode code, LanguageCode language)
{
 auto iter = codemap.find(code);
 if(iter == codemap.end()) {
    STDSTRINGSTREAMW ss;
    ss << L"Error message code 0x" << std::hex << code << std::dec << L" not found.";
    return ss.str();
   }
 auto msgiter = iter->second.find(language);
 if(msgiter == iter->second.end()) return L"Error message not found for this language.";
 return msgiter->second;
}

bool Error(ErrorCode code)
{
 STDSTRINGW error = FindError(code, GetLanguageCode());
 ErrorBox(error.c_str());
 return false;
}

bool Error(ErrorCode code, LanguageCode language)
{
 STDSTRINGW error = FindError(code, language);
 ErrorBox(error.c_str());
 return false;
}

ErrorCode Error(ErrorCode code, int line, const char* file)
{
 STDSTRINGW error = FindError(code, GetLanguageCode());
 STDSTRINGSTREAMW ss;
 ss << error << std::endl;
 ss << L"Line: " << line << std::endl;
 ss << L"File: " << file << std::ends;
 ErrorBox(ss.str().c_str());
 return code;
}

ErrorCode Error(ErrorCode code, int line, const char* file, LanguageCode language)
{
 STDSTRINGW error = FindError(code, language);
 STDSTRINGSTREAMW ss;
 ss << error << std::endl;
 ss << L"Line: " << line << std::endl;
 ss << L"File: " << file << std::ends;
 ErrorBox(ss.str().c_str());
 return code;
}

LanguageCode GetLanguageCode(void)
{
 return ::language;
}

void SetLanguageCode(LanguageCode code)
{
 ::language = code; 
}