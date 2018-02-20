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

 // validate
 if(bfh.bfType != 0x4D42) return DebugErrorCode(EC_BMP_INVALID, __LINE__, __FILE__);
 if(bfh.bfReserved1 != 0x00) return DebugErrorCode(EC_BMP_INVALID, __LINE__, __FILE__);
 if(bfh.bfReserved2 != 0x00) return DebugErrorCode(EC_BMP_INVALID, __LINE__, __FILE__);

 // read bitmap info header
 BITMAPINFOHEADER bih;
 ifile.read((char*)&bih, sizeof(bih));
 if(ifile.fail()) return DebugErrorCode(EC_FILE_READ, __LINE__, __FILE__);

 // validate
 if(bih.biSize != 0x28) return DebugErrorCode(EC_BMP_INVALID, __LINE__, __FILE__);
 if(bih.biWidth < 1) return DebugErrorCode(EC_BMP_INVALID, __LINE__, __FILE__);
 if(bih.biHeight < 1) return DebugErrorCode(EC_BMP_INVALID, __LINE__, __FILE__);
 if(bih.biPlanes != 1) return DebugErrorCode(EC_BMP_INVALID, __LINE__, __FILE__);
 if(bih.biBitCount == 0) return DebugErrorCode(EC_BMP_COMPRESSED, __LINE__, __FILE__);
 if(bih.biCompression != BI_RGB) return DebugErrorCode(EC_BMP_COMPRESSED, __LINE__, __FILE__);

 // image dimensions
 DWORD dx = bih.biWidth;
 DWORD dy = bih.biHeight;

 // depending on the bitcount
 if(bih.biBitCount == 1)
   {
   }
 else if(bih.biBitCount == 4)
   {
   }
 else if(bih.biBitCount == 8)
   {
   }
 else if(bih.biBitCount == 16)
   {
   }
 else if(bih.biBitCount == 24)
   {
    // read image data
    if(bih.biClrUsed) return DebugErrorCode(EC_BMP_OPTIMIZED, __LINE__, __FILE__);
    DWORD pitch = ((0x3*dx + 0x3) & (~0x3));
    DWORD size = pitch*dy;
    unique_ptr<BYTE[]> buffer(new BYTE[size]);
    ifile.read((char*)buffer.get(), size);
    if(ifile.fail()) return DebugErrorCode(EC_FILE_READ, __LINE__, __FILE__);
    // transfer information
    data->dx = dx;
    data->dy = dy;
    data->pitch = pitch;
    data->format = DXGI_FORMAT_B8G8R8X8_UNORM;
    data->size = size;
    data->data = std::move(buffer);
   }
 else if(bih.biBitCount == 32)
   {
    // read image data
    if(bih.biClrUsed) return DebugErrorCode(EC_BMP_OPTIMIZED, __LINE__, __FILE__);
    DWORD pitch = 4*dx;
    DWORD size = pitch*dy;
    unique_ptr<BYTE[]> buffer(new BYTE[size]);
    ifile.read((char*)buffer.get(), size);
    if(ifile.fail()) return DebugErrorCode(EC_FILE_READ, __LINE__, __FILE__);
    // transfer information
    data->dx = dx;
    data->dy = dy;
    data->pitch = pitch;
    data->format = DXGI_FORMAT_B8G8R8A8_UNORM;
    data->size = size;
    data->data = std::move(buffer);
   }
 else
    return DebugErrorCode(EC_BMP_INVALID, __LINE__, __FILE__);

 return EC_SUCCESS;
}

ErrorCode ConvertBMP(LPCWSTR filename, LPCWSTR outfile)
{
 return EC_SUCCESS;
}