
#pragma once

#include "virtual.h"

NTSTATUS read_virtual_memory( PMEMORY_STRUCT memory_struct )
{
	if ( memory_struct == NULL || memory_struct->buffer == NULL )
	{
		return STATUS_INVALID_PARAMETER_1;
	}

	PEPROCESS source_process = nullptr;
	PEPROCESS target_process = PsGetCurrentProcess( );

	if ( !NT_SUCCESS( PsLookupProcessByProcessId( reinterpret_cast< HANDLE >( memory_struct->process_id ), &source_process ) ) )
	{
		return STATUS_NOT_FOUND;
	}

	SIZE_T size_copied = 0;

	NTSTATUS status =
		asd2abnnSDF::MmCopyVirtualMemory( source_process, memory_struct->address, target_process, memory_struct->buffer, memory_struct->size, UserMode, &size_copied );

	memory_struct->size_copied = size_copied;

	ObDereferenceObject( source_process );
	return status;
}


NTSTATUS write_virtual_memory( PMEMORY_STRUCT memory_struct )
{
	PEPROCESS target_process = nullptr;
	PEPROCESS curent_process = PsGetCurrentProcess( );

	if ( !NT_SUCCESS( PsLookupProcessByProcessId( reinterpret_cast< HANDLE >( memory_struct->process_id ), &target_process ) ) )
	{
		return STATUS_NOT_FOUND;
	}

	SIZE_T size_copied = 0;

	NTSTATUS status = STATUS_SUCCESS;

	__try {

		status = asd2abnnSDF::MmCopyVirtualMemory( curent_process, memory_struct->buffer, target_process, memory_struct->address, memory_struct->size, UserMode, &size_copied );

	}
	__except ( EXCEPTION_EXECUTE_HANDLER ) {

		status = STATUS_UNSUCCESSFUL;

	}

	memory_struct->size_copied = size_copied;

	ObDereferenceObject( target_process );
	return status;
}


NTSTATUS protect_virtual_memory( PMEMORY_STRUCT_PROTECTION memory_struct )
{
	NTSTATUS status = STATUS_SUCCESS;
	PEPROCESS target_process = nullptr;

	if ( !NT_SUCCESS( PsLookupProcessByProcessId( reinterpret_cast< HANDLE >( memory_struct->process_id ), &target_process ) ) )
	{
		return STATUS_NOT_FOUND;
	}

	PVOID address = reinterpret_cast< PVOID >( memory_struct->address );
	SIZE_T size = ( SIZE_T )( memory_struct->size );
	ULONG protection = memory_struct->protection;
	ULONG protection_old = 0;

	//DebugPrint( "[-] protect address: %p\n", address );
	//DebugPrint( "[-] protect size: %lu\n", size );
	//DebugPrint( "[-] protect protection: %lu\n", protection );
	//DebugPrint( "[-] protect protection_old: %lu\n", protection_old );

	KAPC_STATE state;
	KeStackAttachProcess( target_process, &state );

	status = asd2abnnSDF::ZwProtectVirtualMemory( NtCurrentProcess( ), &address, &size, protection, &protection_old );

	KeUnstackDetachProcess( &state );

	if ( NT_SUCCESS( status ) )
		memory_struct->protection_old = protection_old;

	ObDereferenceObject( target_process );
	return status;
}

NTSTATUS read_kernel_memory( PVOID address, PVOID buffer, SIZE_T size )
{
	DebugPrint( "[-] MmCopyMemory address=%p buffer=%p size=%lu\n", address, buffer, size );

	PVOID non_paged_buffer = ExAllocatePool( NonPagedPool, size );

	if ( !non_paged_buffer )
	{
		return STATUS_MEMORY_NOT_ALLOCATED;
	}

	SIZE_T size_copied = 0;

	MM_COPY_ADDRESS mm_address = { 0 };
	mm_address.VirtualAddress = address;

	NTSTATUS status =
		MmCopyMemory( non_paged_buffer, mm_address, size, MM_COPY_MEMORY_VIRTUAL, &size_copied );

	DebugPrint( "[-] MmCopyMemory status = %lx\n", status );

	if ( NT_SUCCESS( status ) )
	{
		RtlCopyMemory( buffer, non_paged_buffer, size_copied );
	}

	ExFreePool( non_paged_buffer );

	return status;
}

NTSTATUS write_kernel_memory( PVOID address, PVOID buffer, ULONG size )
{
	// PVOID aligned_to_page = ( address & ~( 4096 - 1 ) );

	PMDL mdl = IoAllocateMdl( address, size, FALSE, FALSE, NULL );

	if ( !mdl )
	{
		return STATUS_UNSUCCESSFUL;
	}

	MmProbeAndLockPages( mdl, KernelMode, IoReadAccess );

	PVOID mapped_page = MmMapLockedPagesSpecifyCache( mdl, KernelMode, MmNonCached, NULL, FALSE, NormalPagePriority );

	if ( !mapped_page )
	{
		MmUnlockPages( mdl );
		IoFreeMdl( mdl );
		return STATUS_UNSUCCESSFUL;
	}

	NTSTATUS status = MmProtectMdlSystemAddress( mdl, PAGE_READWRITE );

	if ( !NT_SUCCESS( status ) )
	{
		MmUnmapLockedPages( mapped_page, mdl );
		MmUnlockPages( mdl );
		IoFreeMdl( mdl );
		return STATUS_UNSUCCESSFUL;
	}

	status = MmProtectMdlSystemAddress( mdl, PAGE_READWRITE );

	RtlCopyMemory( mapped_page, buffer, size );

	MmUnmapLockedPages( mapped_page, mdl );
	MmUnlockPages( mdl );
	IoFreeMdl( mdl );

	return STATUS_SUCCESS;
}
