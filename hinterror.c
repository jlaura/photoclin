#include "pc2d.h"
#include "pcp.h"
#include <Xm/Label.h>

void hinterror (CHAR *ptext1,CHAR *ptext2,INT4 ptype)

/***********************************************************************
*
*_Title hinterror - Write text to specified label field
*
*_DESC  Write text to the specified label field in the readout portion
*       of the main screen:  
*       If ptype=0, text goes to the hint field. 
*       If ptype=1, text goes to the error field.
*       If ptype=2, text goes to the RMS righthand side,B error field.
*       If ptype=3, text goes to the Dip,Az of dip field.
*       If ptype=4, text goes to the Slope of plane field.
*       If ptype=5, text goes to the RMS residual in meters,pixels field.
*
*_HIST  July 27 2001 Janet Barrett, Original version
*_END
************************************************************************/
{
  XmString xstring;
  XColor closest;
  XColor exact;
  char *sptr;

  sptr = (char *) GBLwork.work[0];

/*  XAllocNamedColor(xinfo.display,xinfo.cmap,"green",
                   &closest,&exact);*/

  if (strlen(ptext1) != 0) (void) u_strtrim(ptext1,ptext1);
  if (strlen(ptext2) != 0) (void) u_strtrim(ptext2,ptext2);
  if (ptype == 0) {
    sprintf(sptr,"Hints/Errors:");
    strcat(sptr,ptext1);
    xstring = XmStringCreateSimple(sptr);
    XtVaSetValues (hintsLB,XmNlabelString,xstring,
/*                           XmNforeground,closest.pixel,*/
			   NULL);
  } else if (ptype == 1) {
    sprintf(sptr,"Errors:");
    strcat(sptr,ptext1);
    xstring = XmStringCreateSimple(sptr);
    XtVaSetValues (errorsLB,XmNlabelString,xstring,NULL);
  } else if (ptype == 2) {
    sprintf(sptr,"RMS righthand side,B error=");
    strcat(sptr,ptext1);
    strcat(sptr,ptext2);
    xstring = XmStringCreateSimple(sptr);
    XtVaSetValues (rmsrhsberLB,XmNlabelString,xstring,NULL);
  } else if (ptype == 3) {
    sprintf(sptr,"Dip,Az of dip (degrees)=");
    strcat(sptr,ptext1);
    xstring = XmStringCreateSimple(sptr);
    XtVaSetValues (dipLB,XmNlabelString,xstring,NULL);
  } else if (ptype == 4) {
    sprintf(sptr,"Slope of plane toward Sun (degrees)=");
    strcat(sptr,ptext1);
    xstring = XmStringCreateSimple(sptr);
    XtVaSetValues (plnslpLB,XmNlabelString,xstring,NULL);
  } else if (ptype == 5) {
    sprintf(sptr,"RMS residual to fit (m,pixels)=");
    strcat(sptr,ptext1);
    strcat(sptr,ptext2);
    xstring = XmStringCreateSimple(sptr);
    XtVaSetValues (rmsresLB,XmNlabelString,xstring,NULL);
  }

  XmUpdateDisplay (xinfo.topShell);
  XmStringFree(xstring);

  return;
}
