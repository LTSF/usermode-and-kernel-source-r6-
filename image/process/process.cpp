#include "process.h"

PEPROCESS get_process( HANDLE process_id )
{
	PEPROCESS process;

	if ( !NT_SUCCESS( PsLookupProcessByProcessId( process_id, &process ) ) )
		return NULL;

	return process;
}

PVOID get_process_base( HANDLE process_id )
{
	PEPROCESS process = get_process( process_id );

	if ( !process )
		return NULL;

	return asd2abnnSDF::PsGetProcessSectionBaseAddress( process );
}

PVOID get_process_module_base( HANDLE pid, LPCWSTR module_name )
{
	wchar_t heap_buffer[128] = { 0 };
	wcscpy_s( heap_buffer, module_name );

	PEPROCESS process = nullptr;
	PVOID result = nullptr;

	if ( !NT_SUCCESS( PsLookupProcessByProcessId( pid, &process ) ) )
		return result;

	PPEB64 peb = reinterpret_cast< PPEB64 >(asd2abnnSDF::PsGetProcessPeb( process ) );

	if ( !peb )
		return result;

	KAPC_STATE titan_state;
	KeStackAttachProcess( process, &titan_state);

	for ( PLIST_ENTRY pListEntry = peb->Ldr->InLoadOrderLinks.Flink; pListEntry != &peb->Ldr->InLoadOrderLinks; pListEntry = pListEntry->Flink )
	{
		if ( !pListEntry )
			continue;

		PLDR_DATA_TABLE_ENTRY module_entry = CONTAINING_RECORD( pListEntry, LDR_DATA_TABLE_ENTRY, InLoadOrderLinks );

		UNICODE_STRING unicode_name;
		RtlUnicodeStringInit( &unicode_name, heap_buffer );

		if ( RtlCompareUnicodeString( &module_entry->BaseDllName, &unicode_name, TRUE ) == 0 )
			result = module_entry->DllBase;
	}

	KeUnstackDetachProcess( &titan_state);

	return result;
}
