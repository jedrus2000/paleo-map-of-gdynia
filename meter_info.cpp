//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include "meter_info.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TMeterInfo *MeterInfo;
//---------------------------------------------------------------------------
__fastcall TMeterInfo::TMeterInfo(TComponent* Owner)
  : TForm(Owner)
{
}
//---------------------------------------------------------------------------
