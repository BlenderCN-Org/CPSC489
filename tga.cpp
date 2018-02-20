#include "stdafx.h"
#include "errors.h"
#include "texture.h"
#include "tga.h"

struct TGAHEADER {
 uint08 imageID;
 uint08 color_map_type;
 uint08 image_type;
 uint16 color_map_origin;
 uint16 color_map_length;
 uint08 color_map_bits;
 uint16 xorigin;
 uint16 yorigin;
 uint16 dx;
 uint16 dy;
 uint08 pixel_depth;
 uint08 image_descriptor;
};

struct TGACOLOR {
 unsigned char r;
 unsigned char g;
 unsigned char b;
 unsigned char a;
};

ErrorCode ReadTGAGrayscale(std::ifstream& ifile, const TGAHEADER& header, TGACOLOR& color);
ErrorCode ReadTGAColor(std::ifstream& ifile, const TGAHEADER& header, TGACOLOR& color);
ErrorCode ReadTGAColorPalette(std::ifstream& ifile, const TGAHEADER& header, std::unique_ptr<TGACOLOR[]>& palette);
ErrorCode ReadTGA_01(std::ifstream& ifile, const TGAHEADER& header, TextureData* xid);
ErrorCode ReadTGA_02(std::ifstream& ifile, const TGAHEADER& header, TextureData* xid);
ErrorCode ReadTGA_03(std::ifstream& ifile, const TGAHEADER& header, TextureData* xid);
ErrorCode ReadTGA_09(std::ifstream& ifile, const TGAHEADER& header, TextureData* xid);
ErrorCode ReadTGA_10(std::ifstream& ifile, const TGAHEADER& header, TextureData* xid);
ErrorCode ReadTGA_11(std::ifstream& ifile, const TGAHEADER& header, TextureData* xid);

ErrorCode LoadTGA(LPCWSTR filename, TextureData* xid)
{
 // open file
 using namespace std;
 ifstream ifile(filename, ios::binary);
 if(!ifile) return DebugErrorCode(EC_FILE_OPEN, __LINE__, __FILE__);

 // read header
 TGAHEADER header;
 ifile.read((char*)&header.imageID, sizeof(header.imageID));
 ifile.read((char*)&header.color_map_type, sizeof(header.color_map_type));
 ifile.read((char*)&header.image_type, sizeof(header.image_type));
 ifile.read((char*)&header.color_map_origin, sizeof(header.color_map_origin));
 ifile.read((char*)&header.color_map_length, sizeof(header.color_map_length));
 ifile.read((char*)&header.color_map_bits, sizeof(header.color_map_bits));
 ifile.read((char*)&header.xorigin, sizeof(header.xorigin));
 ifile.read((char*)&header.yorigin, sizeof(header.yorigin));
 ifile.read((char*)&header.dx, sizeof(header.dx));
 ifile.read((char*)&header.dy, sizeof(header.dy));
 ifile.read((char*)&header.pixel_depth, sizeof(header.pixel_depth));
 ifile.read((char*)&header.image_descriptor, sizeof(header.image_descriptor));
 if(ifile.fail()) return DebugErrorCode(EC_FILE_READ, __LINE__, __FILE__);

 // validate
 switch(header.image_type) {
    case(0x00) : return DebugErrorCode(EC_TGA_IMAGE_TYPE_UNSUPPORTED, __LINE__, __FILE__);
    case(0x01) : break;
    case(0x02) : break;
    case(0x03) : break;
    case(0x09) : break;
    case(0x0A) : break;
    case(0x20) :  return DebugErrorCode(EC_TGA_IMAGE_TYPE_UNSUPPORTED, __LINE__, __FILE__);
    case(0x21) :  return DebugErrorCode(EC_TGA_IMAGE_TYPE_UNSUPPORTED, __LINE__, __FILE__);
    default :  return DebugErrorCode(EC_TGA_IMAGE_TYPE_UNSUPPORTED, __LINE__, __FILE__);
   }
 if(!header.dx) return DebugErrorCode(EC_TGA_IMAGE_TYPE_UNSUPPORTED, __LINE__, __FILE__);
 if(!header.dy) return DebugErrorCode(EC_TGA_IMAGE_TYPE_UNSUPPORTED, __LINE__, __FILE__);

 // read image identification field
 if(header.imageID) {
    ifile.seekg(header.imageID, ios::cur);
    if(ifile.fail()) return DebugErrorCode(EC_FILE_SEEK, __LINE__, __FILE__);
   }

 // process palette or color image
 switch(header.image_type) {
    case(0x01) : return ReadTGA_01(ifile, header, xid);
    case(0x02) : return ReadTGA_02(ifile, header, xid);
    case(0x03) : return ReadTGA_03(ifile, header, xid);
    case(0x09) : return ReadTGA_09(ifile, header, xid);
    case(0x0A) : return ReadTGA_10(ifile, header, xid);
    case(0x0B) : return ReadTGA_11(ifile, header, xid);
    default :  return DebugErrorCode(EC_TGA_IMAGE_TYPE_UNSUPPORTED, __LINE__, __FILE__);
   }

 return EC_SUCCESS;
}

ErrorCode ReadTGAColor(std::ifstream& ifile, const TGAHEADER& header, TGACOLOR& color)
{
 if(header.pixel_depth == 16) {
    uint16 temp;
    ifile.read((char*)&temp, sizeof(temp));
    if(ifile.fail()) return DebugErrorCode(EC_FILE_READ, __LINE__, __FILE__);
    color.r = ((temp & 0x7C00) >> 7)/0xF8;
    color.g = ((temp & 0x03E0) >> 2)/0xF8;
    color.b = ((temp & 0x001F) << 3)/0xF8;
    color.a = ((temp & 0x8000) ? 255 : 0);
   }
 else if(header.pixel_depth == 24) {
    unsigned char buffer[3];
    ifile.read((char*)&buffer[0], 3);
    if(ifile.fail()) return DebugErrorCode(EC_FILE_READ, __LINE__, __FILE__);
    color.b = buffer[0];
    color.g = buffer[1];
    color.r = buffer[2];
    color.a = 255;
   }
 else if(header.pixel_depth == 32) {
    unsigned char buffer[4];
    ifile.read((char*)&buffer[0], 4);
    if(ifile.fail()) return DebugErrorCode(EC_FILE_READ, __LINE__, __FILE__);
    color.b = buffer[0];
    color.g = buffer[1];
    color.r = buffer[2];
    color.a = buffer[3];
   }
 else return DebugErrorCode(EC_TGA_PIXEL_DEPTH_UNSUPPORTED, __LINE__, __FILE__);
 return EC_SUCCESS;
}

ErrorCode ReadTGAColorPalette(std::ifstream& ifile, const TGAHEADER& header, std::unique_ptr<TGACOLOR[]>& palette)
{
 if(!header.color_map_length) return DebugErrorCode(EC_TGA_MISSING_COLOR_MAP, __LINE__, __FILE__);
 palette.reset(new TGACOLOR[header.color_map_length]);
 if(header.color_map_bits == 16) {
    for(uint16 i = 0; i < header.color_map_length; i++) {
        uint16 temp;
        ifile.read((char*)&temp, sizeof(temp));
        if(ifile.fail()) return DebugErrorCode(EC_FILE_READ, __LINE__, __FILE__);
        palette[i].r = ((temp & 0x7C00) >> 7)/0xF8;
        palette[i].g = ((temp & 0x03E0) >> 2)/0xF8;
        palette[i].b = ((temp & 0x001F) << 3)/0xF8;
        palette[i].a = ((temp & 0x8000) ? 255 : 0);
       }
   }
 else if(header.color_map_bits == 24) {
    for(uint16 i = 0; i < header.color_map_length; i++) {
        unsigned char buffer[3];
        ifile.read((char*)&buffer[0], 3);
        if(ifile.fail()) return DebugErrorCode(EC_FILE_READ, __LINE__, __FILE__);
        palette[i].b = buffer[0];
        palette[i].g = buffer[1];
        palette[i].r = buffer[2];
        palette[i].a = 255;
       }
   }
 else if(header.color_map_bits == 32) {
    for(uint16 i = 0; i < header.color_map_length; i++) {
        unsigned char buffer[4];
        ifile.read((char*)&buffer[0], 4);
        if(ifile.fail()) return DebugErrorCode(EC_FILE_READ, __LINE__, __FILE__);
        palette[i].b = buffer[0];
        palette[i].g = buffer[1];
        palette[i].r = buffer[2];
        palette[i].a = buffer[3];
       }
   }
 else return DebugErrorCode(EC_TGA_PIXEL_DEPTH_UNSUPPORTED, __LINE__, __FILE__);
 return EC_SUCCESS;
}

ErrorCode ReadTGA_01(std::ifstream& ifile, const TGAHEADER& header, TextureData* xid)
{
 // expecting a color map
 if(header.color_map_type != 1 || header.color_map_length == 0)
    return DebugErrorCode(EC_TGA_MISSING_COLOR_MAP, __LINE__, __FILE__);

 // read palette
 std::unique_ptr<TGACOLOR[]> palette;
 auto result = ReadTGAColorPalette(ifile, header, palette);
 if(result != EC_SUCCESS) return result;

 // always save as RGBA
 uint32 n_pixels = header.dx*header.dy;
 uint32 size = 4*n_pixels;
 std::unique_ptr<BYTE[]> data(new BYTE[size]);

 // read pixels
 if(header.pixel_depth == 8) {
    std::unique_ptr<uint08[]> index_data(new uint08[n_pixels]);
    ifile.read((char*)index_data.get(), n_pixels*sizeof(uint08));
    if(ifile.fail()) return DebugErrorCode(EC_FILE_READ, __LINE__, __FILE__);
    size_t index = 0;
    for(uint32 i = 0; i < n_pixels; i++) {
        data[index++] = palette[index_data[i] - header.color_map_origin].r;
        data[index++] = palette[index_data[i] - header.color_map_origin].g;
        data[index++] = palette[index_data[i] - header.color_map_origin].b;
        data[index++] = palette[index_data[i] - header.color_map_origin].a;
       }
   }
 else if(header.pixel_depth == 16) {
    std::unique_ptr<uint16[]> index_data(new uint16[n_pixels]);
    ifile.read((char*)index_data.get(), n_pixels*sizeof(uint16));
    if(ifile.fail()) return DebugErrorCode(EC_FILE_READ, __LINE__, __FILE__);
    size_t index = 0;
    for(uint32 i = 0; i < n_pixels; i++) {
        data[index++] = palette[index_data[i] - header.color_map_origin].r;
        data[index++] = palette[index_data[i] - header.color_map_origin].g;
        data[index++] = palette[index_data[i] - header.color_map_origin].b;
        data[index++] = palette[index_data[i] - header.color_map_origin].a;
       }
   }
 else if(header.pixel_depth == 32) {
    std::unique_ptr<uint32[]> index_data(new uint32[n_pixels]);
    ifile.read((char*)index_data.get(), n_pixels*sizeof(uint32));
    if(ifile.fail()) return DebugErrorCode(EC_FILE_READ, __LINE__, __FILE__);
    size_t index = 0;
    for(uint32 i = 0; i < n_pixels; i++) {
        data[index++] = palette[index_data[i] - header.color_map_origin].r;
        data[index++] = palette[index_data[i] - header.color_map_origin].g;
        data[index++] = palette[index_data[i] - header.color_map_origin].b;
        data[index++] = palette[index_data[i] - header.color_map_origin].a;
       }
   }
 else
    return DebugErrorCode(EC_TGA_PIXEL_DEPTH, __LINE__, __FILE__);

 // fill out data
 xid->dx = header.dx;
 xid->dy = header.dy;
 xid->pitch = 4*header.dx;
 xid->size = size;
 xid->data = std::move(data);
 xid->format = DXGI_FORMAT_R8G8B8A8_UNORM;

 // success
 return EC_SUCCESS;
}

ErrorCode ReadTGA_02(std::ifstream& ifile, const TGAHEADER& header, TextureData* xid)
{
 // even for uncompressed color images, you can have a color map
 if(header.color_map_type) {
    // compute amount to skip
    uint32 size = header.color_map_length;
    if(header.color_map_bits == 16) size *= 2;
    else if(header.color_map_bits == 24) size *= 3;
    else if(header.color_map_bits == 32) size *= 4;
    else return DebugErrorCode(EC_TGA_PIXEL_DEPTH_UNSUPPORTED, __LINE__, __FILE__);
    // skip data
    ifile.seekg(size, std::ios::cur);
    if(ifile.fail()) return DebugErrorCode(EC_FILE_SEEK, __LINE__, __FILE__);
   }

 // color data properties
 uint32 color_data_elem = header.dx*header.dy;
 uint32 color_data_size = color_data_elem;
 if(header.pixel_depth == 16) color_data_size *= 2;
 else if(header.pixel_depth == 24) color_data_size *= 3;
 else if(header.pixel_depth == 32) color_data_size *= 4;
 else return DebugErrorCode(EC_TGA_PIXEL_DEPTH_UNSUPPORTED, __LINE__, __FILE__);

 // read color data
 std::unique_ptr<BYTE[]> color_data(new BYTE[color_data_size]);
 ifile.read((char*)color_data.get(), color_data_size);
 if(ifile.fail()) return DebugErrorCode(EC_FILE_READ, __LINE__, __FILE__);

 // always save as RGBA
 uint32 n_pixels = header.dx*header.dy;
 uint32 size = 4*n_pixels;
 std::unique_ptr<BYTE[]> data(new BYTE[size]);

 // set pixels
 if(header.pixel_depth == 16) {
    const uint16* ptr = reinterpret_cast<const uint16*>(color_data.get());
    size_t index = 0;
    for(uint32 i = 0; i < n_pixels; i++) {
        uint16 value = ptr[i];
        data[index++] = ((value & 0x7C00) >> 7)/0xF8;
        data[index++] = ((value & 0x03E0) >> 2)/0xF8;
        data[index++] = ((value & 0x001F) << 3)/0xF8;
        data[index++] = ((value & 0x8000) ? 255 : 0);
       }
   }
 else if(header.pixel_depth == 24) {
    size_t index = 0;
    for(uint32 i = 0; i < n_pixels; i++) {
        data[index++] = color_data[3*i + 2];
        data[index++] = color_data[3*i + 1];
        data[index++] = color_data[3*i + 0];
        data[index++] = 255;
       }
   }
 else if(header.pixel_depth == 32) {
    size_t index = 0;
    for(uint32 i = 0; i < n_pixels; i++) {
        data[index++] = color_data[4*i + 2];
        data[index++] = color_data[4*i + 1];
        data[index++] = color_data[4*i + 0];
        data[index++] = color_data[4*i + 3];
       }
   }
 else
    return DebugErrorCode(EC_TGA_PIXEL_DEPTH, __LINE__, __FILE__);

 // fill out data
 xid->dx = header.dx;
 xid->dy = header.dy;
 xid->pitch = 4*header.dx;
 xid->size = size;
 xid->data = std::move(data);
 xid->format = DXGI_FORMAT_R8G8B8A8_UNORM;

 // success
 return EC_SUCCESS;
}

ErrorCode ReadTGA_03(std::ifstream& ifile, const TGAHEADER& header, TextureData* xid)
{
 // even for uncompressed B&W images, you can have a color map
 if(header.color_map_type) {
    // compute amount to skip
    uint32 size = header.color_map_length;
    if(header.color_map_bits == 16) size *= 2;
    else if(header.color_map_bits == 24) size *= 3;
    else if(header.color_map_bits == 32) size *= 4;
    else return DebugErrorCode(EC_TGA_PIXEL_DEPTH_UNSUPPORTED, __LINE__, __FILE__);
    // skip data
    ifile.seekg(size, std::ios::cur);
    if(ifile.fail()) return DebugErrorCode(EC_FILE_SEEK, __LINE__, __FILE__);
   }

 // color data properties
 uint32 color_data_elem = header.dx*header.dy;
 uint32 color_data_size = color_data_elem;
 if(header.pixel_depth != 8) return DebugErrorCode(EC_TGA_PIXEL_DEPTH_UNSUPPORTED, __LINE__, __FILE__);

 // read color data
 std::unique_ptr<BYTE[]> color_data(new BYTE[color_data_size]);
 ifile.read((char*)color_data.get(), color_data_size);
 if(ifile.fail()) return DebugErrorCode(EC_FILE_READ, __LINE__, __FILE__);

 // always save as RGBA
 uint32 n_pixels = header.dx*header.dy;
 uint32 size = 4*n_pixels;
 std::unique_ptr<BYTE[]> data(new BYTE[size]);

 // set pixels
 size_t index = 0;
 for(uint32 i = 0; i < n_pixels; i++) {
     data[index++] = color_data[i];
     data[index++] = color_data[i];
     data[index++] = color_data[i];
     data[index++] = 255;
    }

 // fill out data
 xid->dx = header.dx;
 xid->dy = header.dy;
 xid->pitch = 4*header.dx;
 xid->size = size;
 xid->data = std::move(data);
 xid->format = DXGI_FORMAT_R8G8B8A8_UNORM;

 // success
 return EC_SUCCESS;
}

ErrorCode ReadTGA_09(std::ifstream& ifile, const TGAHEADER& header, TextureData* xid)
{
 // expecting a color map
 if(header.color_map_type != 1 || header.color_map_length == 0)
    return DebugErrorCode(EC_TGA_MISSING_COLOR_MAP, __LINE__, __FILE__);

 // read palette
 std::unique_ptr<TGACOLOR[]> palette;
 auto result = ReadTGAColorPalette(ifile, header, palette);
 if(result != EC_SUCCESS) return result;

 // always save as RGBA
 uint32 n_pixels = header.dx*header.dy;
 uint32 size = 4*n_pixels;
 std::unique_ptr<BYTE[]> data(new BYTE[size]);

 // read data
 uint32 index = 0;
 for(uint32 i = 0; i < n_pixels; i++)
    {
     // read header
     unsigned char c;
     ifile.read((char*)&c, sizeof(uint08));
     if(ifile.fail()) return DebugErrorCode(EC_FILE_READ, __LINE__, __FILE__);

     // determine header and remove header bit
     bool RLE = (c & 0x80) != 0;
     c = c & 0x7F;

     // RLE header
     if(RLE) {
        uint32 color_index;
        if(header.pixel_depth == 8) {
           uint08 temp;
           ifile.read((char*)&temp, sizeof(temp));
           if(ifile.fail()) return DebugErrorCode(EC_FILE_READ, __LINE__, __FILE__);
           color_index = temp;
          }
        else if(header.pixel_depth == 16) {
           uint16 temp;
           ifile.read((char*)&temp, sizeof(temp));
           if(ifile.fail()) return DebugErrorCode(EC_FILE_READ, __LINE__, __FILE__);
           color_index = temp;
          }
        else if(header.pixel_depth == 32) {
           uint32 temp;
           ifile.read((char*)&temp, sizeof(temp));
           if(ifile.fail()) return DebugErrorCode(EC_FILE_READ, __LINE__, __FILE__);
           color_index = temp;
          }
        else return DebugErrorCode(EC_TGA_PIXEL_DEPTH_UNSUPPORTED, __LINE__, __FILE__);
        uint32 repeat = c + 1;
        for(uint32 j = 0; j < repeat; j++) {
            data[index++] = palette[color_index - header.color_map_origin].r;
            data[index++] = palette[color_index - header.color_map_origin].g;
            data[index++] = palette[color_index - header.color_map_origin].b;
            data[index++] = palette[color_index - header.color_map_origin].a;
           }
       }
     // RAW header
     else {
        uint32 n = c + 1;
        for(size_t j = 0; j < n; j++) {
            uint32 color_index;
            if(header.pixel_depth == 8) {
               uint08 temp;
               ifile.read((char*)&temp, sizeof(temp));
               if(ifile.fail()) return DebugErrorCode(EC_FILE_READ, __LINE__, __FILE__);
               color_index = temp;
              }
            else if(header.pixel_depth == 16) {
               uint16 temp;
               ifile.read((char*)&temp, sizeof(temp));
               if(ifile.fail()) return DebugErrorCode(EC_FILE_READ, __LINE__, __FILE__);
               color_index = temp;
              }
            else if(header.pixel_depth == 32) {
               uint32 temp;
               ifile.read((char*)&temp, sizeof(temp));
               if(ifile.fail()) return DebugErrorCode(EC_FILE_READ, __LINE__, __FILE__);
               color_index = temp;
              }
            else return DebugErrorCode(EC_TGA_PIXEL_DEPTH_UNSUPPORTED, __LINE__, __FILE__);
            data[index++] = palette[color_index - header.color_map_origin].r;
            data[index++] = palette[color_index - header.color_map_origin].g;
            data[index++] = palette[color_index - header.color_map_origin].b;
            data[index++] = palette[color_index - header.color_map_origin].a;
           }
       }
    }

 // fill out data
 xid->dx = header.dx;
 xid->dy = header.dy;
 xid->pitch = 4*header.dx;
 xid->size = size;
 xid->data = std::move(data);
 xid->format = DXGI_FORMAT_R8G8B8A8_UNORM;

 // success
 return EC_SUCCESS;
}

ErrorCode ReadTGA_10(std::ifstream& ifile, const TGAHEADER& header, TextureData* xid)
{
 // skip color map
 if(header.color_map_type) {
    uint32 size = header.color_map_length;
    if(header.color_map_bits == 16) size *= 2;
    else if(header.color_map_bits == 24) size *= 3;
    else if(header.color_map_bits == 32) size *= 4;
    else return DebugErrorCode(EC_TGA_PIXEL_DEPTH_UNSUPPORTED, __LINE__, __FILE__);
    ifile.seekg(size, std::ios::cur);
    if(ifile.fail()) return DebugErrorCode(EC_FILE_SEEK, __LINE__, __FILE__);
   }

 // always save as RGBA
 uint32 n_pixels = header.dx*header.dy;
 uint32 size = 4*n_pixels;
 std::unique_ptr<BYTE[]> data(new BYTE[size]);

 // read data
 uint32 index = 0;
 while(index < size)
      {
       // read header
       unsigned char c;
       ifile.read((char*)&c, sizeof(uint08));
       if(ifile.fail()) return DebugErrorCode(EC_FILE_READ, __LINE__, __FILE__);
      
       // determine header and remove header bit
       bool RLE = (c & 0x80) != 0;
       c = c & 0x7F;
      
       // RLE header
       if(RLE) {
          TGACOLOR color;
          auto result = ReadTGAColor(ifile, header, color);
          if(result != EC_SUCCESS) return result;
          uint32 repeat = c + 1;
          for(uint32 j = 0; j < repeat; j++) {
              data[index++] = color.r;
              data[index++] = color.g;
              data[index++] = color.b;
              data[index++] = color.a;
             }
         }
       // RAW header
       else {
          uint32 n = c + 1;
          for(uint32 j = 0; j < n; j++) {
              TGACOLOR color;
              auto result = ReadTGAColor(ifile, header, color);
              if(result != EC_SUCCESS) return result;
              data[index++] = color.r;
              data[index++] = color.g;
              data[index++] = color.b;
              data[index++] = color.a;
             }
         }
      }

 // fill out data
 xid->dx = header.dx;
 xid->dy = header.dy;
 xid->pitch = 4*header.dx;
 xid->size = size;
 xid->data = std::move(data);
 xid->format = DXGI_FORMAT_R8G8B8A8_UNORM;

 // success
 return EC_SUCCESS;
}

ErrorCode ReadTGA_11(std::ifstream& ifile, const TGAHEADER& header, TextureData* xid)
{
 // skip color map
 if(header.color_map_type) {
    uint32 size = header.color_map_length;
    if(header.color_map_bits == 16) size *= 2;
    else if(header.color_map_bits == 24) size *= 3;
    else if(header.color_map_bits == 32) size *= 4;
    else return DebugErrorCode(EC_TGA_PIXEL_DEPTH_UNSUPPORTED, __LINE__, __FILE__);
    ifile.seekg(size, std::ios::cur);
    if(ifile.fail()) return DebugErrorCode(EC_FILE_SEEK, __LINE__, __FILE__);
   }

 // always save as RGBA
 uint32 n_pixels = header.dx*header.dy;
 uint32 size = 4*n_pixels;
 std::unique_ptr<BYTE[]> data(new BYTE[size]);

 // read data
 uint32 index = 0;
 for(uint32 i = 0; i < n_pixels; i++)
    {
     // read header
     unsigned char c;
     ifile.read((char*)&c, sizeof(uint08));
     if(ifile.fail()) return DebugErrorCode(EC_FILE_READ, __LINE__, __FILE__);

     // determine header and remove header bit
     bool RLE = (c & 0x80) != 0;
     c = c & 0x7F;

     // RLE header
     if(RLE) {
        uint08 color;
        ifile.read((char*)&color, sizeof(color));
        if(ifile.fail()) return DebugErrorCode(EC_FILE_READ, __LINE__, __FILE__);
        uint32 repeat = c + 1;
        for(uint32 j = 0; j < repeat; j++) {
            data[index++] = color;
            data[index++] = color;
            data[index++] = color;
            data[index++] = 255;
           }
       }
     // RAW header
     else {
        uint32 n = c + 1;
        for(uint32 j = 0; j < n; j++) {
            uint08 color;
            ifile.read((char*)&color, sizeof(color));
            if(ifile.fail()) return DebugErrorCode(EC_FILE_READ, __LINE__, __FILE__);
            data[index++] = color;
            data[index++] = color;
            data[index++] = color;
            data[index++] = 255;
           }
       }
    }

 // fill out data
 xid->dx = header.dx;
 xid->dy = header.dy;
 xid->pitch = 4*header.dx;
 xid->size = size;
 xid->data = std::move(data);
 xid->format = DXGI_FORMAT_R8G8B8A8_UNORM;

 // success
 return EC_SUCCESS;
}

ErrorCode ConvertTGA(LPCWSTR filename, LPCWSTR laraname)
{
 // success
 return EC_SUCCESS;
}