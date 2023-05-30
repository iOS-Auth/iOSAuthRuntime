#include "Handle.h"
#include "Library.h"
#include "Convert.h"


// 模块句柄
static HMODULE		static_module_iTunesCore		= nullptr;
static HMODULE		static_module_AirTrafficHost		= nullptr;

// 函数偏移
static uint32_t		offset_KBSyncMachineId			= 0x00000000;
static uint32_t		offset_KBSyncLibraryId			= 0x00000000;
static uint32_t		offset_KBSyncCreateToken		= 0x00000000;
static uint32_t		offset_AirFairSyncSessionCreate		= 0x00000000;
static uint32_t		offset_AirFairSyncGrappaCreate		= 0x00000000;
static uint32_t		offset_AirFairSyncGrappaUpdate		= 0x00000000;
static uint32_t		offset_AirFairSyncVerifyRequest		= 0x00000000;
static uint32_t		offset_AirFairSyncSetRequest		= 0x00000000;
static uint32_t		offset_AirFairSyncAccountAuthorize	= 0x00000000;
static uint32_t		offset_AirFairSyncGetResponse		= 0x00000000;
static uint32_t		offset_AirFairSyncCalcSig		= 0x00000000;



// iOSAuth: 处理偏移
IOS_AUTH_RUNTIME_API bool iOSAuthHandleOffset() noexcept
{
	static_module_iTunesCore = (HMODULE)iOSAuthLibraryModuleAddress(iOS_AUTH_MODULE_iTunesCore);
	static_module_AirTrafficHost = (HMODULE)iOSAuthLibraryModuleAddress(iOS_AUTH_MODULE_AirTrafficHost);

	auto 		vVersion = iOSAuthLibraryModuleVersion(iOS_AUTH_MODULE_iTunesCore);
	if(vVersion == L"12.4.3.1")
	{
		offset_KBSyncMachineId				= 0x003B2E80;
		offset_KBSyncLibraryId				= 0x003B2FB0;
		offset_KBSyncCreateToken			= 0x0001CB50;
		offset_AirFairSyncSessionCreate			= 0x0009E230;
		offset_AirFairSyncGrappaCreate			= 0x00006560;
		offset_AirFairSyncGrappaUpdate			= 0x00008650;
		offset_AirFairSyncVerifyRequest			= 0x0009F3B0;
		offset_AirFairSyncSetRequest			= 0x0007A1C0;
		offset_AirFairSyncAccountAuthorize		= 0x00045CC0;
		offset_AirFairSyncGetResponse			= 0x00031F50;
		offset_AirFairSyncCalcSig			= 0x000376D0;
	}
	else
	{
		return false;
	}
	return true;
}



// KBSync: 获取本机Key
IOS_AUTH_RUNTIME_API bool KBSyncMachineKey(AUTH_KEY* _MachineKey) noexcept
{
	typedef int (*iOSAuthFuncType)(AUTH_KEY* _MachineKey);

	auto		vFuncEntry = (iOSAuthFuncType)(((uint32_t)static_module_iTunesCore) + offset_KBSyncMachineId);

	int		vStatus = vFuncEntry(_MachineKey);
	return vStatus == 1;
}

// KBSync: 获取运行库Key
IOS_AUTH_RUNTIME_API bool KBSyncLibraryKey(AUTH_KEY* _LibraryKey) noexcept
{
	typedef int (*iOSAuthFuncType)(AUTH_KEY* _LibraryKey);

	auto		vFuncEntry = (iOSAuthFuncType)(((uint32_t)static_module_iTunesCore) + offset_KBSyncLibraryId);

	int		vStatus = vFuncEntry(_LibraryKey);
	return vStatus == 1;
}

// KBSync: 获取设备Key
IOS_AUTH_RUNTIME_API bool KBSyncDeviceKey(const char* _UDID, AUTH_KEY* _DeviceKey) noexcept
{
	auto		vSize = _UDID ? strlen(_UDID) : 0;
	if(vSize == 0 || _DeviceKey == nullptr)
	{
		return false;
	}

	if(40 <= vSize)
	{
		auto		vBytes = auth_convert_hex_to_bytes(_UDID, vSize);
		std::memcpy(_DeviceKey->data, vBytes.data(), vBytes.size());
	}
	else if(25 == vSize)
	{
		auto		vKey = (uint32_t*)_DeviceKey->data;
		vKey[0] = 0xB0A49760;
		vKey[1] = 0x9F0A0AC2;

		// 转换前8位
		auto		vBytesL = auth_convert_hex_to_bytes(_UDID, 8);
		std::memcpy(vKey + 2, vBytesL.data(), 4);

		// 转换后16位
		auto		vBytesR = auth_convert_hex_to_bytes(_UDID + 9, 16);
		std::memcpy(vKey + 3, vBytesR.data(), 8);
	}
	else
	{
		return false;
	}

	_DeviceKey->size = 20;
	return true;
}

// KBSync: 创建Token
IOS_AUTH_RUNTIME_API NTSTATUS KBSyncTokenCreate(const AUTH_KEY* _MachineKey, const AUTH_KEY* _LibraryKey, const char* _SCInfoDir, AUTH_TOKEN* _Token) noexcept
{
	typedef int (*iOSAuthFuncType)(const AUTH_KEY* _MachineKey, const AUTH_KEY* _LibraryKey, const char* _SCInfoDir, AUTH_TOKEN* _Token);

	auto		vFuncEntry = (iOSAuthFuncType)(((uint32_t)static_module_iTunesCore) + offset_KBSyncCreateToken);

	auto		vStatus = vFuncEntry(_MachineKey, _LibraryKey, _SCInfoDir, _Token);
	return vStatus;
}



// AFSync: 创建会话
IOS_AUTH_RUNTIME_API NTSTATUS AirFairSyncSessionCreate(AUTH_TOKEN _Token, const void* _FPC_D, int _FPC_L, const AUTH_KEY* _DeviceKey, const AUTH_ATTR* _Attr, DWORD _Type, AUTH_SESSION* _Session) noexcept
{
	typedef int (*iOSAuthFuncType)(AUTH_TOKEN _Token, const void* _FPC_D, int _FPC_L, const AUTH_KEY* _DeviceKey, const AUTH_ATTR* _Attr, DWORD _Type, AUTH_SESSION* _Session);

	auto		vFuncEntry = (iOSAuthFuncType)(((uint32_t)static_module_iTunesCore) + offset_AirFairSyncSessionCreate);

	auto		vStatus = vFuncEntry(_Token, _FPC_D, _FPC_L, _DeviceKey, _Attr, _Type, _Session);
	return vStatus;
}

// AFSync: 创建通信密钥
IOS_AUTH_RUNTIME_API NTSTATUS AirFairSyncGrappaCreate(const char* _UDID, void** _GPA_Data, DWORD* _GPA_Size, AUTH_GRAPPA* _Grappa)
{
	typedef int(*iOSAuthFuncType)(uint32_t* _Unknown1, uint32_t* _Unknown2, void** _GPA_Data, DWORD* _GPA_Size);

	auto		vFuncEntry = (iOSAuthFuncType)(((uint32_t)static_module_AirTrafficHost) + offset_AirFairSyncGrappaCreate);

	auto		vATH_Connect = new(std::nothrow) char[512];
	if(vATH_Connect == nullptr)
	{
		return -1;
	}
	std::memset(vATH_Connect, 0, 512);

	auto		vArg_0 = (uint32_t*)(vATH_Connect + (0 * sizeof(uint32_t)));
	auto		vArg_1 = (uint32_t*)(vATH_Connect + (1 * sizeof(uint32_t)));
	auto		vArg_2 = (uint32_t*)(vATH_Connect + (2 * sizeof(uint32_t)));
	auto		vArg_3 = (uint32_t*)(vATH_Connect + (3 * sizeof(uint32_t)));
	auto		vArg_4 = (uint32_t*)(vATH_Connect + (4 * sizeof(uint32_t)));
	auto		vArg_5 = (uint32_t*)(vATH_Connect + (5 * sizeof(uint32_t)));
	auto		vArg_6 = (uint32_t*)(vATH_Connect + (6 * sizeof(uint32_t)));
	auto		vArg_7 = (uint32_t*)(vATH_Connect + (7 * sizeof(uint32_t)));
	auto		vArg_8 = (char*)(vATH_Connect + (8 * sizeof(uint32_t)));

	*vArg_0 = 0;
	*vArg_1 = 0;
	*vArg_2 = 2;
	*vArg_3 = 1;
	*vArg_4 = 1;
	*vArg_5 = 0;
	*vArg_6 = 1;
	*vArg_7 = 0;
	strcpy(vArg_8, _UDID ? _UDID : "");

	auto		vStatus = vFuncEntry(vArg_4, vArg_7, _GPA_Data, _GPA_Size);
	*_Grappa = (AUTH_GRAPPA)*vArg_7;
	return vStatus;
}

// AFSync: 密钥更新
IOS_AUTH_RUNTIME_API NTSTATUS AirFairSyncGrappaUpdate(AUTH_GRAPPA _Grappa, const void* _GPA_Data, DWORD _GPA_Size)
{
	typedef int (*iOSAuthFuncType)(AUTH_GRAPPA _GPA, const void* _GPA_Data, DWORD _GPA_Size);

	auto		vFuncEntry = (iOSAuthFuncType)(((uint32_t)static_module_AirTrafficHost) + offset_AirFairSyncGrappaUpdate);

	auto		vStatus = vFuncEntry(_Grappa, _GPA_Data, _GPA_Size);
	return vStatus;
}

// AFSync: 验证请求
IOS_AUTH_RUNTIME_API NTSTATUS AirFairSyncVerifyRequest(AUTH_SESSION _Session, const void* _RqData, int _RqSize, const void* _SigData, int _SigSize) noexcept
{
	typedef int (*iOSAuthFuncType)(AUTH_SESSION _Session, const void* _RqData, int _RqSize, const void* _SigData, int _SigSize);

	auto		vFuncEntry = (iOSAuthFuncType)(((uint32_t)static_module_iTunesCore) + offset_AirFairSyncVerifyRequest);

	auto		vStatus = vFuncEntry(_Session, _RqData, _RqSize, _SigData, _SigSize);
	return vStatus;
}

// AFSync: 设置请求
IOS_AUTH_RUNTIME_API NTSTATUS AirFairSyncSetRequest(AUTH_SESSION _Session, const void* _RqData, int _RqSize, DWORD _Unknown1, DWORD _Unknown2) noexcept
{
	typedef int (*iOSAuthFuncType)(AUTH_SESSION _Session, const void* _RqData, int _RqSize, DWORD _Unknown1, DWORD _Unknown2);

	auto		vFuncEntry = (iOSAuthFuncType)(((uint32_t)static_module_iTunesCore) + offset_AirFairSyncSetRequest);

	auto		vStatus = vFuncEntry(_Session, _RqData, _RqSize, _Unknown1, _Unknown2);
	return vStatus;
}

// AFSync: DSID授权
IOS_AUTH_RUNTIME_API NTSTATUS AirFairSyncAccountAuthorize(AUTH_SESSION _Session, AUTH_DSID _DSID, DWORD _Unknown1, DWORD _Unknown2) noexcept
{
	typedef int (*iOSAuthFuncType)(AUTH_SESSION _Session, DWORD _DSID_L, DWORD _DSID_H, DWORD _Unknown1, DWORD _Unknown2);

	auto		vFuncEntry = (iOSAuthFuncType)(((uint32_t)static_module_iTunesCore) + offset_AirFairSyncAccountAuthorize);

	auto		vStatus = vFuncEntry(_Session, _DSID.LowPart, _DSID.HighPart, _Unknown1, _Unknown2);
	return vStatus;
}

// AFSync: 获取答复
IOS_AUTH_RUNTIME_API NTSTATUS AirFairSyncGetResponse(AUTH_SESSION _Session, char** _RsData, int* _RsSize, int* _Unknown1, int* _Unknown2) noexcept
{
	typedef NTSTATUS(*iOSAuthFuncType)(AUTH_SESSION _Session, char** _RsData, int* _RsSize, int* _Unknown1, int* _Unknown2);

	auto		vFuncEntry = (iOSAuthFuncType)(((uint32_t)static_module_iTunesCore) + offset_AirFairSyncGetResponse);

	auto		vStatus = vFuncEntry(_Session, _RsData, _RsSize, _Unknown1, _Unknown2);
	return vStatus;
}

// AFSync: 计算SIG
IOS_AUTH_RUNTIME_API NTSTATUS AirFairSyncCalcSig(AUTH_GRAPPA _Grappa, const void* _RsData, int _RsSize, char** _SigData, int* _SigSize) noexcept
{
	typedef int (*iOSAuthFuncType)(AUTH_GRAPPA _Grappa, const void* _RsData, int _RsSize, char** _SigData, int* _SigSize);

	auto		vFuncEntry = (iOSAuthFuncType)(((uint32_t)static_module_iTunesCore) + offset_AirFairSyncCalcSig);

	auto		vStatus = vFuncEntry(_Grappa, _RsData, _RsSize, _SigData, _SigSize);
	return vStatus;
}
