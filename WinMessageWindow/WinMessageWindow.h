#pragma once

#ifndef WinMessageWindow_h__
#define WinMessageWindow_h__

#include <Windows.h>

#include <boost/thread.hpp>
#include <boost/noncopyable.hpp>
#include <boost/function.hpp>

typedef boost::function<LRESULT (HWND, UINT, WPARAM, LPARAM)> WinProcHandler;

class WinMessageWindow: boost::noncopyable
{
public:
	WinMessageWindow(void);
	~WinMessageWindow(void);

	void Start();
	void Stop();

	HWND GetHWND()const {return hwnd_;}
	DWORD GetThreadId() const{ return thread_id_;}

	void setWinProc( const WinProcHandler& handler ) { winProc = handler; }

private:
	static LRESULT CALLBACK _WinProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) ;
	void MonitorThread();

private:

	WinProcHandler winProc;
	boost::thread detector_hide_window_thread_;
	DWORD thread_id_;
	HWND hwnd_;
};

#endif // WinMessageWindow_h__