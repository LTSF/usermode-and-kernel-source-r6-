#pragma once

#include <ntifs.h>

static char character_set[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";

UCHAR random_character( );
NTSTATUS random_ansi_string( char* buffer, size_t length );
NTSTATUS random_wide_string( wchar_t* buffer, size_t length );
ULONG random_long( ULONG max = 0x1000 );