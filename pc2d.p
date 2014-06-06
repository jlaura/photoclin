/**********************************
* Prototypes needed for pc2d
**********************************/

void CBqviewHelp (Widget id, XtPointer client_data, XtPointer call_data);
void CBqviewDestroyWidget (Widget id,XtPointer client_data,XtPointer call_data);
void CBqviewCancelExitPB (Widget id,XtPointer client_data,XtPointer call_data);

void QviewMessage (int dsp);
void QviewExit (void);
void QviewWatch (Widget shell, Boolean add);
/*void QviewResources (void);*/
Boolean QviewResourceErrors (XtPointer client_data);

void BlinkStart (void);
void StretchReset (int dsp);

void LoadStrLut (int dsp);
void CalcAutoStr (int dsp);
void LoadPseudoBW (DSPINFO *d);
void LoadPseudoColor (DSPINFO *d);

Boolean OpenCube (int dsp, char *ptr);
void CreateDisplay (void);
void LoadCube (int dsp);

void LineToPoints (int *xbuf, int*ybuf, int *npts, 
                   int sx, int sy, int ex, int ey);

Widget CreateDisplayMenu (Widget MW);

void CBbrowseFSBokPB (Widget id, XtPointer client_data, XtPointer call_data);
void CBbrowseFSBcancelPB(Widget id, XtPointer client_data, XtPointer call_data);

void CBinputDA (Widget id, XtPointer client_data, XtPointer call_data);
void CBresizeDA (Widget id, XtPointer client_data, XtPointer call_data);
void CBexposeDA (Widget id, XtPointer client_data, XtPointer call_data);
void Limbexpose (int dsp);

Widget CreateToolBar (Widget FM);
void CBtoolBarPB (Widget id, XtPointer client_data, XtPointer call_data);

void ZoomIn (int dsp);
void ZoomOut (int dsp);
void ZoomCenter (int dsp);
void ZoomFill (int dsp);
void Zoom1X (int dsp);

void Roam (int dsp);
void RoamLeft (int dsp);
void RoamRight (int dsp);
void RoamUp (int dsp);
void RoamDown (int dsp);
void RoamNE (int dsp);
void RoamNW (int dsp);
void RoamSE (int dsp);
void RoamSW (int dsp);

void UpdateInfoLB (int dsp);

int percent8 (INT4 *hist, double per, INT4 vmin, INT4 vmax, INT4 *dn);
int percent16 (INT4 *hist, double per, INT4 vmin, INT4 vmax, INT4 *dn);
void StretchLine (UINT1 *buf, int dsp, int chan);

void StretchArea (int dsp, int sx, int sy, int ex, int ey);

void EHpositionDA (Widget id,XtPointer client_data,XEvent *call_data,
                   Boolean *dispatch);
void EHpositionBoxDA (Widget id,XtPointer client_data,XEvent *call_data,
                      Boolean *dispatch);
void EHpositionLineDA (Widget id,XtPointer client_data,XEvent *call_data,
                       Boolean *dispatch);

void ListArea (int dsp);
int ListGetStats (int dsp, int band);

void DoctorImagePixel (int dsp);
void DoctorImageLine (int dsp);
void DoctorImageHorzLine (int dsp);
void DoctorImageUndo (int dsp);
void DoctorImageBox (int dsp);
void DoctorPickDN (int dsp);

void CreateListDB (int dsp);
void CBlistDBexitPB (Widget id, XtPointer client_data, 
                          XtPointer call_data);
void CBlistDBnextPB (Widget id, XtPointer client_data, 
                          XtPointer call_data);
void CBlistDBsavePB (Widget id, XtPointer client_data, 
                          XtPointer call_data);
void LimbInit (int dsp);
void SaveLimbPoint (int dsp);
void DrawLimb(int dsp);
void CreateMeasureDB(int dsp);
void Measure (int dsp);
void Measure_ctr (int dsp);

void ReseauInit (int dsp);
void ReseauBox (int dsp);
void SelectReseau (int dsp);
void SaveReseau (int dsp);
void Reseau2Label (int dsp);

void LoadColorMap (DSPINFO *d);
void medcut (DSPINFO *d);


void DrawTic (int dsp,double samp,double line,int color);
void DrawBox (int dsp,int ss,int es,int sl,int el);

void PlotClose (int dsp);
void PlotTable (int dsp);

Boolean PreLoadCube (int i,char *ptr,float min,float max,Boolean str,
                     Boolean setworld);
