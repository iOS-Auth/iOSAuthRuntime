#include "Exec.h"
#include "Handle.h"
#include "Network.h"
#include "Library.h"


// iOSAuth: 上下文
typedef struct AUTH_CONTEXT
{
	HANDLE 				sync_pipe;
	SOCKET 				sync_socket;

	AUTH_KEY			auth_machine;
	AUTH_KEY			auth_library;
	AUTH_KEY			auth_device;
	AUTH_TOKEN			auth_token;
	AUTH_SESSION			auth_session;
	AUTH_GRAPPA			auth_grappa;
	AUTH_DSID			auth_dsid;

	std::string			device_udid;
	AUTH_ATTR			device_attributes;
	std::string			device_fpc;
	std::string			device_grappa;
	std::string 			device_request;
	std::string			device_request_signature;
}AUTH_CONTEXT;



// iOSAuth: 接收数据
bool auth_exec_recv_package(AUTH_CONTEXT* _Context, char** _Bytes, uint32_t* _Size) noexcept
{
	if(_Context->sync_pipe != INVALID_HANDLE_VALUE)
	{
		if(auth_package_read(_Context->sync_pipe, _Bytes, _Size))
		{
			return true;
		}
	}
	if(_Context->sync_socket != INVALID_SOCKET)
	{
		if(auth_package_recv(_Context->sync_socket, _Bytes, _Size))
		{
			return true;
		}
	}

	return false;
}

// iOSAuth: 发送数据
bool auth_exec_send_package(AUTH_CONTEXT* _Context, const char* _Bytes, uint32_t _Size) noexcept
{
	if(_Context->sync_pipe != INVALID_HANDLE_VALUE)
	{
		if(auth_package_write(_Context->sync_pipe, _Bytes, _Size))
		{
			return true;
		}
	}
	if(_Context->sync_socket != INVALID_SOCKET)
	{
		if(auth_package_send(_Context->sync_socket, _Bytes, _Size))
		{
			return true;
		}
	}

	return false;
}



// iOSAuth: 接收数据1
AUTH_ERROR auth_exec_recv_package_1(AUTH_CONTEXT* _Context) noexcept
{
	uint32_t		vLength = 0;
	char* vBytes = nullptr;
	if(!auth_exec_recv_package(_Context, &vBytes, &vLength))
	{
		return AUTH_E_SOCKET_RECV;
	}

	// 数据详情
	auto			data_address = (const char*)vBytes;
	size_t 			data_length = vLength;

	// 检查长度
	uint32_t 		length_uid = 0;
	uint32_t 		length_fpc = 0;
	uint32_t 		length_gsi = 0x14;
	uint32_t 		length_fpa = 0x10;
	uint32_t 		length_min = 0x04 + 0x04 + length_gsi + length_fpa;

	if(data_length < length_min)
	{
		return AUTH_E_SOCKET_VERIFY;
	}

	// 获取长度
	std::memcpy(&length_uid, data_address + 0, 4);
	std::memcpy(&length_fpc, data_address + 4, 4);

	// 构建偏移
	uint32_t 		offset_gsi = 0x08;
	uint32_t 		offset_fpa = offset_gsi + length_gsi;
	uint32_t 		offset_uid = offset_fpa + length_fpa;
	uint32_t 		offset_fpc = offset_uid + length_uid;
	uint32_t 		offset_end = offset_fpc + length_fpc;

	// 再次检查长度
	if(data_length != offset_end)
	{
		return AUTH_E_SOCKET_VERIFY;
	}

	// 提取数据
	uint32_t		GrappaSupportInfo[5] = { 0 };
	std::memcpy(GrappaSupportInfo, data_address + offset_gsi, length_gsi);
	uint32_t		vFairPlayArg[0x04] = { 0 };
	std::memcpy(vFairPlayArg, data_address + offset_fpa, length_fpa);
	auto			vDeviceUDID = std::string(data_address + offset_uid, length_uid);
	auto			FairPlayCertificate = std::string(data_address + offset_fpc, length_fpc);
	auto			FairPlayDeviceType = vFairPlayArg[0];
	auto			KeyTypeSupportVersion = vFairPlayArg[1];

	_Context->device_attributes.deviceType = FairPlayDeviceType;
	_Context->device_attributes.keyTypeSupportVersion = KeyTypeSupportVersion;
	_Context->device_fpc = FairPlayCertificate;
	_Context->device_udid = vDeviceUDID;

	// 构建设备Key
	KBSyncDeviceKey(vDeviceUDID.data(), &(_Context->auth_device));
	return AUTH_E_SUCCESS;
}

// iOSAuth: 接收数据2
AUTH_ERROR auth_exec_recv_package_2(AUTH_CONTEXT* _Context) noexcept
{
	uint32_t		vLength = 0;
	char* vBytes = nullptr;
	if(!auth_exec_recv_package(_Context, &vBytes, &vLength))
	{
		return AUTH_E_SOCKET_RECV;
	}

	// 数据详情
	auto			data_address = (const char*)vBytes;
	size_t 			data_length = vLength;

	// 检查长度
	uint32_t 		length_gpa = 0x53;
	uint32_t 		length_rqs = 0x15;
	uint32_t 		length_rqd = data_length - length_gpa - length_rqs;
	uint32_t 		length_min = length_gpa + length_rqs;

	if(data_length < length_min)
	{
		return AUTH_E_SOCKET_VERIFY;
	}

	// 构建偏移
	uint32_t 		offset_gpa = 0x00;
	uint32_t 		offset_rqs = offset_gpa + length_gpa;
	uint32_t 		offset_rqd = offset_rqs + length_rqs;
	uint32_t 		offset_end = offset_rqd + length_rqd;

	// 再次检查长度
	if(data_length != offset_end)
	{
		return AUTH_E_SOCKET_VERIFY;
	}

	// 提取数据
	auto			vGrappa = std::string(data_address + offset_gpa, length_gpa);
	auto			vSyncRqSig = std::string(data_address + offset_rqs, length_rqs);
	auto			vSyncRq = std::string(data_address + offset_rqd, length_rqd);

	_Context->device_grappa = vGrappa;
	_Context->device_request = vSyncRq;
	_Context->device_request_signature = vSyncRqSig;

	return AUTH_E_SUCCESS;
}



// iOSAuth: 初始化本机数据
AUTH_ERROR auth_exec_native_init(AUTH_CONTEXT* _Context) noexcept
{
	if(!iOSAuthHandleOffset())
	{
		return AUTH_E_FUNCTION_INIT;
	}

	auto		SC_Dir = R"(C:\ProgramData\Apple Computer\iTunes\SC Info)";
	auto		vError1 = KBSyncMachineKey(&(_Context->auth_machine));
	auto		vError2 = KBSyncLibraryKey(&(_Context->auth_library));
	auto		vErrorT = KBSyncTokenCreate(&(_Context->auth_machine), &(_Context->auth_library), SC_Dir, &(_Context->auth_token));
	if(vError1 && vError2 && ERROR_SUCCESS == vErrorT)
	{
		//  这里需要换成自己的DSID
		//  DSID来源于抓包 iCloud.com 登录。
		_Context->auth_dsid.QuadPart = 99999999999ull;
		return AUTH_E_SUCCESS;
	}
	else
	{
		return AUTH_E_TOKEN_CREATE;
	}
}

// iOSAuth: 计算本机Grappa
AUTH_ERROR auth_exec_native_grappa(AUTH_CONTEXT* _Context) noexcept
{
	const auto& vFPC = _Context->device_fpc;
	auto		vAttributes = &(_Context->device_attributes);
	auto		vKeyDevice = &(_Context->auth_device);

	// 创建会话
	auto		vErrorS = AirFairSyncSessionCreate(_Context->auth_token, (void*)vFPC.data(), (int)vFPC.size(), vKeyDevice, vAttributes, (DWORD)7, &(_Context->auth_session));
	if(ERROR_SUCCESS != vErrorS)
	{
		return AUTH_E_SESSION_CREATE;
	}

	// 创建密钥
	void* GPA_Data = nullptr;
	DWORD		GPA_Size = 0;
	auto		vErrorG = AirFairSyncGrappaCreate(_Context->device_udid.data(), &GPA_Data, &GPA_Size, &(_Context->auth_grappa));
	if(ERROR_SUCCESS != vErrorG)
	{
		return AUTH_E_GRAPPA_CREATE;
	}

	// 发送包
	if(!auth_exec_send_package(_Context, (const char*)GPA_Data, GPA_Size))
	{
		return AUTH_E_SOCKET_SEND;
	}

	return AUTH_E_SUCCESS;
}

// iOSAuth: 授权设备
AUTH_ERROR auth_exec_device_authorize(AUTH_CONTEXT* _Context) noexcept
{
	const auto& vRQ = _Context->device_request;
	const auto& vGPA = _Context->device_grappa;
	auto		DSIDi64 = _Context->auth_dsid;

	// AFSync: 设置请求
	auto		vErrorU = AirFairSyncSetRequest(_Context->auth_session, vRQ.data(), (int)vRQ.size(), 0, 0);
	if(ERROR_SUCCESS != vErrorU)
	{
		return AUTH_E_AFSYNC_RQ_SET;
	}

	// 授权DSID
	auto		vErrorA = AirFairSyncAccountAuthorize(_Context->auth_session, DSIDi64, 0, 0);
	if(ERROR_SUCCESS != vErrorA)
	{
		return AUTH_E_USER_AUTH;
	}

	// AFSync: Grappa更新
	auto		vErrorG = AirFairSyncGrappaUpdate(_Context->auth_grappa, vGPA.data(), vGPA.size());
	if(ERROR_SUCCESS != vErrorG)
	{
		return AUTH_E_UNKNOWN_ERROR;
	}

	// AFSync: 获取答复
	char* vRsData = nullptr;
	int		vRsSize = 0;
	int		vUnknown1 = 0;
	int		vUnknown2 = 0;
	auto		vErrorR = AirFairSyncGetResponse(_Context->auth_session, &vRsData, &vRsSize, &vUnknown1, &vUnknown2);
	if(ERROR_SUCCESS != vErrorR)
	{
		return AUTH_E_AFSYNC_RS_DATA;
	}

	// AFSync: 计算SIG
	char* vSigData = nullptr;
	int		vSigSize = 0;
	auto		vErrorS = AirFairSyncCalcSig(_Context->auth_grappa, vRsData, vRsSize, &vSigData, &vSigSize);
	if(ERROR_SUCCESS != vErrorS)
	{
		return AUTH_E_AFSYNC_RS_SIG;
	}

	// 发送至客户端
	auto		vFullSize = vRsSize + vSigSize;
	std::string	vFullData;
	vFullData.resize(vFullSize);
	std::memcpy(&vFullData[0], vSigData, vSigSize);
	std::memcpy(&vFullData[vSigSize], vRsData, vRsSize);
	if(!auth_exec_send_package(_Context, &vFullData[0], vFullSize))
	{
		return AUTH_E_SOCKET_SEND;
	}

	return AUTH_E_SUCCESS;
}



// iOSAuth: 上下文执行
AUTH_ERROR auth_exec_context_run(AUTH_CONTEXT* _Context) noexcept
{
	auto		vStatus = AUTH_E_UNKNOWN_ERROR;
	do
	{
		vStatus = auth_exec_native_init(_Context);
		if(AUTH_E_SUCCESS != vStatus)
		{
			break;
		}

		vStatus = auth_exec_recv_package_1(_Context);
		if(AUTH_E_SUCCESS != vStatus)
		{
			break;
		}

		vStatus = auth_exec_native_grappa(_Context);
		if(AUTH_E_SUCCESS != vStatus)
		{
			break;
		}

		vStatus = auth_exec_recv_package_2(_Context);
		if(AUTH_E_SUCCESS != vStatus)
		{
			break;
		}

		vStatus = auth_exec_device_authorize(_Context);
		if(AUTH_E_SUCCESS != vStatus)
		{
			break;
		}

		vStatus = AUTH_E_SUCCESS;
	} while(false);
	return vStatus;
}



// iOSAuth: 执行授权
IOS_AUTH_RUNTIME_API AUTH_ERROR auth_exec_sync_pipe(const char* _Pipe) noexcept
{
	if(!iOSAuthLibraryInitialize())
	{
		return AUTH_E_RUNTIME_INIT;
	}

	auto 		vHandle = auth_pipe_open(_Pipe, 3);
	if(vHandle == INVALID_HANDLE_VALUE)
	{
		return AUTH_E_SOCKET_CONNECT;
	}

	auto		vContext = new(std::nothrow) AUTH_CONTEXT();
	if(vContext == nullptr)
	{
		auth_pipe_close(vHandle);
		return AUTH_E_UNKNOWN_ERROR;
	}
	vContext->sync_socket = INVALID_SOCKET;
	vContext->sync_pipe = vHandle;

	auto		vError = auth_exec_context_run(vContext);

	auth_pipe_close(vHandle);
	return vError;
}

// iOSAuth: 执行授权
IOS_AUTH_RUNTIME_API AUTH_ERROR auth_exec_sync_socket(SOCKET _Socket) noexcept
{
	if(!iOSAuthLibraryInitialize())
	{
		return AUTH_E_RUNTIME_INIT;
	}

	auto		vContext = new(std::nothrow) AUTH_CONTEXT();
	if(vContext == nullptr)
	{
		return AUTH_E_UNKNOWN_ERROR;
	}

	vContext->sync_socket = _Socket;
	vContext->sync_pipe = INVALID_HANDLE_VALUE;

	auto		vError = auth_exec_context_run(vContext);
	return vError;
}
