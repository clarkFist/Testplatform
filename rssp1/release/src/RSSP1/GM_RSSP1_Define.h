/**
* @file GM_RSSP1_Define.h
* @brief 定义RSSP1模块用到的宏
* @author Hao Liming
* @date 2010-11-2 15:56:31
* @version
* <pre><b>copyright: CASCO</b></pre>
* <pre><b>email: </b>haoliming@casco.com.cn</pre>
* <pre><b>company: </b>http://www.casco.com.cn</pre>
* <pre><b>All rights reserved.</b></pre>
* <pre><b>modification:</b></pre>
* <pre>Write modifications here.</pre>
*/
#ifndef _GM_RSSP1_H
#define _GM_RSSP1_H

/************  用户定义区1，选择操作系统  4选1 *********/
 /*若为windows，则定义:*/
#ifndef WIN32
#define WIN32
#endif

/* 若为VXWORKS5_5，则定义:
#ifndef VXWORKS5_5
#define VXWORKS5_5
#endif*/
/* 若为VXWORKS6_6，则定义:
#ifndef VXWORKS6_6
#define VXWORKS6_6
#endif*/
/* 若为QNX，则定义:
#ifndef QNX
#define QNX
#endif
*/


/************  用户定义区2， 定义要打印的log信息等级 *********/
/*若定义为小于-10，则 调用 GM_RSSP1_Log_Msg的代码无效
若定义为大于等于-10的数，则代码中 小于等于n的log信息将被打印出来.
RSSP1本身代码定义的log范围是大于等于0*/
#define RSSP1_LOG_LEVEL	(0)


/************  用户定义区3，定义SFM, CFM的使用情况**************/
#ifndef SFM_Part
#define SFM_Part
#endif

#ifndef CFM_Part
#define CFM_Part
#endif

/*#define OFFLINE_TOOL_SUPPORT*/

/************  用户定义区 结束 *********/


/*避免操作系统重复定义*/
/*当为VXWORKS时定义VXWORKS*/
#ifdef VXWORKS5_5
#ifndef VXWORKS
#define VXWORKS
#endif
#undef VXWORKS6_6
#undef WIN32
#undef QNX
#endif

#ifdef VXWORKS6_6
#ifndef VXWORKS
#define VXWORKS
#endif
#undef VXWORKS5_5
#undef WIN32
#undef QNX
#endif

#ifdef WIN32
#undef VXWORKS
#undef VXWORKS5_5
#undef VXWORKS6_6
#undef QNX
#endif

#ifdef QNX
#undef VXWORKS
#undef VXWORKS5_5
#undef VXWORKS6_6
#undef WIN32
#endif

/*#define GM_RSSP1_MAX_SAFETY_CONNECTION_NUM      70U*/
#define GM_RSSP1_MAX_REFRESH_LEN 0
#define GM_RSSP1_MAX_USER_DAT_LEN	1378/*CR:3238, 7048*/
#define CFM_Stanby_Answer
/*#define GM_RSSP1_RxMsgWithTime*/ /*CR:3894 support prectime with message to uper*/
/*#define GM_RSSP1_SYN_NO_DATA*/  /*CR:3894 support syn without user data*/

#endif

