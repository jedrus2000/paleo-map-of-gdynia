//---------------------------------------------------------------------------
#ifndef gfx_toolsH
#define gfx_toolsH
//---------------------------------------------------------------------------
bool InPoly_ver1(POINT *P, int n, POINT q);
bool InPoly_ver2(POINT *P, int n, POINT q);

BOOL G_PtInPolygon(POINT *rgpts, WORD wnumpts, POINT ptTest,
                   RECT *prbound) ;

BOOL G_PtInPolyRect(POINT *rgpts, WORD wnumpts, POINT ptTest,
                    RECT *prbound) ;

BOOL Intersect(POINT p1, POINT p2, POINT p3, POINT p4) ;
int  CCW(POINT p0, POINT p1, POINT p2) ;
#endif
 