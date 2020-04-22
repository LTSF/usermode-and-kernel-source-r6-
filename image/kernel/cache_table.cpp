#include "cache_table.h"

IDDB_CACHE_ENTRY* get_first_ddbcache_entry( )
{
	const char* pattern_piddbcache_table = "48 8D 0D ? ? ? ? 66 89 54 24 ? 66 89 54 24 ? 48 8D 54 24 ? 8B 46 08 89 44 24 40 E8";

	PVOID piddbcache_table_instruction
		= find_pattern( L"ntoskrnl.exe", pattern_piddbcache_table );

	if ( !piddbcache_table_instruction )
		return nullptr;

	UINT_PTR piddbcache_table_address
		= parse_offset( piddbcache_table_instruction, 3 ) + 7;

	if ( !piddbcache_table_address )
		return nullptr;

	PRTL_AVL_TABLE table = reinterpret_cast< PRTL_AVL_TABLE >( piddbcache_table_address );

	if ( !table )
		return nullptr;

	if ( table->NumberGenericTableElements == 0 )
		return nullptr;

	// ntoskrnl loops children to the right

	PRTL_BALANCED_LINKS first_link
		= table->BalancedRoot.RightChild;

	if ( !first_link )
		return nullptr;

	UINT_PTR entry_address
		= reinterpret_cast< UINT_PTR >( first_link ) + sizeof( RTL_BALANCED_LINKS );

	return reinterpret_cast< IDDB_CACHE_ENTRY* >( entry_address );
}

NTSTATUS change_iddb_table_entry( ULONG timestamp )
{
	wchar_t buffer[32] = { 0 };
	random_wide_string( buffer, 30 );

	IDDB_CACHE_ENTRY* first_entry = get_first_ddbcache_entry( );

	// each entry is connected in a linked list and doesn't seem
	// to be affected by the re-ordering of the avl tree

	if ( !first_entry )
		return STATUS_INTERNAL_ERROR;

	// track how many entries were changed

	ULONG count = 0;

	for ( PLIST_ENTRY link = first_entry->List.Flink; link != first_entry->List.Blink; link = link->Flink, count++ )
	{
		IDDB_CACHE_ENTRY* cache_entry = reinterpret_cast< IDDB_CACHE_ENTRY* >( link );

		DebugPrint( "list_id[%lu] length: %d stamp: %x name: %wZ \n",
			count,
			cache_entry->DriverName.Length,
			cache_entry->Timestamp,
			cache_entry->DriverName );

		if ( cache_entry->Timestamp == timestamp )
		{
			USHORT bytes = cache_entry->DriverName.Length;
			random_wide_string( cache_entry->DriverName.Buffer, bytes - 1 );
			cache_entry->Timestamp -= random_long( );
		}

		count++;
	}

	if ( count > 0 )
		return STATUS_SUCCESS;
	else
		return STATUS_NOT_FOUND;
}