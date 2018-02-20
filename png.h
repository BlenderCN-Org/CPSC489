#ifndef __CPSC489_PNG_H
#define __CPSC489_PNG_H

ErrorCode LoadPNG(LPCWSTR filename, TextureData* data);
ErrorCode ConvertPNG(LPCWSTR filename, LPCWSTR outfile = nullptr);

#endif