
#include "WinMessageWindow.h"

const wchar_t* const kWindowsName = L"{7384BC41-6BC3-4151-BCC4-75455AD66446}";

WinMessageWindow::WinMessageWindow(void)
:hwnd_(NULL),
thread_id_(0)
{
}

WinMessageWindow::~WinMessageWindow(void)
{
	Stop();
}

void WinMessageWindow::Start()
{
	detector_hide_window_thread_ = boost::thread(&WinMessageWindow::MonitorThread, this);
}

void WinMessageWindow::Stop()
{
	if (thread_id_ != 0)
	{
		PostThreadMessage(thread_id_, WM_QUIT, 0, 0);
		detector_hide_window_thread_.join();
		thread_id_ = 0;
	}

	if (hwnd_ != NULL)
	{
		DestroyWindow(hwnd_);
		hwnd_ = NULL;
	}
}

void WinMessageWindow::MonitorThread()
{
	thread_id_ = ::GetCurrentThreadId();

	WNDCLASS wndclass;
	HINSTANCE hInstance = GetModuleHandle(NULL);

	wndclass.style        = CS_HREDRAW | CS_VREDRAW;
	wndclass.lpfnWndProc  = &WinMessageWindow::_WinProc;;
	wndclass.cbClsExtra   = 0;
	wndclass.cbWndExtra   = 0;
	wndclass.hInstance    = hInstance;
	wndclass.hIcon        = LoadIcon (NULL, IDI_APPLICATION);
	wndclass.hCursor      = LoadCursor (NULL, IDC_ARROW);
	wndclass.hbrBackground= (HBRUSH) GetStockObject (WHITE_BRUSH);
	wndclass.lpszMenuName = NULL;
	wndclass.lpszClassName= kWindowsName;

	if (!RegisterClass(&wndclass))
	{
		return;
	}

	hwnd_ = CreateWindow( kWindowsName,      // window class name
		kWindowsName,   // window caption
		WS_OVERLAPPEDWINDOW,  // window style
		CW_USEDEFAULT,// initial x position
		CW_USEDEFAULT,// initial y position
		CW_USEDEFAULT,// initial x size
		CW_USEDEFAULT,// initial y size
		NULL,                 // parent window handle
		NULL,            // window menu handle
		hInstance,   // program instance handle
		this) ;      // creation parameters

	if (NULL == hwnd_)
	{
		return ;
	}

	ShowWindow (hwnd_, SW_HIDE);
	UpdateWindow (hwnd_);

	MSG msg;
	while( GetMessage( &msg, NULL, 0, 0))
	{
		DispatchMessage(&msg);
	}
}

LRESULT CALLBACK WinMessageWindow::_WinProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	WinMessageWindow* pThis = NULL;
	if( uMsg == WM_NCCREATE )
	{
		LPCREATESTRUCT lpcs = reinterpret_cast<LPCREATESTRUCT>(lParam);
		pThis = static_cast<WinMessageWindow*>(lpcs->lpCreateParams);
		pThis->hwnd_ = hWnd;
		::SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LPARAM>(pThis));
	} 
	else 
	{
		pThis = reinterpret_cast<WinMessageWindow*>(::GetWindowLongPtr(hWnd, GWLP_USERDATA));
	}

	if( pThis != NULL ) 
	{
	   if (pThis->winProc)
	   {
		   pThis->winProc(hWnd, uMsg, wParam, lParam);
	   }
	}

	return ::DefWindowProc(hWnd, uMsg, wParam, lParam);
}

