#ifndef			_IOS_AUTH_RUNTIME_PROCESS_H_
#define			_IOS_AUTH_RUNTIME_PROCESS_H_

#include "Header.h"


// 创建进程
IOS_AUTH_RUNTIME_API HANDLE auth_process_create(const std::string& _Application, const std::string& _Directory, const std::string& _Param) noexcept;

// 执行进程
IOS_AUTH_RUNTIME_API bool auth_process_exec(const std::string& _Application, const std::string& _Directory, const std::string& _Param) noexcept;

// 等待进程
IOS_AUTH_RUNTIME_API int auth_process_wait(HANDLE _Handle) noexcept;


#endif
