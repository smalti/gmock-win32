#include "gtest\gtest.h"
#include "gmock\gmock.h"
#include "gmock-win32.h"
#include <Windows.h>

// 'reinterpret_cast': pointer truncation from 'HANDLE' to 'DWORD'
#pragma warning( disable: 4311 )
// cast to smaller integer type 'unsigned long' from 'void *'
#ifdef __clang__
#	pragma clang diagnostic ignored "-Wvoid-pointer-to-int-cast"
#endif // __clang__

// 0
MOCK_STDCALL_FUNC( HWINSTA, AnyPopup );
// 1
MOCK_STDCALL_FUNC( BOOL, AbortPath, HDC );
// 2
MOCK_STDCALL_FUNC( BOOL, ActivateActCtx, HANDLE, ULONG_PTR *);
// 3
MOCK_STDCALL_FUNC( BOOL, AddConsoleAliasA, LPSTR, LPSTR, LPSTR);
// 4
MOCK_STDCALL_FUNC( BOOL, AddAccessAllowedAce, PACL, DWORD, DWORD, PSID );
// 5
MOCK_STDCALL_FUNC( BOOL, AddAccessAllowedAceEx, PACL, DWORD, DWORD, DWORD, PSID );
// 6
MOCK_STDCALL_FUNC( BOOL, AddAuditAccessAce, PACL, DWORD, DWORD, PSID, BOOL, BOOL );
// 7
MOCK_STDCALL_FUNC( BOOL, AddAccessAllowedObjectAce, PACL, DWORD, DWORD, DWORD, GUID*, GUID*, PSID );
// 8
MOCK_STDCALL_FUNC( BOOL, AccessCheck, PSECURITY_DESCRIPTOR, HANDLE, DWORD, PGENERIC_MAPPING, PPRIVILEGE_SET, LPDWORD, LPDWORD, LPBOOL );
// 9
MOCK_STDCALL_FUNC( BOOL, AddAuditAccessObjectAce, PACL, DWORD, DWORD, DWORD, GUID*, GUID*, PSID, BOOL, BOOL );
// 10
MOCK_STDCALL_FUNC( HANDLE, CreateFileTransactedA, LPCSTR, DWORD, DWORD, LPSECURITY_ATTRIBUTES, DWORD, DWORD, HANDLE, HANDLE, PUSHORT, PVOID );
// 11
MOCK_STDCALL_FUNC( BOOL, AccessCheckByType, PSECURITY_DESCRIPTOR, PSID, HANDLE, DWORD, POBJECT_TYPE_LIST, DWORD, PGENERIC_MAPPING, PPRIVILEGE_SET, LPDWORD, LPDWORD, LPBOOL );
// 12
MOCK_STDCALL_FUNC( BOOL, ObjectOpenAuditAlarmW, LPCWSTR, LPVOID, LPWSTR, LPWSTR, PSECURITY_DESCRIPTOR, HANDLE, DWORD, DWORD, PPRIVILEGE_SET, BOOL, BOOL, LPBOOL );
// 13
MOCK_STDCALL_FUNC( SC_HANDLE, CreateServiceA, SC_HANDLE, LPCSTR, LPCSTR, DWORD, DWORD, DWORD, DWORD, LPCSTR, LPCSTR, LPDWORD, LPCSTR, LPCSTR , LPCSTR );
// 14
// MOCK_STDCALL_FUNC( HFONT, CreateFontA, int, int, int, int, int, DWORD, DWORD, DWORD, DWORD, DWORD, DWORD, DWORD, DWORD, LPCSTR );
// 15?
// 16
//AccessCheckByTypeAndAuditAlarmW
// 17
//AccessCheckByTypeResultListAndAuditAlarmByHandleW

TEST(from0ToMax, args0) {
	EXPECT_MODULE_FUNC_CALL( AnyPopup );
	AnyPopup( );
}
TEST(from0ToMax, args1) {
	auto arg0 = reinterpret_cast< HDC >( INVALID_HANDLE_VALUE );
	EXPECT_MODULE_FUNC_CALL( AbortPath, 
			arg0 
		);
	AbortPath( 
			arg0 
		);
}
TEST(from0ToMax, args2) {
	auto hActCtx = reinterpret_cast< HANDLE >( INVALID_HANDLE_VALUE );
	auto lpCookie = reinterpret_cast< ULONG_PTR * >( INVALID_HANDLE_VALUE );
	EXPECT_MODULE_FUNC_CALL( ActivateActCtx, 
			hActCtx
			, lpCookie
		);
	ActivateActCtx(
			hActCtx
			, lpCookie
		);
}
TEST(from0ToMax, args3) {
	auto Source = reinterpret_cast< LPSTR >( INVALID_HANDLE_VALUE );
	auto Target = reinterpret_cast< LPSTR >( INVALID_HANDLE_VALUE );
	auto ExeName = reinterpret_cast< LPSTR >( INVALID_HANDLE_VALUE );
	EXPECT_MODULE_FUNC_CALL( AddConsoleAliasA, 
			Source
			, Target
			, ExeName
		);
	AddConsoleAliasA(
			Source
			, Target
			, ExeName
		);
}
TEST(from0ToMax, args4) {
	auto pAcl = reinterpret_cast< PACL >( INVALID_HANDLE_VALUE );
	auto dwAceRevision = reinterpret_cast< DWORD >( INVALID_HANDLE_VALUE );
	auto AccessMask = reinterpret_cast< DWORD >( INVALID_HANDLE_VALUE );
	auto pSid = reinterpret_cast< PSID >( INVALID_HANDLE_VALUE );
	EXPECT_MODULE_FUNC_CALL( AddAccessAllowedAce, 
			pAcl
			, dwAceRevision
			, AccessMask 
			, pSid 
		);
	AddAccessAllowedAce(
			pAcl
			, dwAceRevision
			, AccessMask 
			, pSid 
		);
}
TEST(from0ToMax, args5) {
	auto pAcl = reinterpret_cast< PACL >( INVALID_HANDLE_VALUE );
	auto dwAceRevision = reinterpret_cast< DWORD >( INVALID_HANDLE_VALUE );
	auto AceFlags = reinterpret_cast< DWORD >( INVALID_HANDLE_VALUE );
	auto AccessMask = reinterpret_cast< DWORD >( INVALID_HANDLE_VALUE );
	auto pSid = reinterpret_cast< PSID >( INVALID_HANDLE_VALUE );
	EXPECT_MODULE_FUNC_CALL( AddAccessAllowedAceEx, 
			pAcl
			, dwAceRevision
			, AceFlags
			, AccessMask 
			, pSid 
		);
	AddAccessAllowedAceEx(
			pAcl
			, dwAceRevision
			, AceFlags
			, AccessMask 
			, pSid 
		);
}
TEST(from0ToMax, args6) {
	auto pAcl = reinterpret_cast< PACL >( INVALID_HANDLE_VALUE );
	auto dwAceRevision = reinterpret_cast< DWORD >( INVALID_HANDLE_VALUE );
	auto dwAccessMask = reinterpret_cast< DWORD >( INVALID_HANDLE_VALUE );
	auto pSid = reinterpret_cast< PSID >( INVALID_HANDLE_VALUE );
	auto bAuditSuccess = reinterpret_cast< BOOL >( INVALID_HANDLE_VALUE );
	auto bAuditFailure = reinterpret_cast< BOOL >( INVALID_HANDLE_VALUE );
	EXPECT_MODULE_FUNC_CALL( AddAuditAccessAce, 
			pAcl
			, dwAceRevision
			, dwAccessMask
			, pSid
			, bAuditSuccess
			, bAuditFailure
		);
	AddAuditAccessAce( 
			pAcl
			, dwAceRevision
			, dwAccessMask
			, pSid
			, bAuditSuccess
			, bAuditFailure
		);
}
TEST(from0ToMax, args7) {
	auto pAcl = reinterpret_cast< PACL >( INVALID_HANDLE_VALUE );
	auto dwAceRevision = reinterpret_cast< DWORD >( INVALID_HANDLE_VALUE );
	auto AceFlags = reinterpret_cast< DWORD >( INVALID_HANDLE_VALUE );
	auto AccessMask = reinterpret_cast< DWORD >( INVALID_HANDLE_VALUE );
	auto ObjectTypeGuid = reinterpret_cast< GUID* >( INVALID_HANDLE_VALUE );
	auto InheritedObjectTypeGuid = reinterpret_cast< GUID* >( INVALID_HANDLE_VALUE );
	auto pSid = reinterpret_cast< PSID >( INVALID_HANDLE_VALUE );
	EXPECT_MODULE_FUNC_CALL( AddAccessAllowedObjectAce, 
			pAcl
			, dwAceRevision
			, AceFlags
			, AccessMask
			, ObjectTypeGuid
			, InheritedObjectTypeGuid
			, pSid
		);
	AddAccessAllowedObjectAce( 
			pAcl
			, dwAceRevision
			, AceFlags
			, AccessMask
			, ObjectTypeGuid
			, InheritedObjectTypeGuid
			, pSid
		);
}
TEST(from0ToMax, args8) {
	auto pSecurityDescriptor = reinterpret_cast< PSECURITY_DESCRIPTOR >( INVALID_HANDLE_VALUE );
	auto ClientToken = reinterpret_cast< HANDLE >( INVALID_HANDLE_VALUE );
	auto DesiredAccess = reinterpret_cast< DWORD >( INVALID_HANDLE_VALUE );
	auto GenericMapping = reinterpret_cast< PGENERIC_MAPPING >( INVALID_HANDLE_VALUE );
	auto PrivilegeSet = reinterpret_cast< PPRIVILEGE_SET >( INVALID_HANDLE_VALUE );
	auto PrivilegeSetLength = reinterpret_cast< LPDWORD >( INVALID_HANDLE_VALUE );
	auto GrantedAccess = reinterpret_cast< LPDWORD >( INVALID_HANDLE_VALUE );
	auto AccessStatus = reinterpret_cast< LPBOOL >( INVALID_HANDLE_VALUE );
	EXPECT_MODULE_FUNC_CALL( AccessCheck, 
			pSecurityDescriptor
			, ClientToken
			, DesiredAccess
			, GenericMapping
			, PrivilegeSet
			, PrivilegeSetLength
			, GrantedAccess
			, AccessStatus
		);
	AccessCheck( 
			pSecurityDescriptor
			, ClientToken
			, DesiredAccess
			, GenericMapping
			, PrivilegeSet
			, PrivilegeSetLength
			, GrantedAccess
			, AccessStatus
		);
}
TEST(from0ToMax, args9) {
	auto pAcl = reinterpret_cast< PACL >( INVALID_HANDLE_VALUE );
	auto dwAceRevision = reinterpret_cast< DWORD >( INVALID_HANDLE_VALUE );
	auto AceFlags = reinterpret_cast< DWORD >( INVALID_HANDLE_VALUE );
	auto AccessMask = reinterpret_cast< DWORD >( INVALID_HANDLE_VALUE );
	auto ObjectTypeGuid = reinterpret_cast< GUID* >( INVALID_HANDLE_VALUE );
	auto InheritedObjectTypeGuid = reinterpret_cast< GUID* >( INVALID_HANDLE_VALUE );
	auto pSid = reinterpret_cast< PSID >( INVALID_HANDLE_VALUE );
	auto bAuditSuccess = reinterpret_cast< BOOL >( INVALID_HANDLE_VALUE );
	auto bAuditFailure = reinterpret_cast< BOOL >( INVALID_HANDLE_VALUE );
	EXPECT_MODULE_FUNC_CALL( AddAuditAccessObjectAce, 
			pAcl
			, dwAceRevision
			, AceFlags
			, AccessMask
			, ObjectTypeGuid
			, InheritedObjectTypeGuid
			, pSid
			, bAuditSuccess
			, bAuditFailure
		);
	AddAuditAccessObjectAce( 
			pAcl
			, dwAceRevision
			, AceFlags
			, AccessMask
			, ObjectTypeGuid
			, InheritedObjectTypeGuid
			, pSid
			, bAuditSuccess
			, bAuditFailure
		);
}
TEST(from0ToMax, args10) {
	auto lpFileName = reinterpret_cast< LPCSTR >( INVALID_HANDLE_VALUE );
	auto dwDesiredAccess = reinterpret_cast< DWORD >( INVALID_HANDLE_VALUE );
	auto dwShareMode = reinterpret_cast< DWORD >( INVALID_HANDLE_VALUE );
	auto lpSecurityAttributes = reinterpret_cast< LPSECURITY_ATTRIBUTES >( INVALID_HANDLE_VALUE );
	auto dwCreationDisposition = reinterpret_cast< DWORD >( INVALID_HANDLE_VALUE );
	auto dwFlagsAndAttributes = reinterpret_cast< DWORD >( INVALID_HANDLE_VALUE );
	auto hTemplateFile = reinterpret_cast< HANDLE >( INVALID_HANDLE_VALUE );
	auto hTransaction = reinterpret_cast< HANDLE >( INVALID_HANDLE_VALUE );
	auto pusMiniVersion = reinterpret_cast< PUSHORT >( INVALID_HANDLE_VALUE );
	auto lpExtendedParameter = reinterpret_cast< PVOID >( INVALID_HANDLE_VALUE );
	EXPECT_MODULE_FUNC_CALL( CreateFileTransactedA, 
			lpFileName
			, dwDesiredAccess
			, dwShareMode
			, lpSecurityAttributes
			, dwCreationDisposition
			, dwFlagsAndAttributes
			, hTemplateFile
			, hTransaction
			, pusMiniVersion
			, lpExtendedParameter
		);
	CreateFileTransactedA( 
			lpFileName
			, dwDesiredAccess
			, dwShareMode
			, lpSecurityAttributes
			, dwCreationDisposition
			, dwFlagsAndAttributes
			, hTemplateFile
			, hTransaction
			, pusMiniVersion
			, lpExtendedParameter
		);
}
TEST(from0ToMax, args11) {
	auto pSecurityDescriptor = reinterpret_cast< PSECURITY_DESCRIPTOR >( INVALID_HANDLE_VALUE );
	auto PrincipalSelfSid = reinterpret_cast< PSID >( INVALID_HANDLE_VALUE );
	auto ClientToken = reinterpret_cast< HANDLE >( INVALID_HANDLE_VALUE );
	auto DesiredAccess = reinterpret_cast< DWORD >( INVALID_HANDLE_VALUE );
	auto ObjectTypeList = reinterpret_cast< POBJECT_TYPE_LIST >( INVALID_HANDLE_VALUE );
	auto ObjectTypeListLength = reinterpret_cast< DWORD >( INVALID_HANDLE_VALUE );
	auto GenericMapping = reinterpret_cast< PGENERIC_MAPPING >( INVALID_HANDLE_VALUE );
	auto PrivilegeSet = reinterpret_cast< PPRIVILEGE_SET >( INVALID_HANDLE_VALUE );
	auto PrivilegeSetLength = reinterpret_cast< LPDWORD >( INVALID_HANDLE_VALUE );
	auto GrantedAccess = reinterpret_cast< LPDWORD >( INVALID_HANDLE_VALUE );
	auto AccessStatus = reinterpret_cast< LPBOOL >( INVALID_HANDLE_VALUE );
	EXPECT_MODULE_FUNC_CALL( AccessCheckByType, 
			pSecurityDescriptor
			, PrincipalSelfSid
			, ClientToken
			, DesiredAccess
			, ObjectTypeList
			, ObjectTypeListLength
			, GenericMapping
			, PrivilegeSet
			, PrivilegeSetLength
			, GrantedAccess
			, AccessStatus
		);
	AccessCheckByType( 
			pSecurityDescriptor
			, PrincipalSelfSid
			, ClientToken
			, DesiredAccess
			, ObjectTypeList
			, ObjectTypeListLength
			, GenericMapping
			, PrivilegeSet
			, PrivilegeSetLength
			, GrantedAccess
			, AccessStatus
		);
}
TEST(from0ToMax, args12) {
	auto SubsystemName = reinterpret_cast< LPCWSTR >( INVALID_HANDLE_VALUE );
	auto HandleId = reinterpret_cast< LPVOID >( INVALID_HANDLE_VALUE );
	auto ObjectTypeName = reinterpret_cast< LPWSTR >( INVALID_HANDLE_VALUE );
	auto ObjectName = reinterpret_cast< LPWSTR >( INVALID_HANDLE_VALUE );
	auto pSecurityDescriptor = reinterpret_cast< PSECURITY_DESCRIPTOR >( INVALID_HANDLE_VALUE );
	auto ClientToken = reinterpret_cast< HANDLE >( INVALID_HANDLE_VALUE );
	auto DesiredAccess = reinterpret_cast< DWORD >( INVALID_HANDLE_VALUE );
	auto GrantedAccess = reinterpret_cast< DWORD >( INVALID_HANDLE_VALUE );
	auto Privileges = reinterpret_cast< PPRIVILEGE_SET >( INVALID_HANDLE_VALUE );
	auto ObjectCreation = reinterpret_cast< BOOL >( INVALID_HANDLE_VALUE );
	auto AccessGranted = reinterpret_cast< BOOL >( INVALID_HANDLE_VALUE );
	auto GenerateOnClose = reinterpret_cast< LPBOOL >( INVALID_HANDLE_VALUE );
	EXPECT_MODULE_FUNC_CALL( ObjectOpenAuditAlarmW, 
			SubsystemName
			, HandleId
			, ObjectTypeName
			, ObjectName
			, pSecurityDescriptor
			, ClientToken
			, DesiredAccess
			, GrantedAccess
			, Privileges
			, ObjectCreation
			, AccessGranted
			, GenerateOnClose
		);
	ObjectOpenAuditAlarmW( 
			SubsystemName
			, HandleId
			, ObjectTypeName
			, ObjectName
			, pSecurityDescriptor
			, ClientToken
			, DesiredAccess
			, GrantedAccess
			, Privileges
			, ObjectCreation
			, AccessGranted
			, GenerateOnClose
		);
}
TEST(from0ToMax, args13) {
	auto hSCManager = reinterpret_cast< SC_HANDLE >( INVALID_HANDLE_VALUE );
	auto lpServiceName = reinterpret_cast< LPCSTR >( INVALID_HANDLE_VALUE );
	auto lpDisplayName = reinterpret_cast< LPCSTR >( INVALID_HANDLE_VALUE );
	auto dwDesiredAccess = reinterpret_cast< DWORD >( INVALID_HANDLE_VALUE );
	auto dwServiceType = reinterpret_cast< DWORD >( INVALID_HANDLE_VALUE );
	auto dwStartType = reinterpret_cast< DWORD >( INVALID_HANDLE_VALUE );
	auto dwErrorControl = reinterpret_cast< DWORD >( INVALID_HANDLE_VALUE );
	auto lpBinaryPathName = reinterpret_cast< LPCSTR >( INVALID_HANDLE_VALUE );
	auto lpLoadOrderGroup = reinterpret_cast< LPCSTR >( INVALID_HANDLE_VALUE );
	auto lpdwTagId = reinterpret_cast< LPDWORD >( INVALID_HANDLE_VALUE );
	auto lpDependencies = reinterpret_cast< LPCSTR >( INVALID_HANDLE_VALUE );
	auto lpServiceStartName = reinterpret_cast< LPCSTR >( INVALID_HANDLE_VALUE );
	auto lpPassword = reinterpret_cast< LPCSTR >( INVALID_HANDLE_VALUE );
	EXPECT_MODULE_FUNC_CALL( CreateServiceA, 
			hSCManager
			, lpServiceName
			, lpDisplayName
			, dwDesiredAccess
			, dwServiceType
			, dwStartType
			, dwErrorControl
			, lpBinaryPathName
			, lpLoadOrderGroup
			, lpdwTagId
			, lpDependencies
			, lpServiceStartName
			, lpPassword
		);
	CreateServiceA( 
			hSCManager
			, lpServiceName
			, lpDisplayName
			, dwDesiredAccess
			, dwServiceType
			, dwStartType
			, dwErrorControl
			, lpBinaryPathName
			, lpLoadOrderGroup
			, lpdwTagId
			, lpDependencies
			, lpServiceStartName
			, lpPassword
		);
}
//TEST(from0ToMax, args14) {
//	auto cHeight = reinterpret_cast< int >( INVALID_HANDLE_VALUE );
//	auto cWidth = reinterpret_cast< int >( INVALID_HANDLE_VALUE );
//	auto cEscapement = reinterpret_cast< int >( INVALID_HANDLE_VALUE );
//	auto cOrientation = reinterpret_cast< int >( INVALID_HANDLE_VALUE );
//	auto cWeight = reinterpret_cast< int >( INVALID_HANDLE_VALUE );
//	auto bItalic = reinterpret_cast< DWORD >( INVALID_HANDLE_VALUE );
//	auto bUnderline = reinterpret_cast< DWORD >( INVALID_HANDLE_VALUE );
//	auto bStrikeOut = reinterpret_cast< DWORD >( INVALID_HANDLE_VALUE );
//	auto iCharSet = reinterpret_cast< DWORD >( INVALID_HANDLE_VALUE );
//	auto iOutPrecision = reinterpret_cast< DWORD >( INVALID_HANDLE_VALUE );
//	auto iClipPrecision = reinterpret_cast< DWORD >( INVALID_HANDLE_VALUE );
//	auto iQuality = reinterpret_cast< DWORD >( INVALID_HANDLE_VALUE );
//	auto iPitchAndFamily = reinterpret_cast< DWORD >( INVALID_HANDLE_VALUE );
//	auto pszFaceName = reinterpret_cast< LPCSTR >( INVALID_HANDLE_VALUE );
//	EXPECT_MODULE_FUNC_CALL( CreateFontA, 
//			cHeight
//			, cWidth
//			, cEscapement
//			, cOrientation
//			, cWeight
//			, bItalic
//			, bUnderline
//			, bStrikeOut
//			, iCharSet
//			, iOutPrecision
//			, iClipPrecision
//			, iQuality
//			, iPitchAndFamily
//			, pszFaceName
//		);
//	CreateFontA( 
//			cHeight
//			, cWidth
//			, cEscapement
//			, cOrientation
//			, cWeight
//			, bItalic
//			, bUnderline
//			, bStrikeOut
//			, iCharSet
//			, iOutPrecision
//			, iClipPrecision
//			, iQuality
//			, iPitchAndFamily
//			, pszFaceName
//		);
//}
