#ifndef __CS489_ASCII_H
#define __CS489_ASCII_H

// ASCII Utilities
bool ASCIIParseFile(const wchar_t* filename, std::deque<std::string>& linelist);
bool ASCIIReadString(std::deque<std::string>& linelist, char* str);
bool ASCIIReadUTF8String(std::deque<std::string>& linelist, STDSTRINGW& str);
bool ASCIIReadSint32(std::deque<std::string>& linelist, sint32* x);
bool ASCIIReadUint32(std::deque<std::string>& linelist, uint32* x);
bool ASCIIReadVector4(std::deque<std::string>& linelist, uint08* v, bool repeat = false);
bool ASCIIReadVector4(std::deque<std::string>& linelist, uint16* v, bool repeat = false);
bool ASCIIReadVector8(std::deque<std::string>& linelist, uint16* v, bool repeat = false);
bool ASCIIReadVector2(std::deque<std::string>& linelist, uint32* v, bool repeat = false);
bool ASCIIReadVector3(std::deque<std::string>& linelist, uint32* v, bool repeat = false);
bool ASCIIReadVector4(std::deque<std::string>& linelist, uint32* v, bool repeat = false);
bool ASCIIReadVector2(std::deque<std::string>& linelist, real32* v, bool repeat = false);
bool ASCIIReadVector3(std::deque<std::string>& linelist, real32* v, bool repeat = false);
bool ASCIIReadVector4(std::deque<std::string>& linelist, real32* v, bool repeat = false);
bool ASCIIReadVector8(std::deque<std::string>& linelist, real32* v, bool repeat = false);
bool ASCIIReadMatrix3(std::deque<std::string>& linelist, real32* v, bool repeat = false);
bool ASCIIReadMatrix4(std::deque<std::string>& linelist, real32* v, bool repeat = false);

// arbitrary size arrays
bool ASCIIReadArray(std::deque<std::string>& linelist, std::vector<uint32>& data); 
bool ASCIIReadArray(std::deque<std::string>& linelist, std::vector<real32>& data); 

#endif
