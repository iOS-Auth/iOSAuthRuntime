#ifndef			_IOS_AUTH_RUNTIME_EXEC_H_
#define			_IOS_AUTH_RUNTIME_EXEC_H_

#include "Header.h"


// 授权错误码
typedef enum AUTH_ERROR
{
	AUTH_E_SUCCESS = 0,			// 授权成功

	AUTH_E_SOCKET_CONNECT = 0x10,		// 建立连接
	AUTH_E_SOCKET_RECV = 0x11,		// 接收数据
	AUTH_E_SOCKET_SEND = 0x12,		// 发送数据
	AUTH_E_SOCKET_VERIFY = 0x13,		// 验证数据

	AUTH_E_RUNTIME_INIT = 0x20,		// 运行时初始化
	AUTH_E_FUNCTION_INIT = 0x21,		// 函数初始化

	AUTH_E_TOKEN_CREATE = 0x30,		// 创建会话

	AUTH_E_SESSION_CREATE = 0x40,		// 创建会话

	AUTH_E_GRAPPA_CREATE = 0x50,		// 创建密钥

	AUTH_E_AFSYNC_RQ_SET = 0x60,		// 设置RQ
	AUTH_E_AFSYNC_RQ_VERIFY = 0x61,		// 验证RQ

	AUTH_E_AFSYNC_RS_DATA = 0x70,		// 获取RS数据
	AUTH_E_AFSYNC_RS_SIG = 0x71,		// 获取RSSig

	AUTH_E_USER_AUTH = 0x80,		// 用户授权失败

	AUTH_E_UNKNOWN_ERROR = 255		// 未知错误
}AUTH_ERROR;



// iOSAuth: 执行授权
IOS_AUTH_RUNTIME_API AUTH_ERROR auth_exec_sync_pipe(const char* _Pipe) noexcept;

// iOSAuth: 执行授权
IOS_AUTH_RUNTIME_API AUTH_ERROR auth_exec_sync_socket(SOCKET _Socket) noexcept;



#endif
