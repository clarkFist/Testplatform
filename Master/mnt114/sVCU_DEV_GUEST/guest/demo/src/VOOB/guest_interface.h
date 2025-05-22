/******************************************************************************
 * COPYRIGHT (C) CASCO Signal 2015. VIOP Project. All rights reserved.
 *****************************************************************************/

/**
* @file guest_interface.h
* @brief
* @author Fu Liyu
* @date 2013-5-21
* @version
* <pre><b>copyright: CASCO</b></pre>
* <pre><b>email: fuliyu@casco.com.cn
* <pre><b>company: </b>http://www.casco.com.cn</pre>
* <pre><b>All rights reserved.</b></pre>
* <pre><b>modification:</b></pre>
* <pre>Write modifications here.</pre>
*/

#ifndef __guest_interface_h__
#define __guest_interface_h__

/******************************************************************************
 * INCLUDES
 *****************************************************************************/



/******************************************************************************
 * CONSTANTS OF MODULE
 *****************************************************************************/

/* FPGA ADDR */
#define GUEST_C_FPGA_FUSE_INVOKE                   (0x0002u)         /* 熔丝检测触发的地址 */
#define GUEST_C_FPGA_FUSE_BREAK                    (0x0004u)         /* 熔丝烧毁命令的地址 */
#define GUEST_C_FPGA_FUSE_FEED                     (0x0004u)         /* 熔丝喂狗命令的地址 */
#define GUEST_C_FPGA_24VLIGHT_LAMP                 (0x0006u)         /* 24V面板指示灯控制地址 */
#define GUEST_C_FPGA_ASKFPGARB                     (0x0008u)         /* 通知FPGA回读端口状态的地址 */
#define GUEST_C_FPGA_OUTPUTLIGHT_LAMP              (0x000Au)         /* 面板输出指示灯控制地址 */
#define GUEST_C_FPGA_COUNTER                       (0x0010u)         /* FPGA时钟互检结果存放地址 */
#define GUEST_C_FPGA_FUSET_RBVALUE                 (0x0012u)         /* 熔丝检测回读数据存放地址 */
#define GUEST_C_FPGA_FUSET_RBSTABLE                (0x0014u)         /* 回读熔丝状态的地址 */
#define GUEST_C_FPGA_VERSION_ADDR                  (0x0020u)         /* FPGA版本号存放地址 */
#define GUEST_C_FPGA_OPT_RBVALUE                   (0x8000u)         /* FPGA中存放32次回读结果的地址 */
#define GUEST_C_FPGA_CNTFLAG                       (0x8100u)         /* FPGA中存放计数器和回读完成标示的地址*/

/* GPIO Definition */
#ifdef ALSTOM_CONFIG_CPU_A
#define GUEST_C_GPIO_W_ISOL_11                     MSTEPINC_C_GPIO_ID1
#define GUEST_C_GPIO_W_ISOL_12                     MSTEPINC_C_GPIO_ID4
#define GUEST_C_GPIO_R_ISOL_11                     MSTEPINC_C_GPIO_ID5
#define GUEST_C_GPIO_R_ISOL_12                     MSTEPINC_C_GPIO_ID3
#define GUEST_C_GPIO_12VF_NOK                      MSTEPINC_C_GPIO_ID8
#define GUEST_C_GPIO_24V_NOK1                      MSTEPINC_C_GPIO_ID2
#define GUEST_C_GPIO_24V_NOK2                      MSTEPINC_C_GPIO_ID7
#define GUEST_C_GPIO_FPGA_DONE                     MSTEPINC_C_GPIO_ID6
#define GUEST_C_GPIO_TEST                          MSTEPINC_C_GPIO_ID9
#define GUEST_C_GPIO_DYN_1                         MSTEPINC_C_GPIO_ID10
#define GUEST_C_GPIO_RESETN                        MSTEPINC_C_GPIO_ID11
#define GUEST_C_GPIO_NW_FUSE_ON                    MSTEPINC_C_GPIO_ID12
#define GUEST_C_GPIO_HWI_GP0                       MSTEPINC_C_GPIO_ID13
#define GUEST_C_GPIO_HWI_GP1                       MSTEPINC_C_GPIO_ID14

#define GUEST_C_GPIO_R_OPT_P1                      MSTEPINC_C_GPIO_NO_ID
#define GUEST_C_GPIO_R_OPT_P2                      MSTEPINC_C_GPIO_NO_ID
#define GUEST_C_GPIO_R_OPT_P3                      MSTEPINC_C_GPIO_NO_ID
#define GUEST_C_GPIO_R_OPT_P4                      MSTEPINC_C_GPIO_NO_ID
#define GUEST_C_GPIO_R_OPT_P5                      MSTEPINC_C_GPIO_NO_ID
#define GUEST_C_GPIO_R_OPT_P6                      MSTEPINC_C_GPIO_NO_ID
#define GUEST_C_GPIO_R_OPT_P7                      MSTEPINC_C_GPIO_NO_ID
#define GUEST_C_GPIO_R_OPT_P8                      MSTEPINC_C_GPIO_NO_ID
#define GUEST_C_GPIO_R_OPT_P9                      MSTEPINC_C_GPIO_NO_ID
#define GUEST_C_GPIO_R_OPT_P10                     MSTEPINC_C_GPIO_NO_ID
#define GUEST_C_GPIO_R_OPT_P11                     MSTEPINC_C_GPIO_NO_ID
#define GUEST_C_GPIO_R_OPT_P12                     MSTEPINC_C_GPIO_NO_ID
#define GUEST_C_GPIO_R_OPT_P13                     MSTEPINC_C_GPIO_NO_ID
#define GUEST_C_GPIO_R_OPT_P14                     MSTEPINC_C_GPIO_NO_ID
#define GUEST_C_GPIO_R_OPT_P15                     MSTEPINC_C_GPIO_NO_ID
#define GUEST_C_GPIO_R_OPT_P16                     MSTEPINC_C_GPIO_NO_ID

#define GUEST_C_GPIO_R_OPT_N1                      MSTEPINC_C_GPIO_NO_ID
#define GUEST_C_GPIO_R_OPT_N2                      MSTEPINC_C_GPIO_NO_ID
#define GUEST_C_GPIO_R_OPT_N3                      MSTEPINC_C_GPIO_NO_ID
#define GUEST_C_GPIO_R_OPT_N4                      MSTEPINC_C_GPIO_NO_ID
#define GUEST_C_GPIO_R_OPT_N5                      MSTEPINC_C_GPIO_NO_ID
#define GUEST_C_GPIO_R_OPT_N6                      MSTEPINC_C_GPIO_NO_ID
#define GUEST_C_GPIO_R_OPT_N7                      MSTEPINC_C_GPIO_NO_ID
#define GUEST_C_GPIO_R_OPT_N8                      MSTEPINC_C_GPIO_NO_ID
#define GUEST_C_GPIO_R_OPT_N9                      MSTEPINC_C_GPIO_NO_ID
#define GUEST_C_GPIO_R_OPT_N10                     MSTEPINC_C_GPIO_NO_ID
#define GUEST_C_GPIO_R_OPT_N11                     MSTEPINC_C_GPIO_NO_ID
#define GUEST_C_GPIO_R_OPT_N12                     MSTEPINC_C_GPIO_NO_ID
#define GUEST_C_GPIO_R_OPT_N13                     MSTEPINC_C_GPIO_NO_ID
#define GUEST_C_GPIO_R_OPT_N14                     MSTEPINC_C_GPIO_NO_ID
#define GUEST_C_GPIO_R_OPT_N15                     MSTEPINC_C_GPIO_NO_ID
#define GUEST_C_GPIO_R_OPT_N16                     MSTEPINC_C_GPIO_NO_ID
#endif

#ifdef ALSTOM_CONFIG_CPU_B
#define GUEST_C_GPIO_W_ISOL_11                     MSTEPINC_C_GPIO_ID4
#define GUEST_C_GPIO_W_ISOL_12                     MSTEPINC_C_GPIO_ID1
#define GUEST_C_GPIO_R_ISOL_11                     MSTEPINC_C_GPIO_ID9
#define GUEST_C_GPIO_R_ISOL_12                     MSTEPINC_C_GPIO_ID2
#define GUEST_C_GPIO_12VF_NOK                      MSTEPINC_C_GPIO_ID7
#define GUEST_C_GPIO_24V_NOK1                      MSTEPINC_C_GPIO_ID3
#define GUEST_C_GPIO_24V_NOK2                      MSTEPINC_C_GPIO_ID6
#define GUEST_C_GPIO_FPGA_DONE                     MSTEPINC_C_GPIO_ID8
#define GUEST_C_GPIO_TEST                          MSTEPINC_C_GPIO_ID11
#define GUEST_C_GPIO_DYN_1                         MSTEPINC_C_GPIO_ID14
#define GUEST_C_GPIO_RESETN                        MSTEPINC_C_GPIO_ID5
#define GUEST_C_GPIO_NW_FUSE_ON                    MSTEPINC_C_GPIO_ID10
#define GUEST_C_GPIO_HWI_GP0                       MSTEPINC_C_GPIO_ID12
#define GUEST_C_GPIO_HWI_GP1                       MSTEPINC_C_GPIO_ID13

#define GUEST_C_GPIO_R_OPT_P1                      MSTEPINC_C_GPIO_NO_ID
#define GUEST_C_GPIO_R_OPT_P2                      MSTEPINC_C_GPIO_NO_ID
#define GUEST_C_GPIO_R_OPT_P3                      MSTEPINC_C_GPIO_NO_ID
#define GUEST_C_GPIO_R_OPT_P4                      MSTEPINC_C_GPIO_NO_ID
#define GUEST_C_GPIO_R_OPT_P5                      MSTEPINC_C_GPIO_NO_ID
#define GUEST_C_GPIO_R_OPT_P6                      MSTEPINC_C_GPIO_NO_ID
#define GUEST_C_GPIO_R_OPT_P7                      MSTEPINC_C_GPIO_NO_ID
#define GUEST_C_GPIO_R_OPT_P8                      MSTEPINC_C_GPIO_NO_ID
#define GUEST_C_GPIO_R_OPT_P9                      MSTEPINC_C_GPIO_NO_ID
#define GUEST_C_GPIO_R_OPT_P10                     MSTEPINC_C_GPIO_NO_ID
#define GUEST_C_GPIO_R_OPT_P11                     MSTEPINC_C_GPIO_NO_ID
#define GUEST_C_GPIO_R_OPT_P12                     MSTEPINC_C_GPIO_NO_ID
#define GUEST_C_GPIO_R_OPT_P13                     MSTEPINC_C_GPIO_NO_ID
#define GUEST_C_GPIO_R_OPT_P14                     MSTEPINC_C_GPIO_NO_ID
#define GUEST_C_GPIO_R_OPT_P15                     MSTEPINC_C_GPIO_NO_ID
#define GUEST_C_GPIO_R_OPT_P16                     MSTEPINC_C_GPIO_NO_ID

#define GUEST_C_GPIO_R_OPT_N1                      MSTEPINC_C_GPIO_NO_ID
#define GUEST_C_GPIO_R_OPT_N2                      MSTEPINC_C_GPIO_NO_ID
#define GUEST_C_GPIO_R_OPT_N3                      MSTEPINC_C_GPIO_NO_ID
#define GUEST_C_GPIO_R_OPT_N4                      MSTEPINC_C_GPIO_NO_ID
#define GUEST_C_GPIO_R_OPT_N5                      MSTEPINC_C_GPIO_NO_ID
#define GUEST_C_GPIO_R_OPT_N6                      MSTEPINC_C_GPIO_NO_ID
#define GUEST_C_GPIO_R_OPT_N7                      MSTEPINC_C_GPIO_NO_ID
#define GUEST_C_GPIO_R_OPT_N8                      MSTEPINC_C_GPIO_NO_ID
#define GUEST_C_GPIO_R_OPT_N9                      MSTEPINC_C_GPIO_NO_ID
#define GUEST_C_GPIO_R_OPT_N10                     MSTEPINC_C_GPIO_NO_ID
#define GUEST_C_GPIO_R_OPT_N11                     MSTEPINC_C_GPIO_NO_ID
#define GUEST_C_GPIO_R_OPT_N12                     MSTEPINC_C_GPIO_NO_ID
#define GUEST_C_GPIO_R_OPT_N13                     MSTEPINC_C_GPIO_NO_ID
#define GUEST_C_GPIO_R_OPT_N14                     MSTEPINC_C_GPIO_NO_ID
#define GUEST_C_GPIO_R_OPT_N15                     MSTEPINC_C_GPIO_NO_ID
#define GUEST_C_GPIO_R_OPT_N16                     MSTEPINC_C_GPIO_NO_ID
#endif

/******************************************************************************
 * MACROS DEFINITION
 *****************************************************************************/


/******************************************************************************
 *  TYPE DEFINITION
 *****************************************************************************/


/******************************************************************************
 *  GLOBAL VARIABLES OF MODULE
 *****************************************************************************/


/******************************************************************************
 *  API FUNCTION PROTOTYPE
 *****************************************************************************/


#endif /* __guest_interface_h__ */

/* EOF */
