#ifndef UIMUSICGENE_HPP
#define UIMUSICGENE_HPP

namespace DuiLib
{
class CMusicGeneUI : public  CTileLayoutUI
{
public:
    CMusicGeneUI(CPaintManagerUI& paint_manager);
    ~CMusicGeneUI();

private:
    CPaintManagerUI& paint_manager_;

    CDialogBuilder m_dlgBuilder;
};

} // DuiLib

#endif // UIMICROBLOG_HPP