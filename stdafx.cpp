/*
** CPSC489 Game Development Project
** Copyright (c) 2018  Steven Emory <mobysteve@gmail.com>
**
** This program is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

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