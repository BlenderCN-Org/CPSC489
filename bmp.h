#ifndef __CPSC489_BMP_H
#define __CPSC489_BMP_H

ErrorCode LoadBMP(LPCWSTR filename, TextureData* data);
ErrorCode ConvertBMP(LPCWSTR filename, LPCWSTR outfile = nullptr);

#endif
