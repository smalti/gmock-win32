#pragma once

#ifndef GMOCK_RESULT_
#define GMOCK_RESULT_(tn, ...) \
    tn ::testing::internal::Function<__VA_ARGS__>::Result
#endif

#ifndef GMOCK_ARG_
#define GMOCK_ARG_(tn, N, ...) \
    tn ::testing::internal::Function<__VA_ARGS__>::template Arg< N-1 >::type 
#endif

#ifndef GMOCK_MATCHER_
#define GMOCK_MATCHER_(tn, N, ...) \
    const ::testing::Matcher< GMOCK_ARG_(tn, N, __VA_ARGS__) >&
#endif

#ifndef GMOCK_MOCKER_
#define GMOCK_MOCKER_(arity, constness, func) \
    GTEST_CONCAT_TOKEN_(gmock##constness##arity##_##func##_, __LINE__)
#endif

namespace gmock_win32 {

    void initialize();
    void uninitialize() noexcept;

    struct init_scope
    {
        init_scope()  { initialize(); }
        ~init_scope() { uninitialize(); }
    };

    struct bypass_mocks final
    {
        bypass_mocks()  noexcept;
        ~bypass_mocks() noexcept;
    };

namespace detail {

    extern thread_local int lock;

    struct proxy_base{ ~proxy_base() noexcept; };

    template< typename Reference >
    struct ref_proxy final : proxy_base
    {
        explicit ref_proxy(Reference&& r) noexcept : ref_{ r } { }
        operator Reference() const noexcept { return ref_; }

    private:
        Reference ref_;
    };

    template< typename Reference >
    ref_proxy< Reference > make_proxy(Reference&& r) noexcept
    {
        return ref_proxy< Reference >{ std::forward< Reference >(r) };
    }

    template< typename Derived >
    struct mock_module_base
    {
        static Derived& instance()
        {
            static ::testing::NiceMock< Derived > obj;
            return obj;
        }

        static void** pp_old_fn()
        {
            static void* oldFn = nullptr;
            return &oldFn;
        }
    };

    void patch_module_func   (void*, void*, void**);
    void restore_module_func (void*, void*, void**);

} // namespace detail
} // namespace gmock_win32

#define BYPASS_MOCKS(expr) \
    do \
    { \
        const gmock_win32::bypass_mocks blockMocks{ }; \
        expr; \
    } \
    while (false);

#define MOCK_MODULE_FUNC0_(tn, constness, ct, func, ...) \
struct mock_module_##func : \
    gmock_win32::detail::mock_module_base< mock_module_##func > \
{ \
    GMOCK_RESULT_(tn, __VA_ARGS__) ct func() \
        constness \
    { \
        GMOCK_MOCKER_(0, constness, func).SetOwnerAndName(this, #func); \
        return GMOCK_MOCKER_(0, constness, func).Invoke(); \
    } \
    ::testing::MockSpec<__VA_ARGS__> gmock_##func() \
    { \
        GMOCK_MOCKER_(0, constness, func).RegisterOwner(this); \
        return GMOCK_MOCKER_(0, constness, func).With(); \
    } \
    mutable ::testing::FunctionMocker<__VA_ARGS__> \
        GMOCK_MOCKER_(0, constness, func); \
    static GMOCK_RESULT_(tn, __VA_ARGS__) ct stub() \
    { \
        if (gmock_win32::detail::lock) \
        { \
            return reinterpret_cast< decltype(&stub) >(*pp_old_fn())(); \
        } \
        else \
        { \
            const gmock_win32::bypass_mocks blockMocks{ }; \
            return instance().func(); \
        } \
    } \
};

#define MOCK_MODULE_FUNC0(m, ...) MOCK_MODULE_FUNC0_(, , , m, __VA_ARGS__)
#define MOCK_MODULE_FUNC0_CALLCONV(ct, m, ...) MOCK_MODULE_FUNC0_(, , ct, m, __VA_ARGS__)

#define MOCK_MODULE_FUNC0_STDCALL_CONV(m, ...) MOCK_MODULE_FUNC0_CALLCONV(__stdcall, m, __VA_ARGS__)
#define MOCK_MODULE_FUNC0_CDECL_CONV(m, ...) MOCK_MODULE_FUNC0_CALLCONV(__cdecl, m, __VA_ARGS__)

#define MOCK_MODULE_FUNC1_(tn, constness, ct, func, ...) \
struct mock_module_##func : \
    gmock_win32::detail::mock_module_base< mock_module_##func > \
{ \
    GMOCK_RESULT_(tn, __VA_ARGS__) ct func( \
        GMOCK_ARG_(tn, 1, __VA_ARGS__) gmock_a1) constness \
    { \
        GMOCK_MOCKER_(1, constness, func).SetOwnerAndName(this, #func); \
        return GMOCK_MOCKER_(1, constness, func).Invoke( \
            gmock_a1); \
    } \
    ::testing::MockSpec<__VA_ARGS__> gmock_##func( \
        GMOCK_MATCHER_(tn, 1, __VA_ARGS__) gmock_a1) constness \
    { \
        GMOCK_MOCKER_(1, constness, func).RegisterOwner(this); \
        return GMOCK_MOCKER_(1, constness, func).With( \
            gmock_a1); \
    } \
    mutable ::testing::FunctionMocker<__VA_ARGS__> \
        GMOCK_MOCKER_(1, constness, func); \
    static GMOCK_RESULT_(tn, __VA_ARGS__) ct stub( \
        GMOCK_ARG_(tn, 1, __VA_ARGS__) gmock_a1) \
    { \
        if (gmock_win32::detail::lock) \
        { \
            return reinterpret_cast< decltype(&stub) >(*pp_old_fn())( \
                gmock_a1); \
        } \
        else \
        { \
            const gmock_win32::bypass_mocks blockMocks{ }; \
            return instance().func( \
                gmock_a1); \
        } \
    } \
};

#define MOCK_MODULE_FUNC1(m, ...) MOCK_MODULE_FUNC1_(, , , m, __VA_ARGS__)
#define MOCK_MODULE_FUNC1_CALLCONV(ct, m, ...) MOCK_MODULE_FUNC1_(, , ct, m, __VA_ARGS__)

#define MOCK_MODULE_FUNC1_STDCALL_CONV(m, ...) MOCK_MODULE_FUNC1_CALLCONV(__stdcall, m, __VA_ARGS__)
#define MOCK_MODULE_FUNC1_CDECL_CONV(m, ...) MOCK_MODULE_FUNC1_CALLCONV(__cdecl, m, __VA_ARGS__)

#define MOCK_MODULE_FUNC2_(tn, constness, ct, func, ...) \
struct mock_module_##func : \
    gmock_win32::detail::mock_module_base< mock_module_##func > \
{ \
    GMOCK_RESULT_(tn, __VA_ARGS__) ct func( \
        GMOCK_ARG_(tn, 1, __VA_ARGS__) gmock_a1, \
        GMOCK_ARG_(tn, 2, __VA_ARGS__) gmock_a2) constness \
    { \
        GMOCK_MOCKER_(2, constness, func).SetOwnerAndName(this, #func); \
        return GMOCK_MOCKER_(2, constness, func).Invoke( \
            gmock_a1, gmock_a2); \
    } \
    ::testing::MockSpec<__VA_ARGS__> gmock_##func( \
        GMOCK_MATCHER_(tn, 1, __VA_ARGS__) gmock_a1, \
        GMOCK_MATCHER_(tn, 2, __VA_ARGS__) gmock_a2) constness \
    { \
        GMOCK_MOCKER_(2, constness, func).RegisterOwner(this); \
        return GMOCK_MOCKER_(2, constness, func).With( \
            gmock_a1, gmock_a2); \
    } \
    mutable ::testing::FunctionMocker<__VA_ARGS__> \
        GMOCK_MOCKER_(2, constness, func); \
    static GMOCK_RESULT_(tn, __VA_ARGS__) ct stub( \
        GMOCK_ARG_(tn, 1, __VA_ARGS__) gmock_a1, \
        GMOCK_ARG_(tn, 2, __VA_ARGS__) gmock_a2) \
    { \
        if (gmock_win32::detail::lock) \
        { \
            return reinterpret_cast< decltype(&stub) >(*pp_old_fn())( \
                gmock_a1, gmock_a2); \
        } \
        else \
        { \
            const gmock_win32::bypass_mocks blockMocks{ }; \
            return instance().func( \
                gmock_a1, gmock_a2); \
        } \
    } \
};

#define MOCK_MODULE_FUNC2(m, ...) MOCK_MODULE_FUNC2_(, , , m, __VA_ARGS__)
#define MOCK_MODULE_FUNC2_CALLCONV(ct, m, ...) MOCK_MODULE_FUNC2_(, , ct, m, __VA_ARGS__)

#define MOCK_MODULE_FUNC2_STDCALL_CONV(m, ...) MOCK_MODULE_FUNC2_CALLCONV(__stdcall, m, __VA_ARGS__)
#define MOCK_MODULE_FUNC2_CDECL_CONV(m, ...) MOCK_MODULE_FUNC2_CALLCONV(__cdecl, m, __VA_ARGS__)

#define MOCK_MODULE_FUNC3_(tn, constness, ct, func, ...) \
struct mock_module_##func : \
    gmock_win32::detail::mock_module_base< mock_module_##func > \
{ \
    GMOCK_RESULT_(tn, __VA_ARGS__) ct func( \
        GMOCK_ARG_(tn, 1, __VA_ARGS__) gmock_a1, \
        GMOCK_ARG_(tn, 2, __VA_ARGS__) gmock_a2, \
        GMOCK_ARG_(tn, 3, __VA_ARGS__) gmock_a3) constness \
    { \
        GMOCK_MOCKER_(3, constness, func).SetOwnerAndName(this, #func); \
        return GMOCK_MOCKER_(3, constness, func).Invoke( \
            gmock_a1, gmock_a2, gmock_a3); \
    } \
    ::testing::MockSpec<__VA_ARGS__> gmock_##func( \
        GMOCK_MATCHER_(tn, 1, __VA_ARGS__) gmock_a1, \
        GMOCK_MATCHER_(tn, 2, __VA_ARGS__) gmock_a2, \
        GMOCK_MATCHER_(tn, 3, __VA_ARGS__) gmock_a3) constness \
    { \
        GMOCK_MOCKER_(3, constness, func).RegisterOwner(this); \
        return GMOCK_MOCKER_(3, constness, func).With( \
            gmock_a1, gmock_a2, gmock_a3); \
    } \
    mutable ::testing::FunctionMocker<__VA_ARGS__> \
        GMOCK_MOCKER_(3, constness, func); \
    static GMOCK_RESULT_(tn, __VA_ARGS__) ct stub( \
        GMOCK_ARG_(tn, 1, __VA_ARGS__) gmock_a1, \
        GMOCK_ARG_(tn, 2, __VA_ARGS__) gmock_a2, \
        GMOCK_ARG_(tn, 3, __VA_ARGS__) gmock_a3) \
    { \
        if (gmock_win32::detail::lock) \
        { \
            return reinterpret_cast< decltype(&stub) >(*pp_old_fn())( \
                gmock_a1, gmock_a2, gmock_a3); \
        } \
        else \
        { \
            const gmock_win32::bypass_mocks blockMocks{ }; \
            return instance().func( \
                gmock_a1, gmock_a2, gmock_a3); \
        } \
    } \
};

#define MOCK_MODULE_FUNC3(m, ...) MOCK_MODULE_FUNC3_(, , , m, __VA_ARGS__)
#define MOCK_MODULE_FUNC3_CALLCONV(ct, m, ...) MOCK_MODULE_FUNC3_(, , ct, m, __VA_ARGS__)

#define MOCK_MODULE_FUNC3_STDCALL_CONV(m, ...) MOCK_MODULE_FUNC3_CALLCONV(__stdcall, m, __VA_ARGS__)
#define MOCK_MODULE_FUNC3_CDECL_CONV(m, ...) MOCK_MODULE_FUNC3_CALLCONV(__cdecl, m, __VA_ARGS__)

#define MOCK_MODULE_FUNC4_(tn, constness, ct, func, ...) \
struct mock_module_##func : \
    gmock_win32::detail::mock_module_base< mock_module_##func > \
{ \
    GMOCK_RESULT_(tn, __VA_ARGS__) ct func( \
        GMOCK_ARG_(tn, 1, __VA_ARGS__) gmock_a1, \
        GMOCK_ARG_(tn, 2, __VA_ARGS__) gmock_a2, \
        GMOCK_ARG_(tn, 3, __VA_ARGS__) gmock_a3, \
        GMOCK_ARG_(tn, 4, __VA_ARGS__) gmock_a4) constness \
    { \
        GMOCK_MOCKER_(4, constness, func).SetOwnerAndName(this, #func); \
        return GMOCK_MOCKER_(4, constness, func).Invoke( \
            gmock_a1, gmock_a2, gmock_a3, gmock_a4); \
    } \
    ::testing::MockSpec<__VA_ARGS__> gmock_##func( \
        GMOCK_MATCHER_(tn, 1, __VA_ARGS__) gmock_a1, \
        GMOCK_MATCHER_(tn, 2, __VA_ARGS__) gmock_a2, \
        GMOCK_MATCHER_(tn, 3, __VA_ARGS__) gmock_a3, \
        GMOCK_MATCHER_(tn, 4, __VA_ARGS__) gmock_a4) constness \
    { \
        GMOCK_MOCKER_(4, constness, func).RegisterOwner(this); \
        return GMOCK_MOCKER_(4, constness, func).With( \
            gmock_a1, gmock_a2, gmock_a3, gmock_a4); \
    } \
    mutable ::testing::FunctionMocker<__VA_ARGS__> \
        GMOCK_MOCKER_(4, constness, func); \
    static GMOCK_RESULT_(tn, __VA_ARGS__) ct stub( \
        GMOCK_ARG_(tn, 1, __VA_ARGS__) gmock_a1, \
        GMOCK_ARG_(tn, 2, __VA_ARGS__) gmock_a2, \
        GMOCK_ARG_(tn, 3, __VA_ARGS__) gmock_a3, \
        GMOCK_ARG_(tn, 4, __VA_ARGS__) gmock_a4) \
    { \
        if (gmock_win32::detail::lock) \
        { \
            return reinterpret_cast< decltype(&stub) >(*pp_old_fn())( \
                gmock_a1, gmock_a2, gmock_a3, gmock_a4); \
        } \
        else \
        { \
            const gmock_win32::bypass_mocks blockMocks{ }; \
            return instance().func( \
                gmock_a1, gmock_a2, gmock_a3, gmock_a4); \
        } \
    } \
};

#define MOCK_MODULE_FUNC4(m, ...) MOCK_MODULE_FUNC4_(, , , m, __VA_ARGS__)
#define MOCK_MODULE_FUNC4_CALLCONV(ct, m, ...) MOCK_MODULE_FUNC4_(, , ct, m, __VA_ARGS__)

#define MOCK_MODULE_FUNC4_STDCALL_CONV(m, ...) MOCK_MODULE_FUNC4_CALLCONV(__stdcall, m, __VA_ARGS__)
#define MOCK_MODULE_FUNC4_CDECL_CONV(m, ...) MOCK_MODULE_FUNC4_CALLCONV(__cdecl, m, __VA_ARGS__)

#define MOCK_MODULE_FUNC5_(tn, constness, ct, func, ...) \
struct mock_module_##func : \
    gmock_win32::detail::mock_module_base< mock_module_##func > \
{ \
    GMOCK_RESULT_(tn, __VA_ARGS__) ct func( \
        GMOCK_ARG_(tn, 1, __VA_ARGS__) gmock_a1, \
        GMOCK_ARG_(tn, 2, __VA_ARGS__) gmock_a2, \
        GMOCK_ARG_(tn, 3, __VA_ARGS__) gmock_a3, \
        GMOCK_ARG_(tn, 4, __VA_ARGS__) gmock_a4, \
        GMOCK_ARG_(tn, 5, __VA_ARGS__) gmock_a5) constness \
    { \
        GMOCK_MOCKER_(5, constness, func).SetOwnerAndName(this, #func); \
        return GMOCK_MOCKER_(5, constness, func).Invoke( \
            gmock_a1, gmock_a2, gmock_a3, gmock_a4, gmock_a5); \
    } \
    ::testing::MockSpec<__VA_ARGS__> gmock_##func( \
        GMOCK_MATCHER_(tn, 1, __VA_ARGS__) gmock_a1, \
        GMOCK_MATCHER_(tn, 2, __VA_ARGS__) gmock_a2, \
        GMOCK_MATCHER_(tn, 3, __VA_ARGS__) gmock_a3, \
        GMOCK_MATCHER_(tn, 4, __VA_ARGS__) gmock_a4, \
        GMOCK_MATCHER_(tn, 5, __VA_ARGS__) gmock_a5) constness \
    { \
        GMOCK_MOCKER_(5, constness, func).RegisterOwner(this); \
        return GMOCK_MOCKER_(5, constness, func).With( \
            gmock_a1, gmock_a2, gmock_a3, gmock_a4, gmock_a5); \
    } \
    mutable ::testing::FunctionMocker<__VA_ARGS__> \
        GMOCK_MOCKER_(5, constness, func); \
    static GMOCK_RESULT_(tn, __VA_ARGS__) ct stub( \
        GMOCK_ARG_(tn, 1, __VA_ARGS__) gmock_a1, \
        GMOCK_ARG_(tn, 2, __VA_ARGS__) gmock_a2, \
        GMOCK_ARG_(tn, 3, __VA_ARGS__) gmock_a3, \
        GMOCK_ARG_(tn, 4, __VA_ARGS__) gmock_a4, \
        GMOCK_ARG_(tn, 5, __VA_ARGS__) gmock_a5) \
    { \
        if (gmock_win32::detail::lock) \
        { \
            return reinterpret_cast< decltype(&stub) >(*pp_old_fn())( \
                gmock_a1, gmock_a2, gmock_a3, gmock_a4, gmock_a5); \
        } \
        else \
        { \
            const gmock_win32::bypass_mocks blockMocks{ }; \
            return instance().func( \
                gmock_a1, gmock_a2, gmock_a3, gmock_a4, gmock_a5); \
        } \
    } \
};

#define MOCK_MODULE_FUNC5(m, ...) MOCK_MODULE_FUNC5_(, , , m, __VA_ARGS__)
#define MOCK_MODULE_FUNC5_CALLCONV(ct, m, ...) MOCK_MODULE_FUNC5_(, , ct, m, __VA_ARGS__)

#define MOCK_MODULE_FUNC5_STDCALL_CONV(m, ...) MOCK_MODULE_FUNC5_CALLCONV(__stdcall, m, __VA_ARGS__)
#define MOCK_MODULE_FUNC5_CDECL_CONV(m, ...) MOCK_MODULE_FUNC5_CALLCONV(__cdecl, m, __VA_ARGS__)

#define MOCK_MODULE_FUNC6_(tn, constness, ct, func, ...) \
struct mock_module_##func : \
    gmock_win32::detail::mock_module_base< mock_module_##func > \
{ \
    GMOCK_RESULT_(tn, __VA_ARGS__) ct func( \
        GMOCK_ARG_(tn, 1, __VA_ARGS__) gmock_a1, \
        GMOCK_ARG_(tn, 2, __VA_ARGS__) gmock_a2, \
        GMOCK_ARG_(tn, 3, __VA_ARGS__) gmock_a3, \
        GMOCK_ARG_(tn, 4, __VA_ARGS__) gmock_a4, \
        GMOCK_ARG_(tn, 5, __VA_ARGS__) gmock_a5, \
        GMOCK_ARG_(tn, 6, __VA_ARGS__) gmock_a6) constness \
    { \
        GMOCK_MOCKER_(6, constness, func).SetOwnerAndName(this, #func); \
        return GMOCK_MOCKER_(6, constness, func).Invoke( \
            gmock_a1, gmock_a2, gmock_a3, gmock_a4, gmock_a5, gmock_a6); \
    } \
    ::testing::MockSpec<__VA_ARGS__> gmock_##func( \
        GMOCK_MATCHER_(tn, 1, __VA_ARGS__) gmock_a1, \
        GMOCK_MATCHER_(tn, 2, __VA_ARGS__) gmock_a2, \
        GMOCK_MATCHER_(tn, 3, __VA_ARGS__) gmock_a3, \
        GMOCK_MATCHER_(tn, 4, __VA_ARGS__) gmock_a4, \
        GMOCK_MATCHER_(tn, 5, __VA_ARGS__) gmock_a5, \
        GMOCK_MATCHER_(tn, 6, __VA_ARGS__) gmock_a6) constness \
    { \
        GMOCK_MOCKER_(6, constness, func).RegisterOwner(this); \
        return GMOCK_MOCKER_(6, constness, func).With( \
            gmock_a1, gmock_a2, gmock_a3, gmock_a4, gmock_a5, gmock_a6); \
    } \
    mutable ::testing::FunctionMocker<__VA_ARGS__> \
        GMOCK_MOCKER_(6, constness, func); \
    static GMOCK_RESULT_(tn, __VA_ARGS__) ct stub( \
        GMOCK_ARG_(tn, 1, __VA_ARGS__) gmock_a1, \
        GMOCK_ARG_(tn, 2, __VA_ARGS__) gmock_a2, \
        GMOCK_ARG_(tn, 3, __VA_ARGS__) gmock_a3, \
        GMOCK_ARG_(tn, 4, __VA_ARGS__) gmock_a4, \
        GMOCK_ARG_(tn, 5, __VA_ARGS__) gmock_a5, \
        GMOCK_ARG_(tn, 6, __VA_ARGS__) gmock_a6) \
    { \
        if (gmock_win32::detail::lock) \
        { \
            return reinterpret_cast< decltype(&stub) >(*pp_old_fn())( \
                gmock_a1, gmock_a2, gmock_a3, gmock_a4, gmock_a5, gmock_a6); \
        } \
        else \
        { \
            const gmock_win32::bypass_mocks blockMocks{ }; \
            return instance().func( \
                gmock_a1, gmock_a2, gmock_a3, gmock_a4, gmock_a5, gmock_a6); \
        } \
    } \
};

#define MOCK_MODULE_FUNC6(m, ...) MOCK_MODULE_FUNC6_(, , , m, __VA_ARGS__)
#define MOCK_MODULE_FUNC6_CALLCONV(ct, m, ...) MOCK_MODULE_FUNC6_(, , ct, m, __VA_ARGS__)

#define MOCK_MODULE_FUNC6_STDCALL_CONV(m, ...) MOCK_MODULE_FUNC6_CALLCONV(__stdcall, m, __VA_ARGS__)
#define MOCK_MODULE_FUNC6_CDECL_CONV(m, ...) MOCK_MODULE_FUNC6_CALLCONV(__cdecl, m, __VA_ARGS__)

#define MOCK_MODULE_FUNC7_(tn, constness, ct, func, ...) \
struct mock_module_##func : \
    gmock_win32::detail::mock_module_base< mock_module_##func > \
{ \
    GMOCK_RESULT_(tn, __VA_ARGS__) ct func( \
        GMOCK_ARG_(tn, 1, __VA_ARGS__) gmock_a1, \
        GMOCK_ARG_(tn, 2, __VA_ARGS__) gmock_a2, \
        GMOCK_ARG_(tn, 3, __VA_ARGS__) gmock_a3, \
        GMOCK_ARG_(tn, 4, __VA_ARGS__) gmock_a4, \
        GMOCK_ARG_(tn, 5, __VA_ARGS__) gmock_a5, \
        GMOCK_ARG_(tn, 6, __VA_ARGS__) gmock_a6, \
        GMOCK_ARG_(tn, 7, __VA_ARGS__) gmock_a7) constness \
    { \
        GMOCK_MOCKER_(7, constness, func).SetOwnerAndName(this, #func); \
        return GMOCK_MOCKER_(7, constness, func).Invoke( \
            gmock_a1, gmock_a2, gmock_a3, gmock_a4, gmock_a5, gmock_a6, gmock_a7); \
    } \
    ::testing::MockSpec<__VA_ARGS__> gmock_##func( \
        GMOCK_MATCHER_(tn, 1, __VA_ARGS__) gmock_a1, \
        GMOCK_MATCHER_(tn, 2, __VA_ARGS__) gmock_a2, \
        GMOCK_MATCHER_(tn, 3, __VA_ARGS__) gmock_a3, \
        GMOCK_MATCHER_(tn, 4, __VA_ARGS__) gmock_a4, \
        GMOCK_MATCHER_(tn, 5, __VA_ARGS__) gmock_a5, \
        GMOCK_MATCHER_(tn, 6, __VA_ARGS__) gmock_a6, \
        GMOCK_MATCHER_(tn, 7, __VA_ARGS__) gmock_a7) constness \
    { \
        GMOCK_MOCKER_(7, constness, func).RegisterOwner(this); \
        return GMOCK_MOCKER_(7, constness, func).With( \
            gmock_a1, gmock_a2, gmock_a3, gmock_a4, gmock_a5, gmock_a6, gmock_a7); \
    } \
    mutable ::testing::FunctionMocker<__VA_ARGS__> \
        GMOCK_MOCKER_(7, constness, func); \
    static GMOCK_RESULT_(tn, __VA_ARGS__) ct stub( \
        GMOCK_ARG_(tn, 1, __VA_ARGS__) gmock_a1, \
        GMOCK_ARG_(tn, 2, __VA_ARGS__) gmock_a2, \
        GMOCK_ARG_(tn, 3, __VA_ARGS__) gmock_a3, \
        GMOCK_ARG_(tn, 4, __VA_ARGS__) gmock_a4, \
        GMOCK_ARG_(tn, 5, __VA_ARGS__) gmock_a5, \
        GMOCK_ARG_(tn, 6, __VA_ARGS__) gmock_a6, \
        GMOCK_ARG_(tn, 7, __VA_ARGS__) gmock_a7) \
    { \
        if (gmock_win32::detail::lock) \
        { \
            return reinterpret_cast< decltype(&stub) >(*pp_old_fn())( \
                gmock_a1, gmock_a2, gmock_a3, gmock_a4, gmock_a5, gmock_a6, gmock_a7); \
        } \
        else \
        { \
            const gmock_win32::bypass_mocks blockMocks{ }; \
            return instance().func( \
                gmock_a1, gmock_a2, gmock_a3, gmock_a4, gmock_a5, gmock_a6, gmock_a7); \
        } \
    } \
};

#define MOCK_MODULE_FUNC7(m, ...) MOCK_MODULE_FUNC7_(, , , m, __VA_ARGS__)
#define MOCK_MODULE_FUNC7_CALLCONV(ct, m, ...) MOCK_MODULE_FUNC7_(, , ct, m, __VA_ARGS__)

#define MOCK_MODULE_FUNC7_STDCALL_CONV(m, ...) MOCK_MODULE_FUNC7_CALLCONV(__stdcall, m, __VA_ARGS__)
#define MOCK_MODULE_FUNC7_CDECL_CONV(m, ...) MOCK_MODULE_FUNC7_CALLCONV(__cdecl, m, __VA_ARGS__)

#define MOCK_MODULE_FUNC8_(tn, constness, ct, func, ...) \
struct mock_module_##func : \
    gmock_win32::detail::mock_module_base< mock_module_##func > \
{ \
    GMOCK_RESULT_(tn, __VA_ARGS__) ct func( \
        GMOCK_ARG_(tn, 1, __VA_ARGS__) gmock_a1, \
        GMOCK_ARG_(tn, 2, __VA_ARGS__) gmock_a2, \
        GMOCK_ARG_(tn, 3, __VA_ARGS__) gmock_a3, \
        GMOCK_ARG_(tn, 4, __VA_ARGS__) gmock_a4, \
        GMOCK_ARG_(tn, 5, __VA_ARGS__) gmock_a5, \
        GMOCK_ARG_(tn, 6, __VA_ARGS__) gmock_a6, \
        GMOCK_ARG_(tn, 7, __VA_ARGS__) gmock_a7, \
        GMOCK_ARG_(tn, 8, __VA_ARGS__) gmock_a8) constness \
    { \
        GMOCK_MOCKER_(8, constness, func).SetOwnerAndName(this, #func); \
        return GMOCK_MOCKER_(8, constness, func).Invoke( \
            gmock_a1, gmock_a2, gmock_a3, gmock_a4, gmock_a5, gmock_a6, gmock_a7, gmock_a8); \
    } \
    ::testing::MockSpec<__VA_ARGS__> gmock_##func( \
        GMOCK_MATCHER_(tn, 1, __VA_ARGS__) gmock_a1, \
        GMOCK_MATCHER_(tn, 2, __VA_ARGS__) gmock_a2, \
        GMOCK_MATCHER_(tn, 3, __VA_ARGS__) gmock_a3, \
        GMOCK_MATCHER_(tn, 4, __VA_ARGS__) gmock_a4, \
        GMOCK_MATCHER_(tn, 5, __VA_ARGS__) gmock_a5, \
        GMOCK_MATCHER_(tn, 6, __VA_ARGS__) gmock_a6, \
        GMOCK_MATCHER_(tn, 7, __VA_ARGS__) gmock_a7, \
        GMOCK_MATCHER_(tn, 8, __VA_ARGS__) gmock_a8) constness \
    { \
        GMOCK_MOCKER_(8, constness, func).RegisterOwner(this); \
        return GMOCK_MOCKER_(8, constness, func).With( \
            gmock_a1, gmock_a2, gmock_a3, gmock_a4, gmock_a5, gmock_a6, gmock_a7, gmock_a8); \
    } \
    mutable ::testing::FunctionMocker<__VA_ARGS__> \
        GMOCK_MOCKER_(8, constness, func); \
    static GMOCK_RESULT_(tn, __VA_ARGS__) ct stub( \
        GMOCK_ARG_(tn, 1, __VA_ARGS__) gmock_a1, \
        GMOCK_ARG_(tn, 2, __VA_ARGS__) gmock_a2, \
        GMOCK_ARG_(tn, 3, __VA_ARGS__) gmock_a3, \
        GMOCK_ARG_(tn, 4, __VA_ARGS__) gmock_a4, \
        GMOCK_ARG_(tn, 5, __VA_ARGS__) gmock_a5, \
        GMOCK_ARG_(tn, 6, __VA_ARGS__) gmock_a6, \
        GMOCK_ARG_(tn, 7, __VA_ARGS__) gmock_a7, \
        GMOCK_ARG_(tn, 8, __VA_ARGS__) gmock_a8) \
    { \
        if (gmock_win32::detail::lock) \
        { \
            return reinterpret_cast< decltype(&stub) >(*pp_old_fn())( \
                gmock_a1, gmock_a2, gmock_a3, gmock_a4, gmock_a5, gmock_a6, gmock_a7, gmock_a8); \
        } \
        else \
        { \
            const gmock_win32::bypass_mocks blockMocks{ }; \
            return instance().func( \
                gmock_a1, gmock_a2, gmock_a3, gmock_a4, gmock_a5, gmock_a6, gmock_a7, gmock_a8); \
        } \
    } \
};

#define MOCK_MODULE_FUNC8(m, ...) MOCK_MODULE_FUNC8_(, , , m, __VA_ARGS__)
#define MOCK_MODULE_FUNC8_CALLCONV(ct, m, ...) MOCK_MODULE_FUNC8_(, , ct, m, __VA_ARGS__)

#define MOCK_MODULE_FUNC8_STDCALL_CONV(m, ...) MOCK_MODULE_FUNC8_CALLCONV(__stdcall, m, __VA_ARGS__)
#define MOCK_MODULE_FUNC8_CDECL_CONV(m, ...) MOCK_MODULE_FUNC8_CALLCONV(__cdecl, m, __VA_ARGS__)

#define MOCK_MODULE_FUNC9_(tn, constness, ct, func, ...) \
struct mock_module_##func : \
    gmock_win32::detail::mock_module_base< mock_module_##func > \
{ \
    GMOCK_RESULT_(tn, __VA_ARGS__) ct func( \
        GMOCK_ARG_(tn, 1, __VA_ARGS__) gmock_a1, \
        GMOCK_ARG_(tn, 2, __VA_ARGS__) gmock_a2, \
        GMOCK_ARG_(tn, 3, __VA_ARGS__) gmock_a3, \
        GMOCK_ARG_(tn, 4, __VA_ARGS__) gmock_a4, \
        GMOCK_ARG_(tn, 5, __VA_ARGS__) gmock_a5, \
        GMOCK_ARG_(tn, 6, __VA_ARGS__) gmock_a6, \
        GMOCK_ARG_(tn, 7, __VA_ARGS__) gmock_a7, \
        GMOCK_ARG_(tn, 8, __VA_ARGS__) gmock_a8, \
        GMOCK_ARG_(tn, 9, __VA_ARGS__) gmock_a9) constness \
    { \
        GMOCK_MOCKER_(9, constness, func).SetOwnerAndName(this, #func); \
        return GMOCK_MOCKER_(9, constness, func).Invoke( \
            gmock_a1, gmock_a2, gmock_a3, gmock_a4, gmock_a5, gmock_a6, gmock_a7, gmock_a8, gmock_a9); \
    } \
    ::testing::MockSpec<__VA_ARGS__> gmock_##func( \
        GMOCK_MATCHER_(tn, 1, __VA_ARGS__) gmock_a1, \
        GMOCK_MATCHER_(tn, 2, __VA_ARGS__) gmock_a2, \
        GMOCK_MATCHER_(tn, 3, __VA_ARGS__) gmock_a3, \
        GMOCK_MATCHER_(tn, 4, __VA_ARGS__) gmock_a4, \
        GMOCK_MATCHER_(tn, 5, __VA_ARGS__) gmock_a5, \
        GMOCK_MATCHER_(tn, 6, __VA_ARGS__) gmock_a6, \
        GMOCK_MATCHER_(tn, 7, __VA_ARGS__) gmock_a7, \
        GMOCK_MATCHER_(tn, 8, __VA_ARGS__) gmock_a8, \
        GMOCK_MATCHER_(tn, 9, __VA_ARGS__) gmock_a9) constness \
    { \
        GMOCK_MOCKER_(9, constness, func).RegisterOwner(this); \
        return GMOCK_MOCKER_(9, constness, func).With( \
            gmock_a1, gmock_a2, gmock_a3, gmock_a4, gmock_a5, gmock_a6, gmock_a7, gmock_a8, gmock_a9); \
    } \
    mutable ::testing::FunctionMocker<__VA_ARGS__> \
        GMOCK_MOCKER_(9, constness, func); \
    static GMOCK_RESULT_(tn, __VA_ARGS__) ct stub( \
        GMOCK_ARG_(tn, 1, __VA_ARGS__) gmock_a1, \
        GMOCK_ARG_(tn, 2, __VA_ARGS__) gmock_a2, \
        GMOCK_ARG_(tn, 3, __VA_ARGS__) gmock_a3, \
        GMOCK_ARG_(tn, 4, __VA_ARGS__) gmock_a4, \
        GMOCK_ARG_(tn, 5, __VA_ARGS__) gmock_a5, \
        GMOCK_ARG_(tn, 6, __VA_ARGS__) gmock_a6, \
        GMOCK_ARG_(tn, 7, __VA_ARGS__) gmock_a7, \
        GMOCK_ARG_(tn, 8, __VA_ARGS__) gmock_a8, \
        GMOCK_ARG_(tn, 9, __VA_ARGS__) gmock_a9) \
    { \
        if (gmock_win32::detail::lock) \
        { \
            return reinterpret_cast< decltype(&stub) >(*pp_old_fn())( \
                gmock_a1, gmock_a2, gmock_a3, gmock_a4, gmock_a5, gmock_a6, gmock_a7, gmock_a8, gmock_a9); \
        } \
        else \
        { \
            const gmock_win32::bypass_mocks blockMocks{ }; \
            return instance().func( \
                gmock_a1, gmock_a2, gmock_a3, gmock_a4, gmock_a5, gmock_a6, gmock_a7, gmock_a8, gmock_a9); \
        } \
    } \
};

#define MOCK_MODULE_FUNC9(m, ...) MOCK_MODULE_FUNC9_(, , , m, __VA_ARGS__)
#define MOCK_MODULE_FUNC9_CALLCONV(ct, m, ...) MOCK_MODULE_FUNC9_(, , ct, m, __VA_ARGS__)

#define MOCK_MODULE_FUNC9_STDCALL_CONV(m, ...) MOCK_MODULE_FUNC9_CALLCONV(__stdcall, m, __VA_ARGS__)
#define MOCK_MODULE_FUNC9_CDECL_CONV(m, ...) MOCK_MODULE_FUNC9_CALLCONV(__cdecl, m, __VA_ARGS__)

#define MOCK_MODULE_FUNC10_(tn, constness, ct, func, ...) \
struct mock_module_##func : \
    gmock_win32::detail::mock_module_base< mock_module_##func > \
{ \
    GMOCK_RESULT_(tn, __VA_ARGS__) ct func( \
        GMOCK_ARG_(tn, 1, __VA_ARGS__) gmock_a1, \
        GMOCK_ARG_(tn, 2, __VA_ARGS__) gmock_a2, \
        GMOCK_ARG_(tn, 3, __VA_ARGS__) gmock_a3, \
        GMOCK_ARG_(tn, 4, __VA_ARGS__) gmock_a4, \
        GMOCK_ARG_(tn, 5, __VA_ARGS__) gmock_a5, \
        GMOCK_ARG_(tn, 6, __VA_ARGS__) gmock_a6, \
        GMOCK_ARG_(tn, 7, __VA_ARGS__) gmock_a7, \
        GMOCK_ARG_(tn, 8, __VA_ARGS__) gmock_a8, \
        GMOCK_ARG_(tn, 9, __VA_ARGS__) gmock_a9, \
        GMOCK_ARG_(tn, 10, __VA_ARGS__) gmock_a10) constness \
    { \
        GMOCK_MOCKER_(10, constness, func).SetOwnerAndName(this, #func); \
        return GMOCK_MOCKER_(10, constness, func).Invoke( \
            gmock_a1, gmock_a2, gmock_a3, gmock_a4, gmock_a5, gmock_a6, gmock_a7, gmock_a8, gmock_a9, gmock_a10); \
    } \
    ::testing::MockSpec<__VA_ARGS__> gmock_##func( \
        GMOCK_MATCHER_(tn, 1, __VA_ARGS__) gmock_a1, \
        GMOCK_MATCHER_(tn, 2, __VA_ARGS__) gmock_a2, \
        GMOCK_MATCHER_(tn, 3, __VA_ARGS__) gmock_a3, \
        GMOCK_MATCHER_(tn, 4, __VA_ARGS__) gmock_a4, \
        GMOCK_MATCHER_(tn, 5, __VA_ARGS__) gmock_a5, \
        GMOCK_MATCHER_(tn, 6, __VA_ARGS__) gmock_a6, \
        GMOCK_MATCHER_(tn, 7, __VA_ARGS__) gmock_a7, \
        GMOCK_MATCHER_(tn, 8, __VA_ARGS__) gmock_a8, \
        GMOCK_MATCHER_(tn, 9, __VA_ARGS__) gmock_a9, \
        GMOCK_MATCHER_(tn, 10, __VA_ARGS__) gmock_a10) constness \
    { \
        GMOCK_MOCKER_(10, constness, func).RegisterOwner(this); \
        return GMOCK_MOCKER_(10, constness, func).With( \
            gmock_a1, gmock_a2, gmock_a3, gmock_a4, gmock_a5, gmock_a6, gmock_a7, gmock_a8, gmock_a9, gmock_a10); \
    } \
    mutable ::testing::FunctionMocker<__VA_ARGS__> \
        GMOCK_MOCKER_(10, constness, func); \
    static GMOCK_RESULT_(tn, __VA_ARGS__) ct stub( \
        GMOCK_ARG_(tn, 1, __VA_ARGS__) gmock_a1, \
        GMOCK_ARG_(tn, 2, __VA_ARGS__) gmock_a2, \
        GMOCK_ARG_(tn, 3, __VA_ARGS__) gmock_a3, \
        GMOCK_ARG_(tn, 4, __VA_ARGS__) gmock_a4, \
        GMOCK_ARG_(tn, 5, __VA_ARGS__) gmock_a5, \
        GMOCK_ARG_(tn, 6, __VA_ARGS__) gmock_a6, \
        GMOCK_ARG_(tn, 7, __VA_ARGS__) gmock_a7, \
        GMOCK_ARG_(tn, 8, __VA_ARGS__) gmock_a8, \
        GMOCK_ARG_(tn, 9, __VA_ARGS__) gmock_a9, \
        GMOCK_ARG_(tn, 10, __VA_ARGS__) gmock_a10) \
    { \
        if (gmock_win32::detail::lock) \
        { \
            return reinterpret_cast< decltype(&stub) >(*pp_old_fn())( \
                gmock_a1, gmock_a2, gmock_a3, gmock_a4, gmock_a5, gmock_a6, gmock_a7, gmock_a8, gmock_a9, gmock_a10); \
        } \
        else \
        { \
            const gmock_win32::bypass_mocks blockMocks{ }; \
            return instance().func( \
                gmock_a1, gmock_a2, gmock_a3, gmock_a4, gmock_a5, gmock_a6, gmock_a7, gmock_a8, gmock_a9, gmock_a10); \
        } \
    } \
};

#define MOCK_MODULE_FUNC10(m, ...) MOCK_MODULE_FUNC10_(, , , m, __VA_ARGS__)
#define MOCK_MODULE_FUNC10_CALLCONV(ct, m, ...) MOCK_MODULE_FUNC10_(, , ct, m, __VA_ARGS__)

#define MOCK_MODULE_FUNC10_STDCALL_CONV(m, ...) MOCK_MODULE_FUNC10_CALLCONV(__stdcall, m, __VA_ARGS__)
#define MOCK_MODULE_FUNC10_CDECL_CONV(m, ...) MOCK_MODULE_FUNC10_CALLCONV(__cdecl, m, __VA_ARGS__)

#define MOCK_MODULE_FUNC11_(tn, constness, ct, func, ...) \
struct mock_module_##func : \
    gmock_win32::detail::mock_module_base< mock_module_##func > \
{ \
    GMOCK_RESULT_(tn, __VA_ARGS__) ct func( \
        GMOCK_ARG_(tn, 1, __VA_ARGS__) gmock_a1, \
        GMOCK_ARG_(tn, 2, __VA_ARGS__) gmock_a2, \
        GMOCK_ARG_(tn, 3, __VA_ARGS__) gmock_a3, \
        GMOCK_ARG_(tn, 4, __VA_ARGS__) gmock_a4, \
        GMOCK_ARG_(tn, 5, __VA_ARGS__) gmock_a5, \
        GMOCK_ARG_(tn, 6, __VA_ARGS__) gmock_a6, \
        GMOCK_ARG_(tn, 7, __VA_ARGS__) gmock_a7, \
        GMOCK_ARG_(tn, 8, __VA_ARGS__) gmock_a8, \
        GMOCK_ARG_(tn, 9, __VA_ARGS__) gmock_a9, \
        GMOCK_ARG_(tn, 10, __VA_ARGS__) gmock_a10, \
        GMOCK_ARG_(tn, 11, __VA_ARGS__) gmock_a11) constness \
    { \
        GMOCK_MOCKER_(11, constness, func).SetOwnerAndName(this, #func); \
        return GMOCK_MOCKER_(11, constness, func).Invoke( \
            gmock_a1, gmock_a2, gmock_a3, gmock_a4, gmock_a5, gmock_a6, gmock_a7, gmock_a8, gmock_a9, gmock_a10, gmock_a11); \
    } \
    ::testing::MockSpec<__VA_ARGS__> gmock_##func( \
        GMOCK_MATCHER_(tn, 1, __VA_ARGS__) gmock_a1, \
        GMOCK_MATCHER_(tn, 2, __VA_ARGS__) gmock_a2, \
        GMOCK_MATCHER_(tn, 3, __VA_ARGS__) gmock_a3, \
        GMOCK_MATCHER_(tn, 4, __VA_ARGS__) gmock_a4, \
        GMOCK_MATCHER_(tn, 5, __VA_ARGS__) gmock_a5, \
        GMOCK_MATCHER_(tn, 6, __VA_ARGS__) gmock_a6, \
        GMOCK_MATCHER_(tn, 7, __VA_ARGS__) gmock_a7, \
        GMOCK_MATCHER_(tn, 8, __VA_ARGS__) gmock_a8, \
        GMOCK_MATCHER_(tn, 9, __VA_ARGS__) gmock_a9, \
        GMOCK_MATCHER_(tn, 10, __VA_ARGS__) gmock_a10, \
        GMOCK_MATCHER_(tn, 11, __VA_ARGS__) gmock_a11) constness \
    { \
        GMOCK_MOCKER_(11, constness, func).RegisterOwner(this); \
        return GMOCK_MOCKER_(11, constness, func).With( \
            gmock_a1, gmock_a2, gmock_a3, gmock_a4, gmock_a5, gmock_a6, gmock_a7, gmock_a8, gmock_a9, gmock_a10, gmock_a11); \
    } \
    mutable ::testing::FunctionMocker<__VA_ARGS__> \
        GMOCK_MOCKER_(11, constness, func); \
    static GMOCK_RESULT_(tn, __VA_ARGS__) ct stub( \
        GMOCK_ARG_(tn, 1, __VA_ARGS__) gmock_a1, \
        GMOCK_ARG_(tn, 2, __VA_ARGS__) gmock_a2, \
        GMOCK_ARG_(tn, 3, __VA_ARGS__) gmock_a3, \
        GMOCK_ARG_(tn, 4, __VA_ARGS__) gmock_a4, \
        GMOCK_ARG_(tn, 5, __VA_ARGS__) gmock_a5, \
        GMOCK_ARG_(tn, 6, __VA_ARGS__) gmock_a6, \
        GMOCK_ARG_(tn, 7, __VA_ARGS__) gmock_a7, \
        GMOCK_ARG_(tn, 8, __VA_ARGS__) gmock_a8, \
        GMOCK_ARG_(tn, 9, __VA_ARGS__) gmock_a9, \
        GMOCK_ARG_(tn, 10, __VA_ARGS__) gmock_a10, \
        GMOCK_ARG_(tn, 11, __VA_ARGS__) gmock_a11) \
    { \
        if (gmock_win32::detail::lock) \
        { \
            return reinterpret_cast< decltype(&stub) >(*pp_old_fn())( \
                gmock_a1, gmock_a2, gmock_a3, gmock_a4, gmock_a5, gmock_a6, gmock_a7, gmock_a8, gmock_a9, gmock_a10, gmock_a11); \
        } \
        else \
        { \
            const gmock_win32::bypass_mocks blockMocks{ }; \
            return instance().func( \
                gmock_a1, gmock_a2, gmock_a3, gmock_a4, gmock_a5, gmock_a6, gmock_a7, gmock_a8, gmock_a9, gmock_a10, gmock_a11); \
        } \
    } \
};

#define MOCK_MODULE_FUNC11(m, ...) MOCK_MODULE_FUNC11_(, , , m, __VA_ARGS__)
#define MOCK_MODULE_FUNC11_CALLCONV(ct, m, ...) MOCK_MODULE_FUNC11_(, , ct, m, __VA_ARGS__)

#define MOCK_MODULE_FUNC11_STDCALL_CONV(m, ...) MOCK_MODULE_FUNC11_CALLCONV(__stdcall, m, __VA_ARGS__)
#define MOCK_MODULE_FUNC11_CDECL_CONV(m, ...) MOCK_MODULE_FUNC11_CALLCONV(__cdecl, m, __VA_ARGS__)

#define MOCK_MODULE_FUNC12_(tn, constness, ct, func, ...) \
struct mock_module_##func : \
    gmock_win32::detail::mock_module_base< mock_module_##func > \
{ \
    GMOCK_RESULT_(tn, __VA_ARGS__) ct func( \
        GMOCK_ARG_(tn, 1, __VA_ARGS__) gmock_a1, \
        GMOCK_ARG_(tn, 2, __VA_ARGS__) gmock_a2, \
        GMOCK_ARG_(tn, 3, __VA_ARGS__) gmock_a3, \
        GMOCK_ARG_(tn, 4, __VA_ARGS__) gmock_a4, \
        GMOCK_ARG_(tn, 5, __VA_ARGS__) gmock_a5, \
        GMOCK_ARG_(tn, 6, __VA_ARGS__) gmock_a6, \
        GMOCK_ARG_(tn, 7, __VA_ARGS__) gmock_a7, \
        GMOCK_ARG_(tn, 8, __VA_ARGS__) gmock_a8, \
        GMOCK_ARG_(tn, 9, __VA_ARGS__) gmock_a9, \
        GMOCK_ARG_(tn, 10, __VA_ARGS__) gmock_a10, \
        GMOCK_ARG_(tn, 11, __VA_ARGS__) gmock_a11, \
        GMOCK_ARG_(tn, 12, __VA_ARGS__) gmock_a12) constness \
    { \
        GMOCK_MOCKER_(12, constness, func).SetOwnerAndName(this, #func); \
        return GMOCK_MOCKER_(12, constness, func).Invoke( \
            gmock_a1, gmock_a2, gmock_a3, gmock_a4, gmock_a5, gmock_a6, gmock_a7, gmock_a8, gmock_a9, gmock_a10, gmock_a11, gmock_a12); \
    } \
    ::testing::MockSpec<__VA_ARGS__> gmock_##func( \
        GMOCK_MATCHER_(tn, 1, __VA_ARGS__) gmock_a1, \
        GMOCK_MATCHER_(tn, 2, __VA_ARGS__) gmock_a2, \
        GMOCK_MATCHER_(tn, 3, __VA_ARGS__) gmock_a3, \
        GMOCK_MATCHER_(tn, 4, __VA_ARGS__) gmock_a4, \
        GMOCK_MATCHER_(tn, 5, __VA_ARGS__) gmock_a5, \
        GMOCK_MATCHER_(tn, 6, __VA_ARGS__) gmock_a6, \
        GMOCK_MATCHER_(tn, 7, __VA_ARGS__) gmock_a7, \
        GMOCK_MATCHER_(tn, 8, __VA_ARGS__) gmock_a8, \
        GMOCK_MATCHER_(tn, 9, __VA_ARGS__) gmock_a9, \
        GMOCK_MATCHER_(tn, 10, __VA_ARGS__) gmock_a10, \
        GMOCK_MATCHER_(tn, 11, __VA_ARGS__) gmock_a11, \
        GMOCK_MATCHER_(tn, 12, __VA_ARGS__) gmock_a12) constness \
    { \
        GMOCK_MOCKER_(11, constness, func).RegisterOwner(this); \
        return GMOCK_MOCKER_(11, constness, func).With( \
            gmock_a1, gmock_a2, gmock_a3, gmock_a4, gmock_a5, gmock_a6, gmock_a7, gmock_a8, gmock_a9, gmock_a10, gmock_a11, gmock_a12); \
    } \
    mutable ::testing::FunctionMocker<__VA_ARGS__> \
        GMOCK_MOCKER_(12, constness, func); \
    static GMOCK_RESULT_(tn, __VA_ARGS__) ct stub( \
        GMOCK_ARG_(tn, 1, __VA_ARGS__) gmock_a1, \
        GMOCK_ARG_(tn, 2, __VA_ARGS__) gmock_a2, \
        GMOCK_ARG_(tn, 3, __VA_ARGS__) gmock_a3, \
        GMOCK_ARG_(tn, 4, __VA_ARGS__) gmock_a4, \
        GMOCK_ARG_(tn, 5, __VA_ARGS__) gmock_a5, \
        GMOCK_ARG_(tn, 6, __VA_ARGS__) gmock_a6, \
        GMOCK_ARG_(tn, 7, __VA_ARGS__) gmock_a7, \
        GMOCK_ARG_(tn, 8, __VA_ARGS__) gmock_a8, \
        GMOCK_ARG_(tn, 9, __VA_ARGS__) gmock_a9, \
        GMOCK_ARG_(tn, 10, __VA_ARGS__) gmock_a10, \
        GMOCK_ARG_(tn, 11, __VA_ARGS__) gmock_a11, \
        GMOCK_ARG_(tn, 12, __VA_ARGS__) gmock_a12) \
    { \
        if (gmock_win32::detail::lock) \
        { \
            return reinterpret_cast< decltype(&stub) >(*pp_old_fn())( \
                gmock_a1, gmock_a2, gmock_a3, gmock_a4, gmock_a5, gmock_a6, gmock_a7, gmock_a8, gmock_a9, gmock_a10, gmock_a11, gmock_a12); \
        } \
        else \
        { \
            const gmock_win32::bypass_mocks blockMocks{ }; \
            return instance().func( \
                gmock_a1, gmock_a2, gmock_a3, gmock_a4, gmock_a5, gmock_a6, gmock_a7, gmock_a8, gmock_a9, gmock_a10, gmock_a11, gmock_a12); \
        } \
    } \
};

#define MOCK_MODULE_FUNC12(m, ...) MOCK_MODULE_FUNC12_(, , , m, __VA_ARGS__)
#define MOCK_MODULE_FUNC12_CALLCONV(ct, m, ...) MOCK_MODULE_FUNC12_(, , ct, m, __VA_ARGS__)

#define MOCK_MODULE_FUNC12_STDCALL_CONV(m, ...) MOCK_MODULE_FUNC12_CALLCONV(__stdcall, m, __VA_ARGS__)
#define MOCK_MODULE_FUNC12_CDECL_CONV(m, ...) MOCK_MODULE_FUNC12_CALLCONV(__cdecl, m, __VA_ARGS__)

#define MOCK_MODULE_FUNC13_(tn, constness, ct, func, ...) \
struct mock_module_##func : \
    gmock_win32::detail::mock_module_base< mock_module_##func > \
{ \
    GMOCK_RESULT_(tn, __VA_ARGS__) ct func( \
        GMOCK_ARG_(tn, 1, __VA_ARGS__) gmock_a1, \
        GMOCK_ARG_(tn, 2, __VA_ARGS__) gmock_a2, \
        GMOCK_ARG_(tn, 3, __VA_ARGS__) gmock_a3, \
        GMOCK_ARG_(tn, 4, __VA_ARGS__) gmock_a4, \
        GMOCK_ARG_(tn, 5, __VA_ARGS__) gmock_a5, \
        GMOCK_ARG_(tn, 6, __VA_ARGS__) gmock_a6, \
        GMOCK_ARG_(tn, 7, __VA_ARGS__) gmock_a7, \
        GMOCK_ARG_(tn, 8, __VA_ARGS__) gmock_a8, \
        GMOCK_ARG_(tn, 9, __VA_ARGS__) gmock_a9, \
        GMOCK_ARG_(tn, 10, __VA_ARGS__) gmock_a10, \
        GMOCK_ARG_(tn, 11, __VA_ARGS__) gmock_a11, \
        GMOCK_ARG_(tn, 12, __VA_ARGS__) gmock_a12, \
        GMOCK_ARG_(tn, 13, __VA_ARGS__) gmock_a13) constness \
    { \
        GMOCK_MOCKER_(13, constness, func).SetOwnerAndName(this, #func); \
        return GMOCK_MOCKER_(13, constness, func).Invoke( \
            gmock_a1, gmock_a2, gmock_a3, gmock_a4, gmock_a5, gmock_a6, gmock_a7, gmock_a8, gmock_a9, gmock_a10, gmock_a11, gmock_a12, gmock_a13); \
    } \
    ::testing::MockSpec<__VA_ARGS__> gmock_##func( \
        GMOCK_MATCHER_(tn, 1, __VA_ARGS__) gmock_a1, \
        GMOCK_MATCHER_(tn, 2, __VA_ARGS__) gmock_a2, \
        GMOCK_MATCHER_(tn, 3, __VA_ARGS__) gmock_a3, \
        GMOCK_MATCHER_(tn, 4, __VA_ARGS__) gmock_a4, \
        GMOCK_MATCHER_(tn, 5, __VA_ARGS__) gmock_a5, \
        GMOCK_MATCHER_(tn, 6, __VA_ARGS__) gmock_a6, \
        GMOCK_MATCHER_(tn, 7, __VA_ARGS__) gmock_a7, \
        GMOCK_MATCHER_(tn, 8, __VA_ARGS__) gmock_a8, \
        GMOCK_MATCHER_(tn, 9, __VA_ARGS__) gmock_a9, \
        GMOCK_MATCHER_(tn, 10, __VA_ARGS__) gmock_a10, \
        GMOCK_MATCHER_(tn, 11, __VA_ARGS__) gmock_a11, \
        GMOCK_MATCHER_(tn, 12, __VA_ARGS__) gmock_a12, \
        GMOCK_MATCHER_(tn, 13, __VA_ARGS__) gmock_a13) constness \
    { \
        GMOCK_MOCKER_(13, constness, func).RegisterOwner(this); \
        return GMOCK_MOCKER_(13, constness, func).With( \
            gmock_a1, gmock_a2, gmock_a3, gmock_a4, gmock_a5, gmock_a6, gmock_a7, gmock_a8, gmock_a9, gmock_a10, gmock_a11, gmock_a12, gmock_a13); \
    } \
    mutable ::testing::FunctionMocker<__VA_ARGS__> \
        GMOCK_MOCKER_(13, constness, func); \
    static GMOCK_RESULT_(tn, __VA_ARGS__) ct stub( \
        GMOCK_ARG_(tn, 1, __VA_ARGS__) gmock_a1, \
        GMOCK_ARG_(tn, 2, __VA_ARGS__) gmock_a2, \
        GMOCK_ARG_(tn, 3, __VA_ARGS__) gmock_a3, \
        GMOCK_ARG_(tn, 4, __VA_ARGS__) gmock_a4, \
        GMOCK_ARG_(tn, 5, __VA_ARGS__) gmock_a5, \
        GMOCK_ARG_(tn, 6, __VA_ARGS__) gmock_a6, \
        GMOCK_ARG_(tn, 7, __VA_ARGS__) gmock_a7, \
        GMOCK_ARG_(tn, 8, __VA_ARGS__) gmock_a8, \
        GMOCK_ARG_(tn, 9, __VA_ARGS__) gmock_a9, \
        GMOCK_ARG_(tn, 10, __VA_ARGS__) gmock_a10, \
        GMOCK_ARG_(tn, 11, __VA_ARGS__) gmock_a11, \
        GMOCK_ARG_(tn, 12, __VA_ARGS__) gmock_a12, \
        GMOCK_ARG_(tn, 13, __VA_ARGS__) gmock_a13) \
    { \
        if (gmock_win32::detail::lock) \
        { \
            return reinterpret_cast< decltype(&stub) >(*pp_old_fn())( \
                gmock_a1, gmock_a2, gmock_a3, gmock_a4, gmock_a5, gmock_a6, gmock_a7, gmock_a8, gmock_a9, gmock_a10, gmock_a11, gmock_a12, gmock_a13); \
        } \
        else \
        { \
            const gmock_win32::bypass_mocks blockMocks{ }; \
            return instance().func( \
                gmock_a1, gmock_a2, gmock_a3, gmock_a4, gmock_a5, gmock_a6, gmock_a7, gmock_a8, gmock_a9, gmock_a10, gmock_a11, gmock_a12, gmock_a13); \
        } \
    } \
};

#define MOCK_MODULE_FUNC13(m, ...) MOCK_MODULE_FUNC13_(, , , m, __VA_ARGS__)
#define MOCK_MODULE_FUNC13_CALLCONV(ct, m, ...) MOCK_MODULE_FUNC13_(, , ct, m, __VA_ARGS__)

#define MOCK_MODULE_FUNC13_STDCALL_CONV(m, ...) MOCK_MODULE_FUNC13_CALLCONV(__stdcall, m, __VA_ARGS__)
#define MOCK_MODULE_FUNC13_CDECL_CONV(m, ...) MOCK_MODULE_FUNC13_CALLCONV(__cdecl, m, __VA_ARGS__)

#define MOCK_MODULE_EXPAND(x) x
#define MOCK_MODULE_UNITE(x, y) x y
#define MOCK_MODULE_CONCAT(x, y) x##y

#define MOCK_MODULE_PREFIX(...) 0, ##__VA_ARGS__
#define MOCK_MODULE_LASTOF15(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, ...)  o
#define MOCK_MODULE_SUB_NBARG(...) MOCK_MODULE_EXPAND(MOCK_MODULE_LASTOF15(__VA_ARGS__, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0))
#define MOCK_MODULE_NBARG(...) MOCK_MODULE_SUB_NBARG(MOCK_MODULE_PREFIX(__VA_ARGS__))

// Expand before concatenate
#define MOCK_MODULE_MAKENAME_(count_args, conv) MOCK_MODULE_FUNC##count_args##conv
#define MOCK_MODULE_OVERLOAD(count_args, conv, signat) MOCK_MODULE_MAKENAME_( count_args, conv )signat
#define MOCK_MODULE_FUNC_OVERLOAD(name, ...) MOCK_MODULE_UNITE(MOCK_MODULE_OVERLOAD(name, MOCK_MODULE_NBARG(__VA_ARGS__)), (__VA_ARGS__))

#define MOCK_MODULE_AVOID_OPT(m) \
    __pragma(optimize("", on)) \
    static void patch_module_func_##m() { \
        ::patch_module_func_(mock_module_##m::pp_old_fn(), &::m, &mock_module_##m::stub); \
    } \
    __pragma(optimize("", off))

#define MOCK_MODULE_FUNC(r, m, ...) \
    MOCK_MODULE_OVERLOAD( MOCK_MODULE_NBARG(__VA_ARGS__), , (m, r(__VA_ARGS__)) ) \
    MOCK_MODULE_AVOID_OPT( m )

#define MOCK_CDECL_FUNC(r, m, ...) \
    MOCK_MODULE_OVERLOAD( MOCK_MODULE_NBARG(__VA_ARGS__), _CDECL_CONV, (m, r(__VA_ARGS__)) ) \
    MOCK_MODULE_AVOID_OPT( m )

#define MOCK_STDCALL_FUNC(r, m, ...) \
    MOCK_MODULE_OVERLOAD( MOCK_MODULE_NBARG(__VA_ARGS__), _STDCALL_CONV, (m, r(__VA_ARGS__)) ) \
    MOCK_MODULE_AVOID_OPT( m )

// Hidden from optimizer
template< typename TFunc, typename TStub >
void patch_module_func_(void** old_fn, TFunc func, TStub stub) {
    if (!(*old_fn))
        gmock_win32::detail::patch_module_func(
            func, reinterpret_cast< void* >(stub), old_fn);
}

#define EXPECT_MODULE_FUNC_CALL(func, ...) \
    patch_module_func_##func( ); \
    ++gmock_win32::detail::lock; \
    static_cast< decltype(EXPECT_CALL(mock_module_##func::instance(), \
        func(__VA_ARGS__)))& >(gmock_win32::detail::make_proxy( \
            EXPECT_CALL(mock_module_##func::instance(), func(__VA_ARGS__))))

#define ON_MODULE_FUNC_CALL(func, ...) \
    patch_module_func_##func( ); \
    ++gmock_win32::detail::lock; \
    static_cast< decltype(ON_CALL(mock_module_##func::instance(), \
        func(__VA_ARGS__)))& >(gmock_win32::detail::make_proxy( \
            ON_CALL(mock_module_##func::instance(), func(__VA_ARGS__))))

#define REAL_MODULE_FUNC(func) \
    reinterpret_cast< decltype(&func) >(*mock_module_##func::pp_old_fn())

#define INVOKE_REAL_MODULE_FUNC(func, ...) \
    REAL_MODULE_FUNC(func)(__VA_ARGS__)

#define VERIFY_AND_CLEAR_MODULE_FUNC_EXPECTATIONS(func) \
    ::testing::Mock::VerifyAndClearExpectations(&mock_module_##func::instance());

#define RESTORE_MODULE_FUNC(func) \
    gmock_win32::detail::restore_module_func( \
        *mock_module_##func::pp_old_fn(), mock_module_##func::stub, mock_module_##func::pp_old_fn())
