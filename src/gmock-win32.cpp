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
                ::ImageDirectoryEntryToDataEx(
                    base, TRUE, IMAGE_DIRECTORY_ENTRY_IMPORT, &ulSize, &pFoundHeader));
        }
        __except (InvalidReadExceptionFilter(GetExceptionInformation()))
        {
            // We don't patch module function in that case
            return HRESULT_FROM_WIN32(ERROR_MOD_NOT_FOUND);
        }

        return *resultDescriptor ?
            S_OK : HRESULT_FROM_WIN32(::GetLastError());
    }

    HRESULT writeProcessMemory(
        const LPVOID address, const LPCVOID buffer, const size_t size) noexcept
    {
        if (!address || !buffer)
            return E_INVALIDARG;

        const auto processHandle = ::GetCurrentProcess();

        if (::WriteProcessMemory(processHandle, address, buffer, size, nullptr) == 0
            && (ERROR_NOACCESS == ::GetLastError()))
        {
            DWORD oldProtect{ };
            if (::VirtualProtect(address, size, PAGE_WRITECOPY, &oldProtect))
            {
                std::shared_ptr< void > finalAction(nullptr, [&](auto&&...) {
                    ::VirtualProtect(address, size, oldProtect, &oldProtect);
                });

                if (!::WriteProcessMemory(processHandle, address, buffer, size, nullptr))
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
            findImportProc(::GetModuleHandle(nullptr), funcAddr, &ppfn));

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
            findImportProc(::GetModuleHandle(nullptr), stubFunc, &ppfn));

        return writeProcessMemory(ppfn, &origFunc, sizeof(ppfn));
    }

} // namespace

void mockModule_patchModuleFunc(
    void* funcAddr, void* newFunc, void** oldFunc)
{
    if (FAILED(patchImportFunc(funcAddr, newFunc, oldFunc)))
        throw std::runtime_error{ "failed to patch module function" };
}

void mockModule_restoreModuleFunc(
    void* origFunc, void* stubFunc, void** oldFunc)
{
    if (FAILED(restoreImportFunc(origFunc, stubFunc)))
        throw std::runtime_error{ "failed to restore module function" };

    if (oldFunc)
        *oldFunc = nullptr;
}

namespace gmock_win32 {
namespace detail {

    thread_local int lock = 0;

    void avoid_opt(const int& v)
    {
        wchar_t text[] = { (wchar_t)v, L'\0' };
        ::GetWindowText(nullptr, text, 0);
    }

    proxy_base::proxy_base() noexcept
    {
        avoid_opt(lock);
    }
    
    proxy_base::~proxy_base() noexcept
    {
        avoid_opt(--lock);
    }

} // namespace detail

    bypass_mocks::bypass_mocks() noexcept
    {
        ++detail::lock;
    }

    bypass_mocks::~bypass_mocks() noexcept
    {
        --detail::lock;
    }

} // namespace gmock_win32
