//---------------------------------------------------------------------------
#ifndef import_formH
#define import_formH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
//---------------------------------------------------------------------------
class TImportForm : public TForm
{
__published:	// IDE-managed Components
  TLabel *Label1;
private:	// User declarations
public:		// User declarations
  __fastcall TImportForm(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TImportForm *ImportForm;
//---------------------------------------------------------------------------
#endif
