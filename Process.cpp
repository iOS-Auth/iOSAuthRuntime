#include "Process.h"


// 创建进程
IOS_AUTH_RUNTIME_API HANDLE auth_process_create(const std::string& _Application, const std::string& _Directory, const std::string& _Param) noexcept
{
	SHELLEXECUTEINFOA	vInfoShell = { sizeof(SHELLEXECUTEINFOA) };

	vInfoShell.fMask = SEE_MASK_NOCLOSEPROCESS | SEE_MASK_FLAG_NO_UI;
	vInfoShell.lpFile = _Application.data();
	vInfoShell.lpParameters = _Param.data();
	vInfoShell.lpDirectory = _Directory.data();
	vInfoShell.nShow = SW_HIDE;
	if(ShellExecuteExA(&vInfoShell))
	{
		auto		vHandle = vInfoShell.hProcess;
		if(vHandle != INVALID_HANDLE_VALUE)
		{
			return vHandle;
		}
	}
	return INVALID_HANDLE_VALUE;
}

// 执行进程
IOS_AUTH_RUNTIME_API bool auth_process_exec(const std::string& _Application, const std::string& _Directory, const std::string& _Param) noexcept
{
	SHELLEXECUTEINFOA	vInfoShell = { sizeof(SHELLEXECUTEINFOA) };

	vInfoShell.fMask = SEE_MASK_NOCLOSEPROCESS | SEE_MASK_FLAG_NO_UI;
	vInfoShell.lpFile = _Application.data();
	vInfoShell.lpParameters = _Param.data();
	vInfoShell.lpDirectory = _Directory.data();
	vInfoShell.nShow = SW_HIDE;
	if(ShellExecuteExA(&vInfoShell))
	{
		auto		vHandle = vInfoShell.hProcess;
		if(vHandle != INVALID_HANDLE_VALUE)
		{
			CloseHandle(vHandle);
			return true;
		}
	}
	return false;
}

// 等待进程
IOS_AUTH_RUNTIME_API int auth_process_wait(HANDLE _Handle) noexcept
{
	DWORD		vExitCode = STILL_ACTIVE;

	WaitForSingleObject(_Handle, INFINITE);
	GetExitCodeProcess(_Handle, &vExitCode);
	CloseHandle(_Handle);

	auto		vSync = (int)vExitCode;

	return vSync;
}
