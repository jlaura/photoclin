void LineToPoints (int *xbuf, int *ybuf, int *npts, 
                   int sx, int sy, int ex, int ey)

/***********************************************************************
*
*_Title	LineToPoints - Converts a line segment to corresponding points
*
*_Args	Type   Variable        I/O	Description
*_Parm	int	xbuf		O	Buffer containing X coordinates
*_Parm	int	ybuf		O	Buffer containing Y coordinates
*_Parm	int	npts		O	Number of output points
*_Parm	int	sx,sy,ex,ey	I	The two points defining the
*					line segment.

*_DESC	This routine takes two points (sx,sy) and (ex,ey) and determines
*	all the points which make up that line segment. This is useful
*	for drawing graphic on a display or image
*
*_HIST	Jan 1 1996 Jeff Anderson, Original version
*_END
************************************************************************/

{
  double slope;
  int i;
  int x,y,xinc,yinc;
  int xsize,ysize;

  *npts = 0;

  if (sx > ex) {
    xsize = sx - ex + 1;
    xinc = -1;
  }
  else {
    xsize = ex - sx + 1;
    xinc = 1;
  }

  if (sy > ey) {
    ysize = sy - ey + 1;
    yinc = -1;
  }
  else {
    ysize = ey - sy + 1;
    yinc = 1;
  }

  if (ysize > xsize) {
    slope = (double) (ex - sx) / (double) (ey - sy);   
    y = sy;
    for (i=0; i<ysize; i++) {
      x = slope * (double) (y - sy) + (double) sx + 0.5;
      xbuf[i] = x;
      ybuf[i] = y; 
      *npts = *npts + 1;
      y += yinc;
    }
  }
  else if (xsize == 1) {
    xbuf[0] = sx;
    ybuf[0] = sy;
    *npts = 1;
  }
  else {
    slope = (double) (ey - sy) / (double) (ex - sx);   
    x = sx;
    for (i=0; i<xsize; i++) {
      y = slope * (double) (x - sx) + (double) sy + 0.5;
      xbuf[i] = x;
      ybuf[i] = y; 
      *npts = *npts + 1;
      x += xinc;
    }
  }

  return;
} 

