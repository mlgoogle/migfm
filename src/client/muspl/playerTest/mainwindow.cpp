#include "mainwindow.h"

HINSTANCE _hInstance = NULL;
extern CMigPlayer _myPlayer;

void CMyWindow::OnGraphEvent(long eventCode, LONG_PTR param1, LONG_PTR param2)
{
	switch( eventCode )
	{
	case EC_COMPLETE:
		MessageBoxA( 0, "finish", 0, 0 );
		break;
	}
}

CMyWindow::CMyWindow()
{
	m_hWnd = NULL;
}

CMyWindow::~CMyWindow()
{

}

BOOL CMyWindow::MyRegisterClass()
{
	BOOL bRet = FALSE;

	WNDCLASS wc = {0};
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wc.hCursor = LoadCursor( NULL, IDC_ARROW );
	wc.hInstance = _hInstance;
	wc.lpfnWndProc = WndProc;
	wc.lpszClassName = MYCLASSNAME;
	wc.style = CS_VREDRAW | CS_HREDRAW;

	if( SUCCEEDED( RegisterClass(&wc) ) )
		bRet = TRUE;

	return bRet;
}

HWND CMyWindow::MyCreateWindow()
{
	if( !m_hWnd )
	{
		m_hWnd = CreateWindow( MYCLASSNAME, MYWINDOWNAME, WS_OVERLAPPEDWINDOW,
			0, 0, 800, 600, NULL, NULL, _hInstance, NULL);	

		SetProp( m_hWnd, _THIS, (HANDLE)this );

		ShowWindow( m_hWnd, SW_SHOW );
		UpdateWindow( m_hWnd );
	}

	return m_hWnd;
}

LRESULT CALLBACK CMyWindow::WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	CMyWindow* pThis = (CMyWindow*)::GetProp( hWnd, _THIS );

	switch(msg)
	{
	case WM_DESTROY:
		{
			if(pThis)
				pThis->OnDestroy();
		}
		break;

	case WM_KEYDOWN:
		{
			if(pThis)
			{
				pThis->OnKeyDown(lParam);
			}
		}
		break;

	case WM_MIGPLAYER_EVENT:
		{
			if(pThis)
			{
				_myPlayer.HandleGraphEvent(pThis);
			}
		}
		break;
	}

	return DefWindowProc( hWnd, msg, wParam, lParam );
}

void CMyWindow::OnDestroy()
{
	PostQuitMessage(0);
}

void CMyWindow::OnKeyDown(LPARAM lParam)
{
	switch( lParam )
	{
	case VK_UP:
		_myPlayer.SetVolume(10000);
		break;
	}
}