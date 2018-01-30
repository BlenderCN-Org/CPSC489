#ifndef __ERRORS_H
#define __ERRORS_H

enum ErrorCode {
 // Common Errors
 EC_SUCCESS = 0,
 // Windows Errors
 EC_WIN32_REGISTER_WINDOW,
 EC_WIN32_MAIN_WINDOW,
 // Direct3D Errors
 EC_D3D_CREATE_DEVICE,
 EC_D3D_GET_BUFFER,
 EC_D3D_CREATE_RENDER_TARGET_VIEW,
 EC_D3D_CREATE_TEXTURE2D,
 EC_D3D_CREATE_DEPTH_STENCIL_VIEW,
};

enum LanguageCode {
 LC_ENGLISH = 0,
 LC_FRENCH,
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
