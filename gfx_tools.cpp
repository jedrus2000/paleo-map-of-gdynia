//---------------------------------------------------------------------------
#include <vcl.h>
// #include "windows.h"
#include <limits.h>

#pragma hdrstop

#include "gfx_tools.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)



/*
	Returns true if q is inside polygon P.
*/
bool InPoly_ver1(POINT *P, int n, POINT q)
{
	int	i, i1;		      /* point index; i1 = i-1 mod n */
	int	d;		          /* dimension index */
	double	x;		      /* x intersection of e with ray */
	int	crossings = 0;	/* number of edge/ray crossings */

  
	/* Shift so that q is the origin. */
	for( i = 0; i < n; i++ )
  {
      P[i].x -= q.x;
      P[i].y -= q.y;
	}
	/* For each edge e=(i-1,i), see if crosses ray. */
	for( i = 0; i < n; i++ )
  {
		i1 = ( i + n - 1 ) % n;
		/* if e straddles the x-axis... */
    if( ( ( P[i].y > 0 ) && ( P[i1].y <= 0 ) ) ||
		    ( ( P[i1].y > 0 ) && ( P[i].y <= 0 ) ) )
    {
			/* e straddles ray, so compute intersection with ray. */
 			x = (P[i].x * P[i1].y - P[i1].x * P[i].y)
			 	/ (double)(P[i1].y - P[i].y);
			/* crosses ray if strictly positive intersection. */
			if (x > 0) crossings++;
		}
	}
	/* q inside if an odd number of crossings. */
	if( (crossings % 2) == 1 )
		return	TRUE;
	else
    return	FALSE;
}

bool InPoly_ver2(POINT *P, int n, POINT q)
{
  int	 i, i1;      /* point index; i1 = i-1 mod n */
  int	 d;          /* dimension index */
  double x;          /* x intersection of e with ray */
  int	   Rcross = 0; /* number of right edge/ray crossings */
  int    Lcross = 0; /* number of left edge/ray crossings */

  /* Shift so that q is the origin. Note this destroys the polygon.
     This is done for pedogical clarity. */
  for( i = 0; i < n; i++ )
  {
    P[i].x -= q.x;
    P[i].y -= q.y;
  }

  /* For each edge e=(i-1,i), see if crosses ray. */
  for( i = 0; i < n; i++ )
  {
    /* First see if q=(0,0) is a vertex. */
    if ( P[i].x==0 && P[i].y==0 ) return true;
    i1 = ( i + n - 1 ) % n;
    /* printf("e=(%d,%d)\t", i1, i); */

    /* if e "straddles" the x-axis... */
    /* The commented-out statement is logically equivalent to the one
       following. */
    /* if( ( ( P[i][Y] > 0 ) && ( P[i1][Y] <= 0 ) ) ||
       ( ( P[i1][Y] > 0 ) && ( P[i] [Y] <= 0 ) ) ) { */

    if( ( P[i].y > 0 ) != ( P[i1].y > 0 ) )
    {
      /* e straddles ray, so compute intersection with ray. */
      x = (P[i].x * (double)P[i1].y - P[i1].x * (double)P[i].y)
          	/ (double)(P[i1].y - P[i].y);
      /* printf("straddles: x = %g\t", x); */

      /* crosses ray if strictly positive intersection. */
      if (x > 0) Rcross++;
    }
    /* printf("Right cross=%d\t", Rcross); */

    /* if e straddles the x-axis when reversed... */
    /* if( ( ( P[i] [Y] < 0 ) && ( P[i1][Y] >= 0 ) ) ||
       ( ( P[i1][Y] < 0 ) && ( P[i] [Y] >= 0 ) ) )  { */

    if ( ( P[i].y < 0 ) != ( P[i1].y < 0 ) )
    {
    /* e straddles ray, so compute intersection with ray. */
      x = (P[i].x * P[i1].y - P[i1].x * P[i].y)
          / (double)(P[i1].y - P[i].y);
      /* printf("straddles: x = %g\t", x); */

      /* crosses ray if strictly positive intersection. */
      if (x < 0) Lcross++;
    }
    /* printf("Left cross=%d\n", Lcross); */
  }

  /* q on the edge if left and right cross are not the same parity. */
  if( ( Rcross % 2 ) != (Lcross % 2 ) )
    return true;

  /* q inside iff an odd number of crossings. */
  if( (Rcross % 2) == 1 )
    return true;
  else
    return false;
}

/*************************************************************************

 * FUNCTION:   G_PtInPolygon
 *
 * PURPOSE
 * This routine determines if the point passed is in the polygon. It uses

 * the classical polygon hit-testing algorithm: a horizontal ray starting

 * at the point is extended infinitely rightwards and the number of
 * polygon edges that intersect the ray are counted. If the number is odd,

 * the point is inside the polygon.
 *
 * RETURN VALUE
 * (BOOL) TRUE if the point is inside the polygon, FALSE if not.
 *************************************************************************/


BOOL G_PtInPolygon(POINT *rgpts, WORD wnumpts, POINT ptTest, 
                   RECT *prbound)
{
   RECT   r ;
   POINT  *ppt ;
   WORD   i ;
   POINT  pt1, pt2 ;
   WORD   wnumintsct = 0 ;

   if (!G_PtInPolyRect(rgpts,wnumpts,ptTest,prbound))
      return FALSE ;

   pt1 = pt2 = ptTest ;
   pt2.x = r.right + 50 ;

   // Now go through each of the lines in the polygon and see if it
   // intersects
   for (i = 0, ppt = rgpts ; i < wnumpts-1 ; i++, ppt++)
   {
      if (Intersect(ptTest, pt2, *ppt, *(ppt+1)))
         wnumintsct++ ;
   }

   // And the last line
   if (Intersect(ptTest, pt2, *ppt, *rgpts))
      wnumintsct++ ;

   return (wnumintsct&1) ;

} 

/*************************************************************************

 * FUNCTION:   G_PtInPolyRect
 *
 * PURPOSE
 * This routine determines if a point is within the smallest rectangle
 * that encloses a polygon.
 *
 * RETURN VALUE
 * (BOOL) TRUE or FALSE depending on whether the point is in the rect or

 * not.
 *************************************************************************/


BOOL G_PtInPolyRect(POINT *rgpts, WORD wnumpts, POINT ptTest, 
                     RECT *prbound)
{
   RECT r ;
   // If a bounding rect has not been passed in, calculate it
   if (prbound)
      r = *prbound ;
   else
   {
      int   xmin, xmax, ymin, ymax ;
      POINT *ppt ;
      WORD  i ;

      xmin = ymin = INT_MAX ;
      xmax = ymax = -INT_MAX ;

      for (i=0, ppt = rgpts ; i < wnumpts ; i++, ppt++)
      {
         if (ppt->x < xmin)
            xmin = ppt->x ;
         if (ppt->x > xmax)
            xmax = ppt->x ;
         if (ppt->y < ymin)
            ymin = ppt->y ;
         if (ppt->y > ymax)
            ymax = ppt->y ;
      }
      SetRect(&r, xmin, ymin, xmax, ymax) ;
   }
   return (PtInRect(&r,ptTest)) ;

} 

/*************************************************************************

 * FUNCTION:   Intersect
 *
 * PURPOSE
 * Given two line segments, determine if they intersect.
 *
 * RETURN VALUE
 * TRUE if they intersect, FALSE if not.
 *************************************************************************/


BOOL Intersect(POINT p1, POINT p2, POINT p3, POINT p4) { 
   return ((( CCW(p1, p2, p3) * CCW(p1, p2, p4)) <= 0)
        && (( CCW(p3, p4, p1) * CCW(p3, p4, p2)  <= 0) )) ;
}

/*************************************************************************

 * FUNCTION:   CCW (CounterClockWise)
 *
 * PURPOSE
 * Determines, given three points, if when travelling from the first to
 * the second to the third, we travel in a counterclockwise direction.
 *
 * RETURN VALUE
 * (int) 1 if the movement is in a counterclockwise direction, -1 if
 * not.
 *************************************************************************/

int CCW(POINT p0, POINT p1, POINT p2)

{ 
   LONG dx1, dx2 ;
   LONG dy1, dy2 ;

   dx1 = p1.x - p0.x ; dx2 = p2.x - p0.x ;
   dy1 = p1.y - p0.y ; dy2 = p2.y - p0.y ;

   /* This is basically a slope comparison: we don't do divisions because

    * of divide by zero possibilities with pure horizontal and pure
    * vertical lines.
    */
   return ((dx1 * dy2 > dy1 * dx2) ? 1 : -1) ;

}


