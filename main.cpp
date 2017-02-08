//---------------------------------------------------------------------------
#include <vcl.h>
#include <math.h>
#include <winver.h>
#pragma hdrstop

#include "main.h"
#include "splash.h"
#include "toolbox.h"
#include "object_info.h"
#include "meter_info.h"
#include "gfx_tools.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"

int sdiv = 1;

TMainForm *MainForm;
TList *CityObjects;
TList *DrawObjects;
TStringList *streets_list;

TPoint meter_p_start;
TPoint meter_p_end;
float meter_length = 0;

TViewPort ViewPort;

Graphics::TBitmap *primary_buffer;

TOpID op_id; // curent operation id
TRect zoom_rect;

bool closing_form = false;
bool update_needed = false;

float zoom_out_v = 0.1;
float zoom_in_v = 10.0;

struct SVS_VERSIONINFO {
    WORD  wLength;
    WORD  wValueLength;
    WORD  wType;
    WCHAR szKey[15];
    WORD  Padding1;
    VS_FIXEDFILEINFO Value;
    WORD  Padding2[];
    WORD  Children[];
};

__fastcall TMainForm::GetAppVersion()
{
  HRSRC hRSC = FindResource(NULL, "#1", RT_VERSION);
  DWORD dwResSize;
  HGLOBAL hGLOBAL;
  SVS_VERSIONINFO *ver_info;
  if (hRSC)
  {
    dwResSize = SizeofResource(NULL, hRSC);
    hGLOBAL = LoadResource(NULL, hRSC);
    if (hGLOBAL)
    {
      ver_info = (SVS_VERSIONINFO*) LockResource(hGLOBAL);
    }
  }
}

__fastcall TMainForm::ReportUnique()
{
  TStringList *ids = new TStringList();
  ids->Sorted = true;
  TStringList *colors = new TStringList();
  colors->Sorted = true;
  TStringList *classes = new TStringList();
  classes->Sorted = true;
  TStringList *types = new TStringList();
  types->Sorted = true;

  for (int i=0;i<CityObjects->Count;i++)
  {
    TCityObj *city_obj = ((TCityObj*) CityObjects->Items[i]);
    int idx;
    char txt[1024];

    sprintf(txt, "%07d",city_obj->id);
    if (!(ids->Find(txt, idx)))
      ids->Add(txt);
    sprintf(txt, "%x",city_obj->color);
    if (!(colors->Find(txt, idx)))
      colors->Add(txt);
    sprintf(txt, "%07d",city_obj->class_id);
    if (!(classes->Find(txt, idx)))
      classes->Add(txt);
    sprintf(txt, "%07d",city_obj->type);
    if (!(types->Find(txt, idx)))
      types->Add(txt);
  }

  FILE *stream;

  stream = fopen(".\\types.txt", "wt");
  if (stream)
  {
    for (int i=0;i<types->Count;i++)
    {
      fputs(types->Strings[i].c_str(), stream);
      fputs("\n", stream);
    }
    fclose(stream);
  }

  stream = fopen(".\\ids.txt", "wt");
  if (stream)
  {
    for (int i=0;i<ids->Count;i++)
    {
      fputs(ids->Strings[i].c_str(), stream);
      fputs("\n", stream);
    }
    fclose(stream);
  }

  stream = fopen(".\\classes.txt", "wt");
  if (stream)
  {
    for (int i=0;i<classes->Count;i++)
    {
      fputs(classes->Strings[i].c_str(), stream);
      fputs("\n", stream);
    }
    fclose(stream);
  }

  stream = fopen(".\\colors.txt", "wt");
  if (stream)
  {
    for (int i=0;i<colors->Count;i++)
    {
      fputs(colors->Strings[i].c_str(), stream);
      fputs("\n", stream);
    }
    fclose(stream);
  }

  delete ids;
  delete colors;
  delete classes;
  delete types;
}

__fastcall TMainForm::SelectDrawingObjects()
{
  if ((DrawObjects) && (DrawObjects != CityObjects))
    delete DrawObjects;

  if (((REAL_WORLD_RECT.Width()/ViewPort.real_rect.Width())<2) ||
      ((REAL_WORLD_RECT.Height()/ViewPort.real_rect.Height()<2)))
  {
    DrawObjects = CityObjects;
    return 0;
  }

  DrawObjects = new TList();
  for (int i=0;i<CityObjects->Count;i++)
  {
    // Application->HandleMessage();

    TCityObj *city_obj = ((TCityObj*) CityObjects->Items[i]);
    bool found = false;
    /*
    for (int j=0; j<city_obj->polygons->Count;j++)
    {
      TList *point_list =  ((TList*) city_obj->polygons->Items[j]);
      int points_no = point_list->Count;
      for (int k=0; k<points_no; k++)
      {
        TPoint point = *((TPoint*) point_list->Items[k]);
        if (((point.x >= ViewPort.real_rect.Left) &&
             (point.x <= ViewPort.real_rect.Right)) &&
            ((point.y >= ViewPort.real_rect.Top) &&
             (point.y <= ViewPort.real_rect.Bottom)))
        {
          found = true;
          break;
        }
      }
      if (found) break;
    }
    */
    // TRect checkr;
    // GetClipBox(dc, &checkr);


    // RECT rect_visble = city_obj->bound;

    /*
    rect_visble.left = (rect_visble.left-ViewPort.real_rect.Left)/ViewPort.scale;
    rect_visble.right = (rect_visble.right-ViewPort.real_rect.Left)/ViewPort.scale;
    rect_visble.top = (rect_visble.top-ViewPort.real_rect.Top)/ViewPort.scale;
    rect_visble.bottom = (rect_visble.bottom-ViewPort.real_rect.Top)/ViewPort.scale;
    */

    TRect check_rect;
    IntersectRect(&check_rect, &ViewPort.real_rect, &city_obj->bound);

    if ((check_rect.Width() != 0) && (check_rect.Width() != 0))
      found = true; 
    if (found)
      DrawObjects->Add(city_obj);
  }
  // DeleteObject(region);
  // SelectClipRgn(dc,NULL);
  // ReleaseDC(Panel1->Handle, dc);
}

__fastcall TMainForm::UpdateStatusBar()
{
  // update info about scale
  AnsiString units;
  float mpp = ViewPort.scale/PPM;
  // int dn = ViewPort.scale/100.0;
  if (mpp<1.0)
  {
    if ((mpp *= 100.0) > 1.0)
    {
      units = " cm";
    }
    else
    {
    mpp *=100.0;
    units = " mm";
    }
  }
  else
  if (mpp>10000.0)
  {
    mpp /= 1000.0;
    units = " km";
  }
  else
    units = " m";

  ((TStatusPanel*) this->StatusBar1->Panels->Items[0])->Text =
    "Skala : 1 pkt = "+AnsiString((int)mpp)+units;
}


__fastcall TMainForm::SetThumSize(int value, TScrollBar *sb)
{
  int w_thumb, w_scroll, h;

  if (sb->Kind == sbVertical)
  {
    w_thumb = SM_CYVTHUMB;
    w_scroll = SM_CYVSCROLL;
    h = sb->ClientHeight;
  }
  else
  {
    w_thumb = SM_CXHTHUMB;
    w_scroll = SM_CXHSCROLL;
    h = sb->ClientWidth;
  }

  int MinSize = GetSystemMetrics(w_thumb);
  int TrackSize = h - 2*GetSystemMetrics(w_scroll);

  if (value < MinSize)
    sb->PageSize = MinSize;
  else
    sb->PageSize = value; //
}

__fastcall TMainForm::SetBars()
{
  HorzScrollBar->LargeChange =
    (ViewPort.real_rect.Width()-1)/sdiv;

  HorzScrollBar->SmallChange =
    (ViewPort.real_rect.Width()-1)/10/sdiv;

  VertScrollBar->LargeChange =
    (ViewPort.real_rect.Height()-1)/sdiv;

  VertScrollBar->SmallChange =
    (ViewPort.real_rect.Height()-1)/10/sdiv;

  SetThumSize(ViewPort.real_rect.Height(), VertScrollBar);
  SetThumSize(ViewPort.real_rect.Width(), HorzScrollBar);

  HorzScrollBar->Position =
    ViewPort.real_rect.Left;

  VertScrollBar->Position =
    ViewPort.real_rect.Top;

  //VertScrollBar->Repaint();
  //HorzScrollBar->Repaint();
}

__fastcall TMainForm::SetScale()
{
  Screen->Cursor = crHourGlass;

  // calculate scale
  float scale_x = (float)ViewPort.real_rect.Width()/(float)DrawPanel->Width;
  float scale_y = (float)ViewPort.real_rect.Height()/(float)DrawPanel->Height;
  ViewPort.scale = (scale_x > scale_y) ? scale_x : scale_y;

  // calculate view rect according to screen resolution

  int ph = DrawPanel->Height;
  int pw = DrawPanel->Width;
  int vrh = ViewPort.real_rect.Height();
  int vrw = ViewPort.real_rect.Width();

  ViewPort.real_rect.Left   -= (((float)pw*ViewPort.scale)-(float)vrw)/2.0;
  ViewPort.real_rect.Left   = (ViewPort.real_rect.Left < REAL_WORLD_X1) ? REAL_WORLD_X1 : ViewPort.real_rect.Left;
  ViewPort.real_rect.Top    -= (((float)ph*ViewPort.scale)-(float)vrh)/2.0;
  ViewPort.real_rect.Top    = (ViewPort.real_rect.Top < REAL_WORLD_Y1) ? REAL_WORLD_Y1 : ViewPort.real_rect.Top;
  ViewPort.real_rect.Right  += (((float)pw*ViewPort.scale)-(float)vrw)/2.0;
  ViewPort.real_rect.Right  = (ViewPort.real_rect.Right > REAL_WORLD_X2) ? REAL_WORLD_X2 : ViewPort.real_rect.Right;
  ViewPort.real_rect.Bottom += (((float)ph*ViewPort.scale)-(float)vrh)/2.0;
  ViewPort.real_rect.Bottom = (ViewPort.real_rect.Bottom > REAL_WORLD_Y2) ? REAL_WORLD_Y2 : ViewPort.real_rect.Bottom;

  SelectDrawingObjects();
  UpdateStatusBar();
  SetBars();
  BufferPaint();

  Screen->Cursor = crArrow;
}

__fastcall TMainForm::SetViewArea(TRect v_rect)
{
  // calculate new real coordinates of passed screen rect
  ViewPort.real_rect.Left = ViewPort.real_rect.Left+((float)v_rect.Left*ViewPort.scale);
  if (ViewPort.real_rect.Left < REAL_WORLD_X1)
    ViewPort.real_rect.Left = REAL_WORLD_X1;
  ViewPort.real_rect.Top = ViewPort.real_rect.Top+((float)v_rect.Top*ViewPort.scale);
  if (ViewPort.real_rect.Top < REAL_WORLD_Y1)
    ViewPort.real_rect.Top = REAL_WORLD_Y1;
  ViewPort.real_rect.Right = ViewPort.real_rect.Left+((float)v_rect.Width()*ViewPort.scale);
  if (ViewPort.real_rect.Right > REAL_WORLD_X2)
    ViewPort.real_rect.Right = REAL_WORLD_X2;
  ViewPort.real_rect.Bottom = ViewPort.real_rect.Top+((float)v_rect.Height()*ViewPort.scale);
  if (ViewPort.real_rect.Bottom > REAL_WORLD_Y2)
    ViewPort.real_rect.Bottom = REAL_WORLD_Y2;
}

__fastcall TMainForm::ZoomToWindow()
{
  SetViewArea(zoom_rect);
  update_needed = true; // SetScale();
}

__fastcall TMainForm::ZoomIn()
{
  int w = DrawPanel->Width;
  int h = DrawPanel->Height;
  zoom_rect.Left = w / (int) zoom_in_v;
  zoom_rect.Top =  h / (int) zoom_in_v;;
  zoom_rect.Right = w - zoom_rect.Left;
  zoom_rect.Bottom = h - zoom_rect.Top;
  SetViewArea(zoom_rect);
  update_needed = true; // SetScale();
}

__fastcall TMainForm::ZoomOut()
{
  float x_points = (float)DrawPanel->Width * zoom_out_v;
  float y_points = (float)DrawPanel->Height * zoom_out_v;

  ViewPort.real_rect.Left = (ViewPort.real_rect.Left-(int)(ViewPort.scale*x_points));
  ViewPort.real_rect.Top = (ViewPort.real_rect.Top-(int)(ViewPort.scale*y_points));
  ViewPort.real_rect.Right = (ViewPort.real_rect.Right+(int)(ViewPort.scale*x_points));
  ViewPort.real_rect.Bottom = (ViewPort.real_rect.Bottom+(int)(ViewPort.scale*y_points));
  if (ViewPort.real_rect.Left<REAL_WORLD_X1)
    ViewPort.real_rect.Left = REAL_WORLD_X1;
  if (ViewPort.real_rect.Right>REAL_WORLD_X2)
    ViewPort.real_rect.Right = REAL_WORLD_X2;
  if (ViewPort.real_rect.Top<REAL_WORLD_Y1)
    ViewPort.real_rect.Top = REAL_WORLD_Y1;
  if (ViewPort.real_rect.Bottom>REAL_WORLD_Y2)
    ViewPort.real_rect.Bottom = REAL_WORLD_Y2;

  update_needed = true; // SetScale();
}

/*
TList *__fastcall TMainForm::JoinPolygons(TList *polygons)
{
  if (polygons->Count==1)
    return ((TList*) polygons->Items[0]);

  TList *joined_polygons = new TList();

  for (int i=0;i<polygons->Count;i++)
  {
    TList *points_list = ((TList*) polygons->Items[i]);
    for (int j=0;j<points_list->Count;j++)
    {
      joined_polygons->Add(((TPoint*) points_list->Items[j]));
    }
    points_list = ((TList*) polygons->Items[0]);
    joined_polygons->Add(((TPoint*) points_list->Items[points_list->Count-1]));
  }
  return joined_polygons;
}
*/

TCityObj *__fastcall TMainForm::FindObjectAt(int X, int Y)
{
  POINT p2check = Point(X,Y);

  TList *found_objects = new TList();

  for (int i=0;i<DrawObjects->Count;i++)
  {
    TCityObj *ret_obj = ((TCityObj*) DrawObjects->Items[i]);

    // if (ret_obj->id <= 0 ) continue;

    // if ((ret_obj->id >= 900000) && (ret_obj->id <= 1000000))
    //  continue;


#ifndef NEW_DATA // OLD DATA
    TList *polygons = ((TList*) ret_obj->polygons);
    for (int j=0; j<polygons->Count;j++)
    {
      POINT *point_list = new POINT[((TList*)polygons->Items[j])->Count];
      for (int k=0; k<((TList*)polygons->Items[j])->Count;k++)
      {
        POINT p = (POINT) *((TPoint*) ((TList*)polygons->Items[j])->Items[k]);
        point_list[k] = p;
      }

      if (InPoly_ver2(point_list,
                      ((TList*)polygons->Items[j])->Count,
                      p2check))
      {
        delete point_list;
        found_objects->Add(ret_obj);
      }
      else
        delete point_list;
    }
#else // NEW DATA
    TList *points = ((TList*) ret_obj->points);
    POINT *point_list = new POINT[points->Count];
    for (int j=0; j<points->Count;j++)
    {
        POINT p = (POINT) *((TPoint*) points->Items[j]);
        point_list[j] = p;
    }

    if (InPoly_ver2(point_list,
                    points->Count,
                    p2check))
    {
      delete point_list;
      found_objects->Add(ret_obj);
    }
    else
      delete point_list;
#endif
  }
  if (found_objects->Count<1)
  {
    delete found_objects;
    return NULL;
  }
  else
  {
    TCityObj *ret_obj = NULL;
    // 1) buildings
    for (int i=0;i<found_objects->Count;i++)
    {
      TCityObj *obj = ((TCityObj*) found_objects->Items[i]);
      if (obj->class_id == CLASS_BUILDING)
      {
        ret_obj = obj;
        delete found_objects;
        return ret_obj;
      }
    }
    // 2) main roads
    for (int i=0;i<found_objects->Count;i++)
    {
      TCityObj *obj = ((TCityObj*) found_objects->Items[i]);
      if (obj->class_id == CLASS_MAINROAD)
      {
        ret_obj = obj;
        delete found_objects;
        return ret_obj;
      }
    }
    // 3) roads
    for (int i=0;i<found_objects->Count;i++)
    {
      TCityObj *obj = ((TCityObj*) found_objects->Items[i]);
      if (obj->class_id == CLASS_ROAD)
      {
        ret_obj = obj;
        delete found_objects;
        return ret_obj;
      }
    }
    // 4) quarters
    for (int i=0;i<found_objects->Count;i++)
    {
      TCityObj *obj = ((TCityObj*) found_objects->Items[i]);
      if (obj->class_id == CLASS_QUARTER)
      {
        ret_obj = obj;
        delete found_objects;
        return ret_obj;
      }
    }
    // 5) id 0
    for (int i=0;i<found_objects->Count;i++)
    {
      TCityObj *obj = ((TCityObj*) found_objects->Items[i]);
      if ((obj->class_id == CLASS_UNKNOWN))
      {
        ret_obj = obj;
        delete found_objects;
        return ret_obj;
      }
    }
    // 6) ...desperate...first one
    ret_obj = (TCityObj*) found_objects->Items[0];
    delete found_objects;
    return ret_obj;
  }
}

__fastcall TMainForm::DrawCross(TPoint p)
{
  // draw cross
  DrawPanel->Canvas->Pen->Color = clBlack;
  DrawPanel->Canvas->Pen->Style = psSolid;
  DrawPanel->Canvas->Pen->Mode = pmNotXor;
  DrawPanel->Canvas->MoveTo(p.x, p.y);
  DrawPanel->Canvas->LineTo(p.x, p.y-5);
  DrawPanel->Canvas->MoveTo(p.x, p.y);
  DrawPanel->Canvas->LineTo(p.x, p.y+5);
  DrawPanel->Canvas->MoveTo(p.x, p.y);
  DrawPanel->Canvas->LineTo(p.x-5, p.y);
  DrawPanel->Canvas->MoveTo(p.x, p.y);
  DrawPanel->Canvas->LineTo(p.x+5, p.y);
}

__fastcall TMainForm::DrawFocusRect()
{
  DrawPanel->Canvas->DrawFocusRect(zoom_rect);
}

__fastcall TMainForm::DrawMeterLine(TPoint p_start, TPoint p_end,
                                 bool show_info, bool rop)
{
  DrawPanel->Canvas->Pen->Color = clBlack;
  DrawPanel->Canvas->Pen->Style = psSolid;
  if (rop) DrawPanel->Canvas->Pen->Mode = pmNotXor;
  DrawPanel->Canvas->MoveTo(p_start.x, p_start.y);
  DrawPanel->Canvas->LineTo(p_end.x, p_end.y);

  if (show_info)
  {
    AnsiString units;
    float length = ViewPort.scale*(
      sqrt(((float)p_end.x-(float)p_start.x)*((float)p_end.x-(float)p_start.x)
          +((float)p_end.y-(float)p_start.y)*((float)p_end.y-(float)p_start.y)
          ))/PPM;

    if (!rop)
    {
      meter_length += length;
      length = meter_length;
    }
    else
    {
      length += meter_length;
    }

    if (length<1.0)
    {
      if ((length *= 100.0) > 1.0)
      {
        units = " cm";
      }
      else
      {
        length *=100.0;
        units = " mm";
      }
    }
    else
    if (length>10000.0)
    {
      length /= 1000.0;
      units = " km";
    }
    else
      units = " m";

    MeterTest->Caption = AnsiString((int)length) + units;
    {
      int sign_x = p_end.x-p_start.x;
      int sign_y = p_end.y-p_start.y;

      if (sign_x != 0)
        sign_x /= abs(sign_x);
      else
        sign_x = 1;
      if (sign_y != 0)
        sign_y /= abs(sign_y);
      else
        sign_y = 1;

      int x1 = (p_end.x + p_start.x)/2;
      int y1 = (p_end.y + p_start.y)/2;


      MeterTest->Left = x1 + sign_x*(MeterTest->Width+12);
      MeterTest->Top  = y1 - sign_y*(MeterTest->Height+8);

    }
    if (!MeterTest->Visible)
    {
      // MeterInfo->ManualDock(this->ToolBar1, NULL, NULL);
      MeterTest->Visible = show_info;
    }
  }
    else
  {
    MeterTest->Visible = show_info;
    // MeterInfo->ManualFloat(TRect(0,0,0,0));
  }
}

//---------------------------------------------------------------------------
void __fastcall TMainForm::BufferPaint()
{
//
  if (!DrawObjects) return;

  Graphics::TBitmap *bg_buffer = new Graphics::TBitmap();
  bg_buffer->Height = DrawPanel->ClientHeight;
  bg_buffer->Width = DrawPanel->ClientWidth;

  SetPolyFillMode(bg_buffer->Canvas->Handle, ALTERNATE);

  for (int j=0; j<DrawObjects->Count;j++)
  {
    // Application->HandleMessage();

    TCityObj *city_object = ((TCityObj*) DrawObjects->Items[j]);

    // if (city_object->color != 0xffc000 ) continue;
    // if ((city_object->id < 575000) || (city_object->id > 575999))
    //  continue;

    // if (city_object->class_id != CLASS_FOREST) continue;

    TColor color = city_object->color;

    /*
    if (city_object->outline != 0)
      DrawPanel->Canvas->Pen->Color = city_object->outline;
    else
    */
    bg_buffer->Canvas->Pen->Color = clBlack;

    bg_buffer->Canvas->CopyMode = cmSrcAnd;
    bg_buffer->Canvas->Brush->Color = color;
    bg_buffer->Canvas->Brush->Style = bsSolid;

    BeginPath(bg_buffer->Canvas->Handle);

#ifndef NEW_DATA // OLD DATA
    for (int i=0; i<city_object->polygons->Count;i++)
    {
      TList *polygon;
      polygon = ((TList*) city_object->polygons->Items[i]);

      int points_no = polygon->Count;

      TPoint *points_list = new TPoint[points_no];
      bool crossing = false;
      for (int k=0; k<points_no; k++)
      {
        TPoint point = *((TPoint*) polygon->Items[k]);

        if (crossing == false)
        if ((point.x < ViewPort.real_rect.Left) ||
             (point.x > ViewPort.real_rect.Right) ||
             (point.y < ViewPort.real_rect.Top) ||
             (point.y > ViewPort.real_rect.Bottom))
        {
          crossing = true;
        }
        points_list[k] = point;
      }
      for (int k=0; k<points_no; k++)
      {
        points_list[k].x = (points_list[k].x-ViewPort.real_rect.Left)/ViewPort.scale;
        points_list[k].y = (points_list[k].y-ViewPort.real_rect.Top)/ViewPort.scale;
      }
      // here draw polygon
      if (points_no>2)
      {
        Polygon(bg_buffer->Canvas->Handle, &points_list[0], points_no);
      }
      delete points_list;
    }
#else // NEW DATA
    TList *points;
    points = city_object->points;
    int points_no = points->Count;
    TPoint *points_list = new TPoint[points_no];
    bool crossing = false;
    for (int k=0; k<points_no; k++)
    {
      TPoint point = *((TPoint*) points->Items[k]);

      if (crossing == false)
      if ((point.x < ViewPort.real_rect.Left) ||
           (point.x > ViewPort.real_rect.Right) ||
           (point.y < ViewPort.real_rect.Top) ||
           (point.y > ViewPort.real_rect.Bottom))
      {
        crossing = true;
      }
      points_list[k] = point;
    }
    for (int k=0; k<points_no; k++)
    {
      points_list[k].x = (int) ((float)(points_list[k].x-ViewPort.real_rect.Left)/ViewPort.scale);
      points_list[k].y = (int) ((float)(points_list[k].y-ViewPort.real_rect.Top)/ViewPort.scale);
    }
    // here draw polygon
    if (points_no>2)
    {
      Polygon(bg_buffer->Canvas->Handle, &points_list[0], points_no);
    }
    delete points_list;

#endif
    EndPath(bg_buffer->Canvas->Handle);

    if (
            (city_object->class_id == CLASS_BUILDING)
         || (city_object->class_id == CLASS_POLL)
       )
      StrokeAndFillPath(bg_buffer->Canvas->Handle);
    else
      // StrokePath(bg_buffer->Canvas->Handle);
      FillPath(bg_buffer->Canvas->Handle);
  }
  if (primary_buffer)
  {
    if (primary_buffer->Canvas->LockCount == 0)
    {
      delete primary_buffer;
      primary_buffer = bg_buffer;
      // BitBlt(primary_buffer->Canvas->Handle,0,0,primary_buffer->Width,primary_buffer->Height,
      //        bg_buffer->Canvas->Handle, 0,0, SRCCOPY);
      // primary_buffer->SaveToFile(".\\test.bmp");
      // delete bg_buffer;
      DrawPanel->Refresh();
    }
  }
}


//---------------------------------------------------------------------------
__fastcall TMainForm::TMainForm(TComponent* Owner)
  : TForm(Owner)
{
  this->GetAppVersion();

/* devel

    TList *objects = LoadDatFiles(".\\data_dzielnice");
//    AdjustScale(objects);
    SaveMainDataFile(objects, ".\\data\\all_data.dat");
    delete objects;
//    objects = LoadDatFiles(".\\data_dzielnice");
//    AppendQuarterData(objects, ".\\data\\all_data.dat");
//    delete objects;
*/
  // TList *objects = LoadDatFiles(".\\data");
  // SaveMainDataFile(objects, ".\\data\\all_data.dat");
  // delete objects;
  CityObjects = LoadDataFile(".\\data\\all_data.dat");
  // ReportUnique();                                       // devel
}

//---------------------------------------------------------------------------
void __fastcall TMainForm::FormCreate(TObject *Sender)
{
  this->Caption = Application->Title;

  Application->OnIdle = IdleHandler;

  OIForm = new TOIForm(this);
  // OIForm->Show();

  primary_buffer = new Graphics::TBitmap();

  ViewPort.real_rect = REAL_WORLD_RECT;

  HorzScrollBar->Max = (REAL_WORLD_X2/sdiv);
  HorzScrollBar->Min = (REAL_WORLD_X1/sdiv);

  VertScrollBar->Max = (REAL_WORLD_Y2/sdiv);
  VertScrollBar->Min = (REAL_WORLD_Y1/sdiv);

  update_needed = true; // SetScale();
}

//---------------------------------------------------------------------------
void __fastcall TMainForm::FormShow(TObject *Sender)
{
  DestroySplashScreen();
}

//---------------------------------------------------------------------------
void __fastcall TMainForm::IdleHandler(TObject *Sender, bool &Done)
{
//
  if (update_needed == true)
  {
    SetScale();
    update_needed = false;
  }
}

//---------------------------------------------------------------------------
void __fastcall TMainForm::DrawPanelPaint(TObject *Sender)
{
  if (primary_buffer)
    BitBlt(DrawPanel->Canvas->Handle,0,0,DrawPanel->ClientWidth,DrawPanel->ClientHeight,
           primary_buffer->Canvas->Handle, 0,0, SRCCOPY);
}

//---------------------------------------------------------------------------
void __fastcall TMainForm::Panel1Resize(TObject *Sender)
{
//
  if (!closing_form) update_needed = true; // SetScale();
}

//---------------------------------------------------------------------------
void __fastcall TMainForm::ZoomButtonClick(TObject *Sender)
{
  if ((op_id==OP_ZOOM_AREA_P1) || (op_id==OP_ZOOM_AREA_P2))
    op_id=OP_NOTHING;
  else
    op_id = OP_ZOOM_AREA_P1;
}

void __fastcall TMainForm::AboutClick(TObject *Sender)
{
  ShowSplashScreen(this, true);
}

//---------------------------------------------------------------------------
void __fastcall TMainForm::DrawPanelMouseDown(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{
  if (Screen->Cursor == crHourGlass) return;

  if (Button == mbLeft)
  if (op_id == OP_ZOOM_AREA_P1)
  {
    zoom_rect.Left = X;
    zoom_rect.Top = Y;
    zoom_rect.Right = X;
    zoom_rect.Bottom = Y;

    DrawFocusRect();

    op_id = OP_ZOOM_AREA_P2;
  }
  else
  if (op_id == OP_METER_P1)
  {
    meter_p_start = meter_p_end = TPoint(X,Y);
    DrawCross(meter_p_start);
    op_id = OP_METER_P2;
  }
  else
  if (op_id == OP_METER_P2)
  {
    // DrawCross(meter_p_start);
    DrawCross(meter_p_end);

    meter_p_end.x = X;
    meter_p_end.y = Y;

    DrawMeterLine(meter_p_start, meter_p_end, true, true);
    DrawMeterLine(meter_p_start, meter_p_end, true, false);
    // AB_TODO MeterButton->Down = false;
    // AB_TODO op_id = OP_NOTHING;

    meter_p_start = meter_p_end;
  }
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::DrawPanelMouseUp(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{
  if (op_id == OP_ZOOM_AREA_P2)
  {
    DrawFocusRect();
    ZoomButton->Down = false;
    op_id = OP_NOTHING;
    ZoomToWindow();
  }
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::DrawPanelMouseMove(TObject *Sender,
      TShiftState Shift, int X, int Y)
{
  if (closing_form) return;

  if (Screen->Cursor == crHourGlass) return;

  if (op_id == OP_ZOOM_AREA_P2)
  {
    DrawFocusRect();

    zoom_rect.Right = X;
    zoom_rect.Bottom = Y;

    DrawFocusRect();
  }
  else
  if ((op_id == OP_NOTHING) && (OIForm->Visible) && (DrawObjects))
  {
    op_id = OP_FINDOBJECT;

    this->DrawPanel->Cursor = crHourGlass;

    int rx = ViewPort.real_rect.Left+(ViewPort.scale*(float)X);
    int ry = ViewPort.real_rect.Top+(ViewPort.scale*(float)Y);
    TCityObj *city_object = FindObjectAt(rx,ry);
    if (city_object)
    {
      ((THeaderSection*) OIForm->IDHeader->Sections->Items[1])->
        Text = AnsiString(city_object->id);
      ((THeaderSection*) OIForm->ClassIDHeader->Sections->Items[1])->
        Text = AnsiString(city_object->class_id);
      ((THeaderSection*) OIForm->TextHeader->Sections->Items[1])->
        Text = city_object->text;
      ((THeaderSection*) OIForm->ColorHeader->Sections->Items[1])->
        Text = AnsiString(city_object->type); // city_object->color);
    }
      else
    {
      ((THeaderSection*) OIForm->IDHeader->Sections->Items[1])->
        Text = "--";
      ((THeaderSection*) OIForm->ClassIDHeader->Sections->Items[1])->
        Text = "--";
      ((THeaderSection*) OIForm->TextHeader->Sections->Items[1])->
        Text = "--";
      ((THeaderSection*) OIForm->ColorHeader->Sections->Items[1])->
        Text = "--";
    }
    op_id = OP_NOTHING;
    this->DrawPanel->Cursor = crArrow;
  }
  else
  if (op_id == OP_METER_P2)
  {
    if ((meter_p_end.x != meter_p_start.x) ||
        (meter_p_end.y != meter_p_start.y))
      DrawCross(meter_p_end);
    DrawMeterLine(meter_p_start, meter_p_end, true, true);
    meter_p_end = TPoint(X,Y);
    if ((meter_p_end.x != meter_p_start.x) ||
        (meter_p_end.y != meter_p_start.y))
      DrawCross(meter_p_end);
    DrawMeterLine(meter_p_start, meter_p_end, true, true);
  }
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::ZoomOutButtonClick(TObject *Sender)
{
  ZoomOut();
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::ZoomInButtonClick(TObject *Sender)
{
  ZoomIn();
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::ObjInfoButtonClick(TObject *Sender)
{
  if (OIForm)
  {
    OIForm->Visible = ObjInfoButton->Down;
  }
}

void __fastcall TMainForm::umHidedOIForm(TMessage &message)
{
  if (OIForm)
  {
    ObjInfoButton->Down = OIForm->Visible;
  }
}

//---------------------------------------------------------------------------

void __fastcall TMainForm::MeterButtonClick(TObject *Sender)
{
  if ((op_id==OP_METER_P1) || (op_id==OP_METER_P2))
    op_id=OP_NOTHING;
  else
    op_id = OP_METER_P1;
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::FormCloseQuery(TObject *Sender, bool &CanClose)
{
  if (OIForm) OIForm->Free();
  CanClose = true;
  closing_form = true;
}
//---------------------------------------------------------------------------
/* AB_TODO
void __fastcall TMainForm::phScrollBox1Change(TObject *Sender,
      TphScrollBarKind Kind)
{
//
  if (Screen->Cursor == crHourGlass) return;

  int pos_h = HorzScrollBar->Position;
  int pos_v = VertScrollBar->Position;
  TRect rect;

  rect.Left  = pos_h;
  rect.Right = pos_h + ViewPort.real_rect.Width();

  rect.Top    = pos_v;
  rect.Bottom = pos_v + ViewPort.real_rect.Height();

  ViewPort.real_rect = rect;

  update_needed = true; // SetScale();
}
//---------------------------------------------------------------------------
*/

void __fastcall TMainForm::VertScrollBarChange(TObject *Sender)
{
  if (Screen->Cursor == crHourGlass) return;

  int pos_h = HorzScrollBar->Position;
  int pos_v = VertScrollBar->Position;
  TRect rect;

  rect.Left  = pos_h;
  rect.Right = pos_h + ViewPort.real_rect.Width();

  rect.Top    = pos_v;
  rect.Bottom = pos_v + ViewPort.real_rect.Height();

  ViewPort.real_rect = rect;

  update_needed = true; // SetScale();
}
//---------------------------------------------------------------------------


void __fastcall TMainForm::StreetsListButtonClick(TObject *Sender)
{
// for Krzys

  int scale = 20;

  Graphics::TBitmap *bg_buffer = new Graphics::TBitmap();
  bg_buffer->PixelFormat = pf16bit;
  bg_buffer->Height = REAL_WORLD_Y2/scale;
  bg_buffer->Width = REAL_WORLD_X2/scale;

  SetPolyFillMode(bg_buffer->Canvas->Handle, ALTERNATE);

  for (int j=0; j<DrawObjects->Count;j++)
  {
    // Application->HandleMessage();

    TCityObj *city_object = ((TCityObj*) DrawObjects->Items[j]);

    if ((city_object->class_id != CLASS_QUARTER) &&
       (city_object->class_id != CLASS_OUTSIDE))
        continue;

    // if (city_object->outline != 0)
    //  DrawPanel->Canvas->Pen->Color = city_object->outline;
    // else

    bg_buffer->Canvas->Pen->Color = clBlack;

    bg_buffer->Canvas->CopyMode = cmSrcAnd;
    bg_buffer->Canvas->Brush->Color = city_object->color;
    bg_buffer->Canvas->Brush->Style = bsSolid;

    BeginPath(bg_buffer->Canvas->Handle);
//    for (int i=0; i<city_object->polygons->Count;i++)
    {
      TList *points;
      points = ((TList*) city_object->points);

      int points_no = points->Count;

      TPoint *points_list = new TPoint[points_no];
      bool crossing = false;
      for (int k=0; k<points_no; k++)
      {
        TPoint point = *((TPoint*) points->Items[k]);

        if (crossing == false)
        if ((point.x < 0) ||
             (point.x > REAL_WORLD_X2) ||
             (point.y < 0) ||
             (point.y > REAL_WORLD_Y2))
        {
          crossing = true;
        }
        points_list[k] = point;
      }
      for (int k=0; k<points_no; k++)
      {
        points_list[k].x = (points_list[k].x-0)/scale;
        points_list[k].y = (points_list[k].y-0)/scale;
      }
      // here draw polygon
      if (points_no>2)
      {
        Polygon(bg_buffer->Canvas->Handle, &points_list[0], points_no);
      }
      delete points_list;
    }
    EndPath(bg_buffer->Canvas->Handle);
    // FillPath(bg_buffer->Canvas->Handle);
    StrokeAndFillPath(bg_buffer->Canvas->Handle);
  }
  bg_buffer->SaveToFile(".\\test.bmp");
  delete bg_buffer;
//
}
//---------------------------------------------------------------------------



