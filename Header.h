#ifndef			_IOS_AUTH_RUNTIME_HEADER_H_
#define			_IOS_AUTH_RUNTIME_HEADER_H_

// Compiler : MSVC
#if defined(_MSC_VER)
#define 		IOS_AUTH_COMPILER_MSVC				(1)
#endif

// Compiler : CLANG
#if defined(__clang__)
#define 		IOS_AUTH_COMPILER_CLANG				(1)
#endif

// Compiler : GNUC
#if defined(__GNUC__)
#define 		IOS_AUTH_COMPILER_GNUC				(1)
#endif



// Compiler attribute: api
#if defined(IOS_AUTH_COMPILER_MSVC)
#define			IOS_AUTH_COMPILER_API_EXP			__declspec(dllexport)
#define			IOS_AUTH_COMPILER_API_IMP			__declspec(dllimport)
#define			IOS_AUTH_COMPILER_API_DEF
#elif defined(IOS_AUTH_COMPILER_GNUC) || defined(IOS_AUTH_COMPILER_CLANG)
#define			IOS_AUTH_COMPILER_API_EXP			__attribute__((visibility("default")))
#define			IOS_AUTH_COMPILER_API_IMP			__attribute__((visibility("default")))
#define			IOS_AUTH_COMPILER_API_DEF			__attribute__((visibility("default")))
#else
#define			IOS_AUTH_COMPILER_API_EXP
#define			IOS_AUTH_COMPILER_API_IMP
#define			IOS_AUTH_COMPILER_API_DEF
#endif


#if defined(IOSAUTHRUNTIME_EXPORTS)
#define			IOS_AUTH_RUNTIME_API				IOS_AUTH_COMPILER_API_EXP
#else
#define			IOS_AUTH_RUNTIME_API				IOS_AUTH_COMPILER_API_IMP
#endif


#if defined(_WIN32)
#pragma warning(disable : 4996)
#pragma warning(disable : 4512)
#pragma warning(disable : 4481)
#pragma warning(disable : 4127)		// 条件表达式是常量
#pragma warning(disable : 4125)
#pragma warning(disable : 4200)		// 使用了非标准扩展 : 结构/联合中的零大小数组
#pragma warning(disable : 4293)		// Shift 计数为负或过大，其行为未定义
#pragma warning(disable : 4819)		// 文件包含不能在当前代码页(936)中表示的字符
#endif


// 平台头文件
#if defined(_WIN32)
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <windows.h>
#include <process.h>
#include <UserEnv.h>
#else
#include <unistd.h>
#endif

// C++头文件
#include <string>
#include <cstdint>
#include <vector>
#include <thread>

#endif
