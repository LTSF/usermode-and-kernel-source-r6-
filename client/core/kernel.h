#pragma once

#include <windows.h>
#include <cstdint>
#include <cstdlib>
#include <stdio.h>

enum E_COMMAND_CODE
{
	ID_NULL						= 0,	//

	ID_READ_PROCESS_MEMORY		= 5,	// 
	ID_WRITE_PROCESS_MEMORY		= 6,	//

	ID_READ_KERNEL_MEMORY		= 8,	// 

	ID_GET_PROCESS				= 10,	//
	ID_GET_PROCESS_BASE			= 11,	//
	ID_GET_PROCESS_MODULE		= 12,	//

	ID_SET_PAGE_PROTECTION		= 26,  //

	ID_REMOVE_HOOK				= 99,	//

	ID_CONFIRM_DRIVER_LOADED	= 100,	//
};

#pragma pack( push, 8 )
typedef struct _MEMORY_STRUCT
{
	UINT_PTR	process_id;
	PVOID		address;
	SIZE_T		size;
	SIZE_T		size_copied;
	PVOID		buffer;
} MEMORY_STRUCT, *PMEMORY_STRUCT;
#pragma pack( pop )

#pragma pack( push, 8 )
typedef struct _MEMORY_STRUCT_PROTECTION
{
	UINT_PTR	process_id;
	PVOID		address;
	SIZE_T		size;
	ULONG		protection;
	ULONG		protection_old;
} MEMORY_STRUCT_PROTECTION, *PMEMORY_STRUCT_PROTECTION;
#pragma pack( pop )



template<typename ... A>
uint64_t call_driver_control( void* control_function, const A ... arguments )
{
	if ( !control_function )
		return 0;

	using tFunction = uint64_t( __stdcall* )( A... );
	const auto control = static_cast<tFunction>( control_function );

	return control( arguments ... );
}

template<typename T>
__declspec( noinline ) T read_virtual_memory( void* ctrl, uint64_t pid, uint64_t address )
{
	T buffer = NULL;

	if ( !pid )
		return buffer;

	MEMORY_STRUCT memory_struct	= {0};
	memory_struct.process_id	= pid;
	memory_struct.address		= reinterpret_cast<void*>( address );
	memory_struct.size			= sizeof( T );
	memory_struct.buffer		= &buffer;

	NTSTATUS result
		= ( NTSTATUS )( call_driver_control( ctrl, ID_READ_PROCESS_MEMORY, &memory_struct ) );

	if ( result != 0 )
		return NULL;

	return buffer;
}

template<typename T>
__declspec( noinline ) bool write_virtual_memory( void* ctrl, uint64_t pid, uint64_t address, T buffer )
{
	MEMORY_STRUCT memory_struct	= {0};
	memory_struct.process_id	= pid;
	memory_struct.address		= reinterpret_cast<void*>( address );
	memory_struct.size			= sizeof( T );
	memory_struct.buffer		= &buffer;

	NTSTATUS result
		= ( NTSTATUS )( call_driver_control( ctrl, ID_WRITE_PROCESS_MEMORY, &memory_struct ) );

	if ( result != 0 )
		return false;

	return true;
}

void* kernel_control_function( );
uint64_t read_kernel( void* control_function, uint64_t address, void* buffer, std::size_t size );
