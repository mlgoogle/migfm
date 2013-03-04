#ifndef UISIMILAR_HPP
#define UISIMILAR_HPP

namespace DuiLib
{
class CSimilarUI : public  CTileLayoutUI
{
public:
    CSimilarUI(CPaintManagerUI& paint_manager);
    ~CSimilarUI();

private:
    CPaintManagerUI& paint_manager_;

    CDialogBuilder m_dlgBuilder;
};

} // DuiLib

#endif // UIMICROBLOG_HPP