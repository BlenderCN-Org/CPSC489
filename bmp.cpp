#include "stdafx.h"
#include "errors.h"
#include "texture.h"
#include "bmp.h"

ErrorCode LoadBMP(LPCWSTR filename, TextureData* data)
{
 // validate
 if(!filename) return DebugErrorCode(EC_INVALID_ARG, __LINE__, __FILE__);
 if(!data) return DebugErrorCode(EC_INVALID_ARG, __LINE__, __FILE__);

 // open file
 using namespace std;
 ifstream ifile(filename, ios::binary);
 if(!ifile) return DebugErrorCode(EC_FILE_OPEN, __LINE__, __FILE__);

 // read bitmap file header
 BITMAPFILEHEADER bfh;
 ifile.read((char*)&bfh, sizeof(bfh));
 if(ifile.fail()) return DebugErrorCode(EC_FILE_READ, __LINE__, __FILE__);

 // read bitmap info header
 BITMAPINFOHEADER bih;
 ifile.read((char*)&bih, sizeof(bih));
 if(ifile.fail()) return DebugErrorCode(EC_FILE_READ, __LINE__, __FILE__);

 return EC_SUCCESS;
}

ErrorCode ConvertBMP(LPCWSTR filename, LPCWSTR outfile)
{
 return EC_SUCCESS;
}