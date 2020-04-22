#ifndef DEFINES_H
#define DEFINES_H

#include <ntifs.h>

#ifdef _DEBUG
#define DebugPrint( X, ... ) DbgPrintEx( DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, X, __VA_ARGS__ )
#else
#define DebugPrint( X, ... ) DbgPrintEx( DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, X, __VA_ARGS__ )
#endif

typedef struct _LDR_DATA_TABLE_ENTRY
{
	LIST_ENTRY InLoadOrderLinks;
	LIST_ENTRY InMemoryOrderLinks;
	LIST_ENTRY InInitializationOrderLinks;
	PVOID DllBase;
	PVOID EntryPoint;
	ULONG SizeOfImage;
	UNICODE_STRING FullDllName;
	UNICODE_STRING BaseDllName;
	// ...
} LDR_DATA_TABLE_ENTRY, *PLDR_DATA_TABLE_ENTRY;

typedef struct _KLDR_DATA_TABLE_ENTRY
{
	LIST_ENTRY InLoadOrderLinks;
	PVOID ExceptionTable;
	ULONG ExceptionTableSize;
	PVOID GpValue;
	PVOID NonPagedDebugInfo;
	PVOID DllBase;
	PVOID EntryPoint;
	ULONG SizeOfImage;
	UNICODE_STRING FullDllName;
	UNICODE_STRING BaseDllName;
	ULONG Flags;
	// ...
} KLDR_DATA_TABLE_ENTRY, *PKLDR_DATA_TABLE_ENTRY;

typedef struct _PEB_LDR_DATA
{
	unsigned int Length;
	int Initialized;
	void* SSHandle;
	LIST_ENTRY InLoadOrderLinks;
	LIST_ENTRY InMemoryOrderLinks;
	LIST_ENTRY InInitializationOrderLinks;
	// ...
} PEB_LDR_DATA, *PPEB_LDR_DATA;

typedef struct _COMPLETION_ROUTINE_CONTEXT
{
	PVOID Buffer;
	ULONG BufferLength;
	PVOID OriginalContext;
	PIO_COMPLETION_ROUTINE OriginalRoutine;
} COMPLETION_ROUTINE_CONTEXT, *PCOMPLETION_ROUTINE_CONTEXT;

typedef struct _PEB64
{
	unsigned char InheritedAddressSpace;	// 0x0000 
	unsigned char ReadImageFileExecOptions;	// 0x0001 
	unsigned char BeingDebugged;			// 0x0002 
	unsigned char BitField;					// 0x0003 
	unsigned char pad_0x0004[0x4];			// 0x0004
	PVOID Mutant;							// 0x0008 
	PVOID ImageBaseAddress;					// 0x0010 
	PPEB_LDR_DATA Ldr;						// 0x0018
	// ...
} PEB64, *PPEB64;

typedef struct _RTL_PROCESS_MODULE_INFORMATION
{
	HANDLE Section;
	PVOID MappedBase;
	PVOID ImageBase;
	ULONG ImageSize;
	ULONG Flags;
	USHORT LoadOrderIndex;
	USHORT InitOrderIndex;
	USHORT LoadCount;
	USHORT OffsetToFileName;
	UCHAR  FullPathName[MAXIMUM_FILENAME_LENGTH];
} RTL_PROCESS_MODULE_INFORMATION, *PRTL_PROCESS_MODULE_INFORMATION;

typedef struct _RTL_PROCESS_MODULES
{
	ULONG NumberOfModules;
	RTL_PROCESS_MODULE_INFORMATION Modules[1];
} RTL_PROCESS_MODULES, *PRTL_PROCESS_MODULES;

typedef enum _SYSTEM_INFORMATION_CLASS
{
	SystemModuleInformation = 0xb,
} SYSTEM_INFORMATION_CLASS;

extern "C" __declspec( dllimport ) POBJECT_TYPE* IoDriverObjectType;
extern "C" __declspec( dllimport ) PLIST_ENTRY	 PsLoadedModuleList;

extern "C" __declspec(dllimport)
NTSTATUS NTAPI ZwProtectVirtualMemory(
	HANDLE ProcessHandle,
	PVOID * BaseAddress,
	PULONG ProtectSize,
	ULONG NewProtect,
	PULONG OldProtect
	);

// extern NTKERNELAPI PLIST_ENTRY PsLoadedModuleList;

typedef struct _AUX_ACCESS_DATA
{
	PPRIVILEGE_SET PrivilegesUsed;
	GENERIC_MAPPING GenericMapping;
	ACCESS_MASK AccessesToAudit;
	ACCESS_MASK MaximumAuditMask;
	ULONG Unknown[256];
} AUX_ACCESS_DATA, * PAUX_ACCESS_DATA;

namespace asd2abnnSDF
{
	extern "C" {
		NTSTATUS NTAPI ObReferenceObjectByName(PUNICODE_STRING ObjectName, ULONG Attributes, PACCESS_STATE AccessState, ACCESS_MASK DesiredAccess,
			POBJECT_TYPE ObjectType, KPROCESSOR_MODE AccessMode, PVOID ParseContext OPTIONAL, PVOID* Object);

		NTSTATUS NTAPI MmCopyVirtualMemory(PEPROCESS FromProcess, PVOID FromAddress, PEPROCESS ToProcess, PVOID ToAddress, SIZE_T BufferSize,
			KPROCESSOR_MODE PreviousMode, PSIZE_T NumberOfBytesCopied);

		char* PsGetProcessImageFileName(PEPROCESS Process);

		PVOID NTAPI PsGetProcessSectionBaseAddress(PEPROCESS Process);

		//PIMAGE_NT_HEADERS NTAPI RtlImageNtHeader(PVOID Base);

		NTSYSAPI PIMAGE_NT_HEADERS NTAPI RtlImageNtHeader(PVOID Base);

		PVOID NTAPI RtlFindExportedRoutineByName(PVOID ImageBase, PCCH RoutineName);

		ULONG RtlRandomEx(PULONG Seed);

		PPEB NTAPI PsGetProcessPeb(PEPROCESS Process);

		PEPROCESS NTAPI PsGetCurrentProcessByThread(PETHREAD Thread);

		PEPROCESS NTAPI PsSuspendProcess(PEPROCESS Process);

		NTSTATUS NTAPI ZwQuerySystemInformation(SYSTEM_INFORMATION_CLASS SystemInformationClass, PVOID SystemInformation, ULONG SystemInformationLength, PULONG ReturnLength);

		NTSTATUS NTAPI ZwProtectVirtualMemory(HANDLE ProcessHandle, PVOID* BaseAddress, PSIZE_T ProtectSize, ULONG NewProtect, PULONG OldProtect);

		NTSTATUS NTAPI SeCreateAccessState(PACCESS_STATE AccessState, PAUX_ACCESS_DATA AuxData, ACCESS_MASK DesiredAccess, PGENERIC_MAPPING GenericMapping);

		VOID NTAPI SeDeleteAccessState(PACCESS_STATE AccessState);
	}
}

#endif