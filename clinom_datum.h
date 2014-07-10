/*****************************************************************
*_TITLE clinom_datum.h 
*
*_DESC  Common block defining the datum (tangent plane to 
*       planet @ image center)
*
*_HIST  Jun 01 2001  Randolph Kirk, USGS Flagstaff, Original Version
**********************************************************************/
#ifndef CLINOM_DATUM_H
#define CLINOM_DATUM_H 1
#include "isis_limits.h"
#include "isisdef.h"
#include "binding.h"

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus  */

typedef struct clinom_datum1_f_common {
  FLOAT4 z00;          /* Constant term added to planar datum, in
                          terms of full-res pixels */
  FLOAT4 dzx0,dzy0;    /* Sample, line slopes of datum plane */
  FLOAT4 dz10,dz20;    /* Diagonal slopes of datum plane */
  FLOAT4 z00s;         /* Constant term added to spherical datum */
  FLOAT4 r0;           /* Radius of datum sphere in full-res pixels */
  FLOAT4 xs0,yl0;      /* Center of datum sphere in full-res pixels */
  FLOAT4 z0f;          /* z00, scaled for operation on coarse mesh */
  FLOAT4 z0sf;
  FLOAT4 r0f;          /* r0, scaled for operation on coarse mesh */
  FLOAT4 r0f2;         /* r0f squared */
  FLOAT4 xs0f,yl0f;    /* xs0,yl0 scaled for operation on coarse mesh
                          and offset to refer to indices of nodes
			  rather than elements */
  FLOAT4 rmask1,rmask2;/* Pixels further than rmask2 times the radius
                          of the spherical datum from the center of
			  the sphere will be set to the (local) datum
			  brightness, those closer than rmask1 will be
			  left alone, and those in between will have
			  their brightness interpolated between the 
			  actual and datum values */
  FLOAT4 rmag;         /* Mesh spacing fmag, passed in common to 
                          routines that need it */
} CLINOM_DATUM1_F_COMMON;

extern CLINOM_DATUM1_F_COMMON     FTN_COMMON(datum1);

static CLINOM_DATUM1_F_COMMON *C_DATUM1 = &FTN_COMMON(datum1);

typedef struct clinom_datum2_f_common {
  INT4 datumtyp;       /* Type of datum model: 1=plane perp to line
                          of sight; 2=inclined plane; 3=sphere (but
			  parameters appropriate to 2 now describe
			  tangent plane to this sphere) */
} CLINOM_DATUM2_F_COMMON;

extern CLINOM_DATUM2_F_COMMON     FTN_COMMON(datum2);

static CLINOM_DATUM2_F_COMMON *C_DATUM2 = &FTN_COMMON(datum2);

#ifdef __cplusplus
}
#endif /* __cplusplus  */
#endif
