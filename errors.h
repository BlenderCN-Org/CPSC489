#ifndef __ERRORS_H
#define __ERRORS_H

enum ErrorCode {
 // Common Errors
 EC_SUCCESS = 0,
 // Windows Errors
 EC_WIN32_REGISTER_WINDOW,
 EC_WIN32_MAIN_WINDOW,
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
