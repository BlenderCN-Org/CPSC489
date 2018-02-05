/** \file    stdwin.h
 *  \brief   Windows API Application and Utility Functions
 *  \details Includes utility functions and classes for Win32/Win64 application
 *           programming.
 *  \author  Steven F. Emory
 *  \date    02/05/2018
 */
#ifndef __CPSC489_STDWIN_H
#define __CPSC489_STDWIN_H

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
  PerformanceCounter() { QueryPerformanceFrequency((LARGE_INTEGER*)(&hz)); }
 ~PerformanceCounter() {}
};

#endif

