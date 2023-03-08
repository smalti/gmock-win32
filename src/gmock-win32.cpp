#include "gmock-win32.h"

#include <shlwapi.h>
#include <windows.h>
#include <DbgHelp.h>

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

    HRESULT importDescriptor(
        const HMODULE baseAddress, PIMAGE_IMPORT_DESCRIPTOR* resultDescriptor)
    {
        if (!baseAddress || !resultDescriptor)
            return E_INVALIDARG;

        *resultDescriptor = nullptr;

        ULONG ulSize{ };
        PIMAGE_SECTION_HEADER pFoundHeader{ };

        *resultDescriptor = reinterpret_cast<PIMAGE_IMPORT_DESCRIPTOR>(
            ::ImageDirectoryEntryToDataEx(
                baseAddress, TRUE, IMAGE_DIRECTORY_ENTRY_IMPORT, &ulSize, &pFoundHeader));

        return *resultDescriptor ?
            S_OK : HRESULT_FROM_WIN32(::GetLastError());
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
            const auto pszModName = reinterpret_cast<PSTR>(
                reinterpret_cast<PBYTE>(baseAddress) + importDescr->Name);

            if (::lstrcmpiA(pszModName, importModule) == 0)
                break;
        }

        if (!importDescr->Name)
            return HRESULT_FROM_WIN32(ERROR_MOD_NOT_FOUND);

        *thunkData = reinterpret_cast< PIMAGE_THUNK_DATA32 >(
            reinterpret_cast< PBYTE >(baseAddress) + importDescr->FirstThunk);

        return S_OK;
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

    HRESULT procModuleName(
        const PVOID procAddr, LPSTR moduleName, DWORD moduleNameSize)
    {
        if (!procAddr || !moduleName || !moduleNameSize)
            return E_INVALIDARG;

        HMODULE module{ };
        if (!::GetModuleHandleExA(
            GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
            GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT, static_cast<LPCSTR>(procAddr), &module))
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

    HRESULT patchImportProc(
        PVOID procAddr, const PVOID newProc, PVOID* oldProc)
    {
        if (!procAddr || !newProc || !oldProc)
            return E_INVALIDARG;

        CHAR moduleName[MAX_PATH] = {};
        RETURN_IF_FAILED(procModuleName(procAddr, moduleName, MAX_PATH));

        PIMAGE_THUNK_DATA32 thunkData{};
        RETURN_IF_FAILED(importModuleThunkData(
            ::GetModuleHandle(nullptr), moduleName, &thunkData));

        for (; thunkData->u1.Function; ++thunkData)
        {
            const auto ppfn =
                reinterpret_cast<LPVOID*>(&thunkData->u1.Function);

            if (*ppfn == procAddr)
            {
                HRESULT hRes = E_FAIL;
                if (SUCCEEDED(hRes =
                    writeProcessMemory(ppfn, &newProc, sizeof(ppfn))))
                {
                    if (oldProc)
                        *oldProc = procAddr;
                }

                return hRes;
            }
        }

        return HRESULT_FROM_WIN32(ERROR_PROC_NOT_FOUND);
    }

} // namespace

void patchModuleProc(
    void* procAddr, void* newProc, void** oldProc)
{
    if (FAILED(patchImportProc(procAddr, newProc, oldProc)))
        throw std::runtime_error{ "failed to patch module function" };
}
