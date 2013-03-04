#ifndef UISELFINFO_HPP
#define UISELFINFO_HPP

namespace DuiLib
{
class CSelfInfoUI : public  CTileLayoutUI
{
public:
    CSelfInfoUI(CPaintManagerUI& paint_manager);
    ~CSelfInfoUI();

private:
    CPaintManagerUI& paint_manager_;

    CDialogBuilder m_dlgBuilder;
};

} // DuiLib

#endif // UIMICROBLOG_HPP