#include "stdafx.h"

size_t WideStringHash::operator ()(const std::wstring& str)const
{
 const char* ptr = reinterpret_cast<const char*>(str.c_str());
 size_t hash = 0;
 for(size_t i = 0; i < (sizeof(std::wstring::value_type)/sizeof(char))*str.length(); i++) {
     hash += ptr[i];
     hash += (hash << 10);
     hash ^= (hash >> 6);
    }
 hash += (hash << 3);
 hash ^= (hash >> 11);
 hash += (hash << 15);
 return hash;
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