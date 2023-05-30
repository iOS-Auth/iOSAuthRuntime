#include "Library.h"



// 模块句柄
static HMODULE		static_module_iTunesCore = nullptr;
static HMODULE		static_module_AirTrafficHost = nullptr;
static HMODULE		static_module_CoreFoundation = nullptr;

// 模块版本
static std::wstring 	static_version_iTunesCore;
static std::wstring 	static_version_AirTrafficHost;
static std::wstring 	static_version_CoreFoundation;



// 查找文件版本
static std::wstring iOSAuthLibraryQueryFileVersion(const std::wstring& _Path) noexcept
{
	// 申请版本信息内存
	auto		vVersionSize = GetFileVersionInfoSizeW(_Path.data(), nullptr);
	if(vVersionSize == 0)
	{
		return {};
	}
	auto		vVersionData = new(std::nothrow) wchar_t[vVersionSize + 1];
	if(vVersionData == nullptr)
	{
		return {};
	}
	std::wmemset(vVersionData, 0, vVersionSize + 1);

	// 获取具体版本信息
	if(!GetFileVersionInfoW(_Path.data(), 0, vVersionSize, vVersionData))
	{
		delete[] vVersionData;
		return {};
	}

	// 转换版本信息
	VS_FIXEDFILEINFO* pVsInfo;
	auto			iFileInfoSize = sizeof(VS_FIXEDFILEINFO);
	struct LANGANDCODEPAGE
	{
		WORD    wLanguage;
		WORD    wCodePage;
	}*lpTranslate;

	wchar_t			vVersionFormat[64] = { 0 };
	if(VerQueryValueW(vVersionData, L"\\VarFileInfo\\Translation", (void**)&lpTranslate, &iFileInfoSize))
	{
		unsigned int	version_len = 0;
		if(VerQueryValueW(vVersionData, L"\\", (void**)&pVsInfo, &version_len))
		{
			std::swprintf(vVersionFormat, L"%d.%d.%d.%d", HIWORD(pVsInfo->dwFileVersionMS), LOWORD(pVsInfo->dwFileVersionMS), HIWORD(pVsInfo->dwFileVersionLS), LOWORD(pVsInfo->dwFileVersionLS));
		}
	}
	delete[] vVersionData;
	return vVersionFormat;
}

// 查找模块安装目录
static std::wstring iOSAuthLibraryModuleInstallDir(HKEY _Key, const wchar_t* _Path) noexcept
{
	std::wstring	vValue;
	HKEY		vKey;
	if(ERROR_SUCCESS == RegOpenKeyExW(_Key, _Path, 0, KEY_READ, &vKey))
	{
		wchar_t		vValueByte[MAX_PATH] = { 0 };
		DWORD		vValueSize = MAX_PATH;
		RegQueryValueExW(vKey, L"InstallDir", nullptr, nullptr, (LPBYTE)vValueByte, &vValueSize);
		vValue = vValueByte;

		if (!vValue.empty() && vValue[vValue.size() - 1] == L'\\')
		{
			vValue.pop_back();
		}

		RegCloseKey(vKey);
	}
	return vValue;
}

// 将目录添加进PATH
static bool iOSAuthLibraryEnvAppend(const std::wstring& _Directory) noexcept
{
	auto		vEnvSize = 32767;
	auto		vEnvByte = new(std::nothrow) wchar_t[vEnvSize];
	if (vEnvByte == nullptr)
	{
		return false;
	}
	std::memset(vEnvByte, 0, vEnvSize);
	if (0 == GetEnvironmentVariableW(L"PATH", vEnvByte, vEnvSize))
	{
		delete[] vEnvByte;
		return false;
	}
	auto 		vEnvNew = _Directory + L";" + vEnvByte;
	if (nullptr == std::wcsstr(vEnvByte, _Directory.data()))
	{
		if (FALSE == SetEnvironmentVariableW(L"PATH", vEnvNew.data()))
		{
			delete[] vEnvByte;
			return false;
		}
	}
	delete[] vEnvByte;
	return true;
}

// 加载苹果库
IOS_AUTH_RUNTIME_API bool iOSAuthLibraryInitialize() noexcept
{
	auto		vDirCore = iOSAuthLibraryModuleInstallDir(HKEY_LOCAL_MACHINE, LR"(SOFTWARE\Apple Computer, Inc.\iTunes)");
	auto		vDirAAS = iOSAuthLibraryModuleInstallDir(HKEY_LOCAL_MACHINE, LR"(SOFTWARE\Apple inc.\Apple Application Support)");
	auto		vDirMDS = iOSAuthLibraryModuleInstallDir(HKEY_LOCAL_MACHINE, LR"(SOFTWARE\Apple inc.\Apple Mobile Device Support)");
	if(vDirCore.empty() || vDirAAS.empty() || vDirMDS.empty())
	{
		return false;
	}

	// 设置环境变量
	iOSAuthLibraryEnvAppend(vDirCore);
	iOSAuthLibraryEnvAppend(vDirAAS);
	iOSAuthLibraryEnvAppend(vDirMDS);

	// 检查模块是否存在
	auto 		vPathCore1 = vDirCore + LR"(\iTunes.dll)";
	auto 		vPathCore2 = vDirCore + LR"(\iTunesCore.dll)";
	auto 		vPathAAS = vDirAAS + LR"(\CoreFoundation.dll)";
	auto 		vPathMDS = vDirMDS + LR"(\AirTrafficHost.dll)";

	auto		vAttribCore1 = GetFileAttributesW(vPathCore1.data());
	auto		vAttribCore2 = GetFileAttributesW(vPathCore2.data());
	auto		vAttribAAS = GetFileAttributesW(vPathAAS.data());
	auto		vAttribMDS = GetFileAttributesW(vPathMDS.data());
	if((INVALID_FILE_ATTRIBUTES == vAttribCore1 && INVALID_FILE_ATTRIBUTES == vAttribCore2) || INVALID_FILE_ATTRIBUTES == vAttribAAS || INVALID_FILE_ATTRIBUTES == vAttribMDS)
	{
		return false;
	}

	// 加载模块
	static_module_iTunesCore = LoadLibraryW(vPathCore1.data());
	if(static_module_iTunesCore == nullptr)
	{
		static_module_iTunesCore = LoadLibraryW(vPathCore2.data());
		if(static_module_iTunesCore == nullptr)
		{
			iOSAuthLibraryRelease();
			return false;
		}
	}
	static_module_AirTrafficHost = LoadLibraryW(vPathMDS.data());
	if(static_module_AirTrafficHost == nullptr)
	{
		iOSAuthLibraryRelease();
		return false;
	}
	static_module_CoreFoundation = LoadLibraryW(vPathAAS.data());
	if(static_module_CoreFoundation == nullptr)
	{
		iOSAuthLibraryRelease();
		return false;
	}

	auto 		version_core_1 = iOSAuthLibraryQueryFileVersion(vPathCore1);
	auto 		version_core_2 = iOSAuthLibraryQueryFileVersion(vPathCore2);
	static_version_iTunesCore = version_core_1.empty() ? version_core_2 : version_core_1;
	static_version_AirTrafficHost = iOSAuthLibraryQueryFileVersion(vPathMDS);
	static_version_CoreFoundation = iOSAuthLibraryQueryFileVersion(vPathAAS);
	if(static_version_iTunesCore.empty() || static_version_AirTrafficHost.empty() || static_version_CoreFoundation.empty())
	{
		iOSAuthLibraryRelease();
		return false;
	}

	return true;
}

// 释放苹果库
IOS_AUTH_RUNTIME_API void iOSAuthLibraryRelease() noexcept
{
	if(static_module_iTunesCore)
	{
		FreeLibrary(static_module_iTunesCore);
		static_module_iTunesCore = nullptr;
	}
	if(static_module_AirTrafficHost)
	{
		FreeLibrary(static_module_AirTrafficHost);
		static_module_AirTrafficHost = nullptr;
	}
	if(static_module_CoreFoundation)
	{
		FreeLibrary(static_module_CoreFoundation);
		static_module_CoreFoundation = nullptr;
	}
}

// 获取模块版本
IOS_AUTH_RUNTIME_API std::wstring iOSAuthLibraryModuleVersion(iOS_AUTH_MODULE_TYPE _Module) noexcept
{
	switch(_Module)
	{
		case iOS_AUTH_MODULE_iTunesCore:		return static_version_iTunesCore;
		case iOS_AUTH_MODULE_AirTrafficHost:		return static_version_AirTrafficHost;
		case iOS_AUTH_MODULE_CoreFoundation:		return static_version_CoreFoundation;
		default:					return {};
	}
}

// 获取模块基址
IOS_AUTH_RUNTIME_API void* iOSAuthLibraryModuleAddress(iOS_AUTH_MODULE_TYPE _Module) noexcept
{
	switch(_Module)
	{
		case iOS_AUTH_MODULE_iTunesCore:		return static_module_iTunesCore;
		case iOS_AUTH_MODULE_AirTrafficHost:		return static_module_AirTrafficHost;
		case iOS_AUTH_MODULE_CoreFoundation:		return static_module_CoreFoundation;
		default:					return nullptr;
	}
}

// 获取模块偏移
IOS_AUTH_RUNTIME_API void* iOSAuthLibraryModuleOffset(iOS_AUTH_MODULE_TYPE _Module, iOS_AUTH_MODULE_OFFSET _Offset) noexcept
{
	auto 		vAddress = (iOS_AUTH_MODULE_OFFSET)iOSAuthLibraryModuleAddress(_Module);
	return (void*)(vAddress + _Offset);
}

// 获取模块函数
IOS_AUTH_RUNTIME_API void* iOSAuthLibraryModuleFunction(iOS_AUTH_MODULE_TYPE _Module, const char* _Function) noexcept
{
	auto 		vModule = (HMODULE)iOSAuthLibraryModuleAddress(_Module);
	auto 		vAddress = (void*)GetProcAddress(vModule, _Function);
	return vAddress;
}
