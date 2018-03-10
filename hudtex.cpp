#include "stdafx.h"
#include "errors.h"
#include "app.h"
#include "win.h"
#include "gfx.h"
#include "hudtex.h"

static HDC winDC = NULL;
static const int n_fonts = 2;
static HFONT fontlist[n_fonts] = { NULL, NULL };

ErrorCode InitHUD(void)
{
 // free previous
 FreeHUD();

 // create device context
 winDC = GetDC(GetMainWindow());
 if(!winDC) return DebugErrorCode(EC_HUD_INIT, __LINE__, __FILE__);

 // initalize LOGFONTS
 LOGFONT lf[n_fonts] = {
  {
   16,
   0, 0, 0,
   FW_NORMAL,
   FALSE, FALSE, FALSE,
   DEFAULT_CHARSET,
   OUT_DEFAULT_PRECIS,
   CLIP_DEFAULT_PRECIS,
   DEFAULT_QUALITY,
   DEFAULT_PITCH | FF_MODERN,
   L"Courier New"
  },
  {
   16,
   0, 0, 0,
   FW_NORMAL,
   FALSE, FALSE, FALSE,
   DEFAULT_CHARSET,
   OUT_DEFAULT_PRECIS,
   CLIP_DEFAULT_PRECIS,
   DEFAULT_QUALITY,
   DEFAULT_PITCH | FF_MODERN,
   L"Times New Roman"
  }
 };

 // create fonts
 fontlist[0] = CreateFontIndirect(&lf[0]);
 fontlist[1] = CreateFontIndirect(&lf[1]);
 if(!fontlist[0] || !fontlist[1]) {
    FreeHUD();
    return DebugErrorCode(EC_HUD_INIT, __LINE__, __FILE__);
   }


 return EC_SUCCESS;
}

void FreeHUD(void)
{
 // release fonts
 if(fontlist[0]) DeleteObject(fontlist[0]);
 if(fontlist[1]) DeleteObject(fontlist[1]);
 fontlist[0] = NULL;
 fontlist[1] = NULL;

 // release device context
 if(winDC) ReleaseDC(GetMainWindow(), winDC);
 winDC = NULL;
}

ErrorCode RenderHUD(void)
{
 return EC_SUCCESS;
}