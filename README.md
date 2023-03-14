# Overview

Suppose there is a task to test code that relies on certain system functions. Ideally, the code should be tested against different possible results of these functions. However, creating the necessary conditions for the functions to return specific results can be challenging. For instance, simulating a lack of space when writing to a file requires a lot of effort.

To simplify this process, this library allows you to replace imported module functions, including Win32 system functions, with a stub for testing purposes using GMock. Since it is an extension of the GMock library, you can use the same actions and matchers as you normally would with GMock.

# Implementation idea

To use the mock technique for Win32 APIs, you need to replace the original global Win32 functions with something else. In C++, this is achieved through dynamic polymorphism with virtual functions via vtable and function overloading.

The `Import Address Table` (IAT) patching technique is a documented approach that can be used to achieve this goal. The library extends GMock library by using this technique. However, there is still a limitation - we cannot substitute a Win32 function that is used by run-time dynamic linking (`LoadLibrary` + `GetProcAddress`).

Generally using the mocking global system functions can be a sign of poorly designed code. It means that the code is tightly coupled with the system and relies on a specific implementation with side effects, making it less flexible and difficult to test. But sometimes, it may not be feasible to rewrite existing code before testing it, perhaps due to time constraints or other reasons (thin system wrappers, vtable overhead, etc.). In such cases, it's still better to test the code in its current form rather than having no tests at all.

# How to use

To use the mock behavior for Win32 API (or other DLL) module functions, you'll first need to build the library. Once that's done, you can include the library `gmock-win32.h` header immediately after the `gmock/gmock.h` header in your test code. Several macros are then available to help you achieve the desired mock behavior:

#### 1. Create Win32 (DLL module) function mocks by using `MOCK_STDCALL_FUNC` / `MOCK_CDECL_FUNC` macro (by defining them in global scope).

Macro syntax: `MOCK_*CALLING_CONVENTION*_FUNC(Retval, FunctionName, arg1, arg2...)`:

```cpp
MOCK_STDCALL_FUNC(DWORD, GetCurrentProcessId);
MOCK_STDCALL_FUNC(DWORD, GetProcessIdOfThread, HANDLE);
```

#### 2. Setup expectations via `EXPECT_MODULE_FUNC_CALL` / `ON_MODULE_FUNC_CALL` + use GMock matchers and actions as usual:

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

#### 3. You can use a helper macro `INVOKE_REAL_MODULE_FUNC` to call the original function:

```cpp
ON_MODULE_FUNC_CALL(GetProcessIdOfThread, _).WillByDefault(Invoke([&](HANDLE handle) -> DWORD
{
    return ::GetCurrentThread() == handle ?
        INVOKE_REAL_MODULE_FUNC(GetProcessIdOfThread, handle) : 12345UL;
}));
```

#### 4. If you need to use the mock function in multiple tests with different expectations, you can clear the previous expectations and verify them by using the VERIFY_AND_CLEAR_MODULE_FUNC_EXPECTATIONS macro:

```cpp
VERIFY_AND_CLEAR_MODULE_FUNC_EXPECTATIONS(GetCurrentProcessId);
```

#### 5. You can use the `RESTORE_MODULE_FUNC` macro to restore the original module function and remove the IAT patch. This can be useful in situations where you no longer need the mock behavior in the test executable process or if some other 3rd party code in the process uses the same functions somewhere (so, it can help you avoid conflicts with GMock internal implementation, CRT, and other libraries):

```cpp
RESTORE_MODULE_FUNC(GetCurrentProcessId);
```

# Basic sample

```cpp
#include <gmock/gmock.h>
#include <gmock-win32.h>

MOCK_STDCALL_FUNC(DWORD, GetCurrentProcessId);
MOCK_STDCALL_FUNC(DWORD, GetProcessIdOfThread, HANDLE);

void test()
{
    cout << "  GetCurrentProcessId:  " << ::GetCurrentProcessId() << endl;
    cout << "  GetProcessIdOfThread: " << ::GetProcessIdOfThread(HANDLE(42)) << endl;
}

int main()
{
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

# Build info

- Tested with `googletest 1.8.1`
- Compiled with `Visual Studio 2015` Update3 `v140 toolset` x86 / x64

# Version history

### Version 1.0.3 (14 March 2023)
- Added `VERIFY_AND_CLEAR_MODULE_FUNC_EXPECTATIONS` macro
- Added `RESTORE_MODULE_FUNC` macro

### Version 1.0.2 (12 March 2023)
- Added support of new `MOCK_*` macro, which no longer requires you to specify the argument count in the name

### Version 1.0.1 (10 March 2023)
- Added support of delegating calls to a real function via `INVOKE_REAL_MODULE_FUNC` macro

### Version 1.0.0 (08 March 2023)
- Initial public release

### TODO:
- [ ] Add unit tests for the library
- [ ] Make samples that show how to test Win32 C++ wrappers
- [ ] Create CMake build configurations
- [ ] Verify and add support for googletest v1.10.0 / v1.12.1 / v1.13.0
- [x] Add support of original function call
- [x] Support IAT patching removing
- [x] Add support of MOCK_* macro without specifying argument count
