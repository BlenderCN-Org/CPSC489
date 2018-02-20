/** \file    stdwin.cpp
 *  \brief   Windows API Application and Utility Functions
 *  \details Source file.
 *  \author  Steven F. Emory
 *  \date    02/05/2018
 *  \data    02/19/2018
 *           Updated file with functions for filename analysis, many of which I have used on other
 *           projects for many years.
 */
#include "stdafx.h"
#include "stdwin.h"

#pragma region DIALOG_FUNCTIONS

void CenterDialog(HWND window, BOOL in_parent)
{
 HWND dialog = window;
 HWND parent = ((in_parent && GetParent(dialog)) ? GetParent(dialog) : GetDesktopWindow());

 RECT rect1; GetWindowRect(dialog, &rect1);
 RECT rect2; GetWindowRect(parent, &rect2);

 int dx1 = rect1.right - rect1.left;
 int dx2 = rect2.right - rect2.left;

 int dy1 = rect1.bottom - rect1.top;
 int dy2 = rect2.bottom - rect2.top;

 int x = (dx2 - dx1)/2 + rect1.left;
 int y = (dy2 - dy1)/2 + rect1.top;

 MoveWindow(dialog, x, y, dx1, dy1, TRUE);
}

BOOL ReverseMapDialogRect(HWND dialog, RECT* rect)
{
 // map base rectangle (dialog to window)
 RECT base = { 0, 0, 4, 8 };
 if(!MapDialogRect(dialog, &base)) return FALSE;
 LONG baseunitX = base.right;
 LONG baseunitY = base.bottom;

 // for example on my computer 240, 320 in dialog coordinates
 // is 480, 640 in window coordinates

 // now map window to dialog
 rect->left = MulDiv(rect->left, 4, baseunitX);
 rect->right = MulDiv(rect->right, 4, baseunitX);
 rect->top = MulDiv(rect->top, 8, baseunitY);
 rect->bottom = MulDiv(rect->bottom, 8, baseunitY);
 return TRUE;
}

BOOL ColorDialog(HWND parent, COLORREF* color)
{
 // validate
 if(!parent || !IsWindow(parent)) return FALSE; // parent must be valid
 if(!color) return FALSE; // must have a color

 // small custom color palette
 static COLORREF custom[16] = {
  RGB(  0,  0,  0), RGB(255,255,255), RGB(128,128,128), RGB(192,192,192),
  RGB(128,  0,  0), RGB(  0,128,  0), RGB(  0,  0,128), RGB(128,128,  0),
  RGB(128,  0,128), RGB(  0,128,128), RGB(255, 255, 0), RGB(255,  0,255),
  RGB(  0,255,255), RGB(255,  0,  0), RGB(  0,255,  0), RGB(  0,  0,255)
 };

 // set parameters 
 CHOOSECOLOR data;
 ZeroMemory(&data, sizeof(data));
 data.lStructSize    = sizeof(CHOOSECOLOR);
 data.hwndOwner      = parent;
 data.hInstance      = NULL;
 data.rgbResult      = *color;
 data.lpCustColors   = custom;
 data.Flags          = CC_RGBINIT | CC_FULLOPEN;
 data.lCustData      = 0;
 data.lpfnHook       = NULL;
 data.lpTemplateName = NULL;

 // display color dialog
 if(ChooseColor(&data) == FALSE) return FALSE; 

 // set result
 *color = data.rgbResult;
 return TRUE;
}

// Browse Directory Dialog
int CALLBACK BrowseDirectoryDialogProcA(HWND window, UINT message, LPARAM lparam, LPARAM)
{
 CHAR directory[MAX_PATH];
 switch(message) {
   case(BFFM_INITIALIZED) : {
        if(GetCurrentDirectoryA(MAX_PATH, directory))
           SendMessage(window, BFFM_SETSELECTION,  1, (LPARAM)directory);
        break;
       }
   case(BFFM_SELCHANGED) : {
        if(SHGetPathFromIDListA((LPITEMIDLIST)lparam, directory))
           SendMessage(window, BFFM_SETSTATUSTEXT, 0, (LPARAM)directory);
        break;
       }
  }
 return 0;
}
int CALLBACK BrowseDirectoryDialogProcW(HWND window, UINT message, LPARAM lparam, LPARAM)
{
 WCHAR directory[MAX_PATH];
 switch(message) {
   case(BFFM_INITIALIZED) : {
        if(GetCurrentDirectoryW(MAX_PATH, directory))
           SendMessage(window, BFFM_SETSELECTION,  1, (LPARAM)directory);
        break;
       }
   case(BFFM_SELCHANGED) : {
        if(SHGetPathFromIDListW((LPITEMIDLIST)lparam, directory))
           SendMessage(window, BFFM_SETSTATUSTEXT, 0, (LPARAM)directory);
        break;
       }
  }
 return 0;
}

BOOL BrowseDirectoryDialogA(HWND parent, LPCSTR caption, LPSTR buffer)
{
 CHAR display_name[MAX_PATH];
 display_name[0] = '\0';

 BROWSEINFOA bi = {
  parent,
  0,
  display_name,
  caption,
  BIF_RETURNONLYFSDIRS,
  BrowseDirectoryDialogProcA,
  0
 };

 LPITEMIDLIST pIIL = ::SHBrowseForFolderA(&bi);
 if(SHGetPathFromIDListA(pIIL, buffer)) {
    LPMALLOC pMalloc;
    HRESULT hr = SHGetMalloc(&pMalloc);
    pMalloc->Free(pIIL);
    pMalloc->Release();
    return TRUE;
   }

 return FALSE;
}

BOOL BrowseDirectoryDialogW(HWND parent, LPCWSTR caption, LPWSTR buffer)
{
 WCHAR display_name[MAX_PATH];
 display_name[0] = L'\0';

 BROWSEINFOW bi = {
  parent,
  0,
  display_name,
  caption,
  BIF_RETURNONLYFSDIRS,
  BrowseDirectoryDialogProcW,
  0
 };

 LPITEMIDLIST pIIL = ::SHBrowseForFolderW(&bi);
 if(SHGetPathFromIDListW(pIIL, buffer)) {
    LPMALLOC pMalloc;
    HRESULT hr = SHGetMalloc(&pMalloc);
    pMalloc->Free(pIIL);
    pMalloc->Release();
    return TRUE;
   }

 return FALSE;
}

BOOL OpenFileDialogA(HWND parent, LPCSTR filter, LPCSTR title, LPCSTR defext, LPSTR filename, LPSTR initdir)
{
 // initialize strings
 CHAR buffer1[MAX_PATH];
 CHAR buffer2[MAX_PATH];
 buffer1[0] = '\0';
 buffer2[0] = '\0';

 // open file dialog
 OPENFILENAMEA ofn;
 ZeroMemory(&ofn, sizeof(ofn));
 ofn.lStructSize     = sizeof(ofn);
 ofn.hwndOwner       = parent;
 ofn.hInstance       = (HINSTANCE)GetModuleHandle(0);
 ofn.lpstrFilter     = filter;
 ofn.lpstrFile       = buffer1;
 ofn.nMaxFile        = MAX_PATH;
 ofn.lpstrFileTitle  = buffer2;
 ofn.nMaxFileTitle   = MAX_PATH;
 ofn.lpstrInitialDir = initdir;
 ofn.lpstrTitle      = title;
 ofn.Flags           = OFN_FILEMUSTEXIST;
 ofn.lpstrDefExt     = defext;
 if(!GetOpenFileNameA(&ofn)) return FALSE;

 // copy string
 StrCpyA(filename, buffer1);
 return TRUE;
}

BOOL OpenFileDialogW(HWND parent, LPCWSTR filter, LPCWSTR title, LPCWSTR defext, LPWSTR filename, LPWSTR initdir)
{
 // initialize strings
 WCHAR buffer1[MAX_PATH];
 WCHAR buffer2[MAX_PATH];
 buffer1[0] = L'\0';
 buffer2[0] = L'\0';

 // open file dialog
 OPENFILENAMEW ofn;
 ZeroMemory(&ofn, sizeof(ofn));
 ofn.lStructSize     = sizeof(ofn);
 ofn.hwndOwner       = parent;
 ofn.hInstance       = (HINSTANCE)GetModuleHandle(0);
 ofn.lpstrFilter     = filter;
 ofn.lpstrFile       = buffer1;
 ofn.nMaxFile        = MAX_PATH;
 ofn.lpstrFileTitle  = buffer2;
 ofn.nMaxFileTitle   = MAX_PATH;
 ofn.lpstrInitialDir = initdir;
 ofn.lpstrTitle      = title;
 ofn.Flags           = OFN_FILEMUSTEXIST;
 ofn.lpstrDefExt     = defext;
 if(!GetOpenFileNameW(&ofn)) return FALSE;

 // copy string
 StrCpyW(filename, buffer1);
 return TRUE;
}

BOOL SaveFileDialogA(HWND parent, LPCSTR filter, LPCSTR title, LPCSTR defext, LPSTR filename, LPSTR initdir)
{
 // initialize strings
 char buffer1[MAX_PATH];
 char buffer2[MAX_PATH];
 buffer1[0] = '\0';
 buffer2[0] = '\0';

 // save file dialog
 OPENFILENAMEA ofn;
 ZeroMemory(&ofn, sizeof(ofn));
 ofn.lStructSize     = sizeof(ofn);
 ofn.hwndOwner       = parent;
 ofn.hInstance       = (HINSTANCE)GetModuleHandle(0);
 ofn.lpstrFilter     = filter;
 ofn.lpstrFile       = buffer1;
 ofn.nMaxFile        = MAX_PATH;
 ofn.lpstrFileTitle  = buffer2;
 ofn.nMaxFileTitle   = MAX_PATH;
 ofn.lpstrInitialDir = initdir;
 ofn.lpstrTitle      = title;
 ofn.Flags           = OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT;
 ofn.lpstrDefExt     = defext;
 if(!GetSaveFileNameA(&ofn)) return FALSE;

 // copy string
 StrCpyA(filename, buffer1);
 return TRUE;
}

BOOL SaveFileDialogW(HWND parent, LPCWSTR filter, LPCWSTR title, LPCWSTR defext, LPWSTR filename, LPWSTR initdir)
{
 // initialize strings
 WCHAR buffer1[MAX_PATH];
 WCHAR buffer2[MAX_PATH];
 buffer1[0] = L'\0';
 buffer2[0] = L'\0';

 // save file dialog
 OPENFILENAMEW ofn;
 ZeroMemory(&ofn, sizeof(ofn));
 ofn.lStructSize     = sizeof(ofn);
 ofn.hwndOwner       = parent;
 ofn.hInstance       = (HINSTANCE)GetModuleHandle(0);
 ofn.lpstrFilter     = filter;
 ofn.lpstrFile       = buffer1;
 ofn.nMaxFile        = MAX_PATH;
 ofn.lpstrFileTitle  = buffer2;
 ofn.nMaxFileTitle   = MAX_PATH;
 ofn.lpstrInitialDir = initdir;
 ofn.lpstrTitle      = title;
 ofn.Flags           = OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT;
 ofn.lpstrDefExt     = defext;
 if(!GetSaveFileNameW(&ofn)) return FALSE;

 // copy string
 StrCpyW(filename, buffer1);
 return TRUE;
}

#pragma endregion DIALOG_FUNCTIONS

#pragma region FILENAME_FUNCTIONS

STDSTRINGA GetModulePathnameA(void)
{
 // get filename
 CHAR filename[MAX_PATH];
 GetModuleFileNameA(NULL, &filename[0], MAX_PATH);

 // extract components
 CHAR c_param1[MAX_PATH];
 CHAR c_param2[MAX_PATH];
 CHAR c_param3[MAX_PATH];
 CHAR c_param4[MAX_PATH];
 c_param1[0] = static_cast<CHAR>(0);
 c_param2[0] = static_cast<CHAR>(0);
 c_param3[0] = static_cast<CHAR>(0);
 c_param4[0] = static_cast<CHAR>(0);
 _splitpath_s(filename, c_param1, MAX_PATH, c_param2, MAX_PATH, c_param3, MAX_PATH, c_param4, MAX_PATH);

 // return result
 STDSTRINGA retval(c_param1);
 retval += c_param2;
 return retval;
}

STDSTRINGW GetModulePathnameW(void)
{
 // get filename
 WCHAR filename[MAX_PATH];
 GetModuleFileNameW(NULL, &filename[0], MAX_PATH);

 // extract components
 WCHAR c_param1[MAX_PATH];
 WCHAR c_param2[MAX_PATH];
 WCHAR c_param3[MAX_PATH];
 WCHAR c_param4[MAX_PATH];
 c_param1[0] = static_cast<WCHAR>(0);
 c_param2[0] = static_cast<WCHAR>(0);
 c_param3[0] = static_cast<WCHAR>(0);
 c_param4[0] = static_cast<WCHAR>(0);
 _wsplitpath_s(filename, c_param1, MAX_PATH, c_param2, MAX_PATH, c_param3, MAX_PATH, c_param4, MAX_PATH);

 // return result
 STDSTRINGW retval(c_param1);
 retval += c_param2;
 return retval;
}

STDSTRINGA GetShortFilenameA(LPCSTR filename)
{
 // validate filename
 if(!filename) return STDSTRINGA();
 if(!strlen(filename)) return STDSTRINGA();

 // extract components
 CHAR c_param1[MAX_PATH];
 CHAR c_param2[MAX_PATH];
 CHAR c_param3[MAX_PATH];
 CHAR c_param4[MAX_PATH];
 c_param1[0] = static_cast<CHAR>(0);
 c_param2[0] = static_cast<CHAR>(0);
 c_param3[0] = static_cast<CHAR>(0);
 c_param4[0] = static_cast<CHAR>(0);
 _splitpath_s(filename, c_param1, MAX_PATH, c_param2, MAX_PATH, c_param3, MAX_PATH, c_param4, MAX_PATH);

 // return result
 STDSTRINGA retval(c_param3);
 retval += c_param4;
 return retval;
}

STDSTRINGW GetShortFilenameW(LPCWSTR filename)
{
 // validate filename
 if(!filename) return STDSTRINGW();
 if(!wcslen(filename)) return STDSTRINGW();

 // extract components
 WCHAR c_param1[MAX_PATH];
 WCHAR c_param2[MAX_PATH];
 WCHAR c_param3[MAX_PATH];
 WCHAR c_param4[MAX_PATH];
 c_param1[0] = static_cast<WCHAR>(0);
 c_param2[0] = static_cast<WCHAR>(0);
 c_param3[0] = static_cast<WCHAR>(0);
 c_param4[0] = static_cast<WCHAR>(0);
 _wsplitpath_s(filename, c_param1, MAX_PATH, c_param2, MAX_PATH, c_param3, MAX_PATH, c_param4, MAX_PATH); 

 // return result
 STDSTRINGW retval(c_param3);
 retval += c_param4;
 return retval;
}

STDSTRINGA GetShortFilenameWithoutExtensionA(LPCSTR filename)
{
 // validate filename
 if(!filename) return STDSTRINGA();
 if(!strlen(filename)) return STDSTRINGA();

 // extract components
 CHAR c_param1[MAX_PATH];
 CHAR c_param2[MAX_PATH];
 CHAR c_param3[MAX_PATH];
 CHAR c_param4[MAX_PATH];
 c_param1[0] = static_cast<CHAR>(0);
 c_param2[0] = static_cast<CHAR>(0);
 c_param3[0] = static_cast<CHAR>(0);
 c_param4[0] = static_cast<CHAR>(0);
 _splitpath_s(filename, c_param1, MAX_PATH, c_param2, MAX_PATH, c_param3, MAX_PATH, c_param4, MAX_PATH);

 // return result
 return STDSTRINGA(c_param3);
}

STDSTRINGW GetShortFilenameWithoutExtensionW(LPCWSTR filename)
{
 // validate filename
 if(!filename) return STDSTRINGW();
 if(!wcslen(filename)) return STDSTRINGW();

 // extract components
 WCHAR c_param1[MAX_PATH];
 WCHAR c_param2[MAX_PATH];
 WCHAR c_param3[MAX_PATH];
 WCHAR c_param4[MAX_PATH];
 c_param1[0] = static_cast<WCHAR>(0);
 c_param2[0] = static_cast<WCHAR>(0);
 c_param3[0] = static_cast<WCHAR>(0);
 c_param4[0] = static_cast<WCHAR>(0);
 _wsplitpath_s(filename, c_param1, MAX_PATH, c_param2, MAX_PATH, c_param3, MAX_PATH, c_param4, MAX_PATH);

 // return result
 return STDSTRINGW(c_param3);
}

STDSTRINGA GetPathnameFromFilenameA(LPCSTR filename)
{
 // validate filename
 if(!filename) return STDSTRINGA();
 if(!strlen(filename)) return STDSTRINGA();

 // extract components
 CHAR c_param1[MAX_PATH];
 CHAR c_param2[MAX_PATH];
 CHAR c_param3[MAX_PATH];
 CHAR c_param4[MAX_PATH];

 // extract components
 c_param1[0] = static_cast<CHAR>(0);
 c_param2[0] = static_cast<CHAR>(0);
 c_param3[0] = static_cast<CHAR>(0);
 c_param4[0] = static_cast<CHAR>(0);
 _splitpath_s(filename, c_param1, MAX_PATH, c_param2, MAX_PATH, c_param3, MAX_PATH, c_param4, MAX_PATH);

 // append components
 STDSTRINGA retval = c_param1;
 retval += c_param2;
 return retval;
}

STDSTRINGW GetPathnameFromFilenameW(LPCWSTR filename)
{
 // validate filename
 if(!filename) return STDSTRINGW();
 if(!wcslen(filename)) return STDSTRINGW();

 // extract components
 WCHAR c_param1[MAX_PATH];
 WCHAR c_param2[MAX_PATH];
 WCHAR c_param3[MAX_PATH];
 WCHAR c_param4[MAX_PATH];
 c_param1[0] = static_cast<WCHAR>(0);
 c_param2[0] = static_cast<WCHAR>(0);
 c_param3[0] = static_cast<WCHAR>(0);
 c_param4[0] = static_cast<WCHAR>(0);
 _wsplitpath_s(filename, c_param1, MAX_PATH, c_param2, MAX_PATH, c_param3, MAX_PATH, c_param4, MAX_PATH);

 // append components
 STDSTRINGW retval = c_param1;
 retval += c_param2;
 return retval;
}

STDSTRINGA GetExtensionFromFilenameA(LPCSTR filename)
{
 // compute length of filename
 size_t fnlen = 0;
 if(FAILED(StringCchLengthA(filename, MAX_PATH, &fnlen)))
    return STDSTRINGA();

 // must have a length
 if(!fnlen) return STDSTRINGA();

 // extract components
 CHAR c_param1[MAX_PATH];
 CHAR c_param2[MAX_PATH];
 CHAR c_param3[MAX_PATH];
 CHAR c_param4[MAX_PATH];
 c_param1[0] = static_cast<CHAR>(0);
 c_param2[0] = static_cast<CHAR>(0);
 c_param3[0] = static_cast<CHAR>(0);
 c_param4[0] = static_cast<CHAR>(0);
 _splitpath_s(filename, c_param1, MAX_PATH, c_param2, MAX_PATH, c_param3, MAX_PATH, c_param4, MAX_PATH); 

 // return extension
 return STDSTRINGA(c_param4);
}

STDSTRINGW GetExtensionFromFilenameW(LPCWSTR filename)
{
 // compute length of filename
 size_t fnlen = 0;
 if(FAILED(StringCchLengthW(filename, MAX_PATH, &fnlen)))
    return STDSTRINGW();

 // must have a length
 if(!fnlen) return STDSTRINGW();

 // extract components
 WCHAR c_param1[MAX_PATH];
 WCHAR c_param2[MAX_PATH];
 WCHAR c_param3[MAX_PATH];
 WCHAR c_param4[MAX_PATH];
 c_param1[0] = static_cast<WCHAR>(0);
 c_param2[0] = static_cast<WCHAR>(0);
 c_param3[0] = static_cast<WCHAR>(0);
 c_param4[0] = static_cast<WCHAR>(0);
 _wsplitpath_s(filename, c_param1, MAX_PATH, c_param2, MAX_PATH, c_param3, MAX_PATH, c_param4, MAX_PATH);

 // return extension
 return STDSTRINGW(c_param4);
}

STDSTRINGA ChangeFileExtensionA(LPCSTR filename, LPCSTR extension)
{
 // validate length
 size_t len = 0;
 if(FAILED(StringCchLengthA(filename, MAX_PATH, &len))) return STDSTRINGA();
 if(!len) return STDSTRINGA();

 // extract components
 CHAR c_param1[MAX_PATH];
 CHAR c_param2[MAX_PATH];
 CHAR c_param3[MAX_PATH];
 CHAR c_param4[MAX_PATH];

 // extract components
 c_param1[0] = 0;
 c_param2[0] = 0;
 c_param3[0] = 0;
 c_param4[0] = 0;
 _splitpath_s(filename, c_param1, MAX_PATH, c_param2, MAX_PATH, c_param3, MAX_PATH, c_param4, MAX_PATH);

 // append name + extension
 STDSTRINGA retval(c_param1);
 retval += c_param2;
 retval += c_param3;
 retval += ".";
 retval += extension;

 // return extension
 return retval;
}

STDSTRINGW ChangeFileExtensionW(LPCWSTR filename, LPCWSTR extension)
{
 // validate length
 size_t len = 0;
 if(FAILED(StringCchLength(filename, MAX_PATH, &len))) return STDSTRINGW();
 if(!len) return STDSTRINGW();

 // extract components
 WCHAR c_param1[MAX_PATH];
 WCHAR c_param2[MAX_PATH];
 WCHAR c_param3[MAX_PATH];
 WCHAR c_param4[MAX_PATH];

 // extract components
 c_param1[0] = static_cast<WCHAR>(0);
 c_param2[0] = static_cast<WCHAR>(0);
 c_param3[0] = static_cast<WCHAR>(0);
 c_param4[0] = static_cast<WCHAR>(0);
 _wsplitpath_s(filename, c_param1, MAX_PATH, c_param2, MAX_PATH, c_param3, MAX_PATH, c_param4, MAX_PATH);

 // append name + extension
 STDSTRINGW retval(c_param1);
 retval += c_param2;
 retval += c_param3;
 retval += L".";
 retval += extension;

 // return extension
 return retval;
}

BOOL HasExtensionA(LPCSTR filename, LPCSTR extension)
{
 return (_stricmp(GetExtensionFromFilenameA(filename).c_str(), extension) == 0);
}

BOOL HasExtensionW(LPCWSTR filename, LPCWSTR extension)
{
 return (_wcsicmp(GetExtensionFromFilenameW(filename).c_str(), extension) == 0);
}

BOOL FileExistsA(LPCSTR filename)
{
 DWORD attrs = GetFileAttributesA(filename);
 if(attrs == INVALID_FILE_ATTRIBUTES) return FALSE; // must succeed
 if(attrs & FILE_ATTRIBUTE_DIRECTORY) return FALSE; // must be a file
 return TRUE;
}

BOOL FileExistsW(LPCWSTR filename)
{
 DWORD attrs = GetFileAttributesW(filename);
 if(attrs == INVALID_FILE_ATTRIBUTES) return FALSE; // must succeed
 if(attrs & FILE_ATTRIBUTE_DIRECTORY) return FALSE; // must be a file
 return TRUE;
}

#pragma endregion FILENAME_FUNCTIONS