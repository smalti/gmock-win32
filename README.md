# gmock-win32
The library implements gmock functionality for global win32 functions.

# Overview

Using the mocking technique in global system functions can be a sign of a poorly designed code. It means that the code is tightly coupled and relies on a specific implementation with side-effects, making it less flexible and difficult to test. However, there are times when we need to test such code without rewriting it. Even testing such code is better than having no tests at all.

To use the mock technique for win32 APIs, you need to replace the original global win32 functions with something else. In C++, this is achieved through dynamic polymorphism with virtual functions via vtable and function overloading.

The Import Address Table (IAT) patching technique is a documented approach that can be used to achieve this goal. The library extends GMock library by using this technique. However, there is still a limitation - we cannot substitute a win32 function that is used by run-time dynamic linking (LoadLibrary + GetProcAddress).

# Basic usage example

1. Create win32 function mocks

```cpp
MOCK_MODULE_FUNC0_STDCALL(GetCurrentProcessId,  DWORD(void));
MOCK_MODULE_FUNC1_STDCALL(GetProcessIdOfThread, DWORD(HANDLE));
```

2. Setup expectations

```cpp
ON_MODULE_FUNC_CALL(GetCurrentProcessId).WillByDefault(Return(42));
ON_MODULE_FUNC_CALL(GetProcessIdOfThread, Eq(HANDLE(42))).WillByDefault(Return(1));
```

# Full sample

```cpp
#include "stdafx.h"

#include <gmock/gmock.h>
#include <gmock-win32.h>

using namespace std;

MOCK_MODULE_FUNC0_STDCALL(GetCurrentProcessId,  DWORD(void));
MOCK_MODULE_FUNC1_STDCALL(GetProcessIdOfThread, DWORD(HANDLE));

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

    ON_MODULE_FUNC_CALL(GetCurrentProcessId).WillByDefault(testing::Return(42));
    ON_MODULE_FUNC_CALL(GetProcessIdOfThread, testing::Eq(HANDLE(42))).WillByDefault(testing::Return(1));

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

# Version history

Version 1.0.0 (08 March 2023)
Initial public release
