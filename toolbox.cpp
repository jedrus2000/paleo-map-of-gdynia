//---------------------------------------------------------------------------
#include <vcl.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <winldap.h>
#pragma hdrstop

#include "toolbox.h"
#include "import_form.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)
//#pragma link "wldap32"

// kwadrat file header part
  float  x1,
         y1,
         x2,
         y2,
         ppm;

////////////////////////////////////////////////////////////////////////////
//
// TCityObj - object representing all types of real objects in city
//
////////////////////////////////////////////////////////////////////////////
__fastcall TCityObj::TCityObj(void)
{
#ifndef NEW_DATA
  polygons = NULL;
#else
  points = NULL;
#endif
  class_id = 0;
  id = 0;

#ifdef NEW_DATA
  type = 0;
#endif

  text = "";
  url = "";

#ifndef NEW_DATA
  holes = NULL;
#endif

  color = 0;
}

__fastcall TCityObj::~TCityObj(void)
{
#ifndef NEW_DATA
  delete holes;
  delete polygons;
#else
  delete points;
#endif
}

__fastcall TCityObj::Store(FILE *stream)
{

#ifndef NEW_DATA // OLD DATA
  // store polygons
  //
  // number of polygons
  int polys_no = this->polygons->Count;
  fwrite(&polys_no, sizeof(polys_no), 1, stream);
  // polygons
  for (int x=0; x<polys_no; x++)
  {
    // no of points in polygon
    // TPoint *point_list =  ((TPoint*) this->polygons->Items[x]);
    int points_no = ((TList*) this->polygons->Items[x])->Count;
    fwrite(&points_no, sizeof(points_no), 1, stream);
    // points from polygon
    for (int y=0; y<points_no; y++)
    {
      TPoint p = *((TPoint*) ((TList*) this->polygons->Items[x])->Items[y]);
      fwrite(&p, sizeof(p), 1, stream);
    }
  }
#else // NEW DATA
  // store points
  //
  // number of points
  fwrite(&(this->points->Count), sizeof((this->points->Count)), 1, stream);
  for (int x=0; x<this->points->Count; x++)
  {
      TPoint p = *((TPoint*) this->points->Items[x]);
      fwrite(&p, sizeof(p), 1, stream);
  }
#endif
  // class_id
  //
  fwrite(&this->class_id, sizeof(this->class_id), 1, stream);
  // id
  //
  fwrite(&this->id, sizeof(this->id), 1, stream);

#ifdef NEW_DATA
  // type
  //
  fwrite(&this->type, sizeof(this->type), 1, stream);
#endif

  // text
  //
  // char *text_out = new char[512];
  // strcpy(text_out, this->text.c_str());
  // fprintf(stream, "%s",text_out);
  fwrite(this->text.c_str(), strlen(this->text.c_str())+1, 1, stream);
  //delete text_out;
  // url
  //
  // text_out = new char[512];
  // strcpy(text_out, this->url.c_str());
  // fprintf(stream, "%s",text_out);
  fwrite(this->url.c_str(), strlen(this->url.c_str())+1, 1, stream);

#ifndef NEW_DATA
  // holes
  //
  // store number of holes
  int holes_no = this->holes->Count;
  fwrite(&holes_no, sizeof(holes_no), 1, stream);
  for (int x=0; x<holes_no; x++)
  {
    bool h = *((bool*) this->holes->Items[x]);
    fwrite(&h, sizeof(h), 1, stream);
  }
#endif

  // color
  //
  fwrite(&this->color, sizeof(this->color), 1, stream);
  // outline
  //
  fwrite(&this->outline, sizeof(this->outline), 1, stream);
  // bound
  //
  fwrite(&this->bound, sizeof(this->bound), 1, stream);
}

void _fastcall freads(char *s, FILE *stream)
{
  int x = 0;
  while (!feof(stream))
  {
    BYTE ch;
    fread(&ch, sizeof(BYTE), 1, stream);
    s[x] = (char) ch;
    x++;
    if (ch == '\0') break;
  }
}

__fastcall TCityObj::SetBoundRect()
{
  int x_min = REAL_WORLD_X2;
  int x_max = REAL_WORLD_X1;
  int y_min = REAL_WORLD_Y2;
  int y_max = REAL_WORLD_Y1;

#ifndef NEW_DATA // OLD DATA
  if (this->polygons)
  {
    for (int i=0; i<this->polygons->Count; i++)
    {
      TList *point_list = ((TList*) this->polygons->Items[i]);
      for (int j=0; j<point_list->Count;j++)
      {
        TPoint point = *((TPoint*) point_list->Items[j]);
        if (point.x<x_min)
          x_min = point.x;
        if (point.x>x_max)
          x_max = point.x;
        if (point.y<y_min)
          y_min = point.y;
        if (point.y>y_max)
          y_max = point.y;
      }
    }
  }
#else // NEW DATA
  if (this->points)
  {
    for (int i=0; i<this->points->Count; i++)
    {
        TPoint point = *((TPoint*) this->points->Items[i]);
        if (point.x<x_min)
          x_min = point.x;
        if (point.x>x_max)
          x_max = point.x;
        if (point.y<y_min)
          y_min = point.y;
        if (point.y>y_max)
          y_max = point.y;
    }
  }
#endif

  this->bound = TRect(x_min, y_min, x_max, y_max);
}

bool __fastcall TCityObj::Load(FILE *stream)
{
  // load polygons
  //
  // number of polygons / points
  int polys_no;
  if (fread(&polys_no, sizeof(polys_no), 1, stream)==0)
    // end of file
    return false;

#ifndef NEW_DATA // OLD DATA
  // polygons
  this->polygons = new TList();
  for (int x=0; x<polys_no; x++)
  {
    // no of points in polygon
    // TPoint *point_list =  ((TPoint*) this->polygons->Items[x]);
    int points_no; //  = (*point_list).x;
    fread(&points_no, sizeof(points_no), 1, stream);
    // TPoint *point_list = new TPoint[points_no+1];
    TList *point_list = new TList();
    // point_list[0] = TPoint(points_no, points_no);
    // points from polygon
    for (int y=0; y<points_no; y++)
    {
      TPoint p;
      fread(&p, sizeof(p), 1, stream);
      // point_list[y+1] = p;
      TPoint *p1 = new TPoint(p);
      point_list->Add(p1);
    }
    this->polygons->Add(point_list);
  }
#else // NEW DATA
  this->points = new TList();
  for (int x=0; x<polys_no; x++)
  {
    TPoint p;
    fread(&p, sizeof(p), 1, stream);
    TPoint *p1 = new TPoint(p);
    points->Add(p1);
  }
#endif
  // class_id
  //
  fread(&this->class_id, sizeof(this->class_id), 1, stream);
  // id
  //
  fread(&this->id, sizeof(this->id), 1, stream);

#ifdef NEW_DATA
  // type
  //
  fread(&this->type, sizeof(this->type), 1, stream);
#endif

  // text
  //
  char *text_in = new char[512];
  freads((char*)text_in, stream);
  this->text = AnsiString(text_in);
  delete text_in;
  // url
  //
  text_in = new char[512];
  freads((char*)text_in, stream);
  this->url = AnsiString(text_in);
  delete text_in;

#ifndef NEW_DATA
  // holes
  //
  // load number of holes
  int holes_no;
  fread(&holes_no, sizeof(holes_no), 1, stream);
  this->holes = new TList();
  for (int x=0; x<holes_no; x++)
  {
    bool *h = new bool; //  *((bool*) this->holes->Items[x]);
    fread((bool*)h, sizeof(bool), 1, stream);
    this->holes->Add(h);
  }
#endif

  // color
  //
  fread(&this->color, sizeof(this->color), 1, stream);
  // outline
  //
  fread(&this->outline, sizeof(this->outline), 1, stream);
  // bound
  //
  fread(&this->bound, sizeof(this->bound), 1, stream);

  return true;
}

////////////////////////////////////////////////////////////////////////////
//
// converting Java binary dat file
//
////////////////////////////////////////////////////////////////////////////
int __fastcall flipInt(int bits)
{
  bits = MAKEWPARAM(
          MAKEWORD(HIBYTE(HIWORD(bits)),LOBYTE(HIWORD(bits))),
          MAKEWORD(HIBYTE(LOWORD(bits)),LOBYTE(LOWORD(bits)))
          );

  return bits;
}

bool __fastcall readJBool(FILE *stream)
{
  BYTE bits;
  fread(&bits, sizeof(bits), 1, stream);
  return (bool) bits;
}

unsigned short int __fastcall readJusInt(FILE *stream)
{
  unsigned short int bits;
  fread(&bits, sizeof(bits), 1, stream);

  bits =  MAKEWORD(HIBYTE(bits),LOBYTE(bits));

  return bits;
}

short int __fastcall readJsInt(FILE *stream)
{
  short int bits;
  fread(&bits, sizeof(bits), 1, stream);

  bits =  MAKEWORD(HIBYTE(bits),LOBYTE(bits));

  return bits;
}

int __fastcall readJInt(FILE *stream)
{
  int bits;
  fread(&bits, sizeof(bits), 1, stream);

  bits = MAKEWPARAM(
          MAKEWORD(HIBYTE(HIWORD(bits)),LOBYTE(HIWORD(bits))),
          MAKEWORD(HIBYTE(LOWORD(bits)),LOBYTE(LOWORD(bits)))
          );

  return bits;
}

float __fastcall readJFloat(FILE *stream)
{
  int bits;
  fread(&bits, sizeof(bits), 1, stream);

  bits = MAKEWPARAM(
          MAKEWORD(HIBYTE(HIWORD(bits)),LOBYTE(HIWORD(bits))),
          MAKEWORD(HIBYTE(LOWORD(bits)),LOBYTE(LOWORD(bits)))
          );
  int s = ((bits >> 31) == 0) ? 1 : -1;
  int e = ((bits >> 23) & 0xff);
  int m = (e == 0) ?
		(bits & 0x7fffff) << 1 :
		(bits & 0x7fffff) | 0x800000;

  return ((float)s*(float)m*(float)pow(2.0,(float)e-150.0));
}

TList *__fastcall streamToInts3DS(FILE *stream)
{
  int i = readJInt(stream); // ilosc punktow ?

  TList *ai = new TList();
  // int ai[][][] = new int[i][][];
  for(int l = 0; l < i; l++)
  {
    int j = readJInt(stream);
    // int ai1[][] = new int[j][];
    // ai[l] = ai1;
    TList *ai1 = new TList();
    for(int i1 = 0; i1 < j; i1++)
    {
      int k = readJInt(stream);
      // int ai2[] = new int[k];
      // ai1[i1] = ai2;
      TList *ai2 = new TList();
      for(int j1 = 0; j1 < k; j1++)
      {
        int *k1 = new int;
        *k1 = (int) readJusInt(stream);
        // ai2[j1] = k1;
        ai2->Add(k1);
      }
      ai1->Add(ai2);
    }
    ai->Add(ai1);
  }
  return ai;
}

TList* __fastcall streamToInts1DI(FILE *stream)
{
  // DataInputStream datainputstream = new DataInputStream(inputstream);
  // int i = datainputstream.readInt();
  int i = readJInt(stream);
  // int ai[] = new int[i];
  TList *ai = new TList();
  for(int j = 0; j < i; j++)
  {
    // ai[j] = datainputstream.readInt();
    int *integer = new int;
    *integer = readJInt(stream);
    // ai2[j1] = k1;
    ai->Add(integer);
  }
  return ai;
}

TList* __fastcall streamToInts1DS(FILE *stream)
{
  // DataInputStream datainputstream = new DataInputStream(inputstream);
  // int i = datainputstream.readInt();
  int i = readJInt(stream);
  // int ai[] = new int[i];
  TList *ai = new TList();
  for(int j = 0; j < i; j++)
  {
    // ai[j] = datainputstream.readUnsignedShort();
    // AB unsigned
    short int *n = new short int;
    *n = readJsInt(stream);
    ai->Add(n);
  }
  return ai;
}

TList *__fastcall streamToBools2D(FILE *stream)
{
  // DataInputStream datainputstream = new DataInputStream(inputstream);
  // int i = datainputstream.readInt();
  int i = readJInt(stream);
  // boolean aflag[][] = new boolean[i][];
  TList *aflag = new TList();
  for(int k = 0; k < i; k++)
  {
    // int j = datainputstream.readInt();
    int j = readJInt(stream);
    // boolean aflag1[] = new boolean[j];
    TList *aflag1 = new TList();
    // aflag[k] = aflag1;
    for(int l = 0; l < j; l++)
    {
      bool *b = new bool;
      // aflag1[l] = datainputstream.readBoolean();
      *b = readJBool(stream);
      aflag1->Add(b);
    }
    aflag->Add(aflag1);
  }
  return aflag;
}

AnsiString __fastcall readUTF(FILE *stream)
{
  // read UTF length
  unsigned short int UTF_length = readJusInt(stream);

  if (UTF_length==0) return "";

  char *str;
  str = (char*) malloc(UTF_length+1);

  fread(str, UTF_length, 1, stream);
  str[UTF_length] = '\0';

  LPWSTR lpDestStr;
  int cchDest = 255;
  lpDestStr = (LPWSTR) malloc(255);

  LdapUTF8ToUnicode(str, UTF_length+1, lpDestStr, cchDest);

  AnsiString ansi_str = AnsiString(lpDestStr);

  free(lpDestStr);
  free(str);

  return ansi_str;
}

TStringList *__fastcall streamToStrings(FILE *stream)
{
  // DataInputStream datainputstream = new DataInputStream(inputstream);
  // int i = datainputstream.readInt();
  int i = readJInt(stream);
  // String as[] = new String[i];
  TStringList *as = new TStringList();
  for(int j = 0; j < i; j++)
  {
    // as[j] = readUTF(stream);
    as->Add(readUTF(stream));
  }
  return as;
}

TList* joinPolygons(TList* apolygon)
{
  TList* polygon = NULL;
  int i = apolygon->Count;
  if(i == 1)
  {
    polygon = ((TList*) apolygon->Items[0]);
  }
  else
  {
    polygon = new TList();
    for(int j = 0; j < i; j++)
    {
      TPoint *point;
      TList *polygon1 = (TList*) apolygon->Items[j];
      for(int k = 0; k < polygon1->Count; k++)
      {
        point = (TPoint*) polygon1->Items[k];
        // AB polygon.addPoint(polygon1.xpoints[k], polygon1.ypoints[k]);
        polygon->Add(point);
      }
      polygon1 = (TList*) apolygon->Items[0];
      // AB polygon.addPoint(polygon1.xpoints[polygon1.npoints - 1], polygon1.ypoints[polygon1.npoints - 1]);
      point = (TPoint*) polygon1->Items[polygon1->Count-1];
      polygon->Add(point);
    }
  }
  return polygon;
}

TList* makePolygon(int i, int j, int k, int l)
{
  double d = 1.5707963267948966;
  double d1 = k - i;
  double d2 = l - j;
  double d3 = 0.0;
  if ((d2 != 0.0) || (d1 != 0.0))
    d3 = atan2(d2, d1);
  double d4 = 5.0;

  TList* polygon = new TList();

  int i1 = (int)(d4 * cos(d3 + d));
  int j1 = (int)(d4 * sin(d3 + d));
  // polygon.addPoint(i + i1, j + j1);
  polygon->Add(new TPoint(i + i1, j + j1));
  i1 = (int)(d4 * cos(d3 + d));
  j1 = (int)(d4 * sin(d3 + d));
  // polygon.addPoint(k + i1, l + j1);
  polygon->Add(new TPoint(k + i1, l + j1));
  i1 = (int)(d4 * cos(d3 - d));
  j1 = (int)(d4 * sin(d3 - d));
  // polygon.addPoint(k + i1, l + j1);
  polygon->Add(new TPoint(k + i1, l + j1));
  i1 = (int)(d4 * cos(d3 - d));
  j1 = (int)(d4 * sin(d3 - d));
  // polygon.addPoint(i + i1, j + j1);
  polygon->Add(new TPoint(i + i1, j + j1));
  // polygon.addPoint(polygon.xpoints[0], polygon.ypoints[0]);
  polygon->Add(polygon->Items[0]);

  return polygon;
}

TList* makePolygon(TList* polygon)
{
  int i = polygon->Count; // .npoints;
//  int ai[] = polygon.xpoints;
//  int ai1[] = polygon.ypoints;
  int k;
  int l;
  TPoint *point;

  if(i > 0)
  {
    point = (TPoint*) polygon->Items[0];
    // k = ai[0];
    k = (*point).x;
    l = (*point).y; // ai1[0];
  }
  else
  {
    return new TList(); // return new Polygon();
  }

//  Polygon apolygon[] = new Polygon[i - 1];
  TList *apolygon = new TList();

  int i1 = 0;
  for(int j = 1; j < i; j++)
  {
    // AB apolygon[i1++] = makePolygon(k, l, ai[j], ai1[j]);
    point = (TPoint*) polygon->Items[j];
    apolygon->Add(makePolygon(k, l, (*point).x, (*point).y));
    k = (*point).x; // ai[j];
    l = (*point).y; // ai1[j];
  }

  return joinPolygons(apolygon);
}

TList *makePolygons(TList *apolygon)
{
  // AB Polygon apolygon1[] = new Polygon[apolygon.length];
  int l = apolygon->Count;
  TList *apolygon1 = new TList();
  for(int i = 0; i < l; i++)
    apolygon1->Add(makePolygon((TList*) apolygon->Items[i]));

  return joinPolygons(apolygon1);
}



TList* __fastcall streamToPolygons(FILE *stream)
{
  // int i = datainputstream.readInt();
  int i = readJInt(stream);

  TList *polygons = new TList();
  TList *polygon = NULL;

  for(int j = 0; j < i; j++)
  {
    short word0 = readJsInt(stream); // datainputstream.readShort();
    // AB int ai[] = new int[word0];
    TList *ai = new TList();
    for(int k = 0; k < word0; k++)
    {
      // ai[k] = readJsInt(stream); // datainputstream.readShort();
      short *value = new short;
      *value = readJsInt(stream);
      ai->Add(value);
    }

    short word1 = readJsInt(stream); // datainputstream.readShort();
    int i1 = 0;

    // Polygon apolygon[] = new Polygon[word0];
    TList* apolygon = new TList();

    for(int l = 0; l < word1; l++)
    {
      // if(i1 < ai.length && ai[i1] == l)
      if(i1 < ai->Count && *((short*)ai->Items[i1]) == l)
      {
        // polygon = new Polygon();
        polygon = new TList();
        // AB apolygon[i1] = polygon;
        apolygon->Add(polygon);
        i1++;
      }
      // int j1 = datainputstream.readShort() + 10;
      // int k1 = datainputstream.readShort() + 93;
      short X = readJsInt(stream);
      short Y = readJsInt(stream);

      TPoint *point = new TPoint(
        (int) ((float)X+(ppm*(float)x1)-(ppm*(float)GLOBAL_MAP_X1)),
        (int) (((float)GLOBAL_MAP_Y2*ppm)-(((float)y1*ppm)-(float)Y))
       );

      // AB polygon.addPoint(j1, k1);
      polygon->Add(point);
    }
    // vector.addElement(apolygon);
    polygons->Add(apolygon);
    delete ai;
  }

  // Polygon apolygon1[][] = new Polygon[vector.size()][];
  // vector.copyInto(apolygon1);
  // return apolygon1;
  return polygons;
}


////////////////////////////////////////////////////////////////////////////
//
// function to load dat file
//
////////////////////////////////////////////////////////////////////////////
TList *__fastcall LoadDataFile(AnsiString file_in)
{
  TList *objects = new TList();
  FILE *inf = fopen(file_in.c_str(), "r+b");
  if (inf)
  {
    while (!feof(inf))
    {
      TCityObj *city_obj = new TCityObj();
      if (city_obj->Load(inf) == false)
      {
        delete city_obj;
      }
      else objects->Add(city_obj);

    }
    fclose(inf);
  }
  return objects;
}

__fastcall CorrectObjectsColors(TList *objects)
{
  for (int i=0;i<objects->Count;i++)
  {
      TCityObj *obj = ((TCityObj*) objects->Items[i]);

      BYTE blue = obj->color & 0x000000ff;
      BYTE green = (obj->color >> 8) & 0x000000ff;
      BYTE red = (obj->color >> 16)& 0x000000ff;

      TColor color = RGB(red, green, blue);

      obj->color = (int) color;
  }
}

__fastcall CorrectObjectsIDs(TList *objects)
{
  for (int i=0;i<objects->Count;i++)
  {
      TCityObj *obj = ((TCityObj*) objects->Items[i]);

      if ((obj->id >= BUILDINGS_LOW_ID) && (obj->id <= BUILDINGS_HIGH_ID))
        obj->class_id = (short) CLASS_BUILDING;
      else
      if ((obj->id >= MAIN_ROADS_LOW_ID) && (obj->id <= MAIN_ROADS_HIGH_ID))
        obj->class_id = (short) CLASS_MAINROAD;
      else
      if ((obj->id >= ROADS_LOW_ID) && (obj->id <= ROADS_HIGH_ID))
        obj->class_id = (short) CLASS_ROAD;
      else
      if ((obj->id >= POLL_LOW_ID) && (obj->id <= POLL_HIGH_ID))
        obj->class_id = (short) CLASS_POLL;
      else
      if ((obj->id >= QUARTER_LOW_ID) && (obj->id <= QUARTER_HIGH_ID))
        obj->class_id = (short) CLASS_QUARTER;
      else
      if ((obj->id >= OUTSIDE_LOW_ID) && (obj->id <= OUTSIDE_HIGH_ID))
        obj->class_id = (short) CLASS_OUTSIDE;
      else
      if (obj->color == FOREST_COLOR)
        obj->class_id = (short) CLASS_FOREST;
      else
      if (obj->color == RAIL_COLOR)
        obj->class_id = (short) CLASS_RAIL;
      else
      if (obj->color == BEACH_COLOR)
        obj->class_id = (short) CLASS_BEACH;
      else
      if (obj->color == WATER_COLOR)
        obj->class_id = (short) CLASS_WATER;
      else
      // unknown
      obj->class_id = (short) CLASS_UNKNOWN;
  }
}

__fastcall SaveMainDataFile(TList *objects, AnsiString file_out)
{
  // save objects
  FILE *outf = fopen(file_out.c_str(), "w+b");
  if (outf)
  {
    for (int i=0;i<objects->Count;i++)
    {
      TCityObj *city_obj = ((TCityObj*) objects->Items[i]);
      city_obj->Store(outf);
    }
    fclose(outf);
  }
}

__fastcall AdjustScale(TList *objects)
{
    for (int i=0;i<objects->Count;i++)
    {
      TCityObj *city_obj = ((TCityObj*) objects->Items[i]);

      // correct points
      TList *points;
      points = city_obj->points;
      int points_no = points->Count;
      for (int k=0; k<points_no; k++)
      {
        (*((TPoint*) points->Items[k])).x =
          (*((TPoint*) points->Items[k])).x*8;
        (*((TPoint*) points->Items[k])).y =
          (*((TPoint*) points->Items[k])).y*8;

      }
      city_obj->bound.Left =
        city_obj->bound.Left*8;
      city_obj->bound.Top =
        city_obj->bound.Top*8;
      city_obj->bound.Bottom =
        city_obj->bound.Bottom*8;
      city_obj->bound.Right =
        city_obj->bound.Right*8;
    }
}

__fastcall AppendQuarterData(TList *objects, AnsiString file_out)
{
  FILE *outf = fopen(file_out.c_str(), "a+b");
  if (outf)
  {
    for (int i=0;i<objects->Count;i++)
    {
      TCityObj *city_obj = ((TCityObj*) objects->Items[i]);

      if ((city_obj->class_id != CLASS_QUARTER) &&
          (city_obj->class_id != CLASS_OUTSIDE))
          continue;

      // correct points
      TList *points;
      points = city_obj->points;
      int points_no = points->Count;
      for (int k=0; k<points_no; k++)
      {
/*
        (*((TPoint*) points->Items[k])).x =
          (int) ((float)(*((TPoint*) points->Items[k])).x*(PPM/QUARTERS_PPM));
        (*((TPoint*) points->Items[k])).y =
          (int) ((float)(*((TPoint*) points->Items[k])).y*(PPM/QUARTERS_PPM));
*/
        (*((TPoint*) points->Items[k])).x =
          (*((TPoint*) points->Items[k])).x*30;
        (*((TPoint*) points->Items[k])).y =
          (*((TPoint*) points->Items[k])).y*30;

      }
      city_obj->bound.Left =
        city_obj->bound.Left*30;
      city_obj->bound.Top =
        city_obj->bound.Top*30;
      city_obj->bound.Bottom =
        city_obj->bound.Bottom*30;
      city_obj->bound.Right =
        city_obj->bound.Right*30;

      city_obj->Store(outf);
    }
    fclose(outf);
  }
}

////////////////////////////////////////////////////////////////////////////
//
// main function to load and convert dat file
//
////////////////////////////////////////////////////////////////////////////
TList *__fastcall LoadDatFiles(AnsiString directory)
{
  TList *polygons;

#ifndef NEW_DATA
  TList *holes;
#endif

  TList *ids;
  TStringList *texts;
  TStringList *urls;
  TList *classes;
  TList *acolor;
  TList *outlines;

  TList *objects = new TList();

  ImportForm = new TImportForm(NULL);
  ImportForm->Show();
  //
  // loading original file
  //
  for (int fx=1; fx<1201; fx++)
  {
    char file_name[30];
    sprintf(file_name, AnsiString(directory+"\\kw%03d.dat").c_str(), fx);

    //int up_y = (((fx / 12)-1)*1000);// +GLOBAL_MAP_Y1;
    //int up_x = ((fx-((fx/11)*11)-1)*1000);//+GLOBAL_MAP_X1;

    FILE *DATHandle = fopen(file_name, "r+b");

    if (DATHandle)
    {
      // reading header
      x1 = readJFloat(DATHandle);
      y1 = readJFloat(DATHandle);
      x2 = readJFloat(DATHandle);
      y2 = readJFloat(DATHandle);
      ppm = readJFloat(DATHandle);

      ImportForm->Label1->Caption = "Proccessing file: "+AnsiString(file_name)+
        " PPM = "+AnsiString(ppm);

      Application->ProcessMessages();

#ifdef NEW_DATA
      TList *types;
      types = streamToInts1DS(DATHandle);
#endif
      int up_x = x1;
      int up_y = y1;

#ifndef NEW_DATA // OLD STUFF
      //
      TList *ai;
      ai = streamToInts3DS(DATHandle);
      polygons = new TList();
      for(int i = 0; i < ai->Count; i++)
      {
        TList *apolygon1 = new TList();
        for(int k = 0; k < ((TList*) ai->Items[i])->Count; k++)
        {
          int l = ((TList*) ((TList*) ai->Items[i])->Items[k])->Count / 2;
          // TPoint *point_list = new TPoint[l+2];
          // TPoint point_no = TPoint(l+1,l+1);
          // point_list[0] = point_no;

          TList *point_list = new TList();
          for(int i1 = 0; i1 < l; i1++)
          {
            TPoint *point = new TPoint(
              ((*(int*) ((TList*) ((TList*) ai->Items[i])->Items[k])->Items[i1 * 2])+((int)ppm*(int)up_x))-((int)ppm*GLOBAL_MAP_X1),
              (GLOBAL_MAP_Y2*(int)ppm)-(((int)up_y*(int)ppm)-((*(int*) ((TList*) ((TList*) ai->Items[i])->Items[k])->Items[i1 * 2 + 1])))
             );

            point_list->Add(point);
          }
          point_list->Add(((TPoint*) point_list->Items[0]));
          apolygon1->Add(point_list);
        }
        polygons->Add(apolygon1);
      }
#else // NEW DATA handle here
      //
      TList *polys = streamToPolygons(DATHandle);
      polygons = new TList();
      for(int i = 0; i < polys->Count; i++)
        if( *((short int*)types->Items[i]) == 5)
          // multipolys[i] = joinPolygons(polys[i]);
          polygons->Add(joinPolygons((TList*)polys->Items[i]));
        else
          // multipolys[i] = makePolygon(polys[i]);
          polygons->Add(makePolygons((TList*)polys->Items[i]));
#endif


#ifndef NEW_DATA
      holes = streamToBools2D(DATHandle);
#endif

      ids = streamToInts1DI(DATHandle);
      texts = streamToStrings(DATHandle);
      urls = streamToStrings(DATHandle);
      TList *ai2 = streamToInts1DI(DATHandle);
      acolor = new TList();
      outlines = new TList();
      for(int j1 = 0; j1 < ai2->Count; j1++)
      {
        int *color = new int;
        *color = (*((int*) ai2->Items[j1])) & 0xffffff;
        acolor->Add(color);
        int *color1 = new int;
        *color1= ((*((int*) ai2->Items[j1])) & 0xff000000) >> 24;
        outlines->Add(color1);
      }
      classes = streamToInts1DS(DATHandle);
      fclose(DATHandle);

      //
      // convert data to our objects
      //
      for(int i = 0; i < polygons->Count; i++)
      {
        TCityObj *city_obj = new TCityObj();
        city_obj->id = *((int*) ids->Items[i]);
#ifndef NEW_DATA // OLD DATA
        city_obj->holes = ((TList*) holes->Items[i]);
        city_obj->polygons = ((TList*) polygons->Items[i]);
#else // NEW DATA
        city_obj->points = ((TList*) polygons->Items[i]);
#endif
        city_obj->text = texts->Strings[i];
        city_obj->url = urls->Strings[i];
        city_obj->color = *((int*) acolor->Items[i]);
        city_obj->outline = *((int*) outlines->Items[i]);
        city_obj->class_id = *((short int*) classes->Items[i]);
        city_obj->SetBoundRect();
#ifdef NEW_DATA
        city_obj->type = *((short int*) types->Items[i]);
#endif
        objects->Add(city_obj);
      }
      delete outlines;
      delete ai2;
      delete ids;
      delete acolor;
      delete classes;
      delete polygons;
#ifndef NEW_DATA // OLD STUFF
      delete ai;
      delete holes;
#else // NEW_DATA
      delete types;
#endif
    }
  }
  CorrectObjectsColors(objects);
  CorrectObjectsIDs(objects);

  delete ImportForm;

  return objects;
}

