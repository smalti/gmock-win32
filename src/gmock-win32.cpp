#include "gmock-win32.h"

#include <shlwapi.h>
#include <windows.h>

#pragma warning(push)
#pragma warning(disable: 4091)
#include <dbghelp.h>
#pragma warning(pop)

#include <memory>
#include <stdexcept>

#pragma comment(lib, "dbghelp.lib")
#pragma comment(lib, "shlwapi.lib")

#define RETURN_IF_FAILED(hr)            \
do {                                    \
    const auto hres = HRESULT{ hr };    \
    if (FAILED(hres))                   \
        return hres;                    \
} while (false)

namespace {

    LONG WINAPI InvalidReadExceptionFilter(PEXCEPTION_POINTERS /*pep*/)
    {
        return EXCEPTION_EXECUTE_HANDLER;
    }

    HRESULT importDescriptor(
        const HMODULE baseAddress, PIMAGE_IMPORT_DESCRIPTOR* resultDescriptor)
    {
        if (!baseAddress || !resultDescriptor)
            return E_INVALIDARG;

        *resultDescriptor = nullptr;

        ULONG ulSize{ };
        PIMAGE_SECTION_HEADER pFoundHeader{ };

        __try
        {
            *resultDescriptor = reinterpret_cast< PIMAGE_IMPORT_DESCRIPTOR >(
                ::ImageDirectoryEntryToDataEx(
                    baseAddress, TRUE, IMAGE_DIRECTORY_ENTRY_IMPORT, &ulSize, &pFoundHeader));
        }
        __except (InvalidReadExceptionFilter(GetExceptionInformation()))
        {
            // We don't patch module function in that case
            return HRESULT_FROM_WIN32(ERROR_MOD_NOT_FOUND);
        }

        return *resultDescriptor ?
            S_OK : HRESULT_FROM_WIN32(::GetLastError());
    }

    HRESULT funcModuleName(
        const PVOID funcAddr, LPSTR moduleName, DWORD moduleNameSize)
    {
        if (!funcAddr || !moduleName || !moduleNameSize)
            return E_INVALIDARG;

        HMODULE module{ };
        if (!::GetModuleHandleExA(
            GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
            GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT, static_cast< LPCSTR >(funcAddr), &module))
        {
            return HRESULT_FROM_WIN32(::GetLastError());
        }

        CHAR modulePath[MAX_PATH] = { };
        if (!::GetModuleFileNameA(module, modulePath, MAX_PATH))
            return HRESULT_FROM_WIN32(::GetLastError());

        if (::strcpy_s(moduleName, moduleNameSize, ::PathFindFileNameA(modulePath)))
            return E_NOT_SUFFICIENT_BUFFER;

        return S_OK;
    }

    HRESULT importModuleThunkData(
        const HMODULE baseAddress, const LPCSTR importModule, PIMAGE_THUNK_DATA32* thunkData)
    {
        if (!baseAddress || !importModule || !thunkData)
            return E_INVALIDARG;

        PIMAGE_IMPORT_DESCRIPTOR importDescr{ };
        RETURN_IF_FAILED(importDescriptor(baseAddress, &importDescr));

        for (; importDescr->Name; ++importDescr)
        {
            const auto pszModName = reinterpret_cast< PSTR >(
                reinterpret_cast< PBYTE >(baseAddress) + importDescr->Name);

            if (::lstrcmpiA(pszModName, importModule) == 0)
                break;
        }

        if (!importDescr->Name)
            return HRESULT_FROM_WIN32(ERROR_MOD_NOT_FOUND);

        *thunkData = reinterpret_cast< PIMAGE_THUNK_DATA32 >(
            reinterpret_cast< PBYTE >(baseAddress) + importDescr->FirstThunk);

        return S_OK;
    }

    HRESULT importModuleThunkDataFromAddress(
        const HMODULE baseAddress, PVOID funcAddr, PIMAGE_THUNK_DATA32* thunkData)
    {
        if (!baseAddress || !funcAddr || !thunkData)
            return E_INVALIDARG;

        CHAR moduleName[MAX_PATH] = { };
        RETURN_IF_FAILED(funcModuleName(funcAddr, moduleName, MAX_PATH));

        return importModuleThunkData(baseAddress, moduleName, thunkData);
    }

    HRESULT writeProcessMemory(
        const LPVOID address, const LPCVOID buffer, const size_t size)
    {
        if (::WriteProcessMemory(::GetCurrentProcess(), address, buffer, size, nullptr) == 0
            && (ERROR_NOACCESS == ::GetLastError()))
        {
            DWORD oldProtect{ };
            if (::VirtualProtect(address, size, PAGE_WRITECOPY, &oldProtect))
            {               
                std::shared_ptr< void > finalAction(nullptr, [&](auto&&...) {
                    ::VirtualProtect(address, size, oldProtect, &oldProtect);
                });

                if (!::WriteProcessMemory(::GetCurrentProcess(), address, buffer, size, nullptr))
                    return HRESULT_FROM_WIN32(::GetLastError());

                return S_OK;
            }
            else
            {
                return HRESULT_FROM_WIN32(::GetLastError());
            }
        }
        else
        {
            return S_OK;
        }
    }

    HRESULT patchImportFunc(
        PVOID funcAddr, const PVOID newFunc, PVOID* oldFunc)
    {
        if (!funcAddr || !newFunc || !oldFunc)
            return E_INVALIDARG;

        PIMAGE_THUNK_DATA32 thunkData{ };
        RETURN_IF_FAILED(importModuleThunkDataFromAddress(
            ::GetModuleHandle(nullptr), funcAddr, &thunkData));

        for (; thunkData->u1.Function; ++thunkData)
        {
            const auto ppfn =
                reinterpret_cast< LPVOID* >(&thunkData->u1.Function);

            if (*ppfn == funcAddr)
            {
                HRESULT hRes = E_FAIL;
                if (SUCCEEDED(hRes =
                    writeProcessMemory(ppfn, &newFunc, sizeof(ppfn))))
                {
                    if (oldFunc)
                        *oldFunc = funcAddr;
                }

                return hRes;
            }
        }

        return HRESULT_FROM_WIN32(ERROR_PROC_NOT_FOUND);
    }

    HRESULT restoreImportFunc(PVOID origFunc, const PVOID stubFunc)
    {
        if (!origFunc || !stubFunc)
            return E_INVALIDARG;

        PIMAGE_THUNK_DATA32 thunkData{ };
        RETURN_IF_FAILED(importModuleThunkDataFromAddress(
            ::GetModuleHandle(nullptr), origFunc, &thunkData));

        for (; thunkData->u1.Function; ++thunkData)
        {
            const auto ppfn =
                reinterpret_cast< LPVOID* >(&thunkData->u1.Function);

            if (*ppfn == stubFunc)
                return writeProcessMemory(ppfn, &origFunc, sizeof(ppfn));
        }

        return HRESULT_FROM_WIN32(ERROR_PROC_NOT_FOUND);
    }

} // namespace

void patchModuleFunc(
    void* funcAddr, void* newFunc, void** oldFunc)
{
    if (FAILED(patchImportFunc(funcAddr, newFunc, oldFunc)))
        throw std::runtime_error{ "failed to patch module function" };
}

void restoreModuleFunc(void* origFunc, void* stubFunc, void** oldProc)
{
    if (FAILED(restoreImportFunc(origFunc, stubFunc)))
        throw std::runtime_error{ "failed to restore module function" };

    if (oldProc)
        *oldProc = nullptr;
}
