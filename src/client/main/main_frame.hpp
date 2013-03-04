#ifndef MAINFRAME_HPP
#define MAINFRAME_HPP

#include "skin_change_event.hpp"

class MainFrame : public WindowImplBase
{
public:

	MainFrame();
	~MainFrame();

public:

	LPCTSTR GetWindowClassName() const;	
	virtual void OnFinalMessage(HWND hWnd);
	virtual void InitWindow();
	virtual LRESULT ResponseDefaultKeyEvent(WPARAM wParam);
	virtual CDuiString GetSkinFile();
	virtual CDuiString GetSkinFolder();
	virtual UILIB_RESOURCETYPE GetResourceType() const;
	virtual CControlUI* CreateControl(LPCTSTR pstrClass);
	virtual LRESULT OnSysCommand(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	virtual LRESULT HandleCustomMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	virtual LRESULT OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	virtual LPCTSTR GetResourceID() const;

    DWORD GetBkColor();
    void SetBkColor(DWORD dwBackColor);
    void SetBkImge(int nIndex);

    void SetBkAlpha(int nOpacity)
    {
        m_PaintManager.SetTransparent(nOpacity);
    }

protected:	

	void Notify(TNotifyUI& msg);
	void OnPrepare(TNotifyUI& msg);
	void OnExit(TNotifyUI& msg);
	void OnTimer(TNotifyUI& msg);

private:
    void ShowGene();

private:
	int bk_image_index_;

    bool m_bShowGene;

	SkinChangedObserver skin_changed_observer_;
};

#endif // MAINFRAME_HPP