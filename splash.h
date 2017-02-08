//---------------------------------------------------------------------------
#ifndef splashH
#define splashH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
#include <Graphics.hpp>
#include <jpeg.hpp>
#include <Buttons.hpp>
//---------------------------------------------------------------------------
class TSplashForm : public TForm
{
__published:	// IDE-managed Components
  TImage *Image1;
  TImage *Image2;
  TStaticText *StaticText1;
  TStaticText *StaticText2;
  TStaticText *StaticText3;
  TStaticText *StaticText4;
  TBitBtn *BitBtn1;
private:	// User declarations
public:		// User declarations
  __fastcall TSplashForm(TComponent* Owner);
};

void __fastcall ShowSplashScreen(TComponent* Owner, bool OKButton);
void __fastcall DestroySplashScreen();

//---------------------------------------------------------------------------
extern PACKAGE TSplashForm *SplashForm;
//---------------------------------------------------------------------------
#endif
