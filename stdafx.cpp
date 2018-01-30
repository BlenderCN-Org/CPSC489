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