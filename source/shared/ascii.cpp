#include "stdafx.h"
#include "ascii.h"

#pragma region ASCII_FILE_UTILITIES

bool ASCIIParseFile(const wchar_t* filename, std::deque<std::string>& linelist)
{
 // open file
 std::ifstream ifile(filename);
 if(!ifile) return false;

 // loop forever
 for(;;)
    {
     // read line
     char line[1024];
     ifile.getline(&line[0], 1024);
     if(ifile.eof()) break;
     if(ifile.fail()) return false;

     // remove comments
     if(strlen(line)) {
        std::deque<std::string> split;
        boost::split(split, line, boost::is_any_of("#"));
        if(split.size()) {
           boost::trim_all(split[0]);
           if(split[0].length()) linelist.push_back(split[0]);
          }
       }
    }

 return true;
}

bool ASCIIReadString(std::deque<std::string>& linelist, char* str)
{
 if(linelist.empty()) return false;
 strcpy_s(str, 256, linelist.front().c_str());
 linelist.pop_front();
 return true;
}

bool ASCIIReadSint32(std::deque<std::string>& linelist, sint32* x)
{
 if(linelist.empty()) return false;
 *x = strtol(linelist.front().c_str(), nullptr, 10);
 linelist.pop_front();
 return true;
}

bool ASCIIReadUint32(std::deque<std::string>& linelist, uint32* x)
{
 if(linelist.empty()) return false;
 *x = strtoul(linelist.front().c_str(), nullptr, 10);
 linelist.pop_front();
 return true;
}

bool ASCIIReadVector4(std::deque<std::string>& linelist, uint08* v, bool repeat)
{
 if(linelist.empty()) return false;
 std::deque<std::string> parameters;
 boost::split(parameters, linelist.front(), boost::is_any_of(" "));
 uint32 temp[4];
 if(parameters.size() == 1) {
    temp[0] = strtoul(parameters[0].c_str(), nullptr, 10);
    if(repeat) {
       temp[1] = temp[0];
       temp[2] = temp[0];
       temp[3] = temp[0];
      }
    else {
       temp[1] = 0;
       temp[2] = 0;
       temp[3] = 0;
      }
   }
 else if(parameters.size() == 2) {
    temp[0] = strtoul(parameters[0].c_str(), nullptr, 10);
    temp[1] = strtoul(parameters[1].c_str(), nullptr, 10);
    if(repeat) {
       temp[2] = temp[1];
       temp[3] = temp[1];
      }
    else {
       temp[2] = 0;
       temp[3] = 0;
      }
   }
 else if(parameters.size() == 3) {
    temp[0] = strtoul(parameters[0].c_str(), nullptr, 10);
    temp[1] = strtoul(parameters[1].c_str(), nullptr, 10);
    temp[2] = strtoul(parameters[2].c_str(), nullptr, 10);
    if(repeat) temp[3] = temp[2];
    else temp[3] = 0;
   }
 else if(parameters.size() == 4) {
    temp[0] = strtoul(parameters[0].c_str(), nullptr, 10);
    temp[1] = strtoul(parameters[1].c_str(), nullptr, 10);
    temp[2] = strtoul(parameters[2].c_str(), nullptr, 10);
    temp[3] = strtoul(parameters[3].c_str(), nullptr, 10);
   }
 else return false;
 v[0] = (temp[0] > 255 ? 255 : temp[0]);
 v[1] = (temp[1] > 255 ? 255 : temp[1]);
 v[2] = (temp[2] > 255 ? 255 : temp[2]);
 v[3] = (temp[3] > 255 ? 255 : temp[3]);
 linelist.pop_front();
 return true;
}

bool ASCIIReadVector4(std::deque<std::string>& linelist, uint16* v, bool repeat)
{
 if(linelist.empty()) return false;
 std::deque<std::string> parameters;
 boost::split(parameters, linelist.front(), boost::is_any_of(" "));
 uint32 temp[4];
 if(parameters.size() == 1) {
    temp[0] = strtoul(parameters[0].c_str(), nullptr, 10);
    if(repeat) {
       temp[1] = temp[0];
       temp[2] = temp[0];
       temp[3] = temp[0];
      }
    else {
       temp[1] = 0;
       temp[2] = 0;
       temp[3] = 0;
      }
   }
 else if(parameters.size() == 2) {
    temp[0] = strtoul(parameters[0].c_str(), nullptr, 10);
    temp[1] = strtoul(parameters[1].c_str(), nullptr, 10);
    if(repeat) {
       temp[2] = temp[1];
       temp[3] = temp[1];
      }
    else {
       temp[2] = 0;
       temp[3] = 0;
      }
   }
 else if(parameters.size() == 3) {
    temp[0] = strtoul(parameters[0].c_str(), nullptr, 10);
    temp[1] = strtoul(parameters[1].c_str(), nullptr, 10);
    temp[2] = strtoul(parameters[2].c_str(), nullptr, 10);
    if(repeat) temp[3] = temp[2];
    else temp[3] = 0;
   }
 else if(parameters.size() == 4) {
    temp[0] = strtoul(parameters[0].c_str(), nullptr, 10);
    temp[1] = strtoul(parameters[1].c_str(), nullptr, 10);
    temp[2] = strtoul(parameters[2].c_str(), nullptr, 10);
    temp[3] = strtoul(parameters[3].c_str(), nullptr, 10);
   }
 else return false;
 v[0] = (temp[0] > 0xFFFF ? 0xFFFF : temp[0]);
 v[1] = (temp[1] > 0xFFFF ? 0xFFFF : temp[1]);
 v[2] = (temp[2] > 0xFFFF ? 0xFFFF : temp[2]);
 v[3] = (temp[3] > 0xFFFF ? 0xFFFF : temp[3]);
 linelist.pop_front();
 return true;
}

bool ASCIIReadVector8(std::deque<std::string>& linelist, uint16* v, bool repeat)
{
 if(linelist.empty()) return false;

 // split parameters
 const int n = 8;
 std::deque<std::string> parameters;
 boost::split(parameters, linelist.front(), boost::is_any_of(" "));
 if(parameters.size() < 1 || parameters.size() > n) return false;

 // convert parameters
 uint32 temp[n] = { 0, 0, 0, 0, 0, 0, 0, 0 };
 for(int i = 0; i < parameters.size(); i++) temp[i] = strtoul(parameters[i].c_str(), nullptr, 10);
 if(repeat) for(size_t i = parameters.size(); i < n; i++) temp[i] = temp[parameters.size() - 1];

 // assign data and remove line from list
 for(int i = 0; i < n; i++) {
     if(temp[i] > 0xFFFFul) return false;
     v[i] = temp[i];
    }
 linelist.pop_front();

 return true;
}

bool ASCIIReadVector2(std::deque<std::string>& linelist, uint32* v, bool repeat)
{
 if(linelist.empty()) return false;
 std::deque<std::string> parameters;
 boost::split(parameters, linelist.front(), boost::is_any_of(" "));
 if(parameters.size() == 1) {
    unsigned long a = strtoul(parameters[0].c_str(), nullptr, 10);
    v[0] = static_cast<uint32>(a);
    if(repeat) v[1] = v[0];
    else v[1] = 0;
   }
 else if(parameters.size() == 2) {
    unsigned long a = strtoul(parameters[0].c_str(), nullptr, 10);
    unsigned long b = strtoul(parameters[1].c_str(), nullptr, 10);
    v[0] = static_cast<uint32>(a);
    v[1] = static_cast<uint32>(b);
   }
 else return false;
 linelist.pop_front();
 return true;
}

bool ASCIIReadVector3(std::deque<std::string>& linelist, uint32* v, bool repeat)
{
 if(linelist.empty()) return false;
 std::deque<std::string> parameters;
 boost::split(parameters, linelist.front(), boost::is_any_of(" "));
 if(parameters.size() == 1) {
    unsigned long a = strtoul(parameters[0].c_str(), nullptr, 10);
    v[0] = static_cast<uint32>(a);
    if(repeat) {
       v[1] = v[0];
       v[2] = v[0];
      }
    else {
       v[1] = 0;
       v[2] = 0;
      }
   }
 else if(parameters.size() == 2) {
    unsigned long a = strtoul(parameters[0].c_str(), nullptr, 10);
    unsigned long b = strtoul(parameters[1].c_str(), nullptr, 10);
    v[0] = static_cast<uint32>(a);
    v[1] = static_cast<uint32>(b);
    if(repeat) v[2] = v[1];
    else v[2] = 0;
   }
 else if(parameters.size() == 3) {
    unsigned long a = strtoul(parameters[0].c_str(), nullptr, 10);
    unsigned long b = strtoul(parameters[1].c_str(), nullptr, 10);
    unsigned long c = strtoul(parameters[2].c_str(), nullptr, 10);
    v[0] = static_cast<uint32>(a);
    v[1] = static_cast<uint32>(b);
    v[2] = static_cast<uint32>(c);
   }
 else return false;
 linelist.pop_front();
 return true;
}

bool ASCIIReadVector4(std::deque<std::string>& linelist, uint32* v, bool repeat)
{
 if(linelist.empty()) return false;
 std::deque<std::string> parameters;
 boost::split(parameters, linelist.front(), boost::is_any_of(" "));
 if(parameters.size() == 1) {
    v[0] = strtoul(parameters[0].c_str(), nullptr, 10);
    if(repeat) {
       v[1] = v[0];
       v[2] = v[0];
       v[3] = v[0];
      }
    else {
       v[1] = 0;
       v[2] = 0;
       v[3] = 0;
      }
   }
 else if(parameters.size() == 2) {
    v[0] = strtoul(parameters[0].c_str(), nullptr, 10);
    v[1] = strtoul(parameters[1].c_str(), nullptr, 10);
    if(repeat) {
       v[2] = v[1];
       v[3] = v[1];
      }
    else {
       v[2] = 0;
       v[3] = 0;
      }
   }
 else if(parameters.size() == 3) {
    v[0] = strtoul(parameters[0].c_str(), nullptr, 10);
    v[1] = strtoul(parameters[1].c_str(), nullptr, 10);
    v[2] = strtoul(parameters[2].c_str(), nullptr, 10);
    if(repeat) v[3] = v[2];
    else v[3] = 0;
   }
 else if(parameters.size() == 4) {
    v[0] = strtoul(parameters[0].c_str(), nullptr, 10);
    v[1] = strtoul(parameters[1].c_str(), nullptr, 10);
    v[2] = strtoul(parameters[2].c_str(), nullptr, 10);
    v[3] = strtoul(parameters[3].c_str(), nullptr, 10);
   }
 else return false;
 linelist.pop_front();
 return true;
}

bool ASCIIReadVector2(std::deque<std::string>& linelist, real32* v, bool repeat)
{
 // read past EOF
 if(linelist.empty()) return false;

 // split parameters
 const int n = 2;
 std::deque<std::string> parameters;
 boost::split(parameters, linelist.front(), boost::is_any_of(" "));
 if(parameters.size() < 1 || parameters.size() > n) return false;

 // convert parameters
 real32 temp[n] = { 0.0f, 0.0f };
 for(int i = 0; i < parameters.size(); i++) temp[i] = (float)strtod(parameters[i].c_str(), nullptr);
 if(repeat) for(size_t i = parameters.size(); i < n; i++) temp[i] = temp[parameters.size() - 1];

 // assign data and remove line from list
 for(int i = 0; i < n; i++) v[i] = temp[i];
 linelist.pop_front();

 return true;
}

bool ASCIIReadVector3(std::deque<std::string>& linelist, real32* v, bool repeat)
{
 // read past EOF
 if(linelist.empty()) return false;

 // split parameters
 const int n = 3;
 std::deque<std::string> parameters;
 boost::split(parameters, linelist.front(), boost::is_any_of(" "));
 if(parameters.size() < 1 || parameters.size() > n) return false;

 // convert parameters
 real32 temp[n] = { 0.0f, 0.0f, 0.0f };
 for(int i = 0; i < parameters.size(); i++) temp[i] = (float)strtod(parameters[i].c_str(), nullptr);
 if(repeat) for(size_t i = parameters.size(); i < n; i++) temp[i] = temp[parameters.size() - 1];

 // assign data and remove line from list
 for(int i = 0; i < n; i++) v[i] = temp[i];
 linelist.pop_front();

 return true;
}

bool ASCIIReadVector4(std::deque<std::string>& linelist, real32* v, bool repeat)
{
 // read past EOF
 if(linelist.empty()) return false;

 // split parameters
 const int n = 4;
 std::deque<std::string> parameters;
 boost::split(parameters, linelist.front(), boost::is_any_of(" "));
 if(parameters.size() < 1 || parameters.size() > n) return false;

 // convert parameters
 real32 temp[n] = { 0.0f, 0.0f, 0.0f, 0.0f };
 for(int i = 0; i < parameters.size(); i++) temp[i] = (float)strtod(parameters[i].c_str(), nullptr);
 if(repeat) for(size_t i = parameters.size(); i < n; i++) temp[i] = temp[parameters.size() - 1];

 // assign data and remove line from list
 for(int i = 0; i < n; i++) v[i] = temp[i];
 linelist.pop_front();

 return true;
}

bool ASCIIReadVector8(std::deque<std::string>& linelist, real32* v, bool repeat)
{
 // read past EOF
 if(linelist.empty()) return false;

 // split parameters
 const int n = 8;
 std::deque<std::string> parameters;
 boost::split(parameters, linelist.front(), boost::is_any_of(" "));
 if(parameters.size() < 1 || parameters.size() > n) return false;

 // convert parameters
 real32 temp[n] = { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f };
 for(int i = 0; i < parameters.size(); i++) temp[i] = (float)strtod(parameters[i].c_str(), nullptr);
 if(repeat) for(size_t i = parameters.size(); i < n; i++) temp[i] = temp[parameters.size() - 1];

 // assign data and remove line from list
 for(int i = 0; i < n; i++) v[i] = temp[i];
 linelist.pop_front();

 return true;
}

bool ASCIIReadMatrix3(std::deque<std::string>& linelist, real32* v, bool repeat)
{
 // read past EOF
 if(linelist.empty()) return false;

 // split parameters
 const int n = 9;
 std::deque<std::string> parameters;
 boost::split(parameters, linelist.front(), boost::is_any_of(" "));
 if(parameters.size() < 1 || parameters.size() > n) return false;
 if(!repeat && (parameters.size() != 9)) return false;

 // convert parameters
 real32 temp[n] = {
  0.0f, 0.0f, 0.0f,
  0.0f, 0.0f, 0.0f,
  0.0f, 0.0f, 0.0f
 };
 for(int i = 0; i < parameters.size(); i++) temp[i] = (float)strtod(parameters[i].c_str(), nullptr);
 if(repeat) for(size_t i = parameters.size(); i < n; i++) temp[i] = temp[parameters.size() - 1];

 // assign data and remove line from list
 for(int i = 0; i < n; i++) v[i] = temp[i];
 linelist.pop_front();

 return true;
}

bool ASCIIReadMatrix4(std::deque<std::string>& linelist, real32* v, bool repeat)
{
 // read past EOF
 if(linelist.empty()) return false;

 // split parameters
 const int n = 16;
 std::deque<std::string> parameters;
 boost::split(parameters, linelist.front(), boost::is_any_of(" "));
 if(parameters.size() < 1 || parameters.size() > n) return false;

 // convert parameters
 real32 temp[n] = {
  0.0f, 0.0f, 0.0f, 0.0f,
  0.0f, 0.0f, 0.0f, 0.0f,
  0.0f, 0.0f, 0.0f, 0.0f,
  0.0f, 0.0f, 0.0f, 0.0f
 };
 for(int i = 0; i < parameters.size(); i++) temp[i] = (float)strtod(parameters[i].c_str(), nullptr);
 if(repeat) for(size_t i = parameters.size(); i < n; i++) temp[i] = temp[parameters.size() - 1];

 // assign data and remove line from list
 for(int i = 0; i < n; i++) v[i] = temp[i];
 linelist.pop_front();

 return true;
}

#pragma endregion ASCII_FILE_UTILITIES