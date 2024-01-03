#include "gtest\gtest.h"
#include "gmock\gmock.h"
#include "gmock-win32.h"
#include <Windows.h>
using ::testing::Return;

MOCK_STDCALL_FUNC(DWORD, GetCurrentProcessId);

TEST(auxiliary, invoke_real_wrong) {
	EXPECT_EQ( nullptr, REAL_MODULE_FUNC( GetCurrentProcessId ) );
	EXPECT_ANY_THROW( INVOKE_REAL_MODULE_FUNC( GetCurrentProcessId ) );
}
TEST(auxiliary, invoke_real_correct) {
	EXPECT_MODULE_FUNC_CALL( GetCurrentProcessId ).WillOnce( Return( 0 ) );
	EXPECT_EQ( ::GetCurrentProcessId( ), 0 );
	EXPECT_NE( INVOKE_REAL_MODULE_FUNC( GetCurrentProcessId ), 0 );
}
TEST(auxiliary, restore) {
	EXPECT_MODULE_FUNC_CALL( GetCurrentProcessId ).WillOnce( Return( 0 ) );
	EXPECT_EQ( ::GetCurrentProcessId( ), 0 );
	RESTORE_MODULE_FUNC( GetCurrentProcessId );
	EXPECT_NE( ::GetCurrentProcessId( ), 0 );
}
TEST(auxiliary, restore_then_expect_call) {
	EXPECT_MODULE_FUNC_CALL( GetCurrentProcessId ).WillOnce( Return( 0 ) );
	EXPECT_EQ( ::GetCurrentProcessId( ), 0 );
	RESTORE_MODULE_FUNC( GetCurrentProcessId );
	EXPECT_MODULE_FUNC_CALL( GetCurrentProcessId ).WillOnce( Return( 0 ) );
	EXPECT_EQ( ::GetCurrentProcessId( ), 0 );
}
