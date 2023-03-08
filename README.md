# gmock-win32
The library implements gmock functionality for global win32 functions.

# Basic usage example

1. Create win32 function mocks

```
MOCK_MODULE_FUNC0_STDCALL(GetCurrentProcessId,  DWORD(void));
MOCK_MODULE_FUNC1_STDCALL(GetProcessIdOfThread, DWORD(HANDLE));
```

2. Setup expectations

```
ON_MODULE_FUNC_CALL(GetCurrentProcessId).WillByDefault(Return(42));
ON_MODULE_FUNC_CALL(GetProcessIdOfThread, Eq(HANDLE(42))).WillByDefault(Return(1));
```

# Full sample

```
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
```
before mock expectations set:
GetCurrentProcessId: 23924
GetProcessIdOfThread: 0

after mock expectations set:
GetCurrentProcessId: 42
GetProcessIdOfThread: 1
```
