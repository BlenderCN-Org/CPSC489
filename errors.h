#ifndef __CPSC489_ERRORS_H
#define __CPSC489_ERRORS_H

enum ErrorCode {
 // Common Errors
 EC_SUCCESS = 0,
 EC_FILE_READ,
 EC_FILE_WRITE,
 // Windows Errors
 EC_WIN32_REGISTER_WINDOW,
 EC_WIN32_MAIN_WINDOW,
 // Direct3D: General Errors
 EC_D3D_CREATE_DEVICE,
 EC_D3D_DEVICE,
 EC_D3D_DEVICE_CONTEXT,
 EC_D3D_GET_BUFFER,
 EC_D3D_CREATE_RENDER_TARGET_VIEW,
 EC_D3D_CREATE_TEXTURE2D,
 EC_D3D_CREATE_DEPTH_STENCIL_VIEW,
 EC_D3D_RESIZE_BUFFERS,
 EC_D3D_SET_INPUT_LAYOUT,
 // Direct3D: Shader Errors
 EC_D3D_VERTEX_SHADER_FILE_NOT_FOUND,
 EC_D3D_VERTEX_SHADER_FILE_INVALID,
 EC_D3D_CREATE_VERTEX_SHADER,
 EC_D3D_SET_VERTEX_SHADER,
 EC_D3D_PIXEL_SHADER_FILE_NOT_FOUND,
 EC_D3D_PIXEL_SHADER_FILE_INVALID,
 EC_D3D_CREATE_PIXEL_SHADER,
 EC_D3D_SET_PIXEL_SHADER,
};

enum LanguageCode {
 LC_ENGLISH = 0,
};

// error string functions
void InitErrorStrings(void);
void FreeErrorStrings(void);
STDSTRINGW FindError(ErrorCode code, LanguageCode language = LC_ENGLISH);
inline ErrorCode Success(void) { return EC_SUCCESS; }
inline bool Fail(const ErrorCode& code) { return (code != EC_SUCCESS); }
bool Error(ErrorCode code);
bool Error(ErrorCode code, LanguageCode language);

// language code functions
LanguageCode GetLanguageCode(void);
void SetLanguageCode(LanguageCode code);

#endif
