#include "gmock-win32.h"

#include <windows.h>

#pragma warning(push)
#pragma warning(disable: 4091)
#include <dbghelp.h>
#pragma warning(pop)

#include <memory>
#include <string>
#include <stdexcept>

#pragma comment(lib, "dbghelp.lib")

#define THROW_HRESULT(hr, text)                     \
    throw std::runtime_error{                       \
        std::string{ "message: " } + text +         \
        "; HRESULT code: " + std::to_string(hr) };           

#define THROW_ERROR_CODE(err, text)                 \
    throw std::runtime_error{                       \
        std::string{ "message: " } + text +         \
        "; error code: " + std::to_string(err) };   

#define RETURN_IF_FAILED_HRESULT(hr)                \
do                                                  \
{                                                   \
    const auto hRes = HRESULT{ hr };                \
    if (FAILED(hRes))                               \
        return hRes;                                \
} while (false)

#define THROW_IF_FAILED_HRESULT(hr, text)           \
do                                                  \
{                                                   \
    const auto hRes = HRESULT{ hr };                \
    if (FAILED(hRes))                               \
        THROW_HRESULT(hRes, text);                  \
} while (false)   

namespace {
namespace module {

    constexpr auto kernel32 = L"kernel32";
    constexpr auto dbghelp  = L"dbghelp";

} // namespace module

    using UniqueHMODULE = std::unique_ptr<
        std::remove_pointer_t< HMODULE >, BOOL(WINAPI*)(HMODULE) >;

namespace utils {

    UniqueHMODULE loadModule(const wchar_t* moduleName)
    {
        if (const auto hmodule = ::LoadLibraryW(moduleName))
            return UniqueHMODULE{ hmodule, &FreeLibrary };
        else
            THROW_ERROR_CODE(::GetLastError(), "gmock_win32 initialization failed");

        return UniqueHMODULE{ nullptr, { } };
    }

} // namespace utils

    struct LibCore
    {
        DWORD   (WINAPI* pfn_GetLastError)(VOID){ };
        HANDLE  (WINAPI* pfn_GetCurrentProcess)(VOID){ };
        BOOL    (WINAPI* pfn_WriteProcessMemory)(HANDLE, LPVOID, LPCVOID, SIZE_T, SIZE_T*){ };
        BOOL    (WINAPI* pfn_VirtualProtect)(LPVOID, SIZE_T, DWORD, PDWORD){ };
        HMODULE (WINAPI* pfn_GetModuleHandleA)(LPCSTR){ };
        PVOID   (WINAPI* pfn_ImageDirectoryEntryToDataEx)(PVOID, BOOLEAN, USHORT, PULONG, PIMAGE_SECTION_HEADER*){ };

    public:
        LibCore() :
            dbghelp_ { utils::loadModule(module::dbghelp),},
            kernel32_{ utils::loadModule(module::kernel32) }
        {
            setupDynLinkingFunctions();
        }

    private:

#define SETUP_DYNLINK_FUNC(mod, proc) \
    setupProcAddr(mod##_.get(), #proc, pfn_##proc); \
    if (!pfn_##proc) \
        THROW_ERROR_CODE(pfn_GetLastError(), "failed to setup function")

#define SETUP_DYNLINK_KERNEL32_FUNC(func) SETUP_DYNLINK_FUNC(kernel32, func)
#define SETUP_DYNLINK_DBGHELP_FUNC(func)  SETUP_DYNLINK_FUNC(dbghelp, func)

        void setupDynLinkingFunctions()
        {
            // Kernel32
            SETUP_DYNLINK_KERNEL32_FUNC(GetLastError);
            SETUP_DYNLINK_KERNEL32_FUNC(GetCurrentProcess);
            SETUP_DYNLINK_KERNEL32_FUNC(WriteProcessMemory);
            SETUP_DYNLINK_KERNEL32_FUNC(VirtualProtect);
            SETUP_DYNLINK_KERNEL32_FUNC(GetModuleHandleA);

            // Dbghelp
            SETUP_DYNLINK_DBGHELP_FUNC(ImageDirectoryEntryToDataEx);
        }

#undef SETUP_DYNLINK_FUNC
#undef SETUP_DYNLINK_KERNEL32_FUNC
#undef SETUP_DYNLINK_DBGHELP_FUNC

        template< typename FuncType >
        void setupProcAddr(const HMODULE hmodule, const char* name, FuncType& pfnProc) noexcept
        {
            pfnProc = reinterpret_cast< FuncType >(::GetProcAddress(hmodule, name));
        }

    private:
        const UniqueHMODULE dbghelp_;
        const UniqueHMODULE kernel32_;
    };

    std::unique_ptr< LibCore > core = nullptr;

    LONG WINAPI invalidReadExceptionFilter(PEXCEPTION_POINTERS /*pep*/)
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
                core->pfn_ImageDirectoryEntryToDataEx(
                    base, TRUE, IMAGE_DIRECTORY_ENTRY_IMPORT, &ulSize, &pFoundHeader));
        }
        __except (invalidReadExceptionFilter(GetExceptionInformation()))
        {
            // We don't patch module function in that case
            return HRESULT_FROM_WIN32(ERROR_MOD_NOT_FOUND);
        }

        return *resultDescriptor ?
            S_OK : HRESULT_FROM_WIN32(core->pfn_GetLastError());
    }

    HRESULT writeProcessMemory(
        const LPVOID address, const LPCVOID buffer, const size_t size) noexcept
    {
        if (!address || !buffer)
            return E_INVALIDARG;

        const auto processHandle = core->pfn_GetCurrentProcess();

        if (core->pfn_WriteProcessMemory(processHandle, address, buffer, size, nullptr) == 0
            && (ERROR_NOACCESS == core->pfn_GetLastError()))
        {
            DWORD oldProtect{ };
            if (core->pfn_VirtualProtect(address, size, PAGE_WRITECOPY, &oldProtect))
            {               
                std::shared_ptr< void > finalAction(nullptr, [&](auto&&...) {
                    core->pfn_VirtualProtect(address, size, oldProtect, &oldProtect);
                });

                if (!core->pfn_WriteProcessMemory(processHandle, address, buffer, size, nullptr))
                    return HRESULT_FROM_WIN32(core->pfn_GetLastError());

                return S_OK;
            }
            else
            {
                return HRESULT_FROM_WIN32(core->pfn_GetLastError());
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
        RETURN_IF_FAILED_HRESULT(importDescriptor(base, &descr));

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
        RETURN_IF_FAILED_HRESULT(
            findImportProc(core->pfn_GetModuleHandleA(nullptr), funcAddr, &ppfn));

        RETURN_IF_FAILED_HRESULT(
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
        RETURN_IF_FAILED_HRESULT(
            findImportProc(core->pfn_GetModuleHandleA(nullptr), stubFunc, &ppfn));

        return writeProcessMemory(ppfn, &origFunc, sizeof(ppfn));
    }

} // namespace

namespace gmock_win32 {
namespace detail {

    thread_local int lock = 0;

    void patch_module_func(
        void* funcAddr, void* newFunc, void** oldFunc)
    {
        if (!core)
            throw std::runtime_error{ "gmock_win32 is not initialized" };

        THROW_IF_FAILED_HRESULT(patchImportFunc(
            funcAddr, newFunc, oldFunc), "failed to patch module function");
    }

    void restore_module_func(
        void* origFunc, void* stubFunc, void** oldFunc)
    {
        if (!core)
            throw std::runtime_error{ "gmock_win32 is uninitialized" };

        THROW_IF_FAILED_HRESULT(restoreImportFunc(
            origFunc, stubFunc), "failed to restore module function");

        if (oldFunc)
            *oldFunc = nullptr;
    }
    
    proxy_base::~proxy_base() noexcept
    {
        --lock;
    }

} // namespace detail

    void initialize()
    {
        if (core)
            throw std::runtime_error{ "already initialized" };

        core = std::make_unique< LibCore >();
    }

    void uninitialize() noexcept
    {
        core.reset();
    }

    bypass_mocks::bypass_mocks() noexcept
    {
        ++detail::lock;
    }

    bypass_mocks::~bypass_mocks() noexcept
    {
        --detail::lock;
    }

} // namespace gmock_win32
