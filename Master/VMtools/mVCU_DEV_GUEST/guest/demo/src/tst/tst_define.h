#ifndef __tst_define__
#define __tst_define__
#include "step_includes.h"
#include "step_guest.h"
#include "osw.h"
#include "mstep.h"
#include "gdf.h"
#include "std.h"
#include "dtxt_print_on.h"
#include "dtxt.h"
#include "datatypes.h"



#define VCU_VERSION 112
 
#if VCU_VERSION == 102
	#define Tst_F_UDPFlush(a) MSTEP_F_UDPFlush(a) 
	#define Tst_F_UDPOpen(a,b,c) MSTEP_F_UDPOpen(a,b,c)
#else
	#define Tst_F_UDPFlush(a) MSTEP_F_UDPFlush(a) 
	#define Tst_F_UDPOpen(a,b,c) MSTEP_F_UDPOpen(a,b,c)
#endif

#endif
