# Overview

Suppose there is a task to test code that relies on certain system functions. Ideally, the code should be tested against different possible results of these functions. However, creating the necessary conditions for the functions to return specific results can be challenging. For instance, simulating a lack of space when writing to a file requires a lot of effort.

To simplify this process, this library allows you to replace imported module functions, including win32 system functions, with a stub for testing purposes using GMock.

# Implementation idea

To use the mock technique for win32 APIs, you need to replace the original global win32 functions with something else. In C++, this is achieved through dynamic polymorphism with virtual functions via vtable and function overloading.

The Import Address Table (IAT) patching technique is a documented approach that can be used to achieve this goal. The library extends GMock library by using this technique. However, there is still a limitation - we cannot substitute a win32 function that is used by run-time dynamic linking (LoadLibrary + GetProcAddress).

Generally using the mocking global system functions can be a sign of poorly designed code. It means that the code is tightly coupled with the system and relies on a specific implementation with side effects, making it less flexible and difficult to test. However, there are times when we need to test such code without rewriting it (time lack, vtable overhead, simple system wrappers, etc.). Even testing such code is better than having no tests at all.

# Basic usage example

1. Create win32 function mocks by using `MOCK_STDCALL_FUNC` / `MOCK_CDECL_FUNC` macrodefines (define them in global namespace)

```cpp
MOCK_STDCALL_FUNC(DWORD, GetCurrentProcessId);
MOCK_STDCALL_FUNC(DWORD, GetProcessIdOfThread, HANDLE);
```

2. Setup expectations via `EXPECT_MODULE_FUNC_CALL` / `ON_MODULE_FUNC_CALL`

```cpp
ON_MODULE_FUNC_CALL(GetCurrentProcessId).WillByDefault(Return(42));
ON_MODULE_FUNC_CALL(GetProcessIdOfThread, Eq(HANDLE(42))).WillByDefault(Return(1));
```

3. You can use a helper macrodefine `INVOKE_REAL` to call the original function:

```cpp
ON_MODULE_FUNC_CALL(GetProcessIdOfThread, _).WillByDefault(Invoke([&](HANDLE handle) -> DWORD
{
    return ::GetCurrentThread() == handle ?
        INVOKE_REAL(GetProcessIdOfThread, handle) : 12345UL;
}));
```

# Full sample

```cpp
#include "stdafx.h"

#include <gmock/gmock.h>
#include <gmock-win32.h>

using namespace std;

MOCK_STDCALL_FUNC(DWORD, GetCurrentProcessId);
MOCK_STDCALL_FUNC(DWORD, GetProcessIdOfThread, HANDLE);

struct Foo
{
    void test() { testImpl(); }

private:
    void testImpl()
    {
        cout << "GetCurrentProcessId: "  << ::GetCurrentProcessId() << endl;
        cout << "GetProcessIdOfThread: " << ::GetProcessIdOfThread(HANDLE(42)) << endl;
    }
};

int main()
{
    cout << "before mock expectations set:" << endl;
    Foo{ }.test();

    ON_MODULE_FUNC_CALL(GetCurrentProcessId).WillByDefault(Return(42));
    ON_MODULE_FUNC_CALL(GetProcessIdOfThread, Eq(HANDLE(42))).WillByDefault(Return(1));

    cout << endl << "after mock expectations set:" << endl;
    Foo{ }.test();

    return 0;
}
```

Output:
```console
before mock expectations set:
GetCurrentProcessId: 23924
GetProcessIdOfThread: 0

after mock expectations set:
GetCurrentProcessId: 42
GetProcessIdOfThread: 1
```

# Build info

- Tested with googletest 1.8.1
- Compiled with Visual Studio 2015 Update3 v140 toolset

# Version history

### Version 1.0.2 (12 March 2023)
- Added support of MOCK_* macrodefines without specifying the argument count in name

### Version 1.0.1 (10 March 2023)
- Added support of delegating calls to a real function

### Version 1.0.0 (08 March 2023)
- Initial public release
