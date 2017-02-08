//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop
USERES("mapa_gdyni.res");
USEFORM("main.cpp", MainForm);
USEUNIT("toolbox.cpp");
USEFORM("object_info.cpp", OIForm);
USEUNIT("gfx_tools.cpp");
USEFORM("meter_info.cpp", MeterInfo);
USEFORM("splash.cpp", SplashForm);
USEFORM("import_form.cpp", ImportForm);
//---------------------------------------------------------------------------
#include "splash.h"

//---------------------------------------------------------------------------
WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
  try
  {
     Application->Initialize();
     ShowSplashScreen(static_cast<void*> (NULL), false);
     Application->ProcessMessages();

     Application->Title = "Mapa Gdyni";
     Application->CreateForm(__classid(TMainForm), &MainForm);
     Application->Run();
  }
  catch (Exception &exception)
  {
     Application->ShowException(&exception);
  }
  return 0;
}
//---------------------------------------------------------------------------
