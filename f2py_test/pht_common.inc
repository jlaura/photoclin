C	**********************************************************************
C	INCLUDE FILE
C	I have merged the common block /apars/, containing variables 
C	hnorm,tau,wha,bha,hga into the main include file.  Also, the block
C	/ah_table/ is now /atm_tables/ with more variables used in the Hapke
C	atmosphere routines.  Finally, I tidied the order of statements.
C	NB the comment is not up to date as to version.
c ################################################
c ##  File: pht_common.inc                      ##
c ##  Description: All the commons for photomet ##
c ##  Author: K. Teal Thompson                  ##
c ##  Date of Last Revision:  07 Jul 2000       ##
c ################################################

        real*8 phtrho   ! added to allow reporting of directional
c	                ! hemispheric albedo that relates to thermal
c                       ! balance for simulating Themis images
	real*8          hnorm		!atmospheric shell thickness 
					!normalized to planet radius, 
					!used to modify angles to get 
					!more accurate path lengths 
					!near the terminator
	real*8		tau		!normal optical depth of atmosphere
					!(dummy var?)
	real*8          wha		!single-scattering albedo of 
					!atmospheric particles, not to be 
					!confused with that of the surface 
					!particles
	real*8          bha		!First coefficient of Legendre 
					!polynomial for atmospheric particle
					!phase function; used in ATMAN11,ATMAN12
	real*8          hga		!Henyey-Greenstein assymetry parameter 
					!for atmospheric particle phase
					!function; used in ATMHHG1,ATMHHG2

	integer*4       atmSwitch	!switch controlling the function that
					!is integrated: 0 for AhTable
					!               1 for hahgtTable
					!               2 for hahgsTable
					!               3 for hahgt0Table
	integer*4       ninc		!number of incidence angles
	parameter       (ninc=91)	!0 to 90 inclusive is 91 points
	real*8          incTable(ninc)	!table with incidence angles
	real*8          AhTable(ninc)	!table with hemisperic albedos
					!for a given incidence angle
	real*8          AhTable2(ninc)	!used for spline fit to AhTable
	real*8          Ab		!total bi-hemispheric albedo
	real*8          hahgtTable(ninc)!table with hemisperic integral of
					!single-scattering phase fn for 
					!transmission at given incidence angle
	real*8          hahgtTable2(ninc)!used for spline fit to hahgtTable
	real*8          hahgsB		!average over hahgsTable
	real*8          hahgt0Table(ninc)!table with hemisperic integral of
					!single-scattering phase fn for unscat.
					!transmission at given incidence angle
	real*8          hahgt0Table2(ninc)!used for spline fit to hahgt0Table

	real*8          wh		!single-scattering albedo of
					!average particle
	real*8          hh		!compaction parameter of
					!backscatter function
	real*8          b0		!opposition surge amplitude
	real*8          theta		!average topographic slope
					!angle of surface roughness at
					!subresolution scales
	real*8          hg1		!asymmetry factor of the
					!henyey-greenstein particle
					!phase function
	real*8          hg2		!second asymmetry factor for
					!2-lobed henyey-greenstein
					!particle phase function
	real*8          bh		!first coefficient of
					!legendre polynomial (bh)
	real*8          ch		!second coefficient of
					!legendre polynomial (ch)
	integer*4       iopt		!0=use henyey-greenstein function
					!1=use legendre polynomial

C	decs for General Model
	real*8		incref		!user input reference incidence angle.
	real*8		albedo		!Average image albedo input by the
					!user for topographic normalization.
	real*8		thresh		!parameter limiting how much we 
					!amplify the dns
	real*8		tauref		!user input reference tau
	real*8		wharef
	real*8		bharef
	real*8          hgaref

	character*6     char_pht_function
	character*2     atmosmodel
	real*8          incmatch	!incidence angle to match for mixed model

C	decs for Lunar Lambert/Minnaert polynomial
	integer*4	nbmax,numbcoef
	parameter	(nbmax=20)
	real*8		bcof(nbmax)
	real*8		bee

C	decs for Lunar Lambert polynomial
	integer*4	nlmax,numlcoef
	parameter	(nlmax=20)
	real*8		lcof(nlmax)
	real*8		ll

C	decs for Minnaert polynomial
	integer*4	nkmax,numkcoef
	parameter	(nkmax=20)
	real*8		kcof(nkmax)
	real*8		kay

C	decs for Lunar Lambert/Minnaert empirical
	real*8		bphase(nbmax),bval(nbmax),bee2(nbmax)
	integer*4	numbeepoints	!declared above

C	decs for Lunar Lambert empirical
	real*8		llphase(nlmax),llval(nlmax),ll2(nlmax)
	integer*4	numllpoints

C	decs for Minnaert empirical
	real*8		kayphase(nkmax),kayval(nkmax),kay2(nkmax)
	integer*4	numkaypoints


	common /apars/ hnorm,tau,wha,bha,hga,phtrho

	common /atm_tables/ 
     .			incTable,AhTable,AhTable2,Ab,
     .			hahgtTable,hahgtTable2,
     .			hahgsB,
     .			hahgt0Table,hahgt0Table2,atmSwitch

	common /phop_hap/ wh,hh,b0,theta,hg1,hg2,bh,ch
	common /get_func_hap/ iopt

	common /mod_common/ incref,albedo,thresh,tauref,wharef,bharef,
     .                      hgaref

	common /pht_func_common/ char_pht_function
	common /atm_func_common/ atmosmodel
	common /mod_mixed_common/ incmatch

	common /phop_ll_min_p/ bcof,numbcoef
	common /phop_ll_p/ lcof,numlcoef
	common /phop_min_p/ kcof,numkcoef
	common /phop_ll_min_e/ bphase,bval,bee2,numbeepoints
	common /phop_ll_e/ llphase,llval,ll2,numllpoints
	common /phop_min_e/ kayphase,kayval,kay2,numkaypoints
	common /phop_ll_min/ bee
	common /phop_ll/ ll
	common /phop_min/ kay

