#include "gmock-win32.h"

#include <windows.h>

#pragma warning(push)
#pragma warning(disable: 4091)
#include <dbghelp.h>
#pragma warning(pop)

#include <memory>
#include <stdexcept>

#pragma comment(lib, "dbghelp.lib")

#define RETURN_IF_FAILED(hr)            \
do {                                    \
    const auto hres = HRESULT{ hr };    \
    if (FAILED(hres))                   \
        return hres;                    \
} while (false)

namespace {

	typedef DWORD (WINAPI *pfnGetLastError_t)( VOID );
	pfnGetLastError_t g_pfnGetLastError = nullptr;

	typedef HANDLE (WINAPI *pfnGetCurrentProcess_t)( VOID );
	pfnGetCurrentProcess_t g_pfnGetCurrentProcess = nullptr;

	typedef BOOL (WINAPI *pfnWriteProcessMemory_t)(HANDLE hProcess, LPVOID lpBaseAddress, LPCVOID lpBuffer, SIZE_T nSize, SIZE_T* lpNumberOfBytesWritten);
	pfnWriteProcessMemory_t g_pfnWriteProcessMemory = nullptr;

	typedef BOOL (WINAPI *pfnVirtualProtect_t)(LPVOID lpAddress, SIZE_T dwSize, DWORD flNewProtect, PDWORD lpflOldProtect);
	pfnVirtualProtect_t g_pfnVirtualProtect = nullptr;
	
	typedef HMODULE (WINAPI *pfnGetModuleHandleA_t)(LPCSTR lpModuleName);
	pfnGetModuleHandleA_t g_pfnGetModuleHandleA = nullptr;
	
	typedef PVOID(__stdcall *pfnImageDirectoryEntryToDataEx_t)(PVOID Base, BOOLEAN MappedAsImage, USHORT DirectoryEntry, PULONG Size, PIMAGE_SECTION_HEADER *FoundHeader);
	pfnImageDirectoryEntryToDataEx_t g_pfnImageDirectoryEntryToDataEx = nullptr;

	static bool g_bInit = false;
	bool init() {
		HMODULE module;
		module = LoadLibraryA( "kernel32.dll" );
		if ( !module ) 
			return false;
		g_pfnGetLastError = (pfnGetLastError_t)GetProcAddress( module, "GetLastError" );
		if ( !g_pfnGetLastError ) 
			return false;
		g_pfnGetCurrentProcess = (pfnGetCurrentProcess_t)GetProcAddress( module, "GetCurrentProcess" );
		if ( !g_pfnGetCurrentProcess ) 
			return false;
		g_pfnWriteProcessMemory = (pfnWriteProcessMemory_t)GetProcAddress( module, "WriteProcessMemory" );
		if ( !g_pfnWriteProcessMemory ) 
			return false;
		g_pfnVirtualProtect = (pfnVirtualProtect_t)GetProcAddress( module, "VirtualProtect" );
		if ( !g_pfnVirtualProtect ) 
			return false;
		// Doesnt matter wchar_t or char, GetModuleHandleW or GetModuleHandleA
		g_pfnGetModuleHandleA = (pfnGetModuleHandleA_t)GetProcAddress( module, "GetModuleHandleA" );
		if ( !g_pfnGetModuleHandleA ) 
			return false;
		module = LoadLibraryA( "dbghelp.dll" );
		if ( !module ) 
			return false;
		g_pfnImageDirectoryEntryToDataEx = (pfnImageDirectoryEntryToDataEx_t)GetProcAddress( module, "ImageDirectoryEntryToDataEx" );
		if ( g_pfnImageDirectoryEntryToDataEx ) 
			return false;
		g_bInit = true;
		return true;
	}

    LONG WINAPI InvalidReadExceptionFilter(PEXCEPTION_POINTERS /*pep*/)
    {
        return EXCEPTION_EXECUTE_HANDLER;
    }

    HRESULT importDescriptor(
        const HMODULE base, PIMAGE_IMPORT_DESCRIPTOR* const resultDescriptor) noexcept
    {
        if (!base || !resultDescriptor)
            return E_INVALIDARG;

        *resultDescriptor = nullptr;

        ULONG ulSize{ };
        PIMAGE_SECTION_HEADER pFoundHeader{ };

        __try
        {
            *resultDescriptor = reinterpret_cast< PIMAGE_IMPORT_DESCRIPTOR >(
                g_pfnImageDirectoryEntryToDataEx(
                    base, TRUE, IMAGE_DIRECTORY_ENTRY_IMPORT, &ulSize, &pFoundHeader));
        }
        __except (InvalidReadExceptionFilter(GetExceptionInformation()))
        {
            // We don't patch module function in that case
            return HRESULT_FROM_WIN32(ERROR_MOD_NOT_FOUND);
        }

        return *resultDescriptor ?
            S_OK : HRESULT_FROM_WIN32(g_pfnGetLastError());
    }

    HRESULT writeProcessMemory(
        const LPVOID address, const LPCVOID buffer, const size_t size) noexcept
    {
        if (!address || !buffer)
            return E_INVALIDARG;

        const auto processHandle = g_pfnGetCurrentProcess();

        if (g_pfnWriteProcessMemory(processHandle, address, buffer, size, nullptr) == 0
            && (ERROR_NOACCESS == g_pfnGetLastError()))
        {
            DWORD oldProtect{ };
            if (g_pfnVirtualProtect(address, size, PAGE_WRITECOPY, &oldProtect))
            {               
                std::shared_ptr< void > finalAction(nullptr, [&](auto&&...) {
                    g_pfnVirtualProtect(address, size, oldProtect, &oldProtect);
                });

                if (!g_pfnWriteProcessMemory(processHandle, address, buffer, size, nullptr))
                    return HRESULT_FROM_WIN32(g_pfnGetLastError());

                return S_OK;
            }
            else
            {
                return HRESULT_FROM_WIN32(g_pfnGetLastError());
            }
        }
        else
        {
            return S_OK;
        }
    }

    PIMAGE_THUNK_DATA32 thunkData(
        const HMODULE base, const PIMAGE_IMPORT_DESCRIPTOR descr) noexcept
    {
        return reinterpret_cast< PIMAGE_THUNK_DATA32 >(
            reinterpret_cast< PBYTE >(base) + descr->FirstThunk);
    }

    LPVOID* thunkProc(const PIMAGE_THUNK_DATA32 data) noexcept
    {
        return reinterpret_cast< LPVOID* >(&data->u1.Function);
    }

    HRESULT findImportProc(
        const HMODULE base, const PVOID funcAddr, LPVOID** ppfn) noexcept
    {
        if (!base || !funcAddr || !ppfn)
            return E_INVALIDARG;

        PIMAGE_IMPORT_DESCRIPTOR descr{ };
        RETURN_IF_FAILED(importDescriptor(base, &descr));

        for (; descr->Name; ++descr)
        {
            for (auto data = thunkData(base, descr); data->u1.Function; ++data)
            {
                if (const auto proc = thunkProc(data))
                {
                    if (*proc == funcAddr)
                    {
                        *ppfn = proc;
                        return S_OK;
                    }
                }
            }
        }

        return HRESULT_FROM_WIN32(ERROR_PROC_NOT_FOUND);
    }

    HRESULT patchImportFunc(
        const PVOID funcAddr, const PVOID newFunc, PVOID* oldFunc) noexcept
    {
        if (!newFunc || !funcAddr || !oldFunc)
            return E_INVALIDARG;

        LPVOID* ppfn = nullptr;
        RETURN_IF_FAILED(
            findImportProc(g_pfnGetModuleHandleA(nullptr), funcAddr, &ppfn));

        RETURN_IF_FAILED(
            writeProcessMemory(ppfn, &newFunc, sizeof(ppfn)));

        if (oldFunc)
            *oldFunc = funcAddr;

        return S_OK;
    }

    HRESULT restoreImportFunc(const PVOID origFunc, const PVOID stubFunc) noexcept
    {
        if (!origFunc || !stubFunc)
            return E_INVALIDARG;

        LPVOID* ppfn = nullptr;
        RETURN_IF_FAILED(
            findImportProc(g_pfnGetModuleHandleA(nullptr), stubFunc, &ppfn));

        return writeProcessMemory(ppfn, &origFunc, sizeof(ppfn));
    }

} // namespace

void mockModule_patchModuleFunc(
    void* funcAddr, void* newFunc, void** oldFunc)
{
	if ( !g_bInit )
		if ( !init( ) )
	        throw std::runtime_error{ "failed to initialize patcher" };
    if (FAILED(patchImportFunc(funcAddr, newFunc, oldFunc)))
        throw std::runtime_error{ "failed to patch module function" };
}

void mockModule_restoreModuleFunc(
    void* origFunc, void* stubFunc, void** oldFunc)
{
	if ( !g_bInit )
		if ( !init( ) )
	        throw std::runtime_error{ "failed to initialize patcher" };
    if (FAILED(restoreImportFunc(origFunc, stubFunc)))
        throw std::runtime_error{ "failed to restore module function" };

    if (oldFunc)
        *oldFunc = nullptr;
}
