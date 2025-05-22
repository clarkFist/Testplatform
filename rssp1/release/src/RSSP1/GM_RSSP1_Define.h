/**
* @file GM_RSSP1_Define.h
* @brief ����RSSP1ģ���õ��ĺ�
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

/************  �û�������1��ѡ�����ϵͳ  4ѡ1 *********/
 /*��Ϊwindows������:*/
#ifndef WIN32
#define WIN32
#endif

/* ��ΪVXWORKS5_5������:
#ifndef VXWORKS5_5
#define VXWORKS5_5
#endif*/
/* ��ΪVXWORKS6_6������:
#ifndef VXWORKS6_6
#define VXWORKS6_6
#endif*/
/* ��ΪQNX������:
#ifndef QNX
#define QNX
#endif
*/


/************  �û�������2�� ����Ҫ��ӡ��log��Ϣ�ȼ� *********/
/*������ΪС��-10���� ���� GM_RSSP1_Log_Msg�Ĵ�����Ч
������Ϊ���ڵ���-10������������� С�ڵ���n��log��Ϣ������ӡ����.
RSSP1������붨���log��Χ�Ǵ��ڵ���0*/
#define RSSP1_LOG_LEVEL	(0)


/************  �û�������3������SFM, CFM��ʹ�����**************/
#ifndef SFM_Part
#define SFM_Part
#endif

#ifndef CFM_Part
#define CFM_Part
#endif

/*#define OFFLINE_TOOL_SUPPORT*/

/************  �û������� ���� *********/


/*�������ϵͳ�ظ�����*/
/*��ΪVXWORKSʱ����VXWORKS*/
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

