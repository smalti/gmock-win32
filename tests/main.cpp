#include "gmock-win32.h"

int main(int argc, char **argv)
{
	const gmock_win32::init_scope gmockWin32{ };
	::testing::InitGoogleTest( &argc, argv );

	std::cout << "Running main() from " << __FILE__ << std::endl;
	return RUN_ALL_TESTS( );
}
