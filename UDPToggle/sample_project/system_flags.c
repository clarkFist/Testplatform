/**
 * @file system_flags.c
 * @brief 系统标志和开关桩定义
 * @author VCU Team
 * @date 2024-12-26
 */

#include "vcu_config.h"
#include <stdint.h>
#include <stdbool.h>

/*==============================================================================
 * 系统运行状态标志
 *============================================================================*/

// 系统初始化完成标志
#define SYSTEM_INIT_COMPLETE_FLAG    1

// 系统运行状态标志  
#define SYSTEM_RUNNING_FLAG          1
#define SYSTEM_SHUTDOWN_FLAG         0
#define SYSTEM_ERROR_FLAG            0

// 模块就绪标志
#define MODULE_VCU_READY_FLAG        1    // VCU模块就绪
#define MODULE_COMM_READY_FLAG       1    // 通信模块就绪  
#define MODULE_LOG_READY_FLAG        1    // 日志模块就绪
#define MODULE_MONITOR_READY_FLAG    0    // 监控模块就绪

/*==============================================================================
 * 网络通信开关桩
 *============================================================================*/

// UDP通信开关
#define UDP_COMM_ENABLE             1     // UDP通信启用
#define UDP_BROADCAST_ENABLE        0     // UDP广播启用
#define UDP_MULTICAST_ENABLE        0     // UDP组播启用

// TCP通信开关  
#define TCP_COMM_ENABLE             0     // TCP通信启用
#define TCP_KEEPALIVE_ENABLE        1     // TCP保活启用
#define TCP_NODELAY_ENABLE          1     // TCP无延迟启用

// 串口通信开关
#define SERIAL_COMM_ENABLE          0     // 串口通信启用
#define SERIAL_FLOW_CONTROL_ENABLE  0     // 串口流控启用

/*==============================================================================
 * 数据处理开关桩
 *============================================================================*/

// 数据验证开关
#define DATA_CRC_CHECK_ENABLE       1     // CRC校验启用
#define DATA_CHECKSUM_ENABLE        1     // 校验和启用
#define DATA_ENCRYPTION_ENABLE      0     // 数据加密启用
#define DATA_COMPRESSION_ENABLE     0     // 数据压缩启用

// 数据存储开关
#define DATA_BACKUP_ENABLE          1     // 数据备份启用
#define DATA_CACHE_ENABLE           1     // 数据缓存启用
#define DATA_PERSISTENCE_ENABLE     1     // 数据持久化启用

/*==============================================================================
 * 错误处理开关桩  
 *============================================================================*/

// 错误报告开关
#define ERROR_REPORT_ENABLE         1     // 错误报告启用
#define ERROR_AUTO_RECOVERY_ENABLE  1     // 错误自动恢复启用
#define ERROR_STACK_TRACE_ENABLE    0     // 错误堆栈跟踪启用

// 故障处理开关
#define FAULT_INJECTION_ENABLE      0     // 故障注入启用
#define FAULT_SIMULATION_ENABLE     0     // 故障仿真启用

/*==============================================================================
 * 性能监控开关桩
 *============================================================================*/

// 性能统计开关
#define PERF_CPU_MONITOR_ENABLE     0     // CPU性能监控启用
#define PERF_MEMORY_MONITOR_ENABLE  0     // 内存性能监控启用
#define PERF_NETWORK_MONITOR_ENABLE 0     // 网络性能监控启用

// 性能优化开关
#define PERF_FAST_PATH_ENABLE       1     // 快速路径优化启用
#define PERF_CACHE_OPTIMIZE_ENABLE  1     // 缓存优化启用

/*==============================================================================
 * 枚举类型开关桩定义
 *============================================================================*/

// 系统状态枚举
enum system_state {
    SYS_STATE_INIT = 0,          // 初始化状态
    SYS_STATE_READY,             // 就绪状态
    SYS_STATE_RUNNING,           // 运行状态
    SYS_STATE_PAUSED,            // 暂停状态
    SYS_STATE_ERROR,             // 错误状态
    SYS_STATE_SHUTDOWN           // 关闭状态
};

// 通信状态枚举
typedef enum {
    COMM_STATE_DISCONNECTED = 0, // 断开连接
    COMM_STATE_CONNECTING,       // 连接中
    COMM_STATE_CONNECTED,        // 已连接
    COMM_STATE_ERROR,            // 连接错误
    COMM_STATE_TIMEOUT           // 连接超时
} comm_state_t;

// 日志输出目标枚举
enum log_target {
    LOG_TARGET_NONE = 0,         // 无输出
    LOG_TARGET_CONSOLE,          // 控制台输出
    LOG_TARGET_FILE,             // 文件输出
    LOG_TARGET_NETWORK,          // 网络输出
    LOG_TARGET_ALL               // 全部输出
};

/*==============================================================================
 * 动态配置开关桩
 *============================================================================*/

// 可运行时修改的开关标志
static volatile bool runtime_debug_enable = false;        // 运行时调试启用
static volatile bool runtime_verbose_enable = false;      // 运行时详细输出启用
static volatile bool runtime_stats_enable = true;         // 运行时统计启用

// 临时测试开关（仅用于调试）
#ifdef DEBUG_BUILD
    #define TEMP_TEST_SWITCH_A      1    // 临时测试开关A
    #define TEMP_TEST_SWITCH_B      0    // 临时测试开关B
    #define TEMP_TEST_SWITCH_C      0    // 临时测试开关C
#else
    #define TEMP_TEST_SWITCH_A      0
    #define TEMP_TEST_SWITCH_B      0
    #define TEMP_TEST_SWITCH_C      0
#endif

/*==============================================================================
 * 开关桩状态查询函数
 *============================================================================*/

/**
 * @brief 获取开关桩状态
 * @param switch_name 开关桩名称
 * @return 开关桩状态 (1=开启, 0=关闭)
 */
int get_switch_state(const char* switch_name) {
    // 这里可以实现动态开关桩状态查询
    // 实际项目中可能从配置文件或数据库读取
    return 0;
}

/**
 * @brief 设置开关桩状态
 * @param switch_name 开关桩名称  
 * @param state 开关桩状态 (1=开启, 0=关闭)
 * @return 设置结果 (0=成功, -1=失败)
 */
int set_switch_state(const char* switch_name, int state) {
    // 这里可以实现动态开关桩状态设置
    // 实际项目中可能写入配置文件或数据库
    return 0;
}

/*==============================================================================
 * 开关桩相关的宏定义
 *============================================================================*/

// 开关桩检查宏
#define IS_SWITCH_ENABLED(switch_name)   (switch_name == 1)
#define IS_SWITCH_DISABLED(switch_name)  (switch_name == 0)

// 条件执行宏
#define IF_SWITCH_ENABLED(switch_name, code) \
    do { if (IS_SWITCH_ENABLED(switch_name)) { code } } while(0)

#define IF_SWITCH_DISABLED(switch_name, code) \
    do { if (IS_SWITCH_DISABLED(switch_name)) { code } } while(0)

// 开关桩组合宏
#define SWITCHES_ALL_ENABLED(s1, s2, s3) \
    (IS_SWITCH_ENABLED(s1) && IS_SWITCH_ENABLED(s2) && IS_SWITCH_ENABLED(s3))

#define SWITCHES_ANY_ENABLED(s1, s2, s3) \
    (IS_SWITCH_ENABLED(s1) || IS_SWITCH_ENABLED(s2) || IS_SWITCH_ENABLED(s3)) 