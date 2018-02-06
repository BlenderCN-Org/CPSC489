#ifndef __CPSC489_SHADERS_H
#define __CPSC489_SHADERS_H

// Vertex Shader Identifiers
#define VS_DEFAULT      0
#define VS_CONST_COLOR  1
#define VS_VERTEX_COLOR 2
#define VS_AXES         2
#define VS_SKELETON     3
#define VS_MODEL        4

// Pixel Shader Identifiers
#define PS_DEFAULT      0
#define PS_VERTEX_COLOR 1
#define PS_MODEL        4

// Core Structures
struct VertexShader {
 ID3D11VertexShader* shader;
 std::shared_ptr<char[]> code;
 DWORD size;
};
struct PixelShader {
 ID3D11PixelShader* shader;
 std::shared_ptr<char[]> code;
 DWORD size;
};

// Vertex Shader Functions
ErrorCode InitVertexShaders(void);
void FreeVertexShaders(void);
VertexShader* GetVertexShader(DWORD id);
ErrorCode SetVertexShader(DWORD id);

// Pixel Shader Functions
ErrorCode InitPixelShaders(void);
void FreePixelShaders(void);
PixelShader* GetPixelShader(DWORD id);
ErrorCode SetPixelShader(DWORD id);

#endif
