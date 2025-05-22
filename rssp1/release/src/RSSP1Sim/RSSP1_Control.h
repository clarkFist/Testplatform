/**
* @file CC_Sim_Control.h
* @brief
* @author Misbei
* @date 2012-3-4 17:30:58
* @version
* <pre><b>copyright: CASCO</b></pre>
* <pre><b>email: </b>beihuijie@casco.com.cn</pre>
* <pre><b>company: </b>http://www.casco.com.cn</pre>
* <pre><b>All rights reserved.</b></pre>
* <pre><b>modification:</b></pre>
* <pre>Write modifications here.</pre>
*/
#ifndef _CC_SIM_Control_H
#define _CC_SIM_Control_H

#include <Windows.h>


#ifdef __cplusplus
extern "C"
{
#endif /**< __cplusplus */

	void InvokeRSSP1SimTask();
	
	/************************************************************************/
	/* RSSP1���س��򣬴����緢�ͽ����������ڸ����潻������                                                                                        */
	/************************************************************************/
	UINT RSSP1_Sim_Run(LPVOID p);


#ifdef __cplusplus
}
#endif /**< __cplusplus */

#endif 

