#ifndef __CPSC489_APP_H
#define __CPSC489_APP_H

// Application Functions
HINSTANCE GetInstance(void);
HINSTANCE SetInstance(HINSTANCE handle);

// Message Loops
int MessageLoop(void);
int MessagePump(BOOL (*function)(real32));

// Error and Warning Messages
BOOL ErrorBox(LPCTSTR message);
BOOL ErrorBox(LPCTSTR message, LPCTSTR title);
BOOL ErrorBox(HWND window, LPCTSTR message);
BOOL ErrorBox(HWND window, LPCTSTR message, LPCTSTR title);

#endif
