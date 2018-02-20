#ifndef __CPSC489_TGA_H
#define __CPSC489_TGA_H

ErrorCode LoadTGA(LPCWSTR filename, TextureData* data);
ErrorCode ConvertTGA(LPCWSTR filename, LPCWSTR outfile = nullptr);

#endif
