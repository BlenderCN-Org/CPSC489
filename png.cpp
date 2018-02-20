#include "stdafx.h"
#include "errors.h"
#include "texture.h"
#include "png.h"

ErrorCode LoadPNG(LPCWSTR filename, TextureData* data)
{
 // validate
 if(!filename) return DebugErrorCode(EC_INVALID_ARG, __LINE__, __FILE__);
 if(!data) return DebugErrorCode(EC_INVALID_ARG, __LINE__, __FILE__);

 // create WIC factory
 CComPtr<IWICImagingFactory> wicf;
 HRESULT result = CoCreateInstance(CLSID_WICImagingFactory, NULL, CLSCTX_INPROC_SERVER, IID_IWICImagingFactory, (LPVOID*)&wicf);
 if(FAILED(result)) return DebugErrorCode(EC_PNG_COM_INIT, __LINE__, __FILE__);

 // create WIC PNG decoder
 CComPtr<IWICBitmapDecoder> wicd;
 result = wicf->CreateDecoder(GUID_ContainerFormatPng, NULL, &wicd);
 if(FAILED(result)) return DebugErrorCode(EC_PNG_DECODER, __LINE__, __FILE__);

 // open image
 CComPtr<IStream> wics;
 result = SHCreateStreamOnFileEx(filename, STGM_READ | STGM_SHARE_DENY_WRITE, 0, FALSE, NULL, &wics);
 if(FAILED(result)) return DebugErrorCode(EC_PNG_FILE_OPEN, __LINE__, __FILE__);

 // initialize decoder with stream
 wicd->Initialize(wics, WICDecodeMetadataCacheOnDemand);
 if(FAILED(result)) return DebugErrorCode(EC_PNG_DECODER_INIT, __LINE__, __FILE__);

 // must have at least one frame
 UINT count = 0;
 result = wicd->GetFrameCount(&count);
 if(FAILED(result) || (count == 0))
    return DebugErrorCode(EC_PNG_NO_FRAMES, __LINE__, __FILE__);

 // get frame
 CComPtr<IWICBitmapFrameDecode> frame = NULL;
 if(FAILED(wicd->GetFrame(0, &frame)))
   return DebugErrorCode(EC_PNG_GET_FRAMES, __LINE__, __FILE__);

 // create format converter
 CComPtr<IWICFormatConverter> converter = NULL;
 if(FAILED(wicf->CreateFormatConverter(&converter)))
    return DebugErrorCode(EC_PNG_CONVERTER, __LINE__, __FILE__);

 // initialize format converter (P stands for pre-multiplied alpha)
 // if you don't need pre-multiplied alpha, use GUID_WICPixelFormat32bppBGRA
 result = converter->Initialize(frame, GUID_WICPixelFormat32bppPBGRA, WICBitmapDitherTypeNone, NULL, 0.0f, WICBitmapPaletteTypeCustom);
 if(FAILED(result)) return DebugErrorCode(EC_PNG_CONVERTER_INIT, __LINE__, __FILE__);

 // retrieve dimensions
 UINT dx = 0;
 UINT dy = 0;
 if(FAILED(converter->GetSize(&dx, &dy)) || (!dx || !dy))
    return DebugErrorCode(EC_PNG_GET_SIZE, __LINE__, __FILE__);

 // copy image
 UINT stride = 4*dx;
 UINT size = stride*dy;
 std::unique_ptr<BYTE[]> buffer(new BYTE[size]);
 if(FAILED(converter->CopyPixels(NULL, stride, size, buffer.get())))
    return DebugErrorCode(EC_PNG_COPY, __LINE__, __FILE__);

 // transfer information
 data->dx = dx;
 data->dy = dy;
 data->pitch = 4 * dx;
 data->format = DXGI_FORMAT_B8G8R8X8_UNORM;
 data->size = size;
 data->data = std::move(buffer);

 return EC_SUCCESS;
}