//---------------------------------------------------------------------------
#ifndef mainH
#define mainH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ComCtrls.hpp>
#include <Menus.hpp>
#include <ToolWin.hpp>
#include <ExtCtrls.hpp>
#include <ImgList.hpp>
#include <Messages.hpp>
#include "toolbox.h"


//---------------------------------------------------------------------------
class TMainForm : public TForm
{
__published:	// IDE-managed Components
  TMainMenu *MainMenu1;
  TMenuItem *About;
  TStatusBar *StatusBar1;
  TToolBar *ToolBar1;
  TToolButton *ZoomButton;
  TToolButton *ZoomOutButton;
  TToolButton *MeterButton;
  TImageList *ImageList1;
  TToolButton *ZoomInButton;
  TMenuItem *Wyj1;
  TToolButton *ObjInfoButton;
  TToolButton *ToolButton1;
  TToolButton *StreetsListButton;
        TScrollBar *HorzScrollBar;
        TScrollBar *VertScrollBar;
        TPanel *Panel1;
        TPaintBox *DrawPanel;
        TStaticText *MeterTest;
  /*
  void __fastcall DrawPanelInit(TObject *Sender);
  void __fastcall DrawPanelPaint(TObject *Sender);
  void __fastcall DrawPanelResize(TObject *Sender);
  */
  void __fastcall ZoomButtonClick(TObject *Sender);
  void __fastcall DrawPanelMouseDown(TObject *Sender,
          TMouseButton Button, TShiftState Shift, int X, int Y);
  void __fastcall DrawPanelMouseMove(TObject *Sender, TShiftState Shift,
          int X, int Y);
  void __fastcall ZoomOutButtonClick(TObject *Sender);
  void __fastcall FormCreate(TObject *Sender);
  void __fastcall MeterButtonClick(TObject *Sender);
  void __fastcall FormShow(TObject *Sender);
  void __fastcall Panel1Resize(TObject *Sender);
  void __fastcall DrawPanelPaint(TObject *Sender);
  void __fastcall FormCloseQuery(TObject *Sender, bool &CanClose);
  void __fastcall AboutClick(TObject *Sender);
  void __fastcall ZoomInButtonClick(TObject *Sender);
  void __fastcall ObjInfoButtonClick(TObject *Sender);
  void __fastcall StreetsListButtonClick(TObject *Sender);
        void __fastcall DrawPanelMouseUp(TObject *Sender,
          TMouseButton Button, TShiftState Shift, int X, int Y);
  void __fastcall VertScrollBarChange(TObject *Sender);
/* AB_TODO
  void __fastcall phScrollBox1Change(TObject *Sender,
          TphScrollBarKind Kind);
*/
private:	// User declarations
public:		// User declarations
  __fastcall TMainForm(TComponent* Owner);
  __fastcall UpdateStatusBar();
  __fastcall TransWinToObject(int x, int y, TPoint &point);
  // void __fastcall glWindowPos4f(GLfloat x,GLfloat y,GLfloat z,GLfloat w);
  // void __fastcall glWindowPos2f(GLfloat x, GLfloat y);
//  TList *__fastcall JoinPolygons(TList *polygons);
  void __fastcall BufferPaint();
  __fastcall SetViewArea(TRect v_rect);
  __fastcall DrawFocusRect();
  __fastcall DrawCross(TPoint p);
  __fastcall DrawMeterLine(TPoint p_start, TPoint p_end, bool show_info, bool rop);
  __fastcall SetScale();
  __fastcall ZoomToWindow();
  __fastcall ZoomOut();
  __fastcall ZoomIn();
  __fastcall SelectDrawingObjects();
  __fastcall SetBars();
  __fastcall SetThumSize(int value, TScrollBar *sb);  
  TCityObj *__fastcall FindObjectAt(int X, int Y);
  void __fastcall IdleHandler(TObject *Sender, bool &Done);
  __fastcall GetAppVersion();
  __fastcall ReportUnique();
protected:
  void __fastcall umHidedOIForm(TMessage &message);
BEGIN_MESSAGE_MAP
  MESSAGE_HANDLER(UM_OIFORMHIDE, TMessage, umHidedOIForm)
END_MESSAGE_MAP(TForm)
};
//---------------------------------------------------------------------------
extern PACKAGE TMainForm *MainForm;
//---------------------------------------------------------------------------
#endif
