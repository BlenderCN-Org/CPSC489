#include "stdafx.h"
#include "errors.h"
#include "app.h"
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
    // image properties
    if(bih.biClrUsed) return DebugErrorCode(EC_BMP_OPTIMIZED, __LINE__, __FILE__);
    DWORD src_pitch = ((24*dx + 0x1F) & (~0x1F))/8;
    DWORD src_total = src_pitch*dy;

    // read image data
    unique_ptr<BYTE[]> buffer(new BYTE[src_total]);
    ifile.read((char*)buffer.get(), src_total);
    if(ifile.fail()) return DebugErrorCode(EC_FILE_READ, __LINE__, __FILE__);

    // convert to 32-bit since Direct3D doesn't support DXGI_FORMAT_B8G8R8_UNORM
    // also, BMP data is upside-down and must be reversed
    DWORD dst_pitch = 4*dx;
    DWORD dst_total = dst_pitch*dy;
    unique_ptr<BYTE[]> dst(new BYTE[dst_total]);
    for(size_t r = 0; r < dy; r++) {
        for(size_t c = 0; c < dx; c++) {
            size_t src_index = r*src_pitch + 3*c;
            size_t dst_index = (dy - 1 - r)*dst_pitch + 4*c;
            dst[dst_index + 0] = buffer[src_index + 0];
            dst[dst_index + 1] = buffer[src_index + 1];
            dst[dst_index + 2] = buffer[src_index + 2];
            dst[dst_index + 3] = 0;
           }
       }

    // transfer information
    data->dx = dx;
    data->dy = dy;
    data->pitch = dst_pitch;
    data->format = DXGI_FORMAT_B8G8R8X8_UNORM;
    data->size = dst_total;
    data->data = std::move(dst);
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