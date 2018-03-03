#ifndef __CS_TYPES_H
#define __CS_TYPES_H

// STL Headers
#include<cmath>
#include<functional>

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

#endif
