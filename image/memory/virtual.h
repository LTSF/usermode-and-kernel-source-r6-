#pragma once

#include <ntifs.h>
#include <cstddef>
#include <cstdint>

#include "..\util\defines.h"

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

NTSTATUS read_virtual_memory( PMEMORY_STRUCT memory_struct );
NTSTATUS write_virtual_memory( PMEMORY_STRUCT memory_struct );
NTSTATUS protect_virtual_memory( PMEMORY_STRUCT_PROTECTION memory_struct );
NTSTATUS read_kernel_memory( PVOID address, PVOID buffer, SIZE_T size );
NTSTATUS write_kernel_memory( PVOID address, PVOID buffer, ULONG size );