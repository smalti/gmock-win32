#pragma once

#ifndef GMOCK_RESULT_
#define GMOCK_RESULT_(tn, ...) \
    tn ::testing::internal::Function<__VA_ARGS__>::Result
#endif

#ifndef GMOCK_ARG_
#define GMOCK_ARG_(tn, N, ...) \
    tn ::testing::internal::Function<__VA_ARGS__>::Argument##N
#endif

#ifndef GMOCK_MATCHER_
#define GMOCK_MATCHER_(tn, N, ...) \
    const ::testing::Matcher<GMOCK_ARG_(tn, N, __VA_ARGS__)>&
#endif

#ifndef GMOCK_MOCKER_
#define GMOCK_MOCKER_(arity, constness, func) \
    GTEST_CONCAT_TOKEN_(gmock##constness##arity##_##func##_, __LINE__)
#endif

#define MOCK_MODULE_FUNC0_(tn, constness, ct, func, ...) \
struct mock_module_##func \
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
    static mock_module_##func& instance() \
    { \
        static ::testing::NiceMock< mock_module_##func > obj; \
        return obj; \
    } \
    static GMOCK_RESULT_(tn, __VA_ARGS__) ct stub() \
    { \
        return mock_module_##func::instance().func(); \
    } \
    static void* oldFn_; \
}; void* mock_module_##func::oldFn_ = nullptr;

#define MOCK_MODULE_FUNC0(m, ...) MOCK_MODULE_FUNC0_(, , , m, __VA_ARGS__)
#define MOCK_MODULE_FUNC0_CALLCONV(ct, m, ...) MOCK_MODULE_FUNC0_(, , ct, m, __VA_ARGS__)
#define MOCK_MODULE_FUNC0_STDCALL(m, ...) MOCK_MODULE_FUNC0_CALLCONV(__stdcall, m, __VA_ARGS__)

#define MOCK_MODULE_FUNC1_(tn, constness, ct, func, ...) \
struct mock_module_##func \
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
    static mock_module_##func& instance() \
    { \
        static ::testing::NiceMock< mock_module_##func > obj; \
        return obj; \
    } \
    static GMOCK_RESULT_(tn, __VA_ARGS__) ct stub( \
        GMOCK_ARG_(tn, 1, __VA_ARGS__) gmock_a1) \
    { \
        return mock_module_##func::instance().func( \
            gmock_a1); \
    } \
    static void* oldFn_; \
}; void* mock_module_##func::oldFn_ = nullptr;

#define MOCK_MODULE_FUNC1(m, ...) MOCK_MODULE_FUNC1_(, , , m, __VA_ARGS__)
#define MOCK_MODULE_FUNC1_CALLCONV(ct, m, ...) MOCK_MODULE_FUNC1_(, , ct, m, __VA_ARGS__)
#define MOCK_MODULE_FUNC1_STDCALL(m, ...) MOCK_MODULE_FUNC1_CALLCONV(__stdcall, m, __VA_ARGS__)

#define MOCK_MODULE_FUNC2_(tn, constness, ct, func, ...) \
struct mock_module_##func \
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
    static mock_module_##func& instance() \
    { \
        static ::testing::NiceMock< mock_module_##func > obj; \
        return obj; \
    } \
    static GMOCK_RESULT_(tn, __VA_ARGS__) ct stub( \
        GMOCK_ARG_(tn, 1, __VA_ARGS__) gmock_a1, \
        GMOCK_ARG_(tn, 2, __VA_ARGS__) gmock_a2) \
    { \
        return mock_module_##func::instance().func( \
            gmock_a1, gmock_a2); \
    } \
    static void* oldFn_; \
}; void* mock_module_##func::oldFn_ = nullptr;

#define MOCK_MODULE_FUNC2(m, ...) MOCK_MODULE_FUNC2_(, , , m, __VA_ARGS__)
#define MOCK_MODULE_FUNC2_CALLCONV(ct, m, ...) MOCK_MODULE_FUNC2_(, , ct, m, __VA_ARGS__)
#define MOCK_MODULE_FUNC2_STDCALL(m, ...) MOCK_MODULE_FUNC2_CALLCONV(__stdcall, m, __VA_ARGS__)

#define MOCK_MODULE_FUNC3_(tn, constness, ct, func, ...) \
struct mock_module_##func \
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
    static mock_module_##func& instance() \
    { \
        static ::testing::NiceMock< mock_module_##func > obj; \
        return obj; \
    } \
    static GMOCK_RESULT_(tn, __VA_ARGS__) ct stub( \
        GMOCK_ARG_(tn, 1, __VA_ARGS__) gmock_a1, \
        GMOCK_ARG_(tn, 2, __VA_ARGS__) gmock_a2, \
        GMOCK_ARG_(tn, 3, __VA_ARGS__) gmock_a3) \
    { \
        return mock_module_##func::instance().func( \
            gmock_a1, gmock_a2, gmock_a3); \
    } \
    static void* oldFn_; \
}; void* mock_module_##func::oldFn_ = nullptr;

#define MOCK_MODULE_FUNC3(m, ...) MOCK_MODULE_FUNC3_(, , , m, __VA_ARGS__)
#define MOCK_MODULE_FUNC3_CALLCONV(ct, m, ...) MOCK_MODULE_FUNC3_(, , ct, m, __VA_ARGS__)
#define MOCK_MODULE_FUNC3_STDCALL(m, ...) MOCK_MODULE_FUNC3_CALLCONV(__stdcall, m, __VA_ARGS__)

#define MOCK_MODULE_FUNC4_(tn, constness, ct, func, ...) \
struct mock_module_##func \
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
    static mock_module_##func& instance() \
    { \
        static ::testing::NiceMock< mock_module_##func > obj; \
        return obj; \
    } \
    static GMOCK_RESULT_(tn, __VA_ARGS__) ct stub( \
        GMOCK_ARG_(tn, 1, __VA_ARGS__) gmock_a1, \
        GMOCK_ARG_(tn, 2, __VA_ARGS__) gmock_a2, \
        GMOCK_ARG_(tn, 3, __VA_ARGS__) gmock_a3, \
        GMOCK_ARG_(tn, 4, __VA_ARGS__) gmock_a4) \
    { \
        return mock_module_##func::instance().func( \
            gmock_a1, gmock_a2, gmock_a3, gmock_a4); \
    } \
    static void* oldFn_; \
}; void* mock_module_##func::oldFn_ = nullptr;

#define MOCK_MODULE_FUNC4(m, ...) MOCK_MODULE_FUNC4_(, , , m, __VA_ARGS__)
#define MOCK_MODULE_FUNC4_CALLCONV(ct, m, ...) MOCK_MODULE_FUNC4_(, , ct, m, __VA_ARGS__)
#define MOCK_MODULE_FUNC4_STDCALL(m, ...) MOCK_MODULE_FUNC4_CALLCONV(__stdcall, m, __VA_ARGS__)

#define MOCK_MODULE_FUNC5_(tn, constness, ct, func, ...) \
struct mock_module_##func \
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
    static mock_module_##func& instance() \
    { \
        static ::testing::NiceMock< mock_module_##func > obj; \
        return obj; \
    } \
    static GMOCK_RESULT_(tn, __VA_ARGS__) ct stub( \
        GMOCK_ARG_(tn, 1, __VA_ARGS__) gmock_a1, \
        GMOCK_ARG_(tn, 2, __VA_ARGS__) gmock_a2, \
        GMOCK_ARG_(tn, 3, __VA_ARGS__) gmock_a3, \
        GMOCK_ARG_(tn, 4, __VA_ARGS__) gmock_a4, \
        GMOCK_ARG_(tn, 5, __VA_ARGS__) gmock_a5) \
    { \
        return mock_module_##func::instance().func( \
            gmock_a1, gmock_a2, gmock_a3, gmock_a4, gmock_a5); \
    } \
    static void* oldFn_; \
}; void* mock_module_##func::oldFn_ = nullptr;

#define MOCK_MODULE_FUNC5(m, ...) MOCK_MODULE_FUNC5_(, , , m, __VA_ARGS__)
#define MOCK_MODULE_FUNC5_CALLCONV(ct, m, ...) MOCK_MODULE_FUNC5_(, , ct, m, __VA_ARGS__)
#define MOCK_MODULE_FUNC5_STDCALL(m, ...) MOCK_MODULE_FUNC5_CALLCONV(__stdcall, m, __VA_ARGS__)

#define MOCK_MODULE_FUNC6_(tn, constness, ct, func, ...) \
struct mock_module_##func \
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
    static mock_module_##func& instance() \
    { \
        static ::testing::NiceMock< mock_module_##func > obj; \
        return obj; \
    } \
    static GMOCK_RESULT_(tn, __VA_ARGS__) ct stub( \
        GMOCK_ARG_(tn, 1, __VA_ARGS__) gmock_a1, \
        GMOCK_ARG_(tn, 2, __VA_ARGS__) gmock_a2, \
        GMOCK_ARG_(tn, 3, __VA_ARGS__) gmock_a3, \
        GMOCK_ARG_(tn, 4, __VA_ARGS__) gmock_a4, \
        GMOCK_ARG_(tn, 5, __VA_ARGS__) gmock_a5, \
        GMOCK_ARG_(tn, 6, __VA_ARGS__) gmock_a6) \
    { \
        return mock_module_##func::instance().func( \
            gmock_a1, gmock_a2, gmock_a3, gmock_a4, gmock_a5, gmock_a6); \
    } \
    static void* oldFn_; \
}; void* mock_module_##func::oldFn_ = nullptr;

#define MOCK_MODULE_FUNC6(m, ...) MOCK_MODULE_FUNC6_(, , , m, __VA_ARGS__)
#define MOCK_MODULE_FUNC6_CALLCONV(ct, m, ...) MOCK_MODULE_FUNC6_(, , ct, m, __VA_ARGS__)
#define MOCK_MODULE_FUNC6_STDCALL(m, ...) MOCK_MODULE_FUNC6_CALLCONV(__stdcall, m, __VA_ARGS__)

#define MOCK_MODULE_FUNC7_(tn, constness, ct, func, ...) \
struct mock_module_##func \
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
    static mock_module_##func& instance() \
    { \
        static ::testing::NiceMock< mock_module_##func > obj; \
        return obj; \
    } \
    static GMOCK_RESULT_(tn, __VA_ARGS__) ct stub( \
        GMOCK_ARG_(tn, 1, __VA_ARGS__) gmock_a1, \
        GMOCK_ARG_(tn, 2, __VA_ARGS__) gmock_a2, \
        GMOCK_ARG_(tn, 3, __VA_ARGS__) gmock_a3, \
        GMOCK_ARG_(tn, 4, __VA_ARGS__) gmock_a4, \
        GMOCK_ARG_(tn, 5, __VA_ARGS__) gmock_a5, \
        GMOCK_ARG_(tn, 6, __VA_ARGS__) gmock_a6) \
        GMOCK_ARG_(tn, 7, __VA_ARGS__) gmock_a7) \
    { \
        return mock_module_##func::instance().func( \
            gmock_a1, gmock_a2, gmock_a3, gmock_a4, gmock_a5, gmock_a6, gmock_a7); \
    } \
    static void* oldFn_; \
}; void* mock_module_##func::oldFn_ = nullptr;

#define MOCK_MODULE_FUNC7(m, ...) MOCK_MODULE_FUNC7_(, , , m, __VA_ARGS__)
#define MOCK_MODULE_FUNC7_CALLCONV(ct, m, ...) MOCK_MODULE_FUNC7_(, , ct, m, __VA_ARGS__)
#define MOCK_MODULE_FUNC7_STDCALL(m, ...) MOCK_MODULE_FUNC7_CALLCONV(__stdcall, m, __VA_ARGS__)

#define MOCK_MODULE_FUNC8_(tn, constness, ct, func, ...) \
struct mock_module_##func \
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
    static mock_module_##func& instance() \
    { \
        static ::testing::NiceMock< mock_module_##func > obj; \
        return obj; \
    } \
    static GMOCK_RESULT_(tn, __VA_ARGS__) ct stub( \
        GMOCK_ARG_(tn, 1, __VA_ARGS__) gmock_a1, \
        GMOCK_ARG_(tn, 2, __VA_ARGS__) gmock_a2, \
        GMOCK_ARG_(tn, 3, __VA_ARGS__) gmock_a3, \
        GMOCK_ARG_(tn, 4, __VA_ARGS__) gmock_a4, \
        GMOCK_ARG_(tn, 5, __VA_ARGS__) gmock_a5, \
        GMOCK_ARG_(tn, 6, __VA_ARGS__) gmock_a6) \
        GMOCK_ARG_(tn, 7, __VA_ARGS__) gmock_a7) \
        GMOCK_ARG_(tn, 8, __VA_ARGS__) gmock_a8) \
    { \
        return mock_module_##func::instance().func( \
            gmock_a1, gmock_a2, gmock_a3, gmock_a4, gmock_a5, gmock_a6, gmock_a7, gmock_a8); \
    } \
    static void* oldFn_; \
}; void* mock_module_##func::oldFn_ = nullptr;

#define MOCK_MODULE_FUNC8(m, ...) MOCK_MODULE_FUNC8_(, , , m, __VA_ARGS__)
#define MOCK_MODULE_FUNC8_CALLCONV(ct, m, ...) MOCK_MODULE_FUNC8_(, , ct, m, __VA_ARGS__)
#define MOCK_MODULE_FUNC8_STDCALL(m, ...) MOCK_MODULE_FUNC8_CALLCONV(__stdcall, m, __VA_ARGS__)

void patchModuleFunc(void*, void*, void**);

#define EXPECT_MODULE_FUNC_CALL(func, ...) \
    if (!mock_module_##func::oldFn_) \
    { \
        patchModuleFunc(&func, reinterpret_cast< void* >( \
            &mock_module_##func::stub), &mock_module_##func::oldFn_); \
    } \
    EXPECT_CALL(mock_module_##func::instance(), func(__VA_ARGS__))

#define ON_MODULE_FUNC_CALL(func, ...) \
    if (!mock_module_##func::oldFn_) \
    { \
        patchModuleFunc(&func, reinterpret_cast< void* >( \
            &mock_module_##func::stub), &mock_module_##func::oldFn_); \
    } \
    ON_CALL(mock_module_##func::instance(), func(__VA_ARGS__))

#define INVOKE_REAL(func, ...) \
    reinterpret_cast< decltype(&func) >(mock_module_##func::oldFn_)(__VA_ARGS__)
