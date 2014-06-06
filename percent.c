#include "pc2d.h"

int percent8 (INT4 *hist, double per, INT4 vmin, INT4 vmax, INT4 *dn)

{
  int i,lastdn;
  int npts,totpts,done;
  double lastper,cumper;
  double diff1,diff2;

  if ((per < 0.0) || (per > 100.0)) return -1;
  if (vmax < vmin) return -1;
  
  npts = 0;
  for (i=vmin; i<=vmax; i++) npts += hist[i];
  if (npts == 0) return -1;

  i = vmin;
  while (hist[i] == 0) i++;

  lastdn = i;
  lastper = (double) hist[i] / (double) npts * 100.0;
  totpts = 0;
  done = 0;

  while (!done) {
    totpts += hist[i];
    cumper = (double) totpts / (double) npts * 100.0;
    if (cumper >= per) {
      diff1 = cumper - per;
      diff2 = per - lastper;
      *dn = i;
      if ((diff2 < diff1) && (hist[lastdn] > 0)) *dn = lastdn;
      done = 1;
    }
    else {
      if (hist[i] != 0) {
        lastdn = i;
        lastper = cumper;
      }
      i++;
    }
  }
     
  return 0;
}






int percent16 (INT4 *hist, double per, INT4 vmin, INT4 vmax, INT4 *dn)

{
  int i,lastdn;
  int npts,totpts,done;
  double lastper,cumper;
  double diff1,diff2;

  if ((per < 0.0) || (per > 100.0)) return -1;
  if (vmax < vmin) return -1;
  
  npts = 0;
  for (i=vmin; i<=vmax; i++) npts += hist[i+32768];
  if (npts == 0) return -1;

  i = vmin;
  while (hist[i+32768] == 0) i++;

  lastdn = i;
  lastper = (double) hist[i+32768] / (double) npts * 100.0;
  totpts = 0;
  done = 0;

  while (!done) {
    totpts += hist[i+32768];
    cumper = (double) totpts / (double) npts * 100.0;
    if (cumper >= per) {
      diff1 = cumper - per;
      diff2 = per - lastper;
      *dn = i;
      if ((diff2 < diff1) && (hist[lastdn+32768] > 0)) *dn = lastdn;
      done = 1;
    }
    else {
      if (hist[i+32768] != 0) {
        lastdn = i;
        lastper = cumper;
      }
      i++;
    }
  }
     
  return 0;
}
