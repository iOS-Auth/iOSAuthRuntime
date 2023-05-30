#ifndef			_IOS_AUTH_RUNTIME_NETWORK_H_
#define			_IOS_AUTH_RUNTIME_NETWORK_H_

#include "Header.h"


// 管道 - 获取一个命名管道名称
IOS_AUTH_RUNTIME_API std::string auth_pipe_random_name() noexcept;

// 管道 - 缓冲区大小
IOS_AUTH_RUNTIME_API uint32_t auth_pipe_cache_size() noexcept;

// 管道 - 创建
IOS_AUTH_RUNTIME_API HANDLE auth_pipe_create(const char* _Name) noexcept;

// 管道 - 打开
IOS_AUTH_RUNTIME_API HANDLE auth_pipe_open(const char* _Name) noexcept;

// 管道 - 打开 - 重试次数
IOS_AUTH_RUNTIME_API HANDLE auth_pipe_open(const char* _Name, uint32_t _Retry) noexcept;

// 管道 - 读取
IOS_AUTH_RUNTIME_API int auth_pipe_read(HANDLE _Handle, void* _Bytes, uint32_t _Size) noexcept;

// 管道 - 写入
IOS_AUTH_RUNTIME_API int auth_pipe_write(HANDLE _Handle, const void* _Bytes, uint32_t _Size) noexcept;

// 管道 - 读取 - 所有
IOS_AUTH_RUNTIME_API bool auth_pipe_read_all(HANDLE _Handle, void* _Bytes, uint32_t _Size) noexcept;

// 管道 - 写入 - 所有
IOS_AUTH_RUNTIME_API bool auth_pipe_write_all(HANDLE _Handle, const void* _Bytes, uint32_t _Size) noexcept;

// 管道 - 关闭
IOS_AUTH_RUNTIME_API void auth_pipe_close(HANDLE _Handle) noexcept;

// 管道 - 释放
IOS_AUTH_RUNTIME_API void auth_pipe_release(HANDLE _Handle) noexcept;



// 套接字 - 绑定
IOS_AUTH_RUNTIME_API SOCKET auth_socket_bind(uint16_t _Port) noexcept;

// 套接字 - 连接
IOS_AUTH_RUNTIME_API SOCKET auth_socket_connect(const char* _Address, uint16_t _Port) noexcept;

// 套接字 - 读取
IOS_AUTH_RUNTIME_API int auth_socket_read(SOCKET _Socket, void* _Bytes, uint32_t _Size) noexcept;

// 套接字 - 写入
IOS_AUTH_RUNTIME_API int auth_socket_write(SOCKET _Socket, const void* _Bytes, uint32_t _Size) noexcept;

// 套接字 - 读取 - 所有
IOS_AUTH_RUNTIME_API bool auth_socket_read_all(SOCKET _Socket, void* _Bytes, uint32_t _Size) noexcept;

// 套接字 - 写入 - 所有
IOS_AUTH_RUNTIME_API bool auth_socket_write_all(SOCKET _Socket, const void* _Bytes, uint32_t _Size) noexcept;

// 套接字 - 关闭
IOS_AUTH_RUNTIME_API void auth_socket_close(SOCKET _Socket) noexcept;



// 协议包 - 读取
IOS_AUTH_RUNTIME_API bool auth_package_read(HANDLE _Stream, char** _Bytes, uint32_t* _Size) noexcept;

// 协议包 - 写入
IOS_AUTH_RUNTIME_API bool auth_package_write(HANDLE _Stream, const char* _Bytes, uint32_t _Size) noexcept;

// 协议包 - 接收
IOS_AUTH_RUNTIME_API bool auth_package_recv(SOCKET _Stream, char** _Bytes, uint32_t* _Size) noexcept;

// 协议包 - 发送
IOS_AUTH_RUNTIME_API bool auth_package_send(SOCKET _Stream, const char* _Bytes, uint32_t _Size) noexcept;

// 协议包 - 释放
IOS_AUTH_RUNTIME_API void auth_package_free(const char* _Bytes) noexcept;


#endif
