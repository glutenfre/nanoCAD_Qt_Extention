#include "stdafx.h"

#include "hostUI.h"
#include "hostQt.h"
#include "HelloQtChild.h"

hostUiPaletteSet* m_pPalSet = NULL;

HINSTANCE _hdllInstance =NULL ;
AC_IMPLEMENT_EXTENSION_MODULE(theArxDLL);

class helloQtPalette : public hostQtPalette
{
  DECLARE_DYNAMIC(helloQtPalette)

public:
  helloQtPalette() {};

  afx_msg void    OnSize(UINT nType, int cx, int cy)
  {
    if (m_pWinWidget)
    {
      HWND wnd = (HWND)m_pWinWidget->windowHandle()->winId();
      ::SetWindowPos(wnd, nullptr, 0, 0, cx, cy, SWP_NOZORDER);
    }
  }

  DECLARE_MESSAGE_MAP();
};

BEGIN_MESSAGE_MAP(helloQtPalette, hostQtPalette)
  //{{AFX_MSG_MAP(helloQtPalette)
  ON_WM_SIZE()
  //}}AFX_MSG_MAP
END_MESSAGE_MAP()

IMPLEMENT_DYNAMIC(helloQtPalette, hostQtPalette);

void helloQtPaletteCmd()
{
    if (!m_pPalSet)
    {
        CAcModuleResourceOverride ThisRes;
        m_pPalSet = new hostUiPaletteSet();
        m_pPalSet->Create(L"Polyline coordinates",
            WS_CHILD | WS_DLGFRAME | WS_VISIBLE,
            CRect(30, 50, 250, 300),
            CWnd::FromHandle(adsw_acadMainWnd()),
            PSS_SNAP | PSS_PROPERTIES_MENU | PSS_AUTO_ROLLUP | PSS_CLOSE_BUTTON);
        m_pPalSet->EnableDocking(CBRS_ALIGN_ANY);
        m_pPalSet->RestoreControlBar();

        helloQtPalette* pPal = new helloQtPalette();
        pPal->Create(WS_CHILD | WS_VISIBLE,
            L"Palette",
            m_pPalSet,
            0);
        m_pPalSet->AddPalette(pPal);

        QWidget* pPaletteWidget1 = pPal->paletteWidget();

        HelloQtChild* pWidgetsClass = new HelloQtChild(pPaletteWidget1);

        QVBoxLayout* vbox = new QVBoxLayout(pPaletteWidget1);
        vbox->setSpacing(0);
        vbox->setMargin(0);
        vbox->addWidget(pWidgetsClass);
        vbox->addStretch();

        //WId winId = le3->winId(); // Make Qt windows real HWND windows

        pPaletteWidget1->setLayout(vbox);
        pPaletteWidget1->show();

        CRect cr;
        m_pPalSet->GetClientRect(&cr);
        pPal->OnSize(0, cr.Width(), cr.Height()); // Force to resize palette widget, needed when system scale !=100%
    }
    else
    {
        m_pPalSet->Show(!m_pPalSet->IsWindowVisible());
    }
}

void initApp()
{

  acedRegCmds->addCommand(L"HELLOQT_GROUP",
                          L"_HELLOQTPALETTE",
                          L"HELLOQTPALETTE",
                          ACRX_CMD_TRANSPARENT,
                          helloQtPaletteCmd);
}

void uninitApp()
{
  acedRegCmds->removeGroup(L"HELLOQT_GROUP");

  if (m_pPalSet)
  {
    m_pPalSet->DestroyWindow();
    m_pPalSet = 0;
  }
}

extern "C" __declspec(dllexport) AcRx::AppRetCode
acrxEntryPoint(AcRx::AppMsgCode msg, void* appId) 
{
  switch (msg) 
  {
  case AcRx::kInitAppMsg:
    acrxDynamicLinker->unlockApplication(appId);
    acrxDynamicLinker->registerAppMDIAware(appId);
    initApp();
    helloQtPaletteCmd();
    break;

  case AcRx::kUnloadAppMsg:
    uninitApp();
    break;
  }

  return AcRx::kRetOK;
}