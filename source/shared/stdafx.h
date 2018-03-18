#ifndef __SHARED_STDAFX_H
#define __SHARED_STDAFX_H

#include<iostream>
#include<fstream>
#include<vector>
#include<deque>
#include<memory>
using namespace std;

//
// Windows Version
//
#define WINVER 0x0601
#define _WIN32_WINNT 0x0601

//
// Windows Headers
//

#define NOMINMAX
#define STRSAFE_NO_DEPRECATE
#include<windows.h>
#ifndef RC_INVOKED
#include<tchar.h>
#include<strsafe.h>
#endif

//
// Standard Headers
//

#ifndef RC_INVOKED
#include<iostream>
#include<iomanip>
#include<fstream>
#include<sstream>
#include<memory>
#include<algorithm>
#include<limits>
#include<string>
#include<vector>
#include<array>
#include<deque>
#include<cmath>
#include<unordered_map>
#include<unordered_set>
#include<map>
#include<set>
#endif

//
// Boost Headers
//

#ifndef RC_INVOKED
#include<boost/algorithm/string.hpp>
#include<boost/algorithm/string/classification.hpp>
#include<boost/algorithm/string/split.hpp>
#include<boost/algorithm/string/trim.hpp>
#include<boost/algorithm/string/trim_all.hpp>
#endif

//
// DATA TYPES
//

// built-in types
typedef char sint08;
typedef unsigned char uint08;
typedef short sint16;
typedef unsigned short uint16;
typedef int sint32;
typedef unsigned int uint32;
typedef long long sint64;
typedef unsigned long long uint64;
typedef float real32;
typedef double real64;

// string types
typedef std::basic_string<char> STDSTRINGA;
typedef std::basic_string<wchar_t> STDSTRINGW;

// stringstream types
typedef std::basic_stringstream<char> STDSTRINGSTREAMA;
typedef std::basic_stringstream<wchar_t> STDSTRINGSTREAMW;

//
// STRING FUNCTIONS
//

STDSTRINGA ConvertUTF16ToUTF8(const wchar_t* str);
STDSTRINGW ConvertUTF8ToUTF16(const char* str);

#endif

