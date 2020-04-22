#pragma once

#include <ntstatus.h>

#define NTSTRSAFE_LIB
#include <ntstrsafe.h>

#include <cstddef>
#include <cstdint>

#include "memory/virtual.h"

#include "process/process.h"

#include "kernel/module.h"
#include "kernel/pattern.h"
#include "kernel/cache_table.h"

#include "util/defines.h"
#include "util/version.h"

#include "titan/titanhook.h"

#include "ClearPIDcache.h"

#include "exz.h"

enum E_COMMAND_CODE
{
	ID_NULL						= 0,	//

	ID_READ_PROCESS_MEMORY		= 5,	// 
	ID_WRITE_PROCESS_MEMORY		= 6,	//

	ID_READ_KERNEL_MEMORY		= 8,	// 

	ID_GET_PROCESS				= 10,	//
	ID_GET_PROCESS_BASE			= 11,	//
	ID_GET_PROCESS_MODULE		= 12,	//

	ID_SET_PAGE_PROTECTION		= 26,  //

	ID_REMOVE_HOOK				= 99,	//

	ID_CONFIRM_DRIVER_LOADED	= 100,	//
};

ULONG Titan_Systemm_Call_ = 0x9000;

PVOID titan_g_kernel1 = nullptr;
PVOID titan_g_kernel2 = nullptr;
PVOID titan_g_kernel3 = nullptr;
void __fastcall titan_system_callback( _In_ unsigned int syscall, _Inout_ void** syscall_function );

NTSTATUS Install_Titan_Hook_( );
NTSTATUS Remove_Titan_Hook_( );

uint64_t Titan_Base_Control( uint32_t uCode, uint64_t uParam1, uint64_t uParam2, uint64_t uParam3 );
