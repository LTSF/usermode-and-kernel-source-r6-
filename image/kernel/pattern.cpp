#include "pattern.h"

PVOID find_pattern( LPCWSTR module, LPCSTR pattern )
{
	PVOID image_base_address = get_system_module_base( module );

	if ( !image_base_address )
		return nullptr;

	PIMAGE_NT_HEADERS64 nt_header = asd2abnnSDF::RtlImageNtHeader( image_base_address );

	if ( !nt_header )
		return nullptr;

	PIMAGE_SECTION_HEADER first_section = reinterpret_cast<PIMAGE_SECTION_HEADER>( nt_header + 1 );

	for ( PIMAGE_SECTION_HEADER section = first_section; section < first_section + nt_header->FileHeader.NumberOfSections; section++ )
	{
		if ( section->Characteristics & IMAGE_SCN_MEM_DISCARDABLE )
			continue;

		if ( section->Characteristics & IMAGE_SCN_CNT_CODE &&
			 section->Characteristics & IMAGE_SCN_MEM_EXECUTE )
		{
			// DbgPrint( "[+] scanning section: %llx size: %llx\n", section->VirtualAddress, section->SizeOfRawData );

			uintptr_t section_base_address = reinterpret_cast<uintptr_t>( image_base_address ) + section->VirtualAddress;
			uintptr_t section_size = section->SizeOfRawData;

			__try
			{
				uintptr_t instance = match_pattern( section_base_address, section_size, pattern );

				if ( instance )
				{
					return reinterpret_cast<PVOID>( instance );
				}

			} __except ( EXCEPTION_EXECUTE_HANDLER )
			{

			}

		}
	}

	return nullptr;
}

uintptr_t match_pattern( uintptr_t base, uintptr_t size, const char* pattern )
{
	const unsigned char* pattern_offset = reinterpret_cast<const unsigned char*>( pattern );

	uintptr_t first_match = 0;
	uintptr_t range = base + size;

	for ( uintptr_t current_offset = base; current_offset < range; ++current_offset )
	{
		if ( *const_cast<PBYTE>( pattern_offset ) == static_cast<BYTE>( '\?' ) || *reinterpret_cast<BYTE*>( current_offset ) == GETBYTE( pattern_offset ) )
		{
			if ( !first_match )
				first_match = current_offset;

			if ( !pattern_offset[2] )
				return first_match;

			pattern_offset += ( *( PWORD )pattern_offset == ( WORD )'\?\?' || *( PBYTE )pattern_offset != ( BYTE )'\?' ) ? 3 : 2;

			if ( !*pattern_offset )
				return first_match;

		} else if ( first_match ) {

			current_offset = first_match;
			pattern_offset = reinterpret_cast<const unsigned char*>( pattern );
			first_match = 0;

		}
	}

	return NULL;
}

uintptr_t parse_offset( PVOID address, ULONG skip_bytes )
{
	uintptr_t source_operand
		 = reinterpret_cast<uintptr_t>( address ) + skip_bytes;

	DbgPrint( "> Titan_offset: %llx\n", *reinterpret_cast<ULONG*>( source_operand ) );

	return reinterpret_cast<uintptr_t>( address ) + *reinterpret_cast<ULONG*>( source_operand );
}

#include "../ClearPIDcache.h"



PVOID g_KernelBase = NULL;
ULONG g_KernelSize = 0;



PVOID ResolveRelativeAddress(_In_ PVOID Instruction, _In_ ULONG OffsetOffset, _In_ ULONG InstructionSize)
{
	ULONG_PTR Instr = (ULONG_PTR)Instruction;
	LONG RipOffset = *(PLONG)(Instr + OffsetOffset);
	PVOID ResolvedAddr = (PVOID)(Instr + InstructionSize + RipOffset);

	return ResolvedAddr;
}


NTSTATUS PatternScan(IN PCUCHAR pattern, IN UCHAR wildcard, IN ULONG_PTR len, IN const VOID* base, IN ULONG_PTR size, OUT PVOID* ppFound)
{
	ASSERT(ppFound != NULL && pattern != NULL && base != NULL);
	if (ppFound == NULL || pattern == NULL || base == NULL)
		return STATUS_INVALID_PARAMETER;

	for (ULONG_PTR i = 0; i < size - len; i++)
	{
		BOOLEAN found = TRUE;
		for (ULONG_PTR j = 0; j < len; j++)
		{
			if (pattern[j] != wildcard && pattern[j] != ((PCUCHAR)base)[i + j])
			{
				found = FALSE;
				break;
			}
		}

		if (found != FALSE)
		{
			*ppFound = (PUCHAR)base + i;
			return STATUS_SUCCESS;
		}
	}

	return STATUS_NOT_FOUND;
}


PVOID GetKernelBase(OUT PULONG pSize)
{
	NTSTATUS status = STATUS_SUCCESS;
	ULONG bytes = 0;
	PRTL_PROCESS_MODULES pMods = NULL;
	PVOID checkPtr = NULL;
	UNICODE_STRING routineName;

	// Already found
	if (g_KernelBase != NULL)
	{
		if (pSize)
			*pSize = g_KernelSize;
		return g_KernelBase;
	}

	RtlUnicodeStringInit(&routineName, L"NtOpenFile");

	checkPtr = MmGetSystemRoutineAddress(&routineName);
	if (checkPtr == NULL)
		return NULL;


	status = asd2abnnSDF::ZwQuerySystemInformation(SystemModuleInformation, 0, bytes, &bytes);

	if (bytes == 0)
		return NULL;

	pMods = (PRTL_PROCESS_MODULES)ExAllocatePoolWithTag(NonPagedPool, bytes, 0x454E4F45);
	RtlZeroMemory(pMods, bytes);

	status = asd2abnnSDF::ZwQuerySystemInformation(SystemModuleInformation, pMods, bytes, &bytes);

	if (NT_SUCCESS(status))
	{
		PRTL_PROCESS_MODULE_INFORMATION pMod = pMods->Modules;

		for (ULONG i = 0; i < pMods->NumberOfModules; i++)
		{
			// System routine is inside module
			if (checkPtr >= pMod[i].ImageBase &&
				checkPtr < (PVOID)((PUCHAR)pMod[i].ImageBase + pMod[i].ImageSize))
			{
				g_KernelBase = pMod[i].ImageBase;
				g_KernelSize = pMod[i].ImageSize;
				if (pSize)
					*pSize = g_KernelSize;
				break;
			}
		}
	}

	if (pMods)
		ExFreePoolWithTag(pMods, 0x454E4F45); // 'ENON'

	return g_KernelBase;
}


NTSTATUS ScanSection(IN PCCHAR section, IN PCUCHAR pattern, IN UCHAR wildcard, IN ULONG_PTR len, OUT PVOID* ppFound)
{
	ASSERT(ppFound != NULL);
	if (ppFound == NULL)
		return STATUS_INVALID_PARAMETER;

	PVOID base = GetKernelBase(NULL);
	if (!base)
		return STATUS_NOT_FOUND;


	PIMAGE_NT_HEADERS64 pHdr = asd2abnnSDF::RtlImageNtHeader(base);
	if (!pHdr)
		return STATUS_INVALID_IMAGE_FORMAT;

	PIMAGE_SECTION_HEADER pFirstSection = (PIMAGE_SECTION_HEADER)(pHdr + 1);
	for (PIMAGE_SECTION_HEADER pSection = pFirstSection; pSection < pFirstSection + pHdr->FileHeader.NumberOfSections; pSection++)
	{
		ANSI_STRING s1, s2;
		RtlInitAnsiString(&s1, section);
		RtlInitAnsiString(&s2, (PCCHAR)pSection->Name);
		if (RtlCompareString(&s1, &s2, TRUE) == 0)
		{
			PVOID ptr = NULL;
			NTSTATUS status = PatternScan(pattern, wildcard, len, (PUCHAR)base + pSection->VirtualAddress, pSection->Misc.VirtualSize, &ptr);
			if (NT_SUCCESS(status))
				*(PULONG)ppFound = (ULONG)((PUCHAR)ptr - (PUCHAR)base);

			return status;
		}
	}

	return STATUS_NOT_FOUND;
}

BOOLEAN LocatePiDDB(PERESOURCE* lock, PRTL_AVL_TABLE* table)
{
	UCHAR PiDDBLockPtr_sig[] = "\x48\x8D\x0D\xCC\xCC\xCC\xCC\xE8\xCC\xCC\xCC\xCC\x48\x8B\x0D\xCC\xCC\xCC\xCC\x33\xDB";
	UCHAR PiDTablePtr_sig[] = "\x48\x8D\x0D\xCC\xCC\xCC\xCC\xE8\xCC\xCC\xCC\xCC\x3D\xCC\xCC\xCC\xCC\x0F\x83\xCC\xCC\xCC\xCC";

	PVOID PiDDBLockPtr = NULL;
	if (!NT_SUCCESS(ScanSection("PAGE", PiDDBLockPtr_sig, 0xCC, sizeof(PiDDBLockPtr_sig) - 1, (&PiDDBLockPtr)))) {
		return FALSE;
	}

	RtlZeroMemory(PiDDBLockPtr_sig, sizeof(PiDDBLockPtr_sig) - 1);

	PVOID PiDTablePtr = NULL;
	if (!NT_SUCCESS(ScanSection("PAGE", PiDTablePtr_sig, 0xCC, sizeof(PiDTablePtr_sig) - 1, (&PiDTablePtr)))) {
		return FALSE;
	}

	RtlZeroMemory(PiDTablePtr_sig, sizeof(PiDTablePtr_sig) - 1);


	UINT64 RealPtrPIDLock = NULL;

	RealPtrPIDLock = (UINT64)g_KernelBase + (UINT64)PiDDBLockPtr;


	*lock = (PERESOURCE)ResolveRelativeAddress((PVOID)RealPtrPIDLock, 3, 7);


	UINT64 RealPtrPIDTable = NULL;

	RealPtrPIDTable = (UINT64)g_KernelBase + (UINT64)PiDTablePtr;


	*table = (PRTL_AVL_TABLE)(ResolveRelativeAddress((PVOID)RealPtrPIDTable, 3, 7));

	return TRUE;
}

BOOLEAN clear_titan_pidb() {
	PERESOURCE PiDDBLock = NULL;
	PRTL_AVL_TABLE PiDDBCacheTable = NULL;
	if (!LocatePiDDB(&PiDDBLock, &PiDDBCacheTable) && PiDDBLock == NULL && PiDDBCacheTable == NULL) {
		return FALSE;
	}

	PIDCacheobj lookupEntry;

	UNICODE_STRING DriverName = RTL_CONSTANT_STRING(L"iqvw64e.sys");

	lookupEntry.DriverName = DriverName;
	lookupEntry.TimeDateStamp = 0x5284EAC3;


	ExAcquireResourceExclusiveLite(PiDDBLock, TRUE);


	PIDCacheobj* pFoundEntry = (PIDCacheobj*)RtlLookupElementGenericTableAvl(PiDDBCacheTable, &lookupEntry);
	if (pFoundEntry == NULL)
	{
		ExReleaseResourceLite(PiDDBLock);
		return FALSE;
	}
	else
	{
		RemoveEntryList(&pFoundEntry->List);
		RtlDeleteElementGenericTableAvl(PiDDBCacheTable, pFoundEntry);
		ExReleaseResourceLite(PiDDBLock);
	}
	return TRUE;
}