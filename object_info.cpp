//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include "object_info.h"
#include "toolbox.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TOIForm *OIForm;

//---------------------------------------------------------------------------
__fastcall TOIForm::TOIForm(TComponent* Owner)
  : TForm(Owner)
{
}
//---------------------------------------------------------------------------

void __fastcall TOIForm::FormHide(TObject *Sender)
{
  if (((TForm*) this->Owner)->Handle)
    SendMessage(((TForm*) this->Owner)->Handle, UM_OIFORMHIDE, 0, 0);
}
//---------------------------------------------------------------------------

