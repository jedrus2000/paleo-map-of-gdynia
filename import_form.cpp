//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include "import_form.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TImportForm *ImportForm;
//---------------------------------------------------------------------------
__fastcall TImportForm::TImportForm(TComponent* Owner)
  : TForm(Owner)
{
}
//---------------------------------------------------------------------------
