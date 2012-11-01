#pragma once

#include "stdafx.h"

class CMyWindow : public GraphEventCallback
{
public:
	CMyWindow();
	virtual ~CMyWindow();

	HWND GetWnd() {return m_hWnd;}

	BOOL MyRegisterClass();
	HWND MyCreateWindow();

private:
	static LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
	void OnGraphEvent(long eventCode, LONG_PTR param1, LONG_PTR param2);

	void OnCreate();
	void OnDestroy();
	void OnKeyDown(LPARAM lParam);

private:
	HWND m_hWnd;
};