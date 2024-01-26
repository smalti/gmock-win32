# Overview

[![gmock-win32 lib](https://img.shields.io/badge/gmock-win32-blue)](https://github.com/smalti/gmock-win32)
[![gmock-win32 license](https://img.shields.io/badge/license-MIT-blue)](https://github.com/smalti/gmock-win32/blob/main/LICENSE)
[![gmock-win32 version](https://img.shields.io/github/v/release/smalti/gmock-win32?label=version&color=blue)](https://github.com/smalti/gmock-win32/releases/latest)
[![cpp-logo](https://img.shields.io/badge/C%2B%2B-v14-blue?logo=cplusplus)](https://en.wikipedia.org/wiki/C++)
[![WindowsOs-logo](https://img.shields.io/badge/platform-windows-blue?logo=Windows)](https://en.wikipedia.org/wiki/Microsoft_Windows)

Suppose there is a task to test code that relies on certain system functions. Ideally, the code should be tested against different possible results of these functions. However, creating the necessary conditions for the functions to return specific results can be challenging. For instance, simulating a lack of space when writing to a file requires a lot of effort.

To simplify this process, this library allows you to replace imported module functions, including Win32 system functions, with a stub for testing purposes using GMock. Since it is an extension of the GMock library, you can use the same actions and matchers as you normally would with GMock.

---

Mocking of system functions in general can be a sign of poorly designed code. This means that the code is tightly coupled to the system and relies on a specific implementation with side effects, making it less flexible and difficult to test.

But sometimes it may be impossible (or quite expensive) to rewrite existing code to decouple it from that system API before testing it. Also we must not forget that `wrappers over the system API must also be tested` (and it's usually not recommended to introduce an extra layer of abstraction solely for testing purposes). In such situations, we can use this GMock-lib extension to test that code (at least tests for system wrappers that many projects have).
In any case, try to avoid [breaking the contracts](https://stackoverflow.com/questions/2965483/unit-tests-the-benefit-from-unit-tests-with-contract-changes) by monitoring/checking test design.

# Implementation idea

To use the mock technique for Win32 APIs, you need to replace the original global Win32 functions with something else. In C++, this is achieved through dynamic polymorphism with virtual functions via vtable and function overloading.

The `Import Address Table` (IAT) patching technique is a documented approach that can be used to achieve this goal. The library extends GMock library by using this technique. However, there is still a limitation - we cannot substitute a Win32 function that is used by run-time dynamic linking (`LoadLibrary` + `GetProcAddress`). You can find several GMock library extensions on GitHub, each with its own unique approach. The current implementation was inspired by a header-only library called [gmock-global](https://github.com/apriorit/gmock-global).

# Basic Concepts

To use the mock behavior for Win32 API (or other DLL) module functions, you'll first need to build the library. Once that's done, you can include the library `gmock-win32.h` header immediately after the `gmock/gmock.h` header in your test code. Several macros are then available to help you achieve the desired mock behavior:

#### 1. First, we need to initialize the library by using `initialize` / `uninitialize` or `init_scope` initialization wrapper. Typically, it can be done in `main` function before initializing `GTest`:

```cpp
int main(int argc, char* argv[])
{
    const gmock_win32::init_scope gmockWin32{ };

    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
```

#### 2. Create function mocks by using `MOCK_STDCALL_FUNC` / `MOCK_CDECL_FUNC` macro by defining them in **global scope**:

Macro syntax: `MOCK_*CALLING_CONVENTION*_FUNC(Retval, FunctionName, arg1, arg2...)`:

```cpp
MOCK_STDCALL_FUNC(DWORD, GetCurrentProcessId);
MOCK_STDCALL_FUNC(DWORD, GetProcessIdOfThread, HANDLE);
```

#### 3. Then setup expectations via `EXPECT_MODULE_FUNC_CALL` / `ON_MODULE_FUNC_CALL` + use GMock matchers and actions as usual

Macro syntax: `*EXPECTATION*_MODULE_FUNC_CALL(FunctionName, matchers...)`

EXPECT_MODULE_FUNC_CALL:
```cpp
EXPECT_MODULE_FUNC_CALL(GetCurrentProcessId).WillOnce(Return(42));
EXPECT_MODULE_FUNC_CALL(GetProcessIdOfThread, _).WillRepeatedly(Return(42));
```

ON_MODULE_FUNC_CALL:
```cpp
ON_MODULE_FUNC_CALL(GetCurrentProcessId).WillByDefault(Return(42));
ON_MODULE_FUNC_CALL(GetProcessIdOfThread, Eq(HANDLE(42))).WillByDefault(Return(1));
```

#### 4. To validate and clear expectations after tests use the `VERIFY_AND_CLEAR_MODULE_FUNC` or `VERIFY_AND_CLEAR_MODULE_FUNC_EXPECTATIONS` macro

In `GMock` the expectations of a mock object are verified upon its destruction. However, since we do not destroy our objects, we must manually verify our expectations after test. In the general case, we should verify expectations after every test case, similar to how a Mock object would be destroyed. This can typically be done in the `TearDown` method:

```cpp
// Verifies and removes the expectations
VERIFY_AND_CLEAR_MODULE_FUNC_EXPECTATIONS(GetCurrentProcessId);

// Verifies and removes the expectations and removes the default actions set by ON_CALL
VERIFY_AND_CLEAR_MODULE_FUNC(GetProcessIdOfThread);
```

#### 5. Use the `RESTORE_MODULE_FUNC` macro to restore the original module function and remove the IAT patch after all tests finished

```cpp
RESTORE_MODULE_FUNC(GetCurrentProcessId);
```
## Demonstration of the library API

```cpp
#include <gmock/gmock.h>
#include <gmock-win32.h>

using ::testing::Eq;
using ::testing::Return;

MOCK_STDCALL_FUNC(DWORD, GetCurrentProcessId);
MOCK_STDCALL_FUNC(DWORD, GetProcessIdOfThread, HANDLE);

void test()
{
    cout << "  GetCurrentProcessId:  " << ::GetCurrentProcessId() << endl;
    cout << "  GetProcessIdOfThread: " << ::GetProcessIdOfThread(HANDLE(42)) << endl;
}

int main()
{
    const gmock_win32::init_scope gmockWin32{ };

    // Initial case: we will make actual Win32 function calls and
    // will see their real results before any GMock expectations
    // are set:

    cout << "Before mock expectations set:" << endl;
    test();

    // Now we proceed to set up expectations (this process also
    // involves patching the real Win32 module functions):

    EXPECT_MODULE_FUNC_CALL(GetCurrentProcessId).WillRepeatedly(Return(42));
    ON_MODULE_FUNC_CALL(GetProcessIdOfThread, Eq(HANDLE(42))).WillByDefault(Return(1));

    // After setting up expectations, we will receive faked results that
    // are based on our expectations:

    cout << endl << "After mock expectations set:" << endl;
    test();

    return 0;
}
```

### Output:
```console
Before mock expectations set:
  GetCurrentProcessId:  23924
  GetProcessIdOfThread: 0

After mock expectations set:
  GetCurrentProcessId:  42
  GetProcessIdOfThread: 1
```

### Here are some additional examples of library usage: [gmock-win32-sample](https://github.com/smalti/gmock-win32-sample)

# Advanced Topics

#### 1. Delegating calls to a real function

You can use a helper macro `INVOKE_REAL_MODULE_FUNC` to call the original function:

```cpp
ON_MODULE_FUNC_CALL(GetProcessIdOfThread, _).WillByDefault(Invoke([&](HANDLE handle) -> DWORD
{
    return ::GetCurrentThread() == handle ?
        INVOKE_REAL_MODULE_FUNC(GetProcessIdOfThread, handle) : 12345UL;
}));
```
You can also use the `REAL_MODULE_FUNC` macro to get a reference to the original function without calling it. This can be useful to create a mock that calls through to the real function, but is still instrumented:

```cpp
ON_MODULE_FUNC_CALL(GetProcessIdOfThread, _).WillByDefault(Invoke(REAL_MODULE_FUNC(GetProcessIdOfThread)));
EXPECT_MODULE_FUNC_CALL(GetProcessIdOfThread, _).Times(1);
```

#### 2. Mocking APIs that are also used inside GTest

There are several Win32 API functions used within GTest code that may interact with client tests. This interaction can lead to oversaturated results or even deadlocks. API used in GTest:

```cpp
// Possible list of used APIs in GTest (depends on version of the GTest)
CloseHandle
CreateThread
DeleteCriticalSection
EnterCriticalSection
GetCurrentThreadId
GetCurrentThread
GetLastError
GetTempFileNameA
GetTempPathA
GetThreadPriority
InitializeCriticalSection
InterlockedCompareExchange
LeaveCriticalSection
ResumeThread
SetThreadPriority
Sleep
WaitForSingleObject
```

To address such issues, we have two options. First, we can utilize the `BYPASS_MOCKS` macro to suppress mocks during the execution of GMock code. Alternatively, we can create a `gmock_win32::bypass_mocks{ }` object to apply suppression within a specific scope.

However, it's important to note that you do not need to manually suppress mocks when creating function mocks using the `MOCK_STDCALL_FUNC` or `MOCK_CDECL_FUNC` macros, or when setting up expectations with their clauses using the `ON_MODULE_FUNC_CALL` or `EXPECT_MODULE_FUNC_CALL` macros - this is done automatically for you. Additionally, there's the option to use `RESTORE_MODULE_FUNC` to revert the API to its original state, for instance, before calling `VERIFY_AND_CLEAR_MODULE_FUNC_EXPECTATIONS`.

## Mocks bypassing example:

```cpp
#include <gmock/gmock.h>
#include <gmock-win32.h>

// There's no need to take any action to suppress mocks during
// their creation:

MOCK_STDCALL_FUNC(DWORD, GetCurrentThreadId);
...

TEST(GetCurrentThreadIdTest, BaseTest)
{
    // No action is required to suppress mocks during setup of expectations - it
    // will be handled automatically:

    ON_MODULE_FUNC_CALL(GetCurrentThreadId).WillByDefault(testing::Return(42U));
    EXPECT_MODULE_FUNC_CALL(GetCurrentThreadId).Times(2);

    const auto tid1 = ::GetCurrentThreadId();
    const auto tid2 = ::GetCurrentThreadId();

    // Here we suppress our mocks during the execution of EXPECT_EQ (GTest uses
    // the GetCurrentThreadId function (inside the EXPECT_EQ macro) and therefore
    // we could get wrong results):

    BYPASS_MOCKS(EXPECT_EQ(tid1, 42U));
    BYPASS_MOCKS(EXPECT_EQ(tid2, 42U));

    // Mocks can also be suppressed using a scoped object (during the execution
    // of EXPECT_EQ):
    
    {
        const gmock_win32::bypass_mocks useOrigAPI{ };

        EXPECT_EQ(tid1, 42U);
        EXPECT_EQ(tid2, 42U);
    }

    ...

    // If there is some code that shouldn't be involved during testing
    // we can also suppress it:

    {
        const gmock_win32::bypass_mocks useOrigAPI{ };
        std::cout << std::this_thread::get_id() << std::endl;
    }
    ...
    
    RESTORE_MODULE_FUNC(GetCurrentThreadId);
    VERIFY_AND_CLEAR_MODULE_FUNC_EXPECTATIONS(GetCurrentThreadId);
}

int main(int argc, char* argv[])
{
    const gmock_win32::init_scope gmockWin32{ };

    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
```

# Supported Platforms

* C++ Version >= 14
* MSVC >= 2019 (v142)
* Clang >= 12.0.0
* Windows Client >= 10

# Tested with GTest

* `googletest 1.8.1`
* `googletest 1.10.0`
* `googletest 1.11.0`
* `googletest 1.12.1`
* `googletest 1.13.0`
* `googletest 1.14.0`

# Related Open Source Projects

[GoogleTest](https://github.com/google/googletest)

# Version history

## Version 1.2.2 (25 January 2024)
- Added support for `Win32 API forwarding`
- Added support for macro expansion across all macro definitions
- Added `VERIFY_AND_CLEAR_MODULE_FUNC` macro
- Fixed a typo related to 12 arguments
- Fixed C++20 compilation errors and warning 4702

## Version 1.2.1 (19 October 2023)
- Added macro expansion for redefined Win32 functions (e.g. `GetWindowLongPtrA`)
- Added `initialization` / `uninitialization` of the lib-core
- Added error codes to `std::runtime_error` exceptions
- Unified public header code style

## Version 1.2.0 (05 October 2023)
- Added the possibility to `bypass mocked APIs` used within GTest
- Added compatibility with `Clang`
- Changed the usage of Win32 APIs in the library core from direct usage to runtime dynamic linking
- Disabled optimization for sensitive code sections
- Fixed `LNK1169`: one or more multiply defined symbols found (for an old function pointer)

## Version 1.1.0 (29 August 2023)
- Added support for functions with 9-13 parameters
- Added `REAL_MODULE_FUNC` macro
- Fixed problem with Windows `ApiSet` DLL functions redirection (`AppCompat` via `Shimm DLL`)

<details>
<summary>Old versions</summary>

## Version 1.0.4 (19 March 2023)
- Added support for googletest v1.11.0 / v1.12.1 / v1.13.0

## Version 1.0.3 (14 March 2023)
- Added `VERIFY_AND_CLEAR_MODULE_FUNC_EXPECTATIONS` macro
- Added `RESTORE_MODULE_FUNC` macro

## Version 1.0.2 (12 March 2023)
- Added support of new `MOCK_*` macro, which no longer requires you to specify the argument count in the name

## Version 1.0.1 (10 March 2023)
- Added support of delegating calls to a real function via `INVOKE_REAL_MODULE_FUNC` macro

## Version 1.0.0 (08 March 2023)
- Initial public release
</details>
