#include "stdafx.h"
#include "UIMyFavorite.hpp"

namespace DuiLib
{

CMyFavoriteUI::CMyFavoriteUI(CPaintManagerUI& paint_manager)
: paint_manager_(paint_manager)
{
    SetItemSize(CSize(466, 320));
    CContainerUI* pRoot = static_cast< CContainerUI*>(m_dlgBuilder.Create(_T("music_gene.xml"), (UINT)0, NULL, &paint_manager_));
    if( pRoot == NULL ) 
    {
        pRoot = static_cast<CContainerUI*>(m_dlgBuilder.Create());
    }
    if( pRoot != NULL ) 
    {
        this->Add(pRoot);
    }
}

CMyFavoriteUI::~CMyFavoriteUI()
{}

} // namespace DuiLib