#include "Authorize.h"


// Dynamic library process attach
void ios_auth_runtime_dynamic_library_process_attach()
{
}

// Dynamic library process detach
void ios_auth_runtime_dynamic_library_process_detach()
{
}

// Dynamic library thread attach
void ios_auth_runtime_dynamic_library_thread_attach()
{
}

// Dynamic library thread detach
void ios_auth_runtime_dynamic_library_thread_detach()
{
}


// Dynamic library entry
#if defined(_WIN32)
extern "C" BOOL WINAPI DllMain(HANDLE _DllHandle, DWORD _Reason, LPVOID _Reserved)
{
	UNREFERENCED_PARAMETER(_DllHandle);
	UNREFERENCED_PARAMETER(_Reserved);

	switch(_Reason)
	{
		case DLL_PROCESS_ATTACH:
			ios_auth_runtime_dynamic_library_process_attach();
			break;
		case DLL_THREAD_ATTACH:
			ios_auth_runtime_dynamic_library_thread_attach();
			break;
		case DLL_THREAD_DETACH:
			ios_auth_runtime_dynamic_library_thread_detach();
			break;
		case DLL_PROCESS_DETACH:
			ios_auth_runtime_dynamic_library_process_detach();
			break;
		default:
			break;
	}
	return TRUE;
}
#else
__attribute((constructor)) void ios_auth_runtime_dynamic_library_init(void)
{
	ios_auth_runtime_dynamic_library_process_attach();
}

__attribute((destructor)) void ios_auth_runtime_dynamic_library_fini(void)
{
	ios_auth_runtime_dynamic_library_process_detach();
}
#endif
