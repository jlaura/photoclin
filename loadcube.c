#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "pc2d.h"


void LoadCube (int dsp)
/***********************************************************************
*
*       -----------------
*       |   LoadCube    |
*       -----------------
*
*_Title LoadCube - Load cube into drawing area. 
*
*_Args  Type   Variable        I/O      Description
*_Parm  int    dsp              I       display number
*
*_DESC  This routine sets up the color maps and reads the cube from
*       disk and loads it into the drawing area.
*
*_HIST  Jan 01 1996  Jeff Anderson, USGS, Flagstaff, Original version
*       Jan 29 1998  Tracie Sucharski, Save filled display to temporary
*                      file.
*       Mar 25 1998  TLS, Changed names of variables pertaining
*                      to world button from temp to fill.
*       Dec 05 2001  Janet Barrett, Modified to work with the pc2d
*                    program.
*
*_END
************************************************************************/

{
  INT4 limit[3][2];
  DSPINFO *d;
  FILEINFO *f;
  int sline,wline,lastline,width,i;
  int iline,isamp,xline,xsamp;
  UINT1 *rbuf,*gbuf,*bbuf;
  unsigned char *ptr;
  double line,samp;
  Window win;
  INT4 ret;
  Boolean red_first;
  int *map1,*map2,*map3;
  UINT1 *buf1,*buf2,*buf3;
  Dimension dwidth,dheight;
  int index;
  int bytes,nbytes;

  d = &GBLdsp.d[dsp];
  f = &d->f;

  QviewWatch (xinfo.topShell,True);

  win = XtWindow (d->DA);
  XtVaGetValues (d->DA,XmNwidth,&dwidth,XmNheight,&dheight,NULL);

/*-----------------------------------------------------------------------
/  Read image from temporary file if possible.
/----------------------------------------------------------------------*/
  if (d->read_tmp && d->world) {
    for (xsamp=0,samp=d->ss; xsamp<xinfo.width; xsamp++,samp+=d->inc) {
      d->samplut[xsamp] = samp;
    }
    
    for (xline=0,line=d->sl; xline<xinfo.height; xline++,line+=d->inc) {
      d->linelut[xline] = line;
    }
    rewind (d->fill_fp);
    nbytes = xinfo.height * xinfo.width * BYTEDEPTH;
    bytes = fread (d->ximgptr,sizeof(char),nbytes,d->fill_fp);
    if (ferror(d->fill_fp)) printf ("Error in writing\n");
    XPutImage (xinfo.display,d->pmap,xinfo.imgGC,d->ximg,
                 0,0,0,0,(int)dwidth,(int)dheight);
    XCopyArea (xinfo.display,d->pmap,win,xinfo.imgGC,
                 0,0,(int)dwidth,(int)dheight,0,0);
    XSync (xinfo.display,False);
    QviewWatch (xinfo.topShell,False);
    UpdateInfoLB (dsp);
    return;
  }

  sline = 0;
  wline = dheight / 10;
  lastline = 0;
  width = xinfo.width * xinfo.depth / 8;

  for (xsamp=0,samp=d->ss; xsamp<xinfo.width; xsamp++,samp+=d->inc) {
    d->samplut[xsamp] = samp;
  }

  for (xline=0,line=d->sl; xline<xinfo.height; xline++,line+=d->inc) {
    d->linelut[xline] = line;
  }

  rbuf = (UINT1 *) GBLwork.work[0];
  gbuf = (UINT1 *) GBLwork.work[1];
  bbuf = (UINT1 *) GBLwork.work[2];

/*  Determine if storage is RGB or BGR  */
  red_first = (Boolean) (d->ximg->red_mask < d->ximg->blue_mask);
  if (d->ximg->byte_order == MSBFirst) red_first = !red_first;

  if (red_first) {
    map1 = d->rmap;
    map2 = d->gmap;
    map3 = d->bmap;
/*  Color Display  */
    if (d->band[1] || d->band[2]) {
      buf1 = rbuf;
      buf2 = gbuf;
      buf3 = bbuf;
    }
/*  B/W Display  */
    else {
      buf1 = rbuf;
      buf2 = rbuf;
      buf3 = rbuf;
    }
  }
  else {
    map1 = d->bmap;
    map2 = d->gmap;
    map3 = d->rmap;
    if (d->band[1] || d->band[2]) {
      buf1 = bbuf;
      buf2 = gbuf;
      buf3 = rbuf;
    }
    else {
      buf1 = rbuf;
      buf2 = rbuf;
      buf3 = rbuf;
    }
  }

  for (i=0; i<f->ns; i++) {
    rbuf[i] = 0;
    gbuf[i] = 0;
    bbuf[i] = 0;
  }

  for (xline=0; xline<(int)dheight; xline++) {
    ptr = (unsigned char *) d->ximgptr + xline * xinfo.width * BYTEDEPTH;
    line = d->linelut[xline];
    iline = (int) line;

/*  Outside of image-write out black space  */
    if ((iline < 1) || (iline > f->nl)) {
      if (xinfo.vinfo.class == PseudoColor) {
        for (i=0; i<(int)dwidth; i++) *ptr++ = d->map[0];
      }
      else {
        for (i=0; i<(int)dwidth; i++) {
          *ptr++ = map1[0];
          *ptr++ = map2[0];
          *ptr++ = map3[0];
        }
      }
    }
    else {
      if (iline != lastline) {
        lastline = iline;
        limit[0][0] = 1;
        limit[0][1] = f->ns;
        limit[1][0] = iline;
        limit[1][1] = iline;

        if (d->band[0]) {
          limit[2][0] = d->band[0];
          limit[2][1] = d->band[0];
          q_use_cache (f->fid,0,&ret);
          q_lio_cbrick (f->fid,1,limit,rbuf,&ret);
          if (ret) {
            strcpy (GBLerr.string,"Unable to read from file ");
            strcat (GBLerr.string,f->name);
            u_error ("READFILE",GBLerr.string,-1,1);
            QviewExit ();
          }
          StretchLine (rbuf,dsp,0);
        }

        if (d->band[1]) {
          limit[2][0] = d->band[1];
          limit[2][1] = d->band[1];
          q_use_cache (f->fid,1,&ret);
          q_lio_cbrick (f->fid,1,limit,gbuf,&ret);
          if (ret) {
            strcpy (GBLerr.string,"Unable to read from file ");
            strcat (GBLerr.string,f->name);
            u_error ("READFILE",GBLerr.string,-1,1);
            QviewExit ();
          }
          StretchLine (gbuf,dsp,1);
        }

        if (d->band[2]) {
          limit[2][0] = d->band[2];
          limit[2][1] = d->band[2];
          q_use_cache (f->fid,2,&ret);
          q_lio_cbrick (f->fid,1,limit,bbuf,&ret);
          if (ret) {
            strcpy (GBLerr.string,"Unable to read from file ");
            strcat (GBLerr.string,f->name);
            u_error ("READFILE",GBLerr.string,-1,1);
            QviewExit ();
          }
          StretchLine (bbuf,dsp,2);
        }
      }

      if (xinfo.vinfo.class == PseudoColor) {
        if (d->band[1] || d->band[2]) {
          for (xsamp=0; xsamp<(int)dwidth; xsamp++) {
            isamp = (int) d->samplut[xsamp];
            if ((isamp < 1) || (isamp > f->ns)) {
              *ptr++ = BlackPixelOfScreen (xinfo.screen);
            }
            else {
              index = rbuf[isamp] >> 3;
              index |= (gbuf[isamp] >> 3) << 5; 
              index |= (bbuf[isamp] >> 3) << 10; 
              index = d->pseudolut[index];
              *ptr++ = d->pseudomap[index][3];
            }
          }
        }
        else {
          for (xsamp=0; xsamp<(int)dwidth; xsamp++) {
            isamp = (int) d->samplut[xsamp];
            if ((isamp < 1) || (isamp > f->ns)) {
              *ptr++ = d->map[0];
            }
            else {
              *ptr++ = d->map[buf1[isamp-1]];
            }
          }
        } 
      } 
      else {
        for (xsamp=0; xsamp<(int)dwidth; xsamp++) {
          isamp = (int) d->samplut[xsamp];
          if ((isamp < 1) || (isamp > f->ns)) {
            *ptr++ = map1[0];
            *ptr++ = map2[0];
            *ptr++ = map3[0];
          }
          else {
            *ptr++ = map1[buf1[isamp-1]];
            *ptr++ = map2[buf2[isamp-1]];
            *ptr++ = map3[buf3[isamp-1]];
          }
        }
      }
    }

    if (xline >= wline) {
      XPutImage (xinfo.display,d->pmap,xinfo.imgGC,d->ximg,
                 0,sline,0,sline,(int)dwidth,xline-sline+1);
      XCopyArea (xinfo.display,d->pmap,win,xinfo.imgGC,
                 0,sline,(int)dwidth,xline-sline+1,0,sline);
      sline = xline + 1;
      wline += dheight / 10;
      XSync (xinfo.display,False);
    }
  }

  if (xline > sline) {  
    XPutImage (xinfo.display,d->pmap,xinfo.imgGC,d->ximg,
               0,sline,0,sline,(int)dwidth,(xline-1)-sline+1);
    XCopyArea (xinfo.display,d->pmap,win,xinfo.imgGC,
               0,sline,(int)dwidth,(xline-1)-sline+1,0,sline);
    XSync (xinfo.display,False);
  }

/*---------------------------------------------------------------
/  Save pixmap to temp file if needed.
/--------------------------------------------------------------*/

  if (d->save_tmp && d->world) {

    rewind (d->fill_fp);
    setvbuf (d->fill_fp,NULL,_IOFBF,30000);
    nbytes = xinfo.height * xinfo.width * BYTEDEPTH;
    bytes = fwrite (d->ximgptr,sizeof(char),nbytes,d->fill_fp);
    if (ferror(d->fill_fp)) printf ("Error in writing\n");
    d->save_tmp = False;
    d->read_tmp = True;
  }
/*
  d->read_tmp = False;
*/
  QviewWatch (xinfo.topShell,False);

  UpdateInfoLB (dsp);

  return;
}






