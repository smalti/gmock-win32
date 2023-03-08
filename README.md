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
