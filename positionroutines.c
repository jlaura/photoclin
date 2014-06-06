#include "pc2d.h"
#include "spi.h"
#include "lev.h"

void ConvertPixel (char *str, void *buf, int type, FLOAT4 *scale);

void EHpositionDA (Widget id,
                   XtPointer client_data,
                   XEvent *call_data,
                   Boolean *dispatch)

{
  int dsp;
  DSPINFO *d;
  int x,y;
  int line,samp;
  INT4 limit[3][2];
  INT4 ret;
  FLOAT4 lat,lon;
  FLOAT4 rsamp,rline;
  char *wptr, *wptr2, *wptr3,*wptr4;
  XmString xstring;
  int sdsp,edsp;
  Boolean linked;
  LevData data;

  dsp = (int) client_data;

  if (GBLdsp.linked[dsp]) {
    sdsp = 0;
    edsp = V_MAXDSP-1;
    linked = True;
  }
  else {
    sdsp = dsp;
    edsp = dsp;
    linked = False;
  }

  for (dsp=sdsp; dsp<=edsp; dsp++) {
    if (!GBLdsp.used[dsp]) continue;
    if ((linked) && (!GBLdsp.linked[dsp])) continue;

    d = &GBLdsp.d[dsp];
    if (linked) d->showpos = True;

    wptr = (char *) GBLwork.work[0];
    wptr2 = (char *) GBLwork.work[1];
    wptr3 = (char *) GBLwork.work[2];
    wptr4 = (char *) GBLwork.work[3];

    if (call_data->type == MotionNotify) {
      x = ((XMotionEvent *)call_data)->x;
      y = ((XMotionEvent *)call_data)->y;
      /*
      d->tool_x = x;
      d->tool_y = y;
      */
      if ((x < 0) || (y < 0) || (x >= xinfo.width) || (y >= xinfo.height)) {
        line = -1;
	samp = -1;
      }
      else {
        line = d->linelut[y];
        samp = d->samplut[x];
      }

      if ((line < 1) || (samp < 1) || (line > d->f.nl) || (samp > d->f.ns)) {
/*        strcpy (wptr,"Samp:Line=N/A");*/
        strcpy (wptr,"N/A");
/*        strcpy (wptr2,"Pixel=N/A");*/
        strcpy (wptr2,"N/A");
/*        strcpy (wptr4,"Lat:Lon=N/A");*/
        strcpy (wptr4,"N/A");
      }
      else {
/*        sprintf (wptr,"Samp:Line=%d:%d",samp,line);*/
        sprintf (wptr,"%d:%d",samp,line);
        if (d->f.printlat) {
	  if (!d->f.use_level) {
            rline = (FLOAT4) d->linelut[y];
            rsamp = (FLOAT4) d->samplut[x];
            spi_mfindlat ((INT4)(dsp+1),rsamp,rline,&lat,&lon,&ret);
          }
          else if (d->f.use_level == 1) {
            data.line = d->linelut[y];
            data.samp = d->samplut[x];
            ret = lev1_linesamp_to_latlon (&d->f.spi,&data); 
            lat = data.lat; 
            lon = data.lon;
          }
          else if (d->f.use_level == 2) {
            data.line = d->linelut[y];
            data.samp = d->samplut[x];
            ret = lev2_linesamp_to_latlon (&d->f.map,&data); 
            lat = data.lat; 
            lon = data.lon;
          }
          else {
            ret = -1;
          }

          if (ret == 0) {
/*            sprintf (wptr4,"Lat:Lon=%f:%f",lat,lon);*/
            sprintf (wptr4,"%f:%f",lat,lon);
          }
          else {
/*            strcpy (wptr4,"Lat:Lon=N/A");*/
            strcpy (wptr4,"N/A");
          }
        }
        else {
/*          strcpy (wptr4,"Lat:Lon=N/A");*/
          strcpy (wptr4,"N/A");
        }

/*        strcpy (wptr2,"Pixel=");*/
        limit[0][0] = limit[0][1] = samp;
        limit[1][0] = limit[1][1] = line;
        if (d->band[0]) {
          limit[2][0] = limit[2][1] = d->band[0];
          q_use_cache (d->f.fid,0,&ret);
          q_lio_cbrick (d->f.fid,1,limit,GBLwork.big,&ret);
          if (ret) {
            strcpy (GBLerr.string,"Unable to read from file ");
            strcat (GBLerr.string,d->f.name);
            u_error ("READFILE",GBLerr.string,-1,1);
            QviewExit ();
          }
          ConvertPixel (wptr3, GBLwork.big,d->f.type,d->f.scale);
/*          strcat (wptr2,wptr3);*/
          strcpy (wptr2,wptr3);
        }
        else {
/*          strcat (wptr2,"N/A");*/
          strcpy (wptr2,"N/A");
        }

        if (d->band[1]) {
          limit[2][0] = limit[2][1] = d->band[1];
          q_use_cache (d->f.fid,1,&ret);
          q_lio_cbrick (d->f.fid,1,limit,GBLwork.big,&ret);
          if (ret) {
            strcpy (GBLerr.string,"Unable to read from file ");
            strcat (GBLerr.string,d->f.name);
            u_error ("READFILE",GBLerr.string,-1,1);
            QviewExit ();
          }
/*          strcat (wptr2,":");*/
          ConvertPixel (wptr3, GBLwork.big,d->f.type,d->f.scale);
/*          strcat (wptr2,wptr3);*/
          strcpy (wptr2,wptr3);
        }
        else if (d->band[2]) {
/*          strcat (wptr2,":N/A");*/
          strcpy (wptr2,"N/A");
        }

        if (d->band[2]) {
          limit[2][0] = limit[2][1] = d->band[2];
          q_use_cache (d->f.fid,2,&ret);
          q_lio_cbrick (d->f.fid,1,limit,GBLwork.big,&ret);
          if (ret) {
            strcpy (GBLerr.string,"Unable to read from file ");
            strcat (GBLerr.string,d->f.name);
            u_error ("READFILE",GBLerr.string,-1,1);
            QviewExit ();
          }
/*          strcat (wptr2,":");*/
          ConvertPixel (wptr3, GBLwork.big,d->f.type,d->f.scale);
/*          strcat (wptr2,wptr3);*/
          strcpy (wptr2,wptr3);
        }
        else if (d->band[1]) {
/*          strcat (wptr2,":N/A");*/
          strcpy (wptr2,"N/A");
        }
      }

      xstring = XmStringCreateSimple (wptr);
      XtVaSetValues (d->posLB,XmNlabelString,xstring,NULL);
      XmStringFree (xstring);
      xstring = XmStringCreateSimple (wptr2);
      XtVaSetValues (d->dnLB,XmNlabelString,xstring,NULL);
      XmStringFree (xstring);
      xstring = XmStringCreateSimple (wptr4);
      XtVaSetValues (d->latLB,XmNlabelString,xstring,NULL);
      XmStringFree (xstring);
    }
    else if (call_data->type == LeaveNotify) {
/*      strcpy (wptr,"Samp:Line=N/A");*/
      strcpy (wptr,"N/A");
/*      strcpy (wptr2,"Pixel=N/A");*/
      strcpy (wptr2,"N/A");
/*      strcpy (wptr4,"Lat:Lon=N/A");*/
      strcpy (wptr4,"N/A");

      xstring = XmStringCreateSimple (wptr);
      XtVaSetValues (d->posLB,XmNlabelString,xstring,NULL);
      XmStringFree (xstring);
      xstring = XmStringCreateSimple (wptr2);
      XtVaSetValues (d->dnLB,XmNlabelString,xstring,NULL);
      XmStringFree (xstring);
      xstring = XmStringCreateSimple (wptr4);
      XtVaSetValues (d->latLB,XmNlabelString,xstring,NULL);
      XmStringFree (xstring);
    }
  }

  return;
}



void EHpositionBoxDA (Widget id,
                      XtPointer client_data,
                      XEvent *call_data,
                      Boolean *dispatch)

{
  int dsp = (int) client_data;
  static int ex,ey;
  int sx,sy;
  DSPINFO *d;

  if (call_data->type == MotionNotify) {
    d = &GBLdsp.d[dsp];
    sx = d->tool_sx;
    sy = d->tool_sy;

    if (d->tool_boxon) {
      XCopyArea (xinfo.display,d->pmap,XtWindow(d->DA),xinfo.imgGC,
                 (sx<ex) ? sx : ex, (sy<ey) ? sy : ey,
                 abs (ex-sx)+1, abs(ey-sy) + 1,
                 (sx<ex) ? sx : ex, (sy<ey) ? sy : ey);
      d->tool_boxon = False;
    }

    ex = ((XMotionEvent *)call_data)->x;
    ey = ((XMotionEvent *)call_data)->y;

    if (abs (sx - ex) > 3 || abs (sy - ey) > 3) {
      XSetForeground (xinfo.display,xinfo.imgGC,GBLcolor.red.pixel);
      d->tool_boxon = True;

      XDrawRectangle (xinfo.display,XtWindow(d->DA),xinfo.imgGC,
                      (sx<ex) ? sx : ex, (sy<ey) ? sy : ey,
                      abs (ex-sx), abs(ey-sy));
    }
  }
  else if (call_data->type == LeaveNotify) {
    d = &GBLdsp.d[dsp];
    sx = d->tool_sx;
    sy = d->tool_sy;

    XtRemoveEventHandler (d->DA,PointerMotionMask|LeaveWindowMask,False,
                          EHpositionBoxDA,(XtPointer)dsp);

    if (d->tool_boxon) {
      XCopyArea (xinfo.display,d->pmap,XtWindow(d->DA),xinfo.imgGC,
                 (sx<ex) ? sx : ex, (sy<ey) ? sy : ey,
                 abs (ex-sx)+1, abs(ey-sy) + 1,
                 (sx<ex) ? sx : ex, (sy<ey) ? sy : ey);
    }

    d->tool_boxon = False;
    d->tool_sx = -1;
    d->tool_sy = -1;
  }

  return;
}



void ConvertPixel (char *str, void *buf, int type, FLOAT4 *scale)

{
  FLOAT4 dn;

  if (type == 1) {
    if (*((UINT1 *) buf) < VALID_MIN1) {
      strcpy (str,"NUL");
    }
    else if (*((UINT1 *) buf) > VALID_MAX1) {
      strcpy (str,"HRS");
    }
    else {
      dn = *((UINT1 *) buf);
      sprintf (str,"%g",scale[0]+scale[1]*dn);
    }
  }
  else if (type == 2) {
    if (*((INT2 *) buf) < VALID_MIN2) {
      if (*((INT2 *) buf) == NULL2) {
        strcpy (str,"NUL");
      }
      else if (*((INT2 *) buf) == LOW_REPR_SAT2) {
        strcpy (str,"LRS");
      }
      else if (*((INT2 *) buf) == LOW_INSTR_SAT2) {
        strcpy (str,"LIS");
      }
      else if (*((INT2 *) buf) == HIGH_REPR_SAT2) {
        strcpy (str,"HRS");
      }
      else if (*((INT2 *) buf) == HIGH_INSTR_SAT2) {
        strcpy (str,"HIS");
      }
      else {
        strcpy (str,"ERR");
      }
    }
    else {
      dn = *((INT2 *) buf);
      sprintf (str,"%g",scale[0]+scale[1]*dn);
    }
  }
  else if (type == 3) {
    if (*((FLOAT4 *) buf) < VALID_MIN4) {
      if (*((FLOAT4 *) buf) == NULL4) {
        strcpy (str,"NUL");
      }
      else if (*((FLOAT4 *) buf) == LOW_REPR_SAT4) {
        strcpy (str,"LRS");
      }
      else if (*((FLOAT4 *) buf) == LOW_INSTR_SAT4) {
        strcpy (str,"LIS");
      }
      else if (*((FLOAT4 *) buf) == HIGH_REPR_SAT4) {
        strcpy (str,"HRS");
      }
      else if (*((FLOAT4 *) buf) == HIGH_INSTR_SAT4) {
        strcpy (str,"HIS");
      }
      else {
        strcpy (str,"ERR");
      }
    }
    else {
      dn = *((FLOAT4 *) buf);
      sprintf (str,"%g",dn);
    }
  }
  else {
   strcpy (str,"ERR");
  }

  return;
}




void EHpositionLineDA (Widget id,
                       XtPointer client_data,
                       XEvent *call_data,
                       Boolean *dispatch)

{
  int dsp = (int) client_data;
  static int ex,ey;
  static int minx,maxx,miny,maxy;
  int sx,sy;
  int x,y;
  DSPINFO *d;
  int *xbuf,*ybuf;
  int *lbuf,*sbuf;
  int tsx,tsy,tex,tey;
  XPoint *points;
  int i,npts,apts;
  int line,samp;
  int sl,ss,el,es;

  if (call_data->type == MotionNotify) {
    d = &GBLdsp.d[dsp];
    sx = d->tool_sx;
    sy = d->tool_sy;

    if (d->tool_boxon) {
      if (d->inc < 1) {
        XCopyArea (xinfo.display,d->pmap,XtWindow(d->DA),xinfo.imgGC,
                   minx, miny, maxx - minx + 1, maxy - miny + 1,
                   minx, miny);
      }
      else {
        XCopyArea (xinfo.display,d->pmap,XtWindow(d->DA),xinfo.imgGC,
                   (sx<ex) ? sx : ex, (sy<ey) ? sy : ey,
                   abs (ex-sx)+1, abs(ey-sy) + 1,
                   (sx<ex) ? sx : ex, (sy<ey) ? sy : ey);
      }
      d->tool_boxon = False;
    }

    ex = ((XMotionEvent *)call_data)->x;
    ey = ((XMotionEvent *)call_data)->y;

    XSetForeground (xinfo.display,xinfo.imgGC,GBLcolor.red.pixel);
    d->tool_boxon = True;

    if (d->inc >= 1.0) {
      points = (XPoint *) GBLwork.big;
      xbuf = (int *) GBLwork.work[0];
      ybuf = (int *) GBLwork.work[1];
      LineToPoints (xbuf,ybuf,&npts,sx,sy,ex,ey);
      apts = 0;
      for (i=0; i<npts; i++) {
        line = d->linelut[ybuf[i]];
        samp = d->samplut[xbuf[i]];
        if (line < 1) continue;
        if (line > d->f.nl) continue;
        if (samp < 1) continue;
        if (samp > d->f.ns) continue;
        points->x = xbuf[i];
        points->y = ybuf[i];
        points++;
        apts++;
      }
      XDrawPoints (xinfo.display,XtWindow(d->DA),xinfo.imgGC,
                   (XPoint *)GBLwork.big,apts,CoordModeOrigin);
    }
    else {
      sl = d->linelut[sy];
      el = d->linelut[ey];
      ss = d->samplut[sx];
      es = d->samplut[ex];
      lbuf = (int *) GBLwork.work[0];
      sbuf = (int *) GBLwork.work[1];
      LineToPoints (lbuf,sbuf,&npts,sl,ss,el,es);

      miny = xinfo.height;
      maxy = -1;
      minx = xinfo.width;
      maxx = -1;

      for (i=0; i<npts; i++) {
        line = lbuf[i];
        samp = sbuf[i];
        if (line < 1) continue;
        if (samp < 1) continue;
        if (line > d->f.nl) continue;
        if (samp > d->f.ns) continue;

        if (sx < ex) {
          for (x=sx; x<=ex; x++) if ((int)d->samplut[x] == samp) break;
        }
        else {
          for (x=ex; x<=sx; x++) if ((int)d->samplut[x] == samp) break;
        }

        if (sy<ey) {
          for (y=sy; y<=ey; y++) if ((int)d->linelut[y] == line) break;
        }
        else {
          for (y=ey; y<=sy; y++) if ((int)d->linelut[y] == line) break;
        }

        tsx = x;
        while ((tsx >=0) && ((int)d->samplut[tsx] == samp)) tsx--;
        tsx++;

        tsy = y;
        while ((tsy >=0) && ((int)d->linelut[tsy] == line)) tsy--;
        tsy++;
         
        tex = x;
        while ((tex < xinfo.width) && ((int)d->samplut[tex] == samp)) tex++;
        tex--;

        tey = y;
        while ((tey < xinfo.height) && ((int)d->linelut[tey] == line)) tey++;
        tey--;

        if (tsx < minx) minx = tsx;
        if (tex > maxx) maxx = tex;
        if (tsy < miny) miny = tsy;
        if (tey > maxy) maxy = tey;

        XFillRectangle (xinfo.display,XtWindow(d->DA),xinfo.imgGC,tsx,tsy,
                        (unsigned int) (tex-tsx+1),(unsigned int)(tey-tsy+1));
      }
    }
  }
  else if (call_data->type == LeaveNotify) {
    d = &GBLdsp.d[dsp];
    sx = d->tool_sx;
    sy = d->tool_sy;

    XtRemoveEventHandler (d->DA,PointerMotionMask|LeaveWindowMask,False,
                          EHpositionLineDA,(XtPointer)dsp);

    if (d->tool_boxon) {
      XCopyArea (xinfo.display,d->pmap,XtWindow(d->DA),xinfo.imgGC,
                 (sx<ex) ? sx : ex, (sy<ey) ? sy : ey,
                 abs (ex-sx)+1, abs(ey-sy) + 1,
                 (sx<ex) ? sx : ex, (sy<ey) ? sy : ey);
    }

    d->tool_boxon = False;
    d->tool_sx = -1;
    d->tool_sy = -1;
  }

  return;
}


