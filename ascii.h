#ifndef __CPSC489_ASCII_H
#define __CPSC489_ASCII_H

// ASCII Utilities
ErrorCode ASCIIParseFile(const wchar_t* filename, std::deque<std::string>& linelist);
ErrorCode ASCIIReadString(std::deque<std::string>& linelist, char* str);
ErrorCode ASCIIReadUTF8String(std::deque<std::string>& linelist, STDSTRINGW& str);
ErrorCode ASCIIReadSint32(std::deque<std::string>& linelist, sint32* x);
ErrorCode ASCIIReadUint32(std::deque<std::string>& linelist, uint32* x);
ErrorCode ASCIIReadVector4(std::deque<std::string>& linelist, uint08* v, bool repeat = false);
ErrorCode ASCIIReadVector4(std::deque<std::string>& linelist, uint16* v, bool repeat = false);
ErrorCode ASCIIReadVector8(std::deque<std::string>& linelist, uint16* v, bool repeat = false);
ErrorCode ASCIIReadVector2(std::deque<std::string>& linelist, uint32* v, bool repeat = false);
ErrorCode ASCIIReadVector3(std::deque<std::string>& linelist, uint32* v, bool repeat = false);
ErrorCode ASCIIReadVector4(std::deque<std::string>& linelist, uint32* v, bool repeat = false);
ErrorCode ASCIIReadVector2(std::deque<std::string>& linelist, real32* v, bool repeat = false);
ErrorCode ASCIIReadVector3(std::deque<std::string>& linelist, real32* v, bool repeat = false);
ErrorCode ASCIIReadVector4(std::deque<std::string>& linelist, real32* v, bool repeat = false);
ErrorCode ASCIIReadVector8(std::deque<std::string>& linelist, real32* v, bool repeat = false);
ErrorCode ASCIIReadMatrix3(std::deque<std::string>& linelist, real32* v, bool repeat = false);
ErrorCode ASCIIReadMatrix4(std::deque<std::string>& linelist, real32* v, bool repeat = false);

ErrorCode ASCIIReadUint32Array(std::deque<std::string>& linelist, std::vector<uint32>& data); 

#endif
