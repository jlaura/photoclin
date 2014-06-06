#include "pc2d.h"
#include "pc2d_bitmaps.h"
#include <X11/cursorfont.h>

void ColormapInit (void)

/*------------------------------------------------------------------------
*
*_TITLE  ColormapInit - Initialize the colormaps.
*
*_DESC
*
*_HIST  Jan 01 1996 Jeff Anderson, Original version
*       Mar 05 1999 TLS, If there are less than 200 color cells available
*                     make sure the cells for red,green,white and black
*                     are not used.
*       Aug 23 2000 Jeff Anderson,  Exit if there are not 32 color cells
*                     available.  This will solve problems with Linux
*                     displays.
*
*_END
*
*-----------------------------------------------------------------------*/

{
  unsigned int i,j,red[256],grn[256],blu[256];
  unsigned long pixels[256];

/******************************************************************************
* Determine number of cells used in the default colormap for the
* PseudoColor visual
******************************************************************************/

  if (xinfo.vinfo.class == PseudoColor) {
    for (i=256; i>0; i--) {
      if (XAllocColorCells (xinfo.display,xinfo.cmap,False,NULL,0,pixels,i)) {
        break;
      }
    }

    if (i < 32) {
      printf ("Too many colors are being used by other applications\n");
      printf ("such as netscape, IDL, etc.  Qview needs at least 32 colors.\n");
      exit (1);
    }


    for (j=0; j<256; j++) GBLdsp.cellused[j] = True;
    for (j=0; j<i; j++) GBLdsp.cellused[pixels[j]] = False; 

    XFreeColors (xinfo.display,xinfo.cmap,pixels,i,0);
  }
  else if (xinfo.vinfo.class == DirectColor) {
    for (i=256; i>0; i--) {
      if (XAllocColorCells (xinfo.display,xinfo.cmap,False,NULL,0,pixels,i)) {
        break;
      }
    }

    if (i < 200) {
      for (j=0; j<256; j++) {
        if (j < 55) {
          GBLdsp.rcellused[j] = True;
          GBLdsp.gcellused[j] = True;
          GBLdsp.bcellused[j] = True;
        }
        else {
          GBLdsp.rcellused[j] = False;
          GBLdsp.gcellused[j] = False;
          GBLdsp.bcellused[j] = False;
        }
      }
    }
    else {
      for (j=0; j<i; j++) {
        red[j] = pixels[j] & xinfo.vinfo.red_mask;
        while (red[j] > 255) red[j] = red[j] >> 8;
        grn[j] = pixels[j] & xinfo.vinfo.green_mask;
        while (grn[j] > 255) grn[j] = grn[j] >> 8;
        blu[j] = pixels[j] & xinfo.vinfo.blue_mask;
        while (blu[j] > 255) blu[j] = blu[j] >> 8;
      }

      for (j=0; j<256; j++) {
        GBLdsp.rcellused[j] = True;
        GBLdsp.gcellused[j] = True;
        GBLdsp.bcellused[j] = True;
      }

      for (j=0; j<i; j++) {
        GBLdsp.rcellused[red[j]] = False; 
        GBLdsp.gcellused[grn[j]] = False; 
        GBLdsp.bcellused[blu[j]] = False; 
      }
    }
    
    XFreeColors (xinfo.display,xinfo.cmap,pixels,i,0);
  }


  return;
}



void LoadColorMap (DSPINFO *d)

{
  XColor ccell;
  int i,j,k,l;
  int nummaps,dn;
  unsigned long *pixels;
  int map[256],rmap[256],gmap[256],bmap[256];
  int red[256],grn[256],blu[256];
  unsigned int num;

/******************************************************************************
* Initializations 
******************************************************************************/
  
  pixels = d->cmap_pixs;  
  nummaps = MinCmapsOfScreen (xinfo.screen);

  if (xinfo.vinfo.class == TrueColor) {
    d->cmap = xinfo.cmap;
    num = 256;
  }
  else {
    num = 256;
    while (!XAllocColorCells (xinfo.display,d->cmap,False,NULL,0,pixels,num)) {
      num--;
    }
  }

  d->cmap_count = num;

/******************************************************************************
* Handle PseudoColor 
******************************************************************************/

  if (xinfo.vinfo.class == PseudoColor) {
    if (nummaps <= 1) {
      j = 0;
      for (i=0; i<num; i++) {
/*  Copy reserved colors in default color map in display color map  */
        if (GBLdsp.cellused[pixels[i]]) {
          ccell.pixel = pixels[i];
          XQueryColor (xinfo.display,xinfo.cmap,&ccell);
          XStoreColor (xinfo.display,d->cmap,&ccell); 
        }
/*  Save indices of unused pixels.  Map was used in case of non-contiguous
    free cells  */
        else {
          map[j] = i;
          j++;
        }
      }
      num = j;
    }
    else {
      for (i=0; i<num; i++) map[i] = i;
    }

/*  Calculate mapping tables (DiskDN -> Colormap index)==map[]
      unmap[] Colormap -> DiskDN(Isn't accurate)  */
    for (i=0; i<256; i++) {
      dn = (num - 1) / 255.0 * (double) i + 0.5;
      if (dn < 0) dn = 0;
      if (dn > num-1) dn = num-1;
      d->map[i] = pixels[map[dn]];
      d->unmap[pixels[map[dn]]] = i;
    }

    for (i=0; i<num; i++) {
      d->pseudomap[i][3] = pixels[map[i]];
    }
    d->pseudocells = num;

    if (d->band[1] || d->band[2]) {
      LoadPseudoColor (d);
    }
    else {
      LoadPseudoBW (d);
    }
  }

/******************************************************************************
* Handle TrueColor
******************************************************************************/

  else if (xinfo.vinfo.class == TrueColor) {
    for (i=0; i<256; i++) {
      d->rmap[i] = i;
      d->gmap[i] = i;
      d->bmap[i] = i;
      d->runmap[i] = i;
      d->gunmap[i] = i;
      d->bunmap[i] = i;
    }
  }

/******************************************************************************
* Handle DirectColor with more than one hardware colormap
******************************************************************************/

  else if (xinfo.vinfo.class == DirectColor) {
    if (nummaps <= 1) {
      j = 0; k = 0; l = 0;
      for (i=0; i<num; i++) {
        red[i] = xinfo.vinfo.red_mask & pixels[i];
        grn[i] = xinfo.vinfo.green_mask & pixels[i];
        blu[i] = xinfo.vinfo.blue_mask & pixels[i];
        while (red[i] > 255) red[i] = red[i] >> 8;
        while (grn[i] > 255) grn[i] = grn[i] >> 8;
        while (blu[i] > 255) blu[i] = blu[i] >> 8;

/*  Copy used red cells from default to display colormap  */
        if (GBLdsp.rcellused[red[i]]) {
          ccell.pixel = pixels[i];
          XQueryColor (xinfo.display,xinfo.cmap,&ccell);
          ccell.flags = DoRed;
          XStoreColor (xinfo.display,d->cmap,&ccell); 
        }
/*  Set up indices for unused red cells  */
        else {
          rmap[j] = i;
          j++;
        }

/*  Copy used green cells from default to display colormap  */
        if (GBLdsp.gcellused[grn[i]]) {
          ccell.pixel = pixels[i];
          XQueryColor (xinfo.display,xinfo.cmap,&ccell);
          ccell.flags = DoGreen;
          XStoreColor (xinfo.display,d->cmap,&ccell); 
        }
/*  Set up indices for unused green cells  */
        else {
          gmap[k] = i;
          k++;
        }

/*  Copy used blue cells from default to display colormap  */
        if (GBLdsp.bcellused[blu[i]]) {
          ccell.pixel = pixels[i];
          XQueryColor (xinfo.display,xinfo.cmap,&ccell);
          ccell.flags = DoBlue;
          XStoreColor (xinfo.display,d->cmap,&ccell); 
        }
/*  Set up indices for unused blue cells  */
        else {
          bmap[l] = i;
          l++;
        }
      }
      
      if ((j != k) || (j != l)) {
        printf ("no way!!\n");
        QviewExit ();
      }
      num = j;
    }
    else {
      for (i=0; i<num; i++) {
        red[i] = xinfo.vinfo.red_mask & pixels[i];
        grn[i] = xinfo.vinfo.green_mask & pixels[i];
        blu[i] = xinfo.vinfo.blue_mask & pixels[i];
        while (red[i] > 255) red[i] = red[i] >> 8;
        while (grn[i] > 255) grn[i] = grn[i] >> 8;
        while (blu[i] > 255) blu[i] = blu[i] >> 8;
        rmap[i] = i;
        gmap[i] = i;
        bmap[i] = i;
      }
    }


/*  Set color map values - they are compressed  */
    for (i=0; i<num; i++) {
      dn = 255.0 / (num - 1) * (double) i + 0.5;
      if (dn < 0) dn = 0;
      if (dn > 255) dn = 255;
      ccell.red = dn * 257;
      ccell.green = dn * 257;
      ccell.blue = dn * 257;

      ccell.pixel = pixels[rmap[i]];
      ccell.flags = DoRed;
      XStoreColor (xinfo.display,d->cmap,&ccell); 

      ccell.pixel = pixels[gmap[i]];
      ccell.flags = DoGreen;
      XStoreColor (xinfo.display,d->cmap,&ccell); 

      ccell.pixel = pixels[bmap[i]];
      ccell.flags = DoBlue;
      XStoreColor (xinfo.display,d->cmap,&ccell); 
    }

    for (i=0; i<256; i++) {
      dn = (num - 1) / 255.0 * (double) i + 0.5;
      if (dn < 0) dn = 0;
      if (dn > num-1) dn = num-1;
      d->rmap[i] = red[rmap[dn]];
      d->gmap[i] = grn[gmap[dn]];
      d->bmap[i] = blu[bmap[dn]];
      d->runmap[red[rmap[dn]]] = i;
      d->gunmap[grn[gmap[dn]]] = i;
      d->bunmap[blu[bmap[dn]]] = i;
    }
  }

  if (xinfo.vinfo.class != TrueColor) {
    for (i=0; i<d->cmap_count; i++) {
      d->cmap_cells[i].pixel = d->cmap_pixs[i];
      d->cmap_cells[i].flags = DoRed | DoGreen | DoBlue;
    }
    XQueryColors (xinfo.display,d->cmap,d->cmap_cells,d->cmap_count);
  }

  return;
}

void LoadPseudoBW (DSPINFO *d)

{
  int i;
  int dn;
  int num;
  XColor ccell;

  num = d->pseudocells;

  for (i=0; i<num; i++) {
    dn = 255.0 / (num - 1) * (double) i + 0.5;
    if (dn < 0) dn = 0;
    if (dn > 255) dn = 255;
    ccell.pixel = d->pseudomap[i][3];
    ccell.red = dn * 257;
    ccell.green = dn * 257;
    ccell.blue = dn * 257;
    ccell.flags = DoRed | DoGreen | DoBlue;
    XStoreColor (xinfo.display,d->cmap,&ccell); 
  }

  return;
}


void LoadPseudoColor (DSPINFO *d)

{
  int i;
  int num;
  XColor ccell;

  medcut (d);

  num = d->pseudocells;
  for (i=0; i<num; i++) {
    ccell.pixel = d->pseudomap[i][3];
    ccell.red = d->pseudomap[i][0] * 257;
    ccell.green = d->pseudomap[i][1] * 257;
    ccell.blue = d->pseudomap[i][2] * 257;
    ccell.flags = DoRed | DoGreen | DoBlue;
    XStoreColor (xinfo.display,d->cmap,&ccell); 
  }

  return;
}
