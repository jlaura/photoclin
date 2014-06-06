#include "pc2d.h"

void StretchArea (int dsp, int sx, int sy, int ex, int ey)
/***********************************************************************
*
*_Title	StretchArea - Stretch entire image based on histgram of sub-area
*
*_Args	Type   Variable        I/O	Description
*
*_DESC	This routine will stretch the entire image based on statistics
*       of a sub-area.
*
*_HIST	Jan 1 1996 Jeff Anderson, Original version
*       Mar 23 1998 Tracie Sucharski, Save original min/max
*                    values to the save values.
*
*_END
************************************************************************/


{
  int *unmap1,*unmap2,*unmap3;
  Boolean red_first;
  int rhist[256],ghist[256],bhist[256];
  int *hist1,*hist2,*hist3;
  DSPINFO *d;
  int width;
  UINT1 *ptr;
  int i,x,y;
  int dn;
  XColor ccell[256];
  INT4 rmin,rmax,gmin,gmax,bmin,bmax;
  double slope,rslope,bslope,gslope;
  int rshift,bshift;
  Boolean flags;

  if (xinfo.vinfo.red_mask < xinfo.vinfo.blue_mask) {
    rshift = 0;
    bshift = 16;
  }
  else {
    rshift = 16;
    bshift = 0;
  }

  for (i=0; i<256; i++) {
    rhist[i] = 0;
    ghist[i] = 0;
    bhist[i] = 0;
  }

  d = &GBLdsp.d[dsp];
  red_first = (Boolean) (d->ximg->red_mask < d->ximg->blue_mask);
  if (d->ximg->byte_order == MSBFirst) red_first = !red_first;

  if (red_first) {
    unmap1 = d->runmap;
    unmap2 = d->gunmap;
    unmap3 = d->bunmap;
    hist1 = rhist;
    hist2 = ghist;
    hist3 = bhist;
  }
  else {
    unmap1 = d->bunmap;
    unmap2 = d->gunmap;
    unmap3 = d->runmap;
    hist1 = bhist;
    hist2 = ghist;
    hist3 = rhist;
  }

  width = xinfo.width * xinfo.depth / 8;

  for (y=sy; y<=ey; y++) {
    ptr = (UINT1 *) d->ximgptr + y * width + sx * xinfo.depth / 8;
    if (xinfo.vinfo.class == PseudoColor) {
      for (x=sx; x<=ex; x++) {
        rhist[d->unmap[*ptr]]++;
        ptr++;
      }
    }
    else {
      for (x=sx; x<=ex; x++) {
        hist1[unmap1[*ptr]]++; ptr++;
        hist2[unmap2[*ptr]]++; ptr++;
        hist3[unmap3[*ptr]]++; ptr++;
      }
    }
  }

  if (d->band[0]) {
    if (percent8 (rhist,d->stretch_lowper,1,254,&rmin)) rmin = 1;
    if (percent8 (rhist,d->stretch_hiper,1,254,&rmax)) rmax = 1;
  }
  if (d->band[1]) { 
    if (percent8 (ghist,d->stretch_lowper,1,254,&gmin)) gmin = 1;
    if (percent8 (ghist,d->stretch_hiper,1,254,&gmax)) gmax = 1;
  }
  if (d->band[2]) {
    if (percent8 (bhist,d->stretch_lowper,1,254,&bmin)) bmin = 1;
    if (percent8 (bhist,d->stretch_hiper,1,254,&bmax)) bmax = 1;
  }

  if (xinfo.vinfo.class ==  PseudoColor) {
    if (rmin == rmax) goto DONE;
    slope = (double) (254 - 1) / (rmax - rmin);
    for (i=0; i<=255; i++) {
      dn = slope * (i - rmin) + 1.0;
      if (dn < 0) dn = 0;
      if (dn > 255) dn = 255;
      ccell[i].pixel = d->map[i];
      ccell[i].red = dn * 257; 
      ccell[i].green = dn * 257; 
      ccell[i].blue = dn * 257; 
      ccell[i].flags = DoRed | DoGreen | DoBlue;
    }
    XStoreColors (xinfo.display,d->cmap,ccell,256);
    XSync (xinfo.display,False);
  }
  else if (d->band[1] || d->band[2]) {
    flags = 0;
    if (rmin != rmax) flags |= DoRed;
    if (gmin != gmax) flags |= DoGreen;
    if (bmin != bmax) flags |= DoBlue;
    if (!flags) goto DONE;
    if (flags & DoRed) rslope = (double) (254 - 1) / (rmax - rmin);
    if (flags & DoGreen) gslope = (double) (254 - 1) / (gmax - gmin);
    if (flags & DoBlue) bslope = (double) (254 - 1) / (bmax - bmin);
    for (i=0; i<=255; i++) {
      ccell[i].pixel = 0;
      if (flags & DoRed) {
        dn = rslope * (i - rmin) + 1.0;
        if (dn < 0) dn = 0;
        if (dn > 255) dn = 255;
        ccell[i].red = dn * 257;
        ccell[i].pixel |= (d->rmap[i] << rshift);
      }
      if (flags & DoGreen) {
        dn = gslope * (i - gmin) + 1.0;
        if (dn < 0) dn = 0;
        if (dn > 255) dn = 255;
        ccell[i].green = dn * 257;
        ccell[i].pixel |= (d->gmap[i] << 8);
      }
      if (flags & DoBlue) {
        dn = bslope * (i - bmin) + 1.0;
        if (dn < 0) dn = 0;
        if (dn > 255) dn = 255;
        ccell[i].blue = dn * 257;
        ccell[i].pixel |= (d->bmap[i] << bshift);
      }
      ccell[i].flags = flags;
    }
    XStoreColors (xinfo.display,d->cmap,ccell,256);
    XSync (xinfo.display,False);
  }
  else if (xinfo.vinfo.class == DirectColor) {
    if (rmin == rmax) goto DONE;
    slope = (double) (254 - 1) / (rmax - rmin);
    for (i=0; i<=255; i++) {
      dn = slope * (i - rmin) + 1.0;
      if (dn < 0) dn = 0;
      if (dn > 255) dn = 255;
      ccell[i].pixel = d->rmap[i] << rshift;
      ccell[i].pixel |= (d->gmap[i] << 8);
      ccell[i].pixel |= (d->bmap[i] << bshift);
      ccell[i].red = dn * 257; 
      ccell[i].green = dn * 257; 
      ccell[i].blue = dn * 257; 
      ccell[i].flags = DoRed | DoGreen | DoBlue;
    }
    XStoreColors (xinfo.display,d->cmap,ccell,256);
    XSync (xinfo.display,False);
  }

  DONE:;

  for (i=0; i<d->cmap_count; i++) {
    d->cmap_cells[i].pixel = d->cmap_pixs[i];
    d->cmap_cells[i].flags = DoRed | DoGreen | DoBlue;
  }
  XQueryColors (xinfo.display,d->cmap,d->cmap_cells,d->cmap_count);

/*------------------------------------------------------------------
/  Assign new min/max values to global save values.
/-----------------------------------------------------------------*/
  if (d->f.type == 1 && (!(d->manstr || d->autostr))) {
    if (xinfo.vinfo.class == PseudoColor) {
      d->rmin_real = rmin;
      d->rmax_real = rmax;
    }
    else {
      d->rmin_real = rmin;
      d->rmax_real = rmax;
      d->gmin_real = gmin;
      d->gmax_real = gmax;
      d->bmin_real = bmin;
      d->bmax_real = bmax;
    }
  }
  else {
    if (xinfo.vinfo.class == PseudoColor) {
      slope = (254 - 1) / (d->rmax - d->rmin);
      d->rmin_real = (rmin - 1.0)/slope + d->rmin;
      d->rmax_real = (rmax - 1.0)/slope + d->rmin;
    }
    else {
      slope = (254 - 1) / (d->rmax - d->rmin);
      d->rmin_real = (rmin - 1.0)/slope + d->rmin;
      d->rmax_real = (rmax - 1.0)/slope + d->rmin;
      slope = (254 - 1) / (d->gmax - d->gmin);
      d->gmin_real = (gmin - 1.0)/slope + d->gmin;
      d->gmax_real = (gmax - 1.0)/slope + d->gmin;
      slope = (254 - 1) / (d->bmax - d->bmin);
      d->bmin_real = (bmin - 1.0)/slope + d->bmin;
      d->bmax_real = (bmax - 1.0)/slope + d->bmin;
    }
  }


  return;
}



void StretchReset (dsp)
/***********************************************************************
*
*_Title	StretchReset - Reset stretch back to original stretch.
*
*_Args	Type   Variable        I/O	Description
*_Parm	int	dsp		I	Display number

*_DESC	This routine will redisplay the image at the original
*       stretch values.
*
*_HIST	Jan 1 1996 Jeff Anderson, Original version
*       Mar 23 1998 Tracie Sucharski, Resore original min/max
*                    values to the save values.
*
*_END
************************************************************************/

{
  DSPINFO *d;
  int i;
  XColor ccell[256];

  d = &GBLdsp.d[dsp];

  if (xinfo.vinfo.class ==  PseudoColor) {
    for (i=0; i<=255; i++) {
      ccell[i].pixel = d->map[i];
      ccell[i].red = i * 257; 
      ccell[i].green = i * 257; 
      ccell[i].blue = i * 257; 
      ccell[i].flags = DoRed | DoGreen | DoBlue;
    }
    XStoreColors (xinfo.display,d->cmap,ccell,256);
    XSync (xinfo.display,False);
  }
  else if (xinfo.vinfo.class == DirectColor) {
    for (i=0; i<=255; i++) {
      if (xinfo.vinfo.red_mask < xinfo.vinfo.blue_mask) {
        ccell[i].pixel = d->rmap[i];
        ccell[i].pixel |= (d->gmap[i] << 8);
        ccell[i].pixel |= (d->bmap[i] << 16);
      }
      else {
        ccell[i].pixel = d->bmap[i];
        ccell[i].pixel |= (d->gmap[i] << 8);
        ccell[i].pixel |= (d->rmap[i] << 16);
      }
      ccell[i].red = i * 257; 
      ccell[i].green = i * 257; 
      ccell[i].blue = i * 257; 
      ccell[i].flags = DoRed | DoGreen | DoBlue;
    }
    XStoreColors (xinfo.display,d->cmap,ccell,256);
    XSync (xinfo.display,False);
  }

  for (i=0; i<d->cmap_count; i++) {
    d->cmap_cells[i].pixel = d->cmap_pixs[i];
    d->cmap_cells[i].flags = DoRed | DoGreen | DoBlue;
  }
  XQueryColors (xinfo.display,d->cmap,d->cmap_cells,d->cmap_count);

/*-----------------------------------------------------------------
/  Restore old min/max values to saved actual values.
/----------------------------------------------------------------*/
  d->rmin_real = d->rmin;
  d->rmax_real = d->rmax;
  d->gmin_real = d->gmin;
  d->gmax_real = d->gmax;
  d->bmin_real = d->bmin;
  d->bmax_real = d->bmax;
 
  return;
}
