#include "gtest/gtest.h"

#include <windows.h>
#include <tchar.h>
#include <string>

#include "WinMessageWindow.h"

const UINT_PTR TIMER_EVENT_ID  = 1000;

#define UWM_DEVICECHANGE WM_APP + 1

LRESULT Process(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch(uMsg)
	{
	case WM_DEVICECHANGE:
		SetTimer(hWnd, TIMER_EVENT_ID, 2000, NULL);
		break;
	case WM_TIMER:
		if(TIMER_EVENT_ID == wParam)
		{
			KillTimer(hWnd,wParam);
			printf("WM_TIMER\n");
		}
		break;
	case UWM_DEVICECHANGE:
		printf("UWM_DEVICECHANGE\n");
		break;
	case WM_QUIT:
		printf("quit\n");
		break;
	default:
		break;
	}
	return 0;
}

class WinProcess
{
public:
	LRESULT Process(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		if (WM_DEVICECHANGE == uMsg)
		{
			DWORD tick_begin = GetTickCount();
			printf("%d\n", tick_begin);
		}
		return 0;
	}
};

TEST(WinMessageWindow, Start) 
{
	WinMessageWindow window;

	WinProcess win_process;
	window.Start();
	Sleep(1000);
	window.setWinProc(Process);
	//window.setWinProc(boost::bind(&WinProcess::Process, &win_process, _1, _2, _3, _4));

	printf("go\n");
	getchar();
	EXPECT_TRUE(::PostMessage(window.GetHWND(), UWM_DEVICECHANGE, 0, 0));

	printf("Stop\n");
	getchar();
	window.Stop();
} 
