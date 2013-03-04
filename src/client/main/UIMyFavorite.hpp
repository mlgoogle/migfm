#ifndef UIMYFAVORITE_HPP
#define UIMYFAVORITE_HPP

namespace DuiLib
{
class CMyFavoriteUI : public  CTileLayoutUI
{
public:
    CMyFavoriteUI(CPaintManagerUI& paint_manager);
    ~CMyFavoriteUI();

private:
    CPaintManagerUI& paint_manager_;

    CDialogBuilder m_dlgBuilder;
};

} // DuiLib

#endif // UIMICROBLOG_HPP