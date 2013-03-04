#include "stdafx.h"
#include <windows.h>
#if !defined(UNDER_CE)
#include <shellapi.h>
#endif

#include "UIMusicGene.hpp"
#include "UIMoodMap.hpp"
#include "UIMyFavorite.hpp"
#include "UISelfInfo.hpp"
#include "UISimilar.hpp"
#include "main_frame.hpp"
#include "color_skin.hpp"

const TCHAR* const kTitleControlName = _T("apptitle");
const TCHAR* const kCloseButtonControlName = _T("closebtn");
const TCHAR* const kMinButtonControlName = _T("minbtn");
const TCHAR* const kMaxButtonControlName = _T("maxbtn");
const TCHAR* const kRestoreButtonControlName = _T("restorebtn");

const TCHAR* const kHeadsetButtonControlName = _T("headsetbtn");
const TCHAR* const kVolumeButtonControlName = _T("volumebtn");
const TCHAR* const kFavoriteButtonControlName = _T("favoritebtn");
const TCHAR* const kDeleteButtonControlName = _T("deletebtn");
const TCHAR* const kOptionButtonControlName = _T("optionbtn");
const TCHAR* const kMinBottomButtonControlName = _T("bt_minbottom");

const TCHAR* const kTabControlName = _T("tabs");

const TCHAR* const kMusicGeneButtonControlName = _T("musicgenebtn");
const TCHAR* const kMoodMapButtonControlName = _T("moodmapbtn");
const TCHAR* const kMyFavoriteButtonControlName = _T("myfavoritebtn");
const TCHAR* const kSimilarButtonControlName = _T("similarbtn");
const TCHAR* const kSelfInfoButtonControlName = _T("selfinfobtn");

const TCHAR* const kMusicGeneListControlName = _T("musicgene");
const TCHAR* const kMoodMapListControlName = _T("moodmap");
const TCHAR* const kMyFavoriteListControlName = _T("myfavorite");
const TCHAR* const kSimilarListControlName = _T("similar");
const TCHAR* const kSelfInfoListControlName = _T("selfinfo");

const TCHAR* const kChangeBkSkinControlName = _T("bkskinbtn");
const TCHAR* const kChangeColorSkinControlName = _T("colorskinbtn");

const TCHAR* const kBackgroundControlName = _T("bg");

const int kBackgroundSkinImageCount = 7;

const int MAIN_WINDOW_WIDTH = 470;
const int MAIN_WINDOW_MIN_HEIGHT = 172;
const int MAIN_WINDOW_MAX_HEIGHT = 550;

MainFrame::MainFrame()
: bk_image_index_(0)
, m_bShowGene(true)
{}

MainFrame::~MainFrame()
{
	PostQuitMessage(0);
}

LPCTSTR MainFrame::GetWindowClassName() const
{
	return _T("TXGuiFoundation");
}

CControlUI* MainFrame::CreateControl(LPCTSTR pstrClass)
{
	if (_tcsicmp(pstrClass, _T("MusicGene")) == 0)
    {
        return new CMusicGeneUI(m_PaintManager);
	}
	else if (_tcsicmp(pstrClass, _T("MoodMap")) == 0)
	{
        return new CMoodMapUI(m_PaintManager);
	}
	else if (_tcsicmp(pstrClass, _T("MyFavorite")) == 0)
	{
        return new CMyFavoriteUI(m_PaintManager);
	}
    else if (_tcsicmp(pstrClass, _T("Similar")) == 0)
    {
        return new CSimilarUI(m_PaintManager);
    }
    else if (_tcsicmp(pstrClass, _T("SelfInfo")) == 0)
    {
        return new CSelfInfoUI(m_PaintManager);
    }

	return NULL;
}

void MainFrame::OnFinalMessage(HWND hWnd)
{
	WindowImplBase::OnFinalMessage(hWnd);
	delete this;
}

CDuiString MainFrame::GetSkinFile()
{
	return _T("main_frame.xml");
}

CDuiString MainFrame::GetSkinFolder()
{
	return  _T("skin\\");
}

LRESULT MainFrame::OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = FALSE;
	return 0;
}

LRESULT MainFrame::OnSysCommand(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
#if defined(WIN32) && !defined(UNDER_CE)
	BOOL bZoomed = ::IsZoomed(m_hWnd);
	LRESULT lRes = CWindowWnd::HandleMessage(uMsg, wParam, lParam);
	if (::IsZoomed(m_hWnd) != bZoomed)
	{
		if (!bZoomed)
		{
			CControlUI* pControl = static_cast<CControlUI*>(m_PaintManager.FindControl(kMaxButtonControlName));
			if( pControl ) pControl->SetVisible(false);
			pControl = static_cast<CControlUI*>(m_PaintManager.FindControl(kRestoreButtonControlName));
			if( pControl ) pControl->SetVisible(true);
		}
		else 
		{
			CControlUI* pControl = static_cast<CControlUI*>(m_PaintManager.FindControl(kMaxButtonControlName));
			if( pControl ) pControl->SetVisible(true);
			pControl = static_cast<CControlUI*>(m_PaintManager.FindControl(kRestoreButtonControlName));
			if( pControl ) pControl->SetVisible(false);
		}
	}
#else
	return __super::OnSysCommand(uMsg, wParam, lParam, bHandled);
#endif

	return 0;
}

LRESULT MainFrame::ResponseDefaultKeyEvent(WPARAM wParam)
{
	if (wParam == VK_RETURN)
	{
		return FALSE;
	}
	else if (wParam == VK_ESCAPE)
	{
		return TRUE;
	}
	return FALSE;
}

void MainFrame::OnTimer(TNotifyUI& msg)
{
}

void MainFrame::OnExit(TNotifyUI& msg)
{
	Close();
}

void MainFrame::InitWindow()
{}

DWORD MainFrame::GetBkColor()
{
	CControlUI* background = m_PaintManager.FindControl(kBackgroundControlName);
	if (background != NULL)
		return background->GetBkColor();

	return 0;
}

void MainFrame::SetBkColor(DWORD dwBackColor)
{
	CControlUI* background = m_PaintManager.FindControl(kBackgroundControlName);
	if (background != NULL)
	{
		background->SetBkImage(_T(""));
		background->SetBkColor(dwBackColor);
		background->NeedUpdate();

		SkinChangedParam param;
		param.bkcolor = background->GetBkColor();
		param.bgimage = background->GetBkImage();
		skin_changed_observer_.Broadcast(param);
	}
}

void MainFrame::SetBkImge(int nIndex)
{
    CControlUI* background = m_PaintManager.FindControl(kBackgroundControlName);
    if (background != NULL)
    {
        TCHAR szBuf[MAX_PATH] = {0};
        if (kBackgroundSkinImageCount < nIndex)
            nIndex = 0;

#if defined(UNDER_WINCE)
        _stprintf(szBuf, _T("file='bg%d.png' corner='600,200,1,1'"), nIndex);
#else
        _stprintf_s(szBuf, MAX_PATH - 1, _T("file='bg%d.png' corner='600,200,1,1'"), nIndex);
#endif
        background->SetBkImage(szBuf);

        SkinChangedParam param;
        CControlUI* background = m_PaintManager.FindControl(kBackgroundControlName);
        if (background != NULL)
        {
            param.bkcolor = background->GetBkColor();
            if (_tcslen(background->GetBkImage()) > 0)
            {
#if defined(UNDER_WINCE)
                _stprintf(szBuf, _T("bg%d.png"), nIndex);
#else
                _stprintf_s(szBuf, MAX_PATH - 1, _T("bg%d.png"), nIndex);
#endif
            }

            param.bgimage = szBuf;
        }
        skin_changed_observer_.Broadcast(param);
    }
}

void MainFrame::OnPrepare(TNotifyUI& msg)
{
	CControlUI* background = m_PaintManager.FindControl(kBackgroundControlName);
	if (background != NULL)
	{
		TCHAR szBuf[MAX_PATH] = {0};
#if defined(UNDER_WINCE)
		_stprintf(szBuf, _T("file='bg%d.png' corner='600,200,1,1'"), bk_image_index_);
#else
		_stprintf_s(szBuf, MAX_PATH - 1, _T("file='bg%d.png' corner='600,200,1,1'"), bk_image_index_);
#endif
		background->SetBkImage(szBuf);
	}
}

void MainFrame::Notify(TNotifyUI& msg)
{
	if (_tcsicmp(msg.sType, _T("windowinit")) == 0)
	{
		OnPrepare(msg);
	}
	else if (_tcsicmp(msg.sType, _T("killfocus")) == 0)
	{
		/*if (_tcsicmp(msg.pSender->GetName(), kSearchEditControlName) == 0)
		{
			msg.pSender->SetVisible(false);
			CControlUI* search_tip = static_cast<CRichEditUI*>(m_PaintManager.FindControl(kSearchEditTipControlName));
			if (search_tip != NULL)
			{
				CRichEditUI* search_edit = static_cast<CRichEditUI*>(msg.pSender);
				if (search_edit != NULL)
					search_tip->SetText(search_edit->GetText());
				search_tip->SetVisible(true);
			}
		}*/
	}
	else if (_tcsicmp(msg.sType, _T("click")) == 0)
	{
		if (_tcsicmp(msg.pSender->GetName(), kCloseButtonControlName) == 0)
		{
			OnExit(msg);
		}
		else if (_tcsicmp(msg.pSender->GetName(), kMinButtonControlName) == 0)
		{
#if defined(UNDER_CE)
			::ShowWindow(m_hWnd, SW_MINIMIZE);
#else
			SendMessage(WM_SYSCOMMAND, SC_MINIMIZE, 0);
#endif
		}
		else if (_tcsicmp(msg.pSender->GetName(), kMaxButtonControlName) == 0)
		{
#if defined(UNDER_CE)
			::ShowWindow(m_hWnd, SW_MAXIMIZE);
			CControlUI* pControl = static_cast<CControlUI*>(m_PaintManager.FindControl(kMaxButtonControlName));
			if( pControl ) pControl->SetVisible(false);
			pControl = static_cast<CControlUI*>(m_PaintManager.FindControl(kRestoreButtonControlName));
			if( pControl ) pControl->SetVisible(true);
#else
			SendMessage(WM_SYSCOMMAND, SC_MAXIMIZE, 0);
#endif
		}
		else if (_tcsicmp(msg.pSender->GetName(), kRestoreButtonControlName) == 0)
		{
#if defined(UNDER_CE)
			::ShowWindow(m_hWnd, SW_RESTORE);
			CControlUI* pControl = static_cast<CControlUI*>(m_PaintManager.FindControl(kMaxButtonControlName));
			if( pControl ) pControl->SetVisible(true);
			pControl = static_cast<CControlUI*>(m_PaintManager.FindControl(kRestoreButtonControlName));
			if( pControl ) pControl->SetVisible(false);
#else
			SendMessage(WM_SYSCOMMAND, SC_RESTORE, 0);
#endif
		}
        else if ((_tcsicmp(msg.pSender->GetName(), kOptionButtonControlName) == 0)
            || (_tcsicmp(msg.pSender->GetName(), kMinBottomButtonControlName) == 0))
        {
            ShowGene();
        }
		/*else if (_tcsicmp(msg.pSender->GetName(), kSearchEditTipControlName) == 0)
		{
			msg.pSender->SetVisible(false);
			CRichEditUI* search_edit = static_cast<CRichEditUI*>(m_PaintManager.FindControl(kSearchEditControlName));
			if (search_edit != NULL)
			{
				search_edit->SetText(msg.pSender->GetText());
				search_edit->SetVisible(true);
			}
		}*/
		else if (_tcsicmp(msg.pSender->GetName(), kChangeBkSkinControlName) == 0)
		{
			CControlUI* background = m_PaintManager.FindControl(kBackgroundControlName);
			if (background != NULL)
			{
				TCHAR szBuf[MAX_PATH] = {0};
				++bk_image_index_;
				if (kBackgroundSkinImageCount < bk_image_index_)
					bk_image_index_ = 0;

#if defined(UNDER_WINCE)
				_stprintf(szBuf, _T("file='bg%d.png' corner='600,200,1,1'"), bk_image_index_);
#else
				_stprintf_s(szBuf, MAX_PATH - 1, _T("file='bg%d.png' corner='600,200,1,1'"), bk_image_index_);
#endif
				background->SetBkImage(szBuf);

				SkinChangedParam param;
				CControlUI* background = m_PaintManager.FindControl(kBackgroundControlName);
				if (background != NULL)
				{
					param.bkcolor = background->GetBkColor();
					if (_tcslen(background->GetBkImage()) > 0)
					{
#if defined(UNDER_WINCE)
						_stprintf(szBuf, _T("bg%d.png"), bk_image_index_);
#else
						_stprintf_s(szBuf, MAX_PATH - 1, _T("bg%d.png"), bk_image_index_);
#endif
					}

					param.bgimage = szBuf;
				}
				skin_changed_observer_.Broadcast(param);
			}
		}
		else if (_tcsicmp(msg.pSender->GetName(), kChangeColorSkinControlName) == 0)
		{
			CDuiRect rcWindow;
			GetWindowRect(m_hWnd, &rcWindow);
			rcWindow.top = rcWindow.top + msg.pSender->GetPos().bottom;
			new ColorSkinWindow(this, rcWindow);
		}
	}
	else if (_tcsicmp(msg.sType, _T("timer")) == 0)
	{
		return OnTimer(msg);
	}
	else if (_tcsicmp(msg.sType, _T("selectchanged")) == 0)
	{
		CTabLayoutUI* pTabControl = static_cast<CTabLayoutUI*>(m_PaintManager.FindControl(kTabControlName));
		if (_tcsicmp(msg.pSender->GetName(), kMusicGeneButtonControlName) == 0)
		{
			if (pTabControl && pTabControl->GetCurSel() != 0)
			{
				pTabControl->SelectItem(0);
				//UpdateFriendsList();
			}
		}
		else if (_tcsicmp(msg.pSender->GetName(), kMoodMapButtonControlName) == 0)
		{
			if (pTabControl && pTabControl->GetCurSel() != 1)
			{
				pTabControl->SelectItem(1);
				//UpdateGroupsList();
			}
		}
		else if (_tcsicmp(msg.pSender->GetName(), kMyFavoriteButtonControlName) == 0)
		{
			if (pTabControl && pTabControl->GetCurSel() != 2)
			{
				pTabControl->SelectItem(2);
				//UpdateMicroBlogList();
			}
        }
        else if (_tcsicmp(msg.pSender->GetName(), kSimilarButtonControlName) == 0)
        {
            if (pTabControl && pTabControl->GetCurSel() != 3)
            {
                pTabControl->SelectItem(3);
                //UpdateMusicGene();
            }
        }
        else if (_tcsicmp(msg.pSender->GetName(), kSelfInfoButtonControlName) == 0)
        {
            if (pTabControl && pTabControl->GetCurSel() != 4)
            {
                pTabControl->SelectItem(4);
                //UpdateMicroBlogList();
            }
        }
	}
}

LRESULT MainFrame::HandleCustomMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	return 0;
}

UILIB_RESOURCETYPE MainFrame::GetResourceType() const
{
	return UILIB_ZIPRESOURCE;
}

LPCTSTR MainFrame::GetResourceID() const
{
	return MAKEINTRESOURCE(101);
}

void MainFrame::ShowGene()
{
    if (m_bShowGene)
    {
        SetWindowPos(m_hWnd,NULL,0,0,MAIN_WINDOW_WIDTH,MAIN_WINDOW_MIN_HEIGHT,SWP_NOMOVE);
    }
    else
    {
        SetWindowPos(m_hWnd,NULL,0,0,MAIN_WINDOW_WIDTH,MAIN_WINDOW_MAX_HEIGHT,SWP_NOMOVE);
    }
    m_bShowGene = !m_bShowGene;
}