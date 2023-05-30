#include "Network.h"



// 管道 - 获取一个命名管道名称
IOS_AUTH_RUNTIME_API std::string auth_pipe_random_name() noexcept
{
	GUID 		vPipeGuid;
	::CoCreateGuid(&vPipeGuid);

	char		vPipeName[64] = { 0 };
	std::sprintf(vPipeName, R"(\\.\Pipe\%08lX-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X)",
		vPipeGuid.Data1, vPipeGuid.Data2, vPipeGuid.Data3,
		vPipeGuid.Data4[0], vPipeGuid.Data4[1], vPipeGuid.Data4[2], vPipeGuid.Data4[3],
		vPipeGuid.Data4[4], vPipeGuid.Data4[5], vPipeGuid.Data4[6], vPipeGuid.Data4[7]);
	return vPipeName;
}

// 管道 - 缓冲区大小
IOS_AUTH_RUNTIME_API uint32_t auth_pipe_cache_size() noexcept
{
	static constexpr uint32_t	static_pipe_cache_size = 1024;
	return static_pipe_cache_size;
}

// 管道 - 创建
IOS_AUTH_RUNTIME_API HANDLE auth_pipe_create(const char* _Name) noexcept
{
	auto		vCacheSize = auth_pipe_cache_size();

	// 创建管道
	auto		vHandle = CreateNamedPipeA(_Name,				// pipe name
							PIPE_ACCESS_DUPLEX,		// read/write access
							PIPE_TYPE_MESSAGE |		// message type pipe
							PIPE_READMODE_MESSAGE |		// message-read mode
							PIPE_WAIT,			// blocking mode
							PIPE_UNLIMITED_INSTANCES,	// max. instances
							vCacheSize,			// output buffer size
							vCacheSize,			// input buffer size
							1000,				// client time-out
							nullptr);			// no security attribute
	if(INVALID_HANDLE_VALUE == vHandle)
	{
		return INVALID_HANDLE_VALUE;
	}

	// 连接至管道
	if(!ConnectNamedPipe(vHandle, nullptr))
	{
		if(GetLastError() != ERROR_PIPE_CONNECTED)
		{
			CloseHandle(vHandle);
			return INVALID_HANDLE_VALUE;
		}
	}
	return vHandle;
}

// 管道 - 打开
IOS_AUTH_RUNTIME_API HANDLE auth_pipe_open(const char* _Name) noexcept
{
	auto		vHandle = CreateFileA(_Name, GENERIC_READ | GENERIC_WRITE, 0, nullptr, OPEN_EXISTING, FILE_FLAG_WRITE_THROUGH, nullptr);
	if(INVALID_HANDLE_VALUE == vHandle)
	{
		return INVALID_HANDLE_VALUE;
	}
	return vHandle;
}

// 管道 - 打开 - 重试次数
IOS_AUTH_RUNTIME_API HANDLE auth_pipe_open(const char* _Name, uint32_t _Retry) noexcept
{
	while(_Retry != 0)
	{
		auto		vHandle = auth_pipe_open(_Name);
		if(vHandle != INVALID_HANDLE_VALUE)
		{
			return vHandle;
		}
		Sleep(1000);
		--_Retry;
	}
	return INVALID_HANDLE_VALUE;
}

// 管道 - 读取
IOS_AUTH_RUNTIME_API int auth_pipe_read(HANDLE _Handle, void* _Bytes, uint32_t _Size) noexcept
{
	auto		vSync = static_cast<DWORD>(0);
	auto		vSuccess = ReadFile(_Handle, _Bytes, _Size, &vSync, nullptr);
	return vSuccess ? (int)vSync : -1;
}

// 管道 - 写入
IOS_AUTH_RUNTIME_API int auth_pipe_write(HANDLE _Handle, const void* _Bytes, uint32_t _Size) noexcept
{
	auto		vSync = static_cast<DWORD>(0);
	auto		vSuccess = WriteFile(_Handle, _Bytes, _Size, &vSync, nullptr);
	FlushFileBuffers(_Handle);
	return vSuccess ? (int)vSync : -1;
}

// 管道 - 读取 - 所有
IOS_AUTH_RUNTIME_API bool auth_pipe_read_all(HANDLE _Handle, void* _Bytes, uint32_t _Size) noexcept
{
	auto 		vAlready = 0U;
	auto 		vBlock = auth_pipe_cache_size();
	auto		vBytes = (char*)_Bytes;
	while(vAlready != _Size)
	{
		auto		vSize = ((_Size - vAlready) > vBlock) ? vBlock : (_Size - vAlready);
		auto		vSync = auth_pipe_read(_Handle, vBytes + vAlready, vSize);
		if(vSync <= 0)
		{
			break;
		}
		vAlready += vSync;
	}
	return vAlready == _Size;
}

// 管道 - 写入 - 所有
IOS_AUTH_RUNTIME_API bool auth_pipe_write_all(HANDLE _Handle, const void* _Bytes, uint32_t _Size) noexcept
{
	auto 		vAlready = 0U;
	auto 		vBlock = auth_pipe_cache_size();
	auto		vBytes = (const char*)_Bytes;
	while(vAlready != _Size)
	{
		auto		vSize = ((_Size - vAlready) > vBlock) ? vBlock : (_Size - vAlready);
		auto		vSync = auth_pipe_write(_Handle, vBytes + vAlready, vSize);
		if(vSync <= 0)
		{
			break;
		}
		vAlready += vSync;
	}
	return vAlready == _Size;
}

// 管道 - 关闭
IOS_AUTH_RUNTIME_API void auth_pipe_close(HANDLE _Handle) noexcept
{
	CloseHandle(_Handle);
}

// 管道 - 释放
IOS_AUTH_RUNTIME_API void auth_pipe_release(HANDLE _Handle) noexcept
{
	DisconnectNamedPipe(_Handle);
	CloseHandle(_Handle);
}



// 套接字 - 绑定
IOS_AUTH_RUNTIME_API SOCKET auth_socket_bind(uint16_t _Port) noexcept
{
	auto		vSocket = socket(AF_INET, SOCK_STREAM, 0);
	if(INVALID_SOCKET == vSocket)
	{
		return INVALID_SOCKET;
	}
	struct sockaddr_in	vAddress {};
	vAddress.sin_family = AF_INET;
	vAddress.sin_port = htons(_Port);
	vAddress.sin_addr.s_addr = htonl(INADDR_ANY);

	auto		vError = bind(vSocket, (struct sockaddr*)&vAddress, sizeof(struct sockaddr_in));
	if(SOCKET_ERROR == vError)
	{
		closesocket(vSocket);
		return INVALID_SOCKET;
	}
	listen(vSocket, SOMAXCONN);
	return vSocket;
}

// 套接字 - 连接
IOS_AUTH_RUNTIME_API SOCKET auth_socket_connect(const char* _Address, uint16_t _Port) noexcept
{
	auto		vSocket = socket(AF_INET, SOCK_STREAM, 0);
	if(vSocket != SOCKET_ERROR)
	{
		struct sockaddr_in	vSockAddrIN {};
		vSockAddrIN.sin_addr.s_addr = inet_addr(_Address);
		vSockAddrIN.sin_family = AF_INET;
		vSockAddrIN.sin_port = htons(_Port);

		auto		vError = connect(vSocket, (struct sockaddr*)&vSockAddrIN, sizeof(struct sockaddr));
		if(vError != SOCKET_ERROR)
		{
			return vSocket;
		}
		closesocket(vSocket);
	}
	return INVALID_SOCKET;
}

// 套接字 - 读取
IOS_AUTH_RUNTIME_API int auth_socket_read(SOCKET _Socket, void* _Bytes, uint32_t _Size) noexcept
{
	auto		vSync = recv(_Socket, (char*)_Bytes, (int)_Size, 0);
	return vSync;
}

// 套接字 - 写入
IOS_AUTH_RUNTIME_API int auth_socket_write(SOCKET _Socket, const void* _Bytes, uint32_t _Size) noexcept
{
	auto		vSync = send(_Socket, (const char*)_Bytes, (int)_Size, 0);
	return vSync;
}

// 套接字 - 读取 - 所有
IOS_AUTH_RUNTIME_API bool auth_socket_read_all(SOCKET _Socket, void* _Bytes, uint32_t _Size) noexcept
{
	auto 		vAlready = 0U;
	auto 		vBlock = auth_pipe_cache_size();
	auto		vBytes = (char*)_Bytes;
	while(vAlready != _Size)
	{
		auto		vSize = ((_Size - vAlready) > vBlock) ? vBlock : (_Size - vAlready);
		auto		vSync = auth_socket_read(_Socket, vBytes + vAlready, vSize);
		if(vSync <= 0)
		{
			break;
		}
		vAlready += vSync;
	}
	return vAlready == _Size;
}

// 套接字 - 写入 - 所有
IOS_AUTH_RUNTIME_API bool auth_socket_write_all(SOCKET _Socket, const void* _Bytes, uint32_t _Size) noexcept
{
	auto 		vAlready = 0U;
	auto 		vBlock = auth_pipe_cache_size();
	auto		vBytes = (const char*)_Bytes;
	while(vAlready != _Size)
	{
		auto		vSize = ((_Size - vAlready) > vBlock) ? vBlock : (_Size - vAlready);
		auto		vSync = auth_socket_write(_Socket, vBytes + vAlready, vSize);
		if(vSync <= 0)
		{
			break;
		}
		vAlready += vSync;
	}
	return vAlready == _Size;
}

// 套接字 - 关闭
IOS_AUTH_RUNTIME_API void auth_socket_close(SOCKET _Socket) noexcept
{
	closesocket(_Socket);
}



// 协议包 - 读取
IOS_AUTH_RUNTIME_API bool auth_package_read(HANDLE _Stream, char** _Bytes, uint32_t* _Size) noexcept
{
	uint32_t		vSync = 0;
	char* vBytes = nullptr;

	if(!auth_pipe_read_all(_Stream, &vSync, 4))
	{
		return false;
	}

	vBytes = new(std::nothrow) char[vSync];
	if(vBytes == nullptr)
	{
		return false;
	}

	if(!auth_pipe_read_all(_Stream, vBytes, vSync))
	{
		delete[] vBytes;
		return false;
	}

	*_Bytes = vBytes;
	*_Size = vSync;
	return true;
}

// 协议包 - 写入
IOS_AUTH_RUNTIME_API bool auth_package_write(HANDLE _Stream, const char* _Bytes, uint32_t _Size) noexcept
{
	if(!auth_pipe_write_all(_Stream, &_Size, 4))
	{
		return false;
	}

	if(!auth_pipe_write_all(_Stream, _Bytes, _Size))
	{
		return false;
	}

	return true;
}

// 协议包 - 接收
IOS_AUTH_RUNTIME_API bool auth_package_recv(SOCKET _Stream, char** _Bytes, uint32_t* _Size) noexcept
{
	uint32_t		vSync = 0;
	char* vBytes = nullptr;

	if(!auth_socket_read_all(_Stream, &vSync, 4))
	{
		return false;
	}

	vBytes = new(std::nothrow) char[vSync];
	if(vBytes == nullptr)
	{
		return false;
	}

	if(!auth_socket_read_all(_Stream, vBytes, vSync))
	{
		delete[] vBytes;
		return false;
	}

	*_Bytes = vBytes;
	*_Size = vSync;
	return true;
}

// 协议包 - 发送
IOS_AUTH_RUNTIME_API bool auth_package_send(SOCKET _Stream, const char* _Bytes, uint32_t _Size) noexcept
{
	if(!auth_socket_write_all(_Stream, &_Size, 4))
	{
		return false;
	}

	if(!auth_socket_write_all(_Stream, _Bytes, _Size))
	{
		return false;
	}

	return true;
}

// 协议包 - 释放
IOS_AUTH_RUNTIME_API void auth_package_free(const char* _Bytes) noexcept
{
	delete[] _Bytes;
}
