#include "pc2d.h"

typedef struct {
  int lower;
  int upper;
  int count;
  int level;
  int rmin,rmax;
  int gmin,gmax;
  int bmin,bmax;
} cube_t;

void medcut_shrink (cube_t *cube);
void medcut_hist3d (DSPINFO *d, INT4 *hist);
int medcut_compare (const void *a1, const void *a2);

struct {
  int longdim;
  INT4 histptr[32768];
} GBLmedcut;

void medcut (DSPINFO *d)

{
  cube_t cube,cubea,cubeb;
  cube_t list[256];
  int i,j;
  INT4 *histptr;
  INT4 *hist;
  size_t width,num;
  void *base;
  int level,ncubes,splitpos;
  int count,median,color;
  int lr,lg,lb;
  int longdim;
  int value;
  double rsum,gsum,bsum;
  int range,maxrange;

  QviewWatch (d->shell,True);

/* Get the 3-d histogram */
  hist = d->pseudolut;
  medcut_hist3d (d,hist);

/* Create the initial cube */
  cube.count = 0;
  color = 0;
  histptr = GBLmedcut.histptr;
  for (i=0; i<32768; i++) {
    if (hist[i]) {
      histptr[color++] = i;
      cube.count += hist[i];
    }
  }

  cube.lower = 0;
  cube.upper = color - 1;
  cube.level = 0;
  medcut_shrink (&cube); 

/* Insert initial cube into list of cubes */
  ncubes = 0;
  list[ncubes++] = cube; 

/* Loop and split up cubes in the list until we have enough cubes */
  while (ncubes < d->pseudocells){
    level = 255;
    splitpos = -1;
    maxrange = -1;

    for (i=0; i<ncubes; i++) {
      if (list[i].lower == list[i].upper) continue;
 
    lr = list[i].rmax - list[i].rmin;
    lg = list[i].gmax - list[i].gmin;
    lb = list[i].bmax - list[i].bmin;
    range = lr;
    if (lg > range) range = lg;
    if (lb > range) range = lb;
    if (range > maxrange) {
      splitpos = i;      
      maxrange = range;
    }
   
/* 
      if (list[i].level < level) {
        level = list[i].level;
        splitpos = i;
      }
*/
    }

    if (splitpos == -1) break;

    cube = list[splitpos];
    lr = cube.rmax - cube.rmin;
    lg = cube.gmax - cube.gmin;
    lb = cube.bmax - cube.bmin;

    longdim = 0;
    if (lg >= lr && lg >= lb) longdim = 1;
    if (lb >= lr && lb >= lg) longdim = 2;
    GBLmedcut.longdim = longdim;

    base = (void *) &histptr[cube.lower];
    num = cube.upper - cube.lower + 1;
    width = sizeof (histptr[0]);
    qsort (base, num, width, medcut_compare);

    count = 0;
    for (i=cube.lower; i<cube.upper; i++) {
      if (count >= cube.count/2) break;
      color = histptr[i];
      count += hist[color];
    }
    median = i;

    cubea = cube;
    cubea.upper = median - 1;
    cubea.count = count;
    cubea.level++;
    medcut_shrink (&cubea);
    list[splitpos] = cubea;

    cubeb = cube;
    cubeb.lower = median;
    cubeb.count = cube.count - count;
    cubeb.level++;
    medcut_shrink (&cubeb);
    list[ncubes++] = cubeb;
  }

  for (i=0; i<ncubes; i++) {
    cube = list[i];
    rsum = gsum = bsum = 0.0;
    for (j=cube.lower; j<=cube.upper; j++) {
      color = histptr[j];
      value = (color & 31) << 3;
      rsum += (double) value * (double) hist[color];
      value = ((color >> 5) & 31) << 3;
      gsum += (double) value * (double) hist[color];
      value = ((color >> 10) & 31) << 3;
      bsum += (double) value * (double) hist[color];
    }
    d->pseudomap[i][0] = rsum / cube.count;
    d->pseudomap[i][1] = gsum / cube.count;
    d->pseudomap[i][2] = bsum / cube.count;
  } 

  for (i=0; i<ncubes; i++) {
    cube = list[i];
    for (j=cube.lower; j<=cube.upper; j++) {
      color = histptr[j];
      hist[color] = i;
    }
  }

  QviewWatch (d->shell,False);

  return;
}


void medcut_hist3d (DSPINFO *d, INT4 *hist)

{
  int line,samp,i;
  INT4 limit[3][2];
  UINT1 *rbuf,*gbuf,*bbuf;
  int index;
  INT4 ret;
  int dsp;

  for (dsp=0; dsp<V_MAXDSP; dsp++) {
    if (d == &GBLdsp.d[dsp]) break;
  }

  rbuf = (UINT1 *) GBLwork.work[0];
  gbuf = (UINT1 *) GBLwork.work[1];
  bbuf = (UINT1 *) GBLwork.work[2];

  limit[0][0] = 1;
  limit[0][1] = d->f.ns;
  for (samp=0; samp<d->f.ns; samp++) {
    rbuf[samp] = 0;
    gbuf[samp] = 0;
    bbuf[samp] = 0;
  }

  for (i=0; i<32*32*32; i++) hist[i] = 0;

  for (line=1; line<=d->f.nl; line++) {
    limit[1][0] = line;
    limit[1][1] = line;

    if (d->band[0]) {
      limit[2][0] = d->band[0];
      limit[2][1] = d->band[0];
      q_use_cache (d->f.fid,0,&ret);
      q_lio_cbrick (d->f.fid,1,limit,rbuf,&ret);
      if (ret) {
        strcpy (GBLerr.string,"Unable to read from file ");
        strcat (GBLerr.string,d->f.name);
        u_error ("READFILE",GBLerr.string,-1,1);
        QviewExit ();
      }
      StretchLine (rbuf,dsp,0);
    }

    if (d->band[1]) {
      limit[2][0] = d->band[1];
      limit[2][1] = d->band[1];
      q_use_cache (d->f.fid,1,&ret);
      q_lio_cbrick (d->f.fid,1,limit,gbuf,&ret);
      if (ret) {
        strcpy (GBLerr.string,"Unable to read from file ");
        strcat (GBLerr.string,d->f.name);
        u_error ("READFILE",GBLerr.string,-1,1);
        QviewExit ();
      }
      StretchLine (gbuf,dsp,1);
    }

    if (d->band[2]) {
      limit[2][0] = d->band[2];
      limit[2][1] = d->band[2];
      q_use_cache (d->f.fid,2,&ret);
      q_lio_cbrick (d->f.fid,1,limit,bbuf,&ret);
      if (ret) {
        strcpy (GBLerr.string,"Unable to read from file ");
        strcat (GBLerr.string,d->f.name);
        u_error ("READFILE",GBLerr.string,-1,1);
        QviewExit ();
      }
      StretchLine (bbuf,dsp,2);
    }

    for (samp=0; samp<d->f.ns; samp++) {
      index = rbuf[samp] >> 3;
      index |= (gbuf[samp] >> 3) << 5; 
      index |= (bbuf[samp] >> 3) << 10;
      hist[index]++;
    }
  }

  return;
}


void medcut_shrink (cube_t *cube)

{
  int value;
  int i,color;
  INT4 *histptr;

  histptr = GBLmedcut.histptr;

  cube->rmin = 31; cube->rmax = 0;
  cube->gmin = 31; cube->gmax = 0;
  cube->bmin = 31; cube->bmax = 0;

  for (i=cube->lower; i<=cube->upper; i++) {
    color = histptr[i];

    value = color & 31;
    if (value < cube->rmin) cube->rmin = value;
    if (value > cube->rmax) cube->rmax = value;

    value = (color >> 5) & 31;
    if (value < cube->gmin) cube->gmin = value;
    if (value > cube->gmax) cube->gmax = value;

    value = (color >> 10) & 31;
    if (value < cube->bmin) cube->bmin = value;
    if (value > cube->bmax) cube->bmax = value;
  }

  return;
}


int medcut_compare (const void *a1, const void *a2)

{
  INT4 color1,color2;
  int c1,c2;

  color1 = *(INT4 *) a1;
  color2 = *(INT4 *) a2;

  switch (GBLmedcut.longdim) {
    case 0:
      c1 = color1 & 31;
      c2 = color2 & 31;
    break;

    case 1:
      c1 = (color1 >> 5) & 31;
      c2 = (color2 >> 5) & 31;
    break;

    case 2:
      c1 = (color1 >> 10) & 31;
      c2 = (color2 >> 10) & 31;
    break;
  }

  return ((int)(c1-c2));
}
