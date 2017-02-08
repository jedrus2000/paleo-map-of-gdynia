//---------------------------------------------------------------------------
#ifndef object_infoH
#define object_infoH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Grids.hpp>
#include <ComCtrls.hpp>
//---------------------------------------------------------------------------
class TOIForm : public TForm
{
__published:	// IDE-managed Components
  THeaderControl *IDHeader;
  THeaderControl *ClassIDHeader;
  THeaderControl *TextHeader;
  THeaderControl *ColorHeader;
  void __fastcall FormHide(TObject *Sender);
private:	// User declarations
public:		// User declarations
  __fastcall TOIForm(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TOIForm *OIForm;
//---------------------------------------------------------------------------
#endif
