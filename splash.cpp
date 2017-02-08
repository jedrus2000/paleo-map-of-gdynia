//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include "splash.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TSplashForm *SplashForm;

void __fastcall ShowSplashScreen(TComponent* Owner, bool OKButton)
{
    SplashForm = new TSplashForm(Owner);
    SplashForm->BitBtn1->Visible = OKButton;
    if (OKButton)
      SplashForm->ShowModal();
      else
        SplashForm->Show();
}

void __fastcall DestroySplashScreen()
{
    if (SplashForm) SplashForm->Close();
}

//---------------------------------------------------------------------------
__fastcall TSplashForm::TSplashForm(TComponent* Owner)
  : TForm(Owner)
{
}
//---------------------------------------------------------------------------
