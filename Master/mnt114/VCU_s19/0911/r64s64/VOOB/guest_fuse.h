/**
* @file guest_fuse.h
* @brief
* @author Wang Jiantao
* @date 2013-7-2
* @version
* <pre><b>copyright: CASCO</b></pre>
* <pre><b>email: wangjiantao@casco.com.cn
* <pre><b>company: </b>http://www.casco.com.cn</pre>
* <pre><b>All rights reserved.</b></pre>
* <pre><b>modification:</b></pre>
* <pre>Write modifications here.</pre>
*/

#ifndef __GUEST_FUSE__
#define __GUEST_FUSE__

#define GUEST_FUSE_TEST_DELAY                     10    /* Fuse test each step delay */

extern void GUEST_F_FuseTestSetOn(void);
extern void GUEST_F_FuseTestSetOff(void);
extern  void GUEST_F_FUSE_Test(void);

#endif /* GUEST_FUSE */

/* EOF */
