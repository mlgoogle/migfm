#include "stdafx.h"
#include "mainwindow.h"

#ifdef _DEBUG
#pragma comment(lib, "..\\Debug\\muspl.lib")
#else
#pragma comment(lib, "..\\Release\\muspl.lib")
#endif

HWND _hWnd = NULL;
CMigPlayer _myPlayer;

int WINAPI WinMain( __in HINSTANCE hInstance, __in_opt HINSTANCE hPrevInstance, __in_opt LPSTR lpCmdLine, __in int nShowCmd )
{
	CMyWindow myWindow;
	if( myWindow.MyRegisterClass() )
	{
		_hWnd = myWindow.MyCreateWindow();
	}

	if( _hWnd )
	{
		_myPlayer.SetEventWindow(_hWnd, WM_MIGPLAYER_EVENT);
		//_myPlayer.OpenMusic(_T("E:\\project\\miglab\\migfm\\src\\client\\muspl\\Debug\\Hello1.mp3"));
		_myPlayer.OpenMusic(_T("http://60.191.220.135/wu.mp3"));
		_myPlayer.Play();
	}

	MSG msg = {0};

	while( WM_QUIT != msg.message )
	{
		if( GetMessage(&msg, NULL, 0, 0) )
		{
			TranslateMessage( &msg );
			DispatchMessage( &msg );
		}
	}

	return 0;
}