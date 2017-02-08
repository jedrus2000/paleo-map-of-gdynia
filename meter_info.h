//---------------------------------------------------------------------------
#ifndef meter_infoH
#define meter_infoH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
//---------------------------------------------------------------------------
class TMeterInfo : public TForm
{
__published:	// IDE-managed Components
  TStaticText *Value;
private:	// User declarations
public:		// User declarations
  __fastcall TMeterInfo(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TMeterInfo *MeterInfo;
//---------------------------------------------------------------------------
#endif
