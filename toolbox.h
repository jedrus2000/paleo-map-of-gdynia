//---------------------------------------------------------------------------
#ifndef toolboxH
#define toolboxH
#include <stdio.h>
//---------------------------------------------------------------------------
#define UM_OIFORMHIDE  (WM_USER+1)

#define NEW_DATA

const int GLOBAL_MAP_X1 = 10000;
const int GLOBAL_MAP_Y1 = 39000;
const int GLOBAL_MAP_X2 = 25000;
const int GLOBAL_MAP_Y2 = 59000;
const float PPM         =   3.0;// 24.0;
// const float QUARTERS_PPM =  0.8;

// using it in drawing
const int REAL_WORLD_X1 =      0;
const int REAL_WORLD_Y1 =      0;
const int REAL_WORLD_X2 = (int) ((float)(GLOBAL_MAP_X2-GLOBAL_MAP_X1)*PPM);
const int REAL_WORLD_Y2 = (int) ((float)(GLOBAL_MAP_Y2-GLOBAL_MAP_Y1)*PPM);
const TRect REAL_WORLD_RECT = TRect(REAL_WORLD_X1,REAL_WORLD_Y1,
                                    REAL_WORLD_X2,REAL_WORLD_Y2);

// ids
#define BUILDINGS_LOW_ID            100000
#define BUILDINGS_HIGH_ID           200000
#define MAIN_ROADS_LOW_ID            11000
#define MAIN_ROADS_HIGH_ID           12000
#define ROADS_LOW_ID                     1
#define ROADS_HIGH_ID                10999
#define POLL_LOW_ID                 900000
#define POLL_HIGH_ID                999999
#define QUARTER_LOW_ID              575000
#define QUARTER_HIGH_ID             575999
#define OUTSIDE_LOW_ID              580000
#define OUTSIDE_HIGH_ID             580100

// my classes
#define CLASS_UNKNOWN                    0
#define CLASS_BUILDING                   1
#define CLASS_MAINROAD                   2
#define CLASS_ROAD                       3
#define CLASS_POLL                       4
#define CLASS_FOREST                     5
#define CLASS_BEACH                      6
#define CLASS_WATER                      7
#define CLASS_RAIL                       8
#define CLASS_OUTSIDE                    9
#define CLASS_QUARTER                   10

// original colors
#define FOREST_COLOR               0x33cc66
#define RAIL_COLOR                 0x423b35
#define BEACH_COLOR                0x80ffff
#define BUILDING_COLOR             0x00aeff
#define AREAS_COLOR                0xccffff
#define ROADS_COLOR                0xd6d3d6
#define WATER_COLOR                0xffcc66
#define OUTSIDE_COLOR              0x09e2e6

enum TOpID {OP_NOTHING,
            OP_FINDOBJECT,
            OP_ZOOM_AREA_P1,
            OP_ZOOM_AREA_P2,
            OP_METER_P1,
            OP_METER_P2 };

struct TViewPort
{
  TRect real_rect; // real word rect
  TRect view_rect; // view rectangle
  float scale;
};
  
class TCityObj : public TObject
{
  public:
    __fastcall TCityObj(void); // constructor
    __fastcall ~TCityObj(void); // destructor
    __fastcall Store(FILE *stream);
    bool __fastcall Load(FILE *stream);
    __fastcall SetBoundRect();
    short int class_id;
    int id;
#ifdef NEW_DATA
    short int type;
    TList *points; //
#endif
    AnsiString text;
    AnsiString url;
#ifndef NEW_DATA
    TList *holes;
    TList *polygons; //
#endif
    int color;
    int outline;
    TRect bound;
  private:
};                    

TList *__fastcall LoadDatFiles(AnsiString directory);
TList *__fastcall LoadDataFile(AnsiString file_in);
__fastcall SaveMainDataFile(TList *objects, AnsiString file_out);
__fastcall AppendQuarterData(TList *objects, AnsiString file_out);
__fastcall AdjustScale(TList *objects);

#endif
