/**
* @file guest_isolation.h
* @brief
* @author Wang Jiantao
* @date 2013-5-21
* @version
* <pre><b>copyright: CASCO</b></pre>
* <pre><b>email: wangjiantao@casco.com.cn
* <pre><b>company: </b>http://www.casco.com.cn</pre>
* <pre><b>All rights reserved.</b></pre>
* <pre><b>modification:</b></pre>
* <pre>Write modifications here.</pre>
*/

#ifndef __GUEST_ISOLATION__
#define __GUEST_ISOLATION__

#define GUEST_BRANCH_RELAY_NUM                     1    /* The num of relay for one branch */
#define GUEST_ISOT_STEP_DELAY                      80  /* The isolation test delay for every step */

/** Definition of the isolation cell two branches relays states */
typedef enum
{
    GUEST_C_ISOLATION_GPIO_BRANCHES_OFF  = 0u, /**<Two branches GPIO OFF, connect     */
    GUEST_C_ISOLATION_GPIO_BRANCH2_OFF   = 1u, /**<DOWN branch GPIO OFF, connect      */
    GUEST_C_ISOLATION_GPIO_BRANCH1_OFF   = 2u, /**<UP branch GPIO OFF, connect        */
    GUEST_C_ISOLATION_GPIO_BRANCHES_ON   = 3u, /**<Two branches GPIO ON, disconnect   */
} GUST_E_ISOLATION_RELAYS;


extern void GUEST_F_ISO_Test(void);
extern void GUEST_F_SetIsolationRelays(GUST_E_ISOLATION_RELAYS relaysStates);

#endif /* GUEST_ISOLATION */

/* EOF */
