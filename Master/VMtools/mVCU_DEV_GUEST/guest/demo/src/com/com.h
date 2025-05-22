/*
 * com.h
 *
 *  Created on: Jul 5, 2017
 *      Author: 61239
 */

#ifndef COM_H_
#define COM_H_
#include "step_constant.h"
#include "step_includes.h"
#include "mstep.h"
#include "dtxt_print_on.h"
#include "dtxt.h"
#include "std.h"
#include "osw.h"
#include "gdf.h"



extern MSTEPINC_T_NetworkLed COM_C_NetworkLed;
extern MSTEPINC_T_MaintenanceTable COM_V_MaintenanceTable;
extern INT8U COM_C_ApplicationSwVersion[15u];
extern MSTEPINC_T_TaskInfo COM_V_TaskInfo;

#endif /* COM_H_ */
