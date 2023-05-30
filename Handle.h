#ifndef			_IOS_AUTH_RUNTIME_HANDLE_H_
#define			_IOS_AUTH_RUNTIME_HANDLE_H_

#include "Header.h"


// 授权Key
typedef struct AUTH_KEY
{
	uint32_t 			size;
	uint8_t 			data[20];
}AUTH_KEY;

// 授权帐号ID
typedef ULARGE_INTEGER			AUTH_DSID;

// 授权属性
typedef struct AUTH_ATTR
{
	ULONG				deviceType;
	ULONG				keyTypeSupportVersion;
}AUTH_ATTR;

// 授权Token
typedef void* AUTH_TOKEN;

// 授权会话
typedef void* AUTH_SESSION;

// 授权密钥
typedef void* AUTH_GRAPPA;



// iOSAuth: 处理偏移
IOS_AUTH_RUNTIME_API bool iOSAuthHandleOffset() noexcept;



// KBSync: 获取本机Key
IOS_AUTH_RUNTIME_API bool KBSyncMachineKey(AUTH_KEY* _MachineKey) noexcept;

// KBSync: 获取运行库Key
IOS_AUTH_RUNTIME_API bool KBSyncLibraryKey(AUTH_KEY* _LibraryKey) noexcept;

// KBSync: 获取设备Key
IOS_AUTH_RUNTIME_API bool KBSyncDeviceKey(const char* _UDID, AUTH_KEY* _DeviceKey) noexcept;

// KBSync: 创建Token
IOS_AUTH_RUNTIME_API NTSTATUS KBSyncTokenCreate(const AUTH_KEY* _MachineKey, const AUTH_KEY* _LibraryKey, const char* _SCInfoDir, AUTH_TOKEN* _Token) noexcept;



// AFSync: 创建会话
IOS_AUTH_RUNTIME_API NTSTATUS AirFairSyncSessionCreate(AUTH_TOKEN _Token, const void* _FPC_D, int _FPC_L, const AUTH_KEY* _DeviceKey, const AUTH_ATTR* _Attr, DWORD _Type, AUTH_SESSION* _Session) noexcept;

// AFSync: 密钥创建
IOS_AUTH_RUNTIME_API NTSTATUS AirFairSyncGrappaCreate(const char* _UDID, void** _GPA_Data, DWORD* _GPA_Size, AUTH_GRAPPA* _Grappa);

// AFSync: 密钥更新
IOS_AUTH_RUNTIME_API NTSTATUS AirFairSyncGrappaUpdate(AUTH_GRAPPA _Grappa, const void* _GPA_Data, DWORD _GPA_Size);

// AFSync: 验证请求
IOS_AUTH_RUNTIME_API NTSTATUS AirFairSyncVerifyRequest(AUTH_SESSION _Session, const void* _RqData, int _RqSize, const void* _SigData, int _SigSize) noexcept;

// AFSync: 设置请求
IOS_AUTH_RUNTIME_API NTSTATUS AirFairSyncSetRequest(AUTH_SESSION _Session, const void* _RqData, int _RqSize, DWORD _Unknown1, DWORD _Unknown2) noexcept;

// AFSync: DSID授权
IOS_AUTH_RUNTIME_API NTSTATUS AirFairSyncAccountAuthorize(AUTH_SESSION _Session, AUTH_DSID _DSID, DWORD _Unknown1, DWORD _Unknown2) noexcept;

// AFSync: 获取答复
IOS_AUTH_RUNTIME_API NTSTATUS AirFairSyncGetResponse(AUTH_SESSION _Session, char** _RsData, int* _RsSize, int* _Unknown1, int* _Unknown2) noexcept;

// AFSync: 计算SIG
IOS_AUTH_RUNTIME_API NTSTATUS AirFairSyncCalcSig(AUTH_GRAPPA _Grappa, const void* _RsData, int _RsSize, char** _SigData, int* _SigSize) noexcept;


#endif
