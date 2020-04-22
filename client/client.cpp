#include "client.h"

int main( )
{
	SetConsoleTitle( "" );

	// ** get function that is hooked by driver **

	void* fn = kernel_control_function( );
	unsigned long pid = GetCurrentProcessId( );

	uint64_t loaded_check = call_driver_control( fn, ID_CONFIRM_DRIVER_LOADED );

	if ( loaded_check != 0 )
		printf( "[!] driver communication not set up\n" );



	// ** basic memory **

	uint64_t process_base = call_driver_control( fn, ID_GET_PROCESS_BASE, pid );

	printf( "[-] process_base: %llx\n", process_base );
	
	uint32_t read_result = read_virtual_memory<uint32_t>( fn, pid, process_base );

	printf( "[-] read_result: %lx\n", read_result );

	

	// ** module **

	const wchar_t module_name[] = L"kernel32.dll";
	uint64_t module_base = call_driver_control( fn, ID_GET_PROCESS_MODULE, pid, module_name ); // hardcoded for kernel32.dll
	printf( "[-] kernel32.dll: %llx\n", module_base );


	// ** unhook **

	printf( "[-] unhook result: %llx\n", call_driver_control( fn, ID_REMOVE_HOOK ) );



	while ( ( GetAsyncKeyState( VK_F12 ) & 1 ) == 0 )
		Sleep( 100 );

	return 0;
}
