/**
 * @file vcu_config.h
 * @brief VCU测试开关桩配置定义
 * @version 1.0
 * @date 2024-12-26
 */

#ifndef VCU_CONFIG_H
#define VCU_CONFIG_H

/*==============================================================================
 * VCU系统控制开关桩
 *============================================================================*/

// VCU系统实时时钟相关
#define VCU_SyRTC_0001_01    0    // 系统实时时钟桩001
#define VCU_SyRTC_0002_01    1    // 系统实时时钟桩002
#define VCU_SyRTC_0003_01    0    // 系统实时时钟桩003

// VCU通信控制
#define VCU_COMM_ENABLE      1    // VCU通信使能
#define VCU_COMM_TIMEOUT     0    // VCU通信超时控制
#define VCU_HEARTBEAT_ON     1    // 心跳信号开启

// VCU监控功能
#define VCU_MONITOR_ENABLE   1    // VCU监控功能使能
#define VCU_STAT_COLLECT     0    // 统计数据收集

/*==============================================================================
 * 安全控制开关桩
 *============================================================================*/

#define SAFETY_MODE_ENABLE    1   // 安全模式启用
#define SAFETY_CHECK_DISABLE  0   // 安全检查禁用
#define EMERGENCY_STOP_FLAG   0   // 紧急停止标志
#define FAULT_TOLERANCE_ON    1   // 容错功能开启

/*==============================================================================
 * 调试功能开关桩
 *============================================================================*/

#define DEBUG_LOG_ENABLE      0   // 调试日志启用
#define DEBUG_TRACE_ON        0   // 调试跟踪开启
#define DEBUG_ASSERT_DISABLE  1   // 调试断言禁用
#define VERBOSE_OUTPUT_FLAG   0   // 详细输出标志

/*==============================================================================
 * 测试功能开关桩
 *============================================================================*/

#define TEST_MODE_ENABLE      0   // 测试模式启用
#define TEST_SIMULATION_ON    0   // 测试仿真开启
#define TEST_INJECT_FAULT     0   // 故障注入测试
#define TEST_PERFORMANCE      0   // 性能测试开关

/*==============================================================================
 * 功能特性开关桩
 *============================================================================*/

#define FEATURE_AUTO_RECONNECT  1  // 自动重连功能
#define FEATURE_DATA_BACKUP     1  // 数据备份功能
#define FEATURE_REMOTE_CONFIG   0  // 远程配置功能
#define FEATURE_OTA_UPDATE      0  // OTA升级功能

/*==============================================================================
 * 枚举类型开关桩
 *============================================================================*/

// VCU运行模式枚举
typedef enum {
    VCU_MODE_NORMAL = 0,      // 正常模式
    VCU_MODE_DEBUG,           // 调试模式
    VCU_MODE_TEST,            // 测试模式
    VCU_MODE_MAINTENANCE,     // 维护模式
    VCU_MODE_FAULT_SAFE       // 故障安全模式
} vcu_mode_t;

// 通信协议选择
enum comm_protocol {
    COMM_PROTOCOL_UDP = 0,    // UDP协议
    COMM_PROTOCOL_TCP,        // TCP协议
    COMM_PROTOCOL_SERIAL,     // 串口协议
    COMM_PROTOCOL_CAN         // CAN总线协议
};

// 日志级别枚举
typedef enum {
    LOG_LEVEL_DEBUG = 0,      // 调试级别
    LOG_LEVEL_INFO,           // 信息级别
    LOG_LEVEL_WARNING,        // 警告级别
    LOG_LEVEL_ERROR,          // 错误级别
    LOG_LEVEL_CRITICAL        // 严重错误级别
} log_level_t;

/*==============================================================================
 * 配置参数开关桩
 *============================================================================*/

// 网络配置
#define CONFIG_UDP_PORT         18125   // UDP端口号
#define CONFIG_TIMEOUT_MS       5000    // 超时时间(毫秒)
#define CONFIG_RETRY_COUNT      3       // 重试次数
#define CONFIG_BUFFER_SIZE      4096    // 缓冲区大小

// 系统配置
#define CONFIG_MAX_DEVICES      32      // 最大设备数
#define CONFIG_MAX_SWITCHES     256     // 最大开关桩数
#define CONFIG_LOG_FILE_SIZE    1048576 // 日志文件大小(1MB)

/*==============================================================================
 * 条件编译开关桩
 *============================================================================*/

#ifdef VCU_DEBUG_BUILD
    #define DEBUG_MEMORY_CHECK    1   // 内存检查
    #define DEBUG_STACK_CHECK     1   // 栈检查
    #define DEBUG_TIMING_CHECK    1   // 时序检查
#else
    #define DEBUG_MEMORY_CHECK    0
    #define DEBUG_STACK_CHECK     0
    #define DEBUG_TIMING_CHECK    0
#endif

#ifdef VCU_RELEASE_BUILD
    #define OPTIMIZE_PERFORMANCE  1   // 性能优化
    #define MINIMIZE_MEMORY       1   // 内存优化
    #define DISABLE_LOGGING       1   // 禁用日志
#else
    #define OPTIMIZE_PERFORMANCE  0
    #define MINIMIZE_MEMORY       0
    #define DISABLE_LOGGING       0
#endif

/*==============================================================================
 * 平台相关开关桩
 *============================================================================*/

#if defined(WIN32) || defined(_WIN32)
    #define PLATFORM_WINDOWS_SUPPORT   1   // Windows平台支持
    #define PLATFORM_POSIX_SUPPORT     0   // POSIX平台支持
#elif defined(__linux__) || defined(__unix__)
    #define PLATFORM_WINDOWS_SUPPORT   0
    #define PLATFORM_POSIX_SUPPORT     1
#else
    #define PLATFORM_WINDOWS_SUPPORT   0
    #define PLATFORM_POSIX_SUPPORT     0
#endif

#endif /* VCU_CONFIG_H */ 