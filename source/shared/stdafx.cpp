#include "stdafx.h"

//
// STRING FUNCTIONS
//

STDSTRINGA ConvertUTF16ToUTF8(const wchar_t* str)
{
 // validate
 if((str == NULL) || (*str == L'\0'))
    return STDSTRINGA();

 // fail if invalid input character is encountered
 #if (WINVER >= 0x0600)
 DWORD flags = WC_ERR_INVALID_CHARS;
 #else
 DWORD flags = 0;
 #endif

 // determine size of destination buffer including null
 size_t n = wcslen(str) + 1;
 int size = WideCharToMultiByte(CP_UTF8, flags, str, static_cast<int>(n), NULL, 0, NULL, NULL);
 if(size < 2) return STDSTRINGA();

 // convert and return result
 STDSTRINGA retval;
 retval.resize(size); 
 if(!WideCharToMultiByte(CP_UTF8, flags, str, static_cast<int>(n), &retval[0], size, NULL, NULL)) return STDSTRINGA();
 return retval;
}

STDSTRINGW ConvertUTF8ToUTF16(const char* str)
{
 // validate
 if((str == NULL) || (*str == '\0'))
    return STDSTRINGW();

 // retrieve string length (in CHAR)
 size_t len;
 HRESULT result = StringCchLengthA(str, 2048, &len);
 if(FAILED(result)) return STDSTRINGW();
 len++; // null-terminator
 
 // get required string length (in WCHAR)
 int clen = static_cast<int>(len);
 int wlen = MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, str, clen, NULL, 0);
 if(wlen == 0) return STDSTRINGW();

 // allocate and convert to WCHAR string
 STDSTRINGW retval(wlen, L' ');
 if(!MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, str, clen, &retval[0], wlen)) return STDSTRINGW();
 return retval;
}