#include <ntifs.h>

#include "..\kernel\pattern.h"
#include "..\util\random.h"

typedef struct _IDDB_CACHE_ENTRY
{
	LIST_ENTRY List;
	UNICODE_STRING DriverName;
	ULONG Timestamp;
	NTSTATUS LoadStatus;
	char _0x0028[16];
} IDDB_CACHE_ENTRY, *PIDDB_CACHE_ENTRY;

const ULONG TIMESTAMP_IQW64E = 0x5284EAC3;
const ULONG TIMESTAMP_CAPCOM = 0x57CD1415;

IDDB_CACHE_ENTRY* get_first_ddbcache_entry( );
NTSTATUS change_iddb_table_entry( ULONG timestamp );