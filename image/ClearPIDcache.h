#pragma once
#include <ntdef.h>
#include <ntifs.h>
#include <ntddk.h>
#include <windef.h>
#include <Ntstrsafe.h>




typedef struct PiDDBCacheEntry
{
	LIST_ENTRY		List;
	UNICODE_STRING	DriverName;
	ULONG			TimeDateStamp;
	NTSTATUS		LoadStatus;
	char			_0x0028[16]; // data from the shim engine, or uninitialized memory for custom drivers
}PIDCacheobj;

// BBscan section structs
#define IMAGE_NUMBEROF_DIRECTORY_ENTRIES        16


//extern "C" NTSYSAPI
//PIMAGE_NT_HEADERS
//NTAPI
//RtlImageNtHeader(PVOID Base);


PVOID ResolveRelativeAddress(_In_ PVOID Instruction, _In_ ULONG OffsetOffset, _In_ ULONG InstructionSize);
NTSTATUS PatternScan(IN PCUCHAR pattern, IN UCHAR wildcard, IN ULONG_PTR len, IN const VOID* base, IN ULONG_PTR size, OUT PVOID* ppFound);
PVOID GetKernelBase(OUT PULONG pSize);
NTSTATUS ScanSection(IN PCCHAR section, IN PCUCHAR pattern, IN UCHAR wildcard, IN ULONG_PTR len, OUT PVOID* ppFound);
BOOLEAN LocatePiDDB(PERESOURCE* lock, PRTL_AVL_TABLE* table);
BOOLEAN clear_titan_pidb();