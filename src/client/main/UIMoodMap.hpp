#ifndef UIMOODMAP_HPP
#define UIMOODMAP_HPP

namespace DuiLib
{
class CMoodMapUI : public  CTileLayoutUI
{
public:
    CMoodMapUI(CPaintManagerUI& paint_manager);
    ~CMoodMapUI();

private:
    CPaintManagerUI& paint_manager_;

    CDialogBuilder m_dlgBuilder;
};

} // DuiLib

#endif // UIMICROBLOG_HPP