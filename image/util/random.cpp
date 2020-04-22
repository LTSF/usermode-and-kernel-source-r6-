#include "random.h"

UCHAR random_character( )
{
	ULONG seed = KeQueryTimeIncrement( );
	ULONG64 key = RtlRandomEx( &seed ) % sizeof( character_set ) - 2;
	return character_set[key];
}

NTSTATUS random_ansi_string( char* buffer, size_t length )
{
	if ( !buffer )
		return STATUS_INVALID_PARAMETER_1;

	ULONG seed = KeQueryTimeIncrement( );

	for ( int n = 0; n <= length; n++ )
	{
		ULONG64 key
			= RtlRandomEx( &seed ) % sizeof( character_set ) - 2;

		buffer[n] = character_set[key];
	}

	return STATUS_SUCCESS;
}

NTSTATUS random_wide_string( wchar_t* buffer, size_t length )
{
	if ( !buffer )
		return STATUS_INVALID_PARAMETER_1;

	ULONG seed = KeQueryTimeIncrement( );

	for ( int n = 0; n <= length; n++ )
	{
		ULONG64 key
			= RtlRandomEx( &seed ) % sizeof( character_set ) - 2;

		buffer[n] = character_set[key];
	}

	return STATUS_SUCCESS;
}

ULONG random_long( ULONG max )
{
	ULONG seed = KeQueryTimeIncrement( );
	ULONG random = RtlRandomEx( &seed ) % max;

	// fix later

	if ( random == 0 ) random = 1;

	return random;
}
