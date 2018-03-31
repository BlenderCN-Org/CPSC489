#include "stdafx.h"
#include "stdwin.h"
#include "app.h"
#include "errors.h"

// error variables
typedef std::map<LanguageCode, STDSTRINGW> language_map_t;
typedef std::map<ErrorCode, language_map_t> codemap_t;
static codemap_t codemap;
static LanguageCode language = LC_ENGLISH;

// error debugging variables
struct LastErrorInfo {
 ErrorCode code;
 std::string line;
 std::string file;
 __int64 time;
};
static bool do_debug = false;
static std::ofstream debug;
static PerformanceCounter timer;
static constexpr size_t max_queue = 16;
static LastErrorInfo last_error_list[max_queue];
static size_t n_debug = 0;

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
 InsertErrorString(EC_UNKNOWN, LC_ENGLISH, L"Unknown error.");

 // Audio Errors
 InsertErrorString(EC_AUDIO_FORMAT, LC_ENGLISH, L"Invalid audio format (only WAVE and XWMA are supported).");
 InsertErrorString(EC_AUDIO_INIT, LC_ENGLISH, L"Failed to initialize XAudio 2.9.");
 InsertErrorString(EC_AUDIO_INVALID, LC_ENGLISH, L"Invalid audio file.");

 // File Errors
 InsertErrorString(EC_FILE_EMPTY, LC_ENGLISH, L"File is empty.");
 InsertErrorString(EC_FILE_OPEN, LC_ENGLISH, L"Failed to open file.");
 InsertErrorString(EC_FILE_SEEK, LC_ENGLISH, L"Failed to seek file position.");
 InsertErrorString(EC_FILE_READ, LC_ENGLISH, L"Failed to read file.");
 InsertErrorString(EC_FILE_WRITE, LC_ENGLISH, L"Failed to write to file.");
 InsertErrorString(EC_FILE_EOF, LC_ENGLISH, L"Read past EOF (end-of-file).");
 InsertErrorString(EC_FILE_PARSE, LC_ENGLISH, L"Parsed file data is not in the expected or correct format.");
 InsertErrorString(EC_FILE_PATHNAME, LC_ENGLISH, L"Invalid pathname.");
 InsertErrorString(EC_FILE_FILENAME, LC_ENGLISH, L"Invalid filename.");
 InsertErrorString(EC_FILE_EXTENSION, LC_ENGLISH, L"Invalid file extension.");
 InsertErrorString(EC_INVALID_ARG, LC_ENGLISH, L"Invalid argument(s).");

 // Stream Errors
 InsertErrorString(EC_STREAM_READ, LC_ENGLISH, L"Stream read failure.");
 InsertErrorString(EC_STREAM_SEEK, LC_ENGLISH, L"Stream seek failure.");

 // Windows Errors
 InsertErrorString(EC_WIN32_REGISTER_WINDOW, LC_ENGLISH, L"Failed to register window class.");
 InsertErrorString(EC_WIN32_MAIN_WINDOW, LC_ENGLISH, L"Invalid main window.");

 // Image Errors
 InsertErrorString(EC_IMAGE_FORMAT, LC_ENGLISH, L"Unsupported image format.");
 InsertErrorString(EC_PNG_COM_INIT, LC_ENGLISH, L"Failed to initialize PNG COM interfaces.");
 InsertErrorString(EC_PNG_DECODER, LC_ENGLISH, L"Failed to create PNG decoder.");
 InsertErrorString(EC_PNG_FILE_OPEN, LC_ENGLISH, L"Failed to open PNG file.");
 InsertErrorString(EC_PNG_DECODER_INIT, LC_ENGLISH, L"Failed to initialize PNG decoder.");
 InsertErrorString(EC_PNG_NO_FRAMES, LC_ENGLISH, L"PNG file has no frame(s).");
 InsertErrorString(EC_PNG_GET_FRAMES, LC_ENGLISH, L"Failed to get PNG frame(s).");
 InsertErrorString(EC_PNG_CONVERTER, LC_ENGLISH, L"Failed to create PNG converter.");
 InsertErrorString(EC_PNG_CONVERTER_INIT, LC_ENGLISH, L"Failed to initialize PNG converter.");
 InsertErrorString(EC_PNG_GET_SIZE, LC_ENGLISH, L"Failed to retrieve PNG dimensions.");
 InsertErrorString(EC_PNG_COPY, LC_ENGLISH, L"Failed to copy PNG data.");

 // Direct3D: General Errors
 InsertErrorString(EC_D3D_CREATE_DEVICE, LC_ENGLISH, L"Failed to create Direct3D device.");
 InsertErrorString(EC_D3D_DEVICE, LC_ENGLISH, L"Must have a valid Direct3D device for this operation.");
 InsertErrorString(EC_D3D_DEVICE_CONTEXT, LC_ENGLISH, L"Must have a valid Direct3D device context for this operation.");
 InsertErrorString(EC_D3D_GET_BACKBUFFER, LC_ENGLISH, L"IDXGISwapChain::GetBuffer failed.");
 InsertErrorString(EC_D3D_CREATE_RENDER_TARGET_VIEW, LC_ENGLISH, L"ID3D11Device::CreateRenderTargetView failed.");
 InsertErrorString(EC_D3D_CREATE_SHADER_RESOURCE, LC_ENGLISH, L"Failed to create Direct3D shader resource.");
 InsertErrorString(EC_D3D_INSERT_SHADER_RESOURCE, LC_ENGLISH, L"Failed to cache shader resource.");
 InsertErrorString(EC_D3D_SHADER_RESOURCE_REFERENCE_COUNT, LC_ENGLISH, L"Invalid shader resource reference count.");
 InsertErrorString(EC_D3D_SHADER_RESOURCE, LC_ENGLISH, L"Shader resource lookup failed.");
 InsertErrorString(EC_D3D_CREATE_TEXTURE2D, LC_ENGLISH, L"ID3D11Device::CreateTexture2D failed.");
 InsertErrorString(EC_D3D_TEXTURE_DIMENSIONS, LC_ENGLISH, L"Invalid Direct3D texture dimensions.");
 InsertErrorString(EC_D3D_CREATE_DEPTH_STENCIL_VIEW, LC_ENGLISH, L"ID3D11Device::CreateDepthStencilView failed.");
 InsertErrorString(EC_D3D_RESIZE_BUFFERS, LC_ENGLISH, L"IDXGISwapChain::ResizeBuffers failed.");
 InsertErrorString(EC_D3D_CREATE_BUFFER, LC_ENGLISH, L"Failed to create Direct3D buffer.");
 InsertErrorString(EC_D3D_MAP_RESOURCE, LC_ENGLISH, L"Failed to map Direct3D resource.");

 // Direct3D: State Errors
 InsertErrorString(EC_D3D_BLEND_STATE_CREATE, LC_ENGLISH, L"Failed to create Direct3D blend state.");
 InsertErrorString(EC_D3D_BLEND_STATE, LC_ENGLISH, L"Failed to set Direct3D blend state.");
 InsertErrorString(EC_D3D_RASTERIZER_STATE_CREATE, LC_ENGLISH, L"Failed to create Direct3D rasterizer state.");
 InsertErrorString(EC_D3D_RASTERIZER_STATE, LC_ENGLISH, L"Failed to set Direct3D rasterizer state.");
 InsertErrorString(EC_D3D_SAMPLER_STATE_CREATE, LC_ENGLISH, L"Failed to create Direct3D sampler state.");
 InsertErrorString(EC_D3D_SAMPLER_STATE, LC_ENGLISH, L"Failed to set Direct3D sampler state.");
 InsertErrorString(EC_D3D_STENCIL_STATE_CREATE, LC_ENGLISH, L"Failed to create Direct3D stencil state");
 InsertErrorString(EC_D3D_STENCIL_STATE, LC_ENGLISH, L"Failed to set Direct3D stencil state.");

 // Direct3D: Input Layout Errors
 InsertErrorString(EC_D3D_INPUT_LAYOUT_CREATE, LC_ENGLISH, L"Failed to create Direct3D input layout object.");
 InsertErrorString(EC_D3D_INPUT_LAYOUT_LOOKUP, LC_ENGLISH, L"Direct3D input layout index not found.");
 InsertErrorString(EC_D3D_INPUT_LAYOUT_SHADER_LOOKUP, LC_ENGLISH, L"Input layout index is not associated with a vertex shader.");
 InsertErrorString(EC_D3D_INPUT_LAYOUT_SHADER_SIGNATURE, LC_ENGLISH, L"Input layout index is not associated with a compiled vertex shader object.");
 InsertErrorString(EC_D3D_INPUT_LAYOUT, LC_ENGLISH, L"Direct3D input layout index out of bounds.");

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
 InsertErrorString(EC_MODEL_FACELIST, LC_ENGLISH, L"Invalid facelist.");
 InsertErrorString(EC_MODEL_KEYFRAMED_BONES, LC_ENGLISH, L"Animations must have at least one keyframed bone.");
 InsertErrorString(EC_MODEL_KEYFRAMES, LC_ENGLISH, L"Keyframed bones must have at least one keyframe.");
 InsertErrorString(EC_MODEL_MATERIAL, LC_ENGLISH, L"Model contains a mesh that has no materials defined.");
 InsertErrorString(EC_MODEL_MATERIAL_INDEX, LC_ENGLISH, L"Model contains a material with an out-of-range material index.");
 InsertErrorString(EC_MODEL_MATERIAL_NAME, LC_ENGLISH, L"Model contains a mesh with an invalid material name.");
 InsertErrorString(EC_MODEL_MESH, LC_ENGLISH, L"Model contains no meshes.");
 InsertErrorString(EC_MODEL_MESHNAME, LC_ENGLISH, L"Invalid model mesh name.");
 InsertErrorString(EC_MODEL_VERTICES, LC_ENGLISH, L"Invalid number of vertices.");
 InsertErrorString(EC_MODEL_UV_CHANNELS, LC_ENGLISH, L"Invalid number of UV channels.");
 InsertErrorString(EC_MODEL_COLOR_CHANNELS, LC_ENGLISH, L"Invalid number of color channels.");
 InsertErrorString(EC_MODEL_TEXTURES, LC_ENGLISH, L"Model contains a mesh that has no textures defined.");
 InsertErrorString(EC_MODEL_TEXTURE_SEMANTIC, LC_ENGLISH, L"Invalid texture semantic (e.g. diffuse, normal).");
 InsertErrorString(EC_MODEL_TEXTURE_CHANNEL, LC_ENGLISH, L"Texture reference out of bounds.");
 InsertErrorString(EC_MODEL_TEXTURE_FILENAME, LC_ENGLISH, L"Invalid texture filename.");
 InsertErrorString(EC_MODEL_TEXTURE_NAME, LC_ENGLISH, L"Invalid texture name.");
 InsertErrorString(EC_MODEL_TEXTURE_RESOURCES, LC_ENGLISH, L"Invalid number of texture resources.");

 // Animation Errors
 InsertErrorString(EC_ANIM_INDEX, LC_ENGLISH, L"Animation index out of bounds.");

 // Game Errors
 InsertErrorString(EC_LOAD_LEVEL, LC_ENGLISH, L"Failed to load level.");
 InsertErrorString(EC_HUD_INIT, LC_ENGLISH, L"Failed to initialize HUD.");
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

void EnableErrorDebugging(bool state)
{
 do_debug = state;
 if(do_debug) {
    STDSTRINGSTREAMW ss;
    ss << GetModulePathname().c_str() << L"debug.txt";
    debug.open(ss.str().c_str());
    if(!debug) return;
    // reset
    timer.reset();
    for(size_t i = 0; i < max_queue; i++) {
        last_error_list[i].code = EC_SUCCESS;
        last_error_list[i].line.clear();
        last_error_list[i].file.clear();
        last_error_list[i].time = 0ull;
       }
    n_debug = 0;
   }
 else
    debug.close();
}

ErrorCode DebugErrorCode(ErrorCode code, int line, const char* file)
{
 if(do_debug && debug.is_open()) {
    STDSTRINGW error = FindError(code, GetLanguageCode());
    auto str = ConvertUTF16ToUTF8(error.c_str());
    debug << str.c_str() << std::endl;
    debug << " Line: " << line << std::endl;
    debug << " File: " << file << std::endl;
   }
 return code;
}

ErrorCode DebugErrorCode(ErrorCode code, int line, const char* file, LanguageCode language)
{
 if(do_debug && debug.is_open()) {
    STDSTRINGW error = FindError(code, language);
    auto str = ConvertUTF16ToUTF8(error.c_str());
    debug << str.c_str() << std::endl;
    debug << " Line: " << line << std::endl;
    debug << " File: " << file << std::endl;
   }
 return code;
}

bool Fail(const ErrorCode& code, int line, const char* file)
{
 if(do_debug && debug.is_open()) {
    STDSTRINGW error = FindError(code, GetLanguageCode());
    auto str = ConvertUTF16ToUTF8(error.c_str());
    debug << str.c_str() << std::endl;
    debug << " Line: " << line << std::endl;
    debug << " File: " << file << std::endl;
   }
 return false;
}

bool Fail(ErrorCode code, int line, const char* file, LanguageCode language)
{
 if(do_debug && debug.is_open()) {
    STDSTRINGW error = FindError(code, language);
    auto str = ConvertUTF16ToUTF8(error.c_str());
    debug << str.c_str() << std::endl;
    debug << " Line: " << line << std::endl;
    debug << " File: " << file << std::endl;
   }
 return false;
}

LanguageCode GetLanguageCode(void)
{
 return ::language;
}

void SetLanguageCode(LanguageCode code)
{
 ::language = code; 
}