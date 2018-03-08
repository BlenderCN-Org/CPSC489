/** \file    stdwin.h
 *  \brief   Windows API Application and Utility Functions
 *  \details Includes utility functions and classes for Win32/Win64 application
 *           programming.
 *  \author  Steven F. Emory
 *  \date    02/05/2018
 */
#ifndef __CPSC489_STDWIN_H
#define __CPSC489_STDWIN_H

#pragma region PERFORMANCE_COUNTER

class PerformanceCounter {
 private :
  __int64 t0;
  __int64 t1;
  __int64 hz;
 public :
  void begin(void) { QueryPerformanceCounter((LARGE_INTEGER*)(&t0)); }
  void end(void) { QueryPerformanceCounter((LARGE_INTEGER*)(&t1)); }
  __int64 ticks(void)const { return (t1 - t0); }
 public :
  double milliseconds(__int64 dt)const { return ((double)(dt/1000))/((double)hz); }
  double seconds(__int64 dt)const { return (double)dt/(double)hz; }
  double minutes(__int64 dt)const { return (double)dt/(double)(hz*60ll); }
  double hours(__int64 dt)const { return (double)dt/(double)(hz*3600ll); }
  double days(__int64 dt)const { return (double)dt/(double)(hz*86400ll); }
 public :
  double milliseconds(void)const { return milliseconds(ticks()); }
  double seconds(void)const { return seconds(ticks()); }
  double minutes(void)const { return minutes(ticks()); }
  double hours(void)const { return hours(ticks()); }
  double days(void)const { return days(ticks()); }
 public :
  void reset(void) {
   QueryPerformanceFrequency((LARGE_INTEGER*)(&hz));
   t0 = 0ull;
   t1 = 0ull;
  }
 public :
  PerformanceCounter() { QueryPerformanceFrequency((LARGE_INTEGER*)(&hz)); }
 ~PerformanceCounter() {}
};

#pragma endregion PERFORMANCE_COUNTER

#pragma region WINDOW_FUNCTIONS

inline BOOL GetWindowDimensions(HWND window, LPSIZE size)
{
 RECT rect;
 if(!GetWindowRect(window, &rect)) return FALSE;
 size->cx = rect.right - rect.left;
 size->cy = rect.bottom - rect.top;
 return TRUE;
}

inline BOOL GetClientDimensions(HWND window, LPSIZE size)
{
 RECT rect;
 if(!GetClientRect(window, &rect)) return FALSE;
 size->cx = rect.right - rect.left;
 size->cy = rect.bottom - rect.top;
 return TRUE;
}

#pragma endregion WINDOW_FUNCTIONS

#pragma region DIALOG_FUNCTIONS

void CenterDialog(HWND window, BOOL in_parent = TRUE);
BOOL ReverseMapDialogRect(HWND dialog, RECT* rect);
BOOL ColorDialog(HWND parent, COLORREF* color);

#ifdef UNICODE
#define BrowseDirectoryDialog BrowseDirectoryDialogW
#else
#define BrowseDirectoryDialog BrowseDirectoryDialogA
#endif
BOOL BrowseDirectoryDialogA(HWND parent, LPCSTR caption, LPSTR buffer);
BOOL BrowseDirectoryDialogW(HWND parent, LPCWSTR caption, LPWSTR buffer);

#ifdef UNICODE
#define OpenFileDialog OpenFileDialogW
#else
#define OpenFileDialog OpenFileDialogA
#endif
BOOL OpenFileDialogA(HWND parent, LPCSTR filter, LPCSTR title, LPCSTR defext, LPSTR filename, LPSTR initdir = 0);
BOOL OpenFileDialogW(HWND parent, LPCWSTR filter, LPCWSTR title, LPCWSTR defext, LPWSTR filename, LPWSTR initdir = 0);

#ifdef UNICODE
#define SaveFileDialog SaveFileDialogW
#else
#define SaveFileDialog SaveFileDialogA
#endif
BOOL SaveFileDialogA(HWND parent, LPCSTR filter, LPCSTR title, LPCSTR defext, LPSTR filename, LPSTR initdir = 0);
BOOL SaveFileDialogW(HWND parent, LPCWSTR filter, LPCWSTR title, LPCWSTR defext, LPWSTR filename, LPWSTR initdir = 0);

#pragma endregion DIALOG_FUNCTIONS

#pragma region FILENAME_FUNCTIONS

#ifdef UNICODE
#define GetModulePathname GetModulePathnameW
#else
#define GetModulePathname GetModulePathnameA
#endif
STDSTRINGA GetModulePathnameA(void);
STDSTRINGW GetModulePathnameW(void);

#ifdef UNICODE
#define GetShortFilename GetShortFilenameW
#else
#define GetShortFilename GetShortFilenameA
#endif
STDSTRINGA GetShortFilenameA(LPCSTR filename);
STDSTRINGW GetShortFilenameW(LPCWSTR filename);

#ifdef UNICODE
#define GetShortFilenameWithoutExtension GetShortFilenameWithoutExtensionW
#else
#define GetShortFilenameWithoutExtension GetShortFilenameWithoutExtensionA
#endif
STDSTRINGA GetShortFilenameWithoutExtensionA(LPCSTR filename);
STDSTRINGW GetShortFilenameWithoutExtensionW(LPCWSTR filename);

#ifdef UNICODE
#define GetPathnameFromFilename GetPathnameFromFilenameW
#else
#define GetPathnameFromFilename GetPathnameFromFilenameA
#endif
STDSTRINGA GetPathnameFromFilenameA(LPCSTR filename);
STDSTRINGW GetPathnameFromFilenameW(LPCWSTR filename);

#ifdef UNICODE
#define GetExtensionFromFilename GetExtensionFromFilenameW
#else
#define GetExtensionFromFilename GetExtensionFromFilenameA
#endif
STDSTRINGA GetExtensionFromFilenameA(LPCSTR filename);
STDSTRINGW GetExtensionFromFilenameW(LPCWSTR filename);

#ifdef UNICODE
#define ChangeFileExtension ChangeFileExtensionW
#else
#define ChangeFileExtension ChangeFileExtensionA
#endif
STDSTRINGA ChangeFileExtensionA(LPCSTR filename, LPCSTR extension);
STDSTRINGW ChangeFileExtensionW(LPCWSTR filename, LPCWSTR extension);

#ifdef UNICODE
#define HasExtension HasExtensionW
#else
#define HasExtension HasExtensionA
#endif
BOOL HasExtensionA(LPCSTR filename, LPCSTR extension);
BOOL HasExtensionW(LPCWSTR filename, LPCWSTR extension);

#ifdef UNICODE
#define FileExists FileExistsW
#else
#define FileExists FileExistsA
#endif
BOOL FileExistsA(LPCSTR filename);
BOOL FileExistsW(LPCWSTR filename);

#pragma endregion FILENAME_FUNCTIONS

#endif
