#include "pc2d.h"
#include <Xm/DrawingA.h>
#include <X11/keysym.h>

void CBinputDA (Widget id, XtPointer client_data, XtPointer call_data)

/***********************************************************************
*
*_Title CBinputDA - callback for mouse button events in the drawing area
*
*_Args  Type            Variable        I/O     Description
*_Parm  Widget          id              I       See Xt toolkit reference 
*                                               manual
*_Parm  XtPointer       client_data     I       Display number
*_Parm  XtPointer       call_data       I       See Xt toolkit reference
*                                               manual
 
*_DESC  This routine is called when the user presses mouse buttons or
*       keyboard keys in the drawing area.
*
*_HIST  Jan 01 1996 Jeff Anderson, Original version
*       Dec 19 1996 Tracie Sucharski, Added capability of moving cursor
*           using the arrow keys and pressing 1,2 or 3 to simulate the
*           mouse buttons.
*       Dec 19 1997 Tracie Sucharski, Added limbfit capability
*       Jan 29 1998 TLS, Save filled display to temporary file.
*       Mar 07 1998 TLS, Added measuring tool.
*       May 12 1998 TLS, Added call to DoctorPickDN for the middle
*                         mouse button in doctor mode.
*       Nov 12 1998 TLS, If reseau application is active, redraw
*                         boxes around reseaus.
*       Feb 08 2002 Janet Barrett, Modified to work with the pc2d
*                   program.
*    
*_END
************************************************************************/
 
{
  int dsp = (int) client_data;
  XmDrawingAreaCallbackStruct *s;
  char string[256];
  int button,state;
  DSPINFO *d;
  Dimension width,height;
  int sx,sy,ex,ey;
  double linc,sinc;
  double sl,ss,el,es;
  int cdsp;
  KeySym keysym;
  Window root, child;
  int root_x, root_y, da_x, da_y;
  unsigned int key_buttons;

  void SaveRegPoint (void);

  s = (XmDrawingAreaCallbackStruct *) call_data;
  d = &GBLdsp.d[dsp];
  
  if (s->reason != XmCR_INPUT) return;
  
  if (s->event->type == KeyPress)
    XLookupString ((XKeyEvent *)s->event,string,sizeof(string),&keysym,NULL);
  
  if (s->event->type == KeyPress && (keysym != XK_1 && keysym != XK_2 &&
				     keysym != XK_3) ) {
    switch (keysym) {
      case XK_S :
      case XK_s :
        if (dsp == 0) SaveRegPoint ();
      break;

      case XK_Up :
        if ( XQueryPointer (xinfo.display, XtWindow(d->DA), &root, &child,
			  &root_x, &root_y, &da_x, &da_y, &key_buttons) )
	   XWarpPointer (xinfo.display, XtWindow(d->DA), None, 1, 1, 0, 0,
		      0, -1);
      break;
      case XK_Down :
        if ( XQueryPointer (xinfo.display, XtWindow(d->DA), &root, &child,
			  &root_x, &root_y, &da_x, &da_y, &key_buttons) )
	  XWarpPointer (xinfo.display, XtWindow(d->DA), None, 1, 1, 0, 0,
		      0, 1);
      break;
      case XK_Left :
        if ( XQueryPointer (xinfo.display, XtWindow(d->DA), &root, &child,
			  &root_x, &root_y, &da_x, &da_y, &key_buttons) )
	  XWarpPointer (xinfo.display, XtWindow(d->DA), None, 1, 1, 0, 0,
		      -1, 0);
      break;
      case XK_Right :
        if ( XQueryPointer (xinfo.display, XtWindow(d->DA), &root, &child,
			  &root_x, &root_y, &da_x, &da_y, &key_buttons) )
	  XWarpPointer (xinfo.display, XtWindow(d->DA), None, 1, 1, 0, 0,
		      1, 0);
      break;
    }
  }
  else if (s->event->type == ButtonPress) {
    button = ((XButtonEvent *)(s->event))->button;
    state = ((XButtonEvent *)(s->event))->state;
    if (button == 1 && state == 0) {
      if ((d->curtool == V_ZOOM_TOOL) || (d->curtool == V_STRETCH_TOOL) ||
	  ((d->curtool == V_DOCTOR_TOOL) && (d->rx_rect))) {
	d->tool_sx = ((XButtonEvent *)(s->event))->x;
	d->tool_sy = ((XButtonEvent *)(s->event))->y;
	d->tool_boxon = False;
	XtAddEventHandler (d->DA,PointerMotionMask|LeaveWindowMask,False,
			   EHpositionBoxDA,(XtPointer)dsp);
      }
      else if ((d->curtool == V_DOCTOR_TOOL) && (d->rx_se_line)) {
	d->tool_sx = ((XButtonEvent *)(s->event))->x;
	d->tool_sy = ((XButtonEvent *)(s->event))->y;
	d->tool_boxon = False;
	XtAddEventHandler (d->DA,PointerMotionMask|LeaveWindowMask,False,
			   EHpositionLineDA,(XtPointer)dsp);
      }
      else if (d->curtool == V_MEASURE_TOOL) {
	if (!d->measure_ctr) {
	  d->tool_sx = ((XButtonEvent *)(s->event))->x;
	  d->tool_sy = ((XButtonEvent *)(s->event))->y;
	  d->tool_boxon = False;
	  XtAddEventHandler (d->DA,PointerMotionMask|LeaveWindowMask,False,
			   EHpositionLineDA,(XtPointer)dsp);
	}
      }

    }
  }
  else if (s->event->type == ButtonRelease || ( (s->event->type == KeyPress) &&
		    (keysym == XK_1 || keysym == XK_2 || keysym == XK_3) ) ) {
    if (s->event->type == ButtonRelease) {
      button = ((XButtonEvent *)(s->event))->button;
      state = ((XButtonEvent *)(s->event))->state;
      d->tool_x = ((XButtonEvent *)(s->event))->x;
      d->tool_y = ((XButtonEvent *)(s->event))->y;
    }
    else if (s->event->type == KeyPress) {
      d->tool_x = ((XKeyEvent *)(s->event))->x;
      d->tool_y = ((XKeyEvent *)(s->event))->y;
      d->tool_sx = d->tool_x;
      d->tool_sy = d->tool_y;
    }
    
    if (d->tool_x < 0) return;
    if (d->tool_y < 0) return;
    if (d->tool_x >= xinfo.width) return;
    if (d->tool_y >= xinfo.height) return;
    d->zoom_savex = d->tool_x;
    d->zoom_savey = d->tool_y;
    
    if ((state & ControlMask) == 0) {

      if (button == 1 || keysym == XK_1) {
	if (d->curtool == V_ZOOM_TOOL) {
	  d->world = False;
	  XtRemoveEventHandler (d->DA,PointerMotionMask|LeaveWindowMask,False,
				EHpositionBoxDA,(XtPointer)dsp);
	  if (d->tool_boxon) {
	    sx = d->tool_sx;
	    sy = d->tool_sy;
	    ex = d->tool_x;
	    ey = d->tool_y;
	    XCopyArea (xinfo.display,d->pmap,XtWindow(d->DA),xinfo.imgGC,
		       (sx<ex) ? sx : ex, (sy<ey) ? sy : ey,
		       abs (ex-sx)+1, abs(ey-sy) + 1,
		       (sx<ex) ? sx : ex, (sy<ey) ? sy : ey);

	    sl = d->linelut[(sy<ey) ? sy : ey];
	    el = d->linelut[(sy>ey) ? sy : ey];
	    ss = d->samplut[(sx<ex) ? sx : ex];
	    es = d->samplut[(sx>ex) ? sx : ex];

	    XtVaGetValues (d->DA,XmNwidth,&width,XmNheight,&height,NULL);
	    linc = (el - sl + 1.0) / (double) height;
	    sinc = (es - ss + 1.0) / (double) width;
	    d->inc = linc;
	    if (linc < sinc) d->inc = sinc;
	    d->sl = sl;
	    d->ss = ss;

	    LoadCube (dsp);  

	    if (GBLdsp.linked[dsp]) {
	      cdsp = dsp;
	      for (dsp=0; dsp<V_MAXDSP; dsp++) {
		if (!GBLdsp.used[dsp]) continue;
		if (!GBLdsp.linked[dsp]) continue;
		if (dsp == cdsp) continue;

		d = &GBLdsp.d[dsp];
		sl = d->linelut[(sy<ey) ? sy : ey];
		el = d->linelut[(sy>ey) ? sy : ey];
		ss = d->samplut[(sx<ex) ? sx : ex];
		es = d->samplut[(sx>ex) ? sx : ex];

		XtVaGetValues (d->DA,XmNwidth,&width,XmNheight,&height,NULL);
		linc = (el - sl + 1.0) / (double) height;
		sinc = (es - ss + 1.0) / (double) width;
		d->inc = linc;
		if (linc < sinc) d->inc = sinc;
		d->sl = sl;
		d->ss = ss;

		d->world = False;
		LoadCube (dsp);  
	      }
	    }
	  }
	  else if (d->tool_sx != -1) {
	    ZoomIn (dsp);
	  }
	}
	else if (d->curtool == V_ROAM_TOOL) {
	  Roam (dsp);
	}
	else if (d->curtool == V_STRETCH_TOOL) {
	  d->read_tmp = False;
	  XtRemoveEventHandler (d->DA,PointerMotionMask|LeaveWindowMask,False,
				EHpositionBoxDA,(XtPointer)dsp);
	  if (d->tool_boxon) {
	    sx = d->tool_sx;
	    sy = d->tool_sy;
	    ex = d->tool_x;
	    ey = d->tool_y;
	    XCopyArea (xinfo.display,d->pmap,XtWindow(d->DA),xinfo.imgGC,
		       (sx<ex) ? sx : ex, (sy<ey) ? sy : ey,
		       abs (ex-sx)+1, abs(ey-sy) + 1,
		       (sx<ex) ? sx : ex, (sy<ey) ? sy : ey);

	    StretchArea (dsp, (sx<ex) ? sx : ex, (sy<ey) ? sy : ey,
			 (sx<ex) ? ex : sx, (sy<ey) ? ey : sy);
	  }
	  else if (d->tool_sx != -1) {
	    XtVaGetValues (d->DA,XmNwidth,&width,XmNheight,&height,NULL);
	    StretchArea (dsp,0,0,(int)(width - 1),(int)(height - 1));
	  }
	}
	else if (d->curtool == V_LIST_TOOL) {
	  ListArea (dsp); 
	}
      }
      else if (button == 2 || keysym == XK_2) {
	if (d->curtool == V_ZOOM_TOOL) {
	  d->world = False;
	  Zoom1X (dsp);
	}
      }
      else if (button == 3 || keysym == XK_3) {
	if (d->curtool == V_ZOOM_TOOL) {
	  d->world = False;
	  ZoomOut (dsp);
	}
	else if (d->curtool == V_ROAM_TOOL) {
	  d->world = False;
	  ZoomCenter (dsp);
	}
	else if (d->curtool == V_STRETCH_TOOL) {
	  StretchReset (dsp);
	}
      }

    }

  }
  return;
}



void CBresizeDA (Widget id, XtPointer client_data, XtPointer call_data)

/***********************************************************************
*
*_Title CBresizeDA - Callback when drawing area is resized.
*
*_Args  Type            Variable        I/O     Description
*_Parm  Widget          id              I       See Xt toolkit reference 
*                                               manual
*_Parm  XtPointer       client_data     I       Display number
*_Parm  XtPointer       call_data       I       See Xt toolkit reference
*                                               manual
 
*_DESC  This routine is called when the drawing area window is resized.
*
*_HIST  Jan 01 1996 Jeff Anderson, Original version
*       Jan 06 1998 Tracie Sucharski, Refill when windo is resized.
*       Jan 13 1998 TLS, Check flag to see if full image was displayed.
*       Jan 29 1998 TLS, Set read_tmp to False to indicate that the file
*                     needs to be re-read from disk.
*       Feb 08 2002 Janet Barrett, Modified to work with the pc2d
*                   program.
*       
*    
*_END
************************************************************************/
{
  int dsp = (int) client_data;
  DSPINFO *d;
  Dimension dwidth,dheight;
  double line,samp;
   
  d = &GBLdsp.d[dsp];
  XSetForeground (xinfo.display,xinfo.imgGC,BlackPixelOfScreen(xinfo.screen));
  XFillRectangle (xinfo.display,XtWindow(d->DA),xinfo.imgGC,0,0,
                  xinfo.width,xinfo.height); 
  XSync (xinfo.display,False);

  d->read_tmp = False;

  XtVaGetValues (d->DA,XmNwidth,&dwidth,XmNheight,&dheight,NULL);
  if (d->world) {
    d->sl = d->scs_sl;
    d->ss = d->scs_ss;
    d->inc = (double) ((d->scs_el - d->scs_sl + 1)) / (double)dheight;
    if (d->inc < ((double) (d->scs_es - d->scs_ss + 1) / (double)dwidth)) {
      d->inc = (double) (d->scs_es - d->scs_ss + 1) / (double)dwidth;
    }
  }
  else {
    samp = d->zoom_savex;
    line = d->zoom_savey;
    d->sl = line - d->inc * (double) dheight / 2.0;
    d->ss = samp - d->inc * (double) dwidth / 2.0;
  }
  LoadCube (dsp);

  return;
}




void CBexposeDA (Widget id, XtPointer client_data, XtPointer call_data)

/***********************************************************************
*
*_Title CBexposeDA - Callback when drawing area is exposed.
*
*_Args  Type            Variable        I/O     Description
*_Parm  Widget          id              I       See Xt toolkit reference 
*                                               manual
*_Parm  XtPointer       client_data     I       Display number
*_Parm  XtPointer       call_data       I       See Xt toolkit reference
*                                               manual
 
*_DESC  This routine is called when the drawing area window is exposed.
*
*_HIST  Jan 01 1996 Jeff Anderson, Original version
*       Dec 09 1997 Tracie Sucharski, If limbfit is active, redraw
*                      necessary graphics such as the tics or the
*                      limb.
*       Nov 12 1998 TLS, If reseau application is active, redraw
*                      boxes around reseaus.
*       Feb 08 2002 Janet Barrett, Modified to work with the pc2d
*                   program.
*    
*_END
************************************************************************/
{
  int dsp = (int) client_data;
  DSPINFO *d;
  Dimension dwidth,dheight;

  d = &GBLdsp.d[dsp];

  if (d->ximg != NULL) {
    XtVaGetValues(d->DA,XmNwidth,&dwidth,XmNheight,&dheight,NULL);
    XCopyArea (xinfo.display,d->pmap,XtWindow(d->DA),xinfo.imgGC,
               0,0,(int)dwidth,(int)dheight,0,0);
    XSync(xinfo.display,False);
  }

  return;
} 
