#ifndef			_IOS_AUTH_RUNTIME_LIBRARY_H_
#define			_IOS_AUTH_RUNTIME_LIBRARY_H_

#include "Header.h"



// 模块类型
typedef enum iOS_AUTH_MODULE_TYPE
{
	iOS_AUTH_MODULE_iTunesCore = 0,
	iOS_AUTH_MODULE_AirTrafficHost = 1,
	iOS_AUTH_MODULE_CoreFoundation = 2,
	iOS_AUTH_MODULE_Unknown = 255
}iOS_AUTH_MODULE_TYPE;

// 模块偏移
typedef 	uint32_t			iOS_AUTH_MODULE_OFFSET;



// 加载苹果库
IOS_AUTH_RUNTIME_API bool iOSAuthLibraryInitialize() noexcept;

// 释放苹果库
IOS_AUTH_RUNTIME_API void iOSAuthLibraryRelease() noexcept;

// 获取模块版本
IOS_AUTH_RUNTIME_API std::wstring iOSAuthLibraryModuleVersion(iOS_AUTH_MODULE_TYPE _Module) noexcept;

// 获取模块基址
IOS_AUTH_RUNTIME_API void* iOSAuthLibraryModuleAddress(iOS_AUTH_MODULE_TYPE _Module) noexcept;

// 获取模块偏移
IOS_AUTH_RUNTIME_API void* iOSAuthLibraryModuleOffset(iOS_AUTH_MODULE_TYPE _Module, iOS_AUTH_MODULE_OFFSET _Offset) noexcept;

// 获取模块函数
IOS_AUTH_RUNTIME_API void* iOSAuthLibraryModuleFunction(iOS_AUTH_MODULE_TYPE _Module, const char* _Function) noexcept;



#endif
