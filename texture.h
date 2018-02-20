/** \file    texture.h
 *  \brief   Support for loading, unloading, and finding textures.
 *  \details Although a texture can be reused many times - it should only be loaded once. To make
 *           sure texture data is shared, an unordered map can be used to keep a case-insensitive
 *           reference count on texture data that has already been loaded.
 *  \author  Steven F. Emory
 *  \date    02/19/2018
 */

#ifndef __CPSC489_TEXTURE_H
#define __CPSC489_TEXTURE_H

struct TextureData {
 DWORD dx;
 DWORD dy;
 DWORD pitch;
 DXGI_FORMAT format;
 std::unique_ptr<BYTE[]> data;
};

// Texture Functions
ErrorCode LoadTexture(LPCWSTR filename, ID3D11ShaderResourceView** srv);
ErrorCode FreeTexture(LPCWSTR filename);
ID3D11ShaderResourceView* FindTexture(LPCWSTR filename);

#endif
