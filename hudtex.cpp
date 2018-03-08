#include "stdafx.h"
#include "errors.h"
#include "app.h"
#include "win.h"
#include "gfx.h"
#include "hudtex.h"

static HDC winDC = NULL;
static const int n_fonts = 2;
static HFONT fontlist[n_fonts];

ErrorCode InitHUD(void)
{
 // free previous
 FreeHUD();

 // create device context
 winDC = GetDC(GetMainWindow());
 if(!winDC) return DebugErrorCode(EC_HUD_INIT, __LINE__, __FILE__);

 return EC_SUCCESS;
}

void FreeHUD(void)
{
 // release device context
 if(winDC) ReleaseDC(GetMainWindow(), winDC);
 winDC = NULL;
}

ErrorCode RenderHUD(void)
{
 return EC_SUCCESS;
}