#include "driver.h"

void __fastcall titan_system_callback(_In_ unsigned int titan_syscall, _Inout_ void** syscall_function)
{
	EXP_2_MAGIC_USSS( );
	UNREFERENCED_PARAMETER(titan_syscall);
	if (titan_syscall == Titan_Systemm_Call_)
		*syscall_function = Titan_Base_Control;
}

NTSTATUS Install_Titan_Hook_( )
{
	ULONG Titan_W_Version = get_titan_system_windows_versionn( );

	DebugPrint("[-] Titan_W_Version : %lu\n", Titan_W_Version);

	EXP_2_MAGIC_USSS( );
	switch ( Titan_W_Version )
	{
	case 1803:
		Titan_Systemm_Call_ = 0x13f1;
		break;
	case 1809:
		Titan_Systemm_Call_ = 0x1401;
		break;
	case 1903:
		Titan_Systemm_Call_ = 0x140b;
		break;
	case 1909:
		Titan_Systemm_Call_ = 0x140b;
		break;
	default:
		DebugPrint("[!] Titan_Not supported kernel version\n");
		return STATUS_NOT_FOUND;
	}
	EXP_2_MAGIC_USSS( );
	DebugPrint("[+] TitanTrack: %lu\n", Titan_Systemm_Call_);

	NTSTATUS titan_status = InitTitan_EX_Hook(titan_system_callback);

	if (!NT_SUCCESS(titan_status))
	{
		DebugPrint("[-] ezxexzzhook: Failed to initialize with status: 0x%lx\n", titan_status);
	}
	EXP_2_MAGIC_USSS( );
	DebugPrint("[+] waqaxhook installed\n");

	return titan_status;
}

NTSTATUS Remove_Titan_Hook_( )
{
	EXP_2_MAGIC_USSS( );
	xTitanx_ex_Release( );
	DebugPrint("[-] Titan_Removing hook\n");

	return STATUS_SUCCESS;
}

uint64_t Titan_Base_Control(uint32_t Titan_u_Code, uint64_t titan_param_1, uint64_t titan_param_2, uint64_t titan_param_3)
{
	EXP_2_MAGIC_USSS( );
	UNREFERENCED_PARAMETER(Titan_u_Code);
	UNREFERENCED_PARAMETER(titan_param_1);
	UNREFERENCED_PARAMETER(titan_param_2);
	UNREFERENCED_PARAMETER(titan_param_3);

	DebugPrint("[-] Titan_X_Controller code: %lx\n", Titan_u_Code);

	EXP_2_MAGIC_USSS( );
	switch (Titan_u_Code)
	{
	case ID_READ_PROCESS_MEMORY:
		return read_virtual_memory(
			reinterpret_cast<PMEMORY_STRUCT>(titan_param_1));

	case ID_WRITE_PROCESS_MEMORY:
		return write_virtual_memory(
			reinterpret_cast<PMEMORY_STRUCT>(titan_param_1));

	case ID_READ_KERNEL_MEMORY:
		return read_kernel_memory(
			reinterpret_cast<PVOID>(titan_param_1),
			reinterpret_cast<PVOID>(titan_param_2),
			titan_param_3);

	case ID_SET_PAGE_PROTECTION:
		return protect_virtual_memory(reinterpret_cast<PMEMORY_STRUCT_PROTECTION>(titan_param_1));

	case ID_GET_PROCESS:
		return reinterpret_cast<uint64_t>(get_process(
			reinterpret_cast<HANDLE>(titan_param_1)));

	case ID_GET_PROCESS_BASE:
		return reinterpret_cast<uint64_t>(get_process_base(
			reinterpret_cast<HANDLE>(titan_param_1)));

	case ID_GET_PROCESS_MODULE:
		return reinterpret_cast<uint64_t>(
			get_process_module_base(
				reinterpret_cast<HANDLE>(titan_param_1),
				reinterpret_cast<LPCWSTR>(titan_param_2)
			));

	case ID_CONFIRM_DRIVER_LOADED:
		return STATUS_SUCCESS;

	case ID_REMOVE_HOOK:
		return Remove_Titan_Hook_( );

	default:
		break;
	}

	return static_cast<uint64_t>(STATUS_INVALID_PARAMETER);
}


NTSTATUS DriverEntry(struct _DRIVER_OBJECT* Titan_Object, PUNICODE_STRING Titan_Registry_Patch)
{
	UNREFERENCED_PARAMETER(Titan_Object);
	UNREFERENCED_PARAMETER(Titan_Registry_Patch);

	EXP_2_MAGIC_USSS( );
	change_iddb_table_entry(TIMESTAMP_IQW64E);
	clear_titan_pidb( );

	return Remove_Titan_Hook_( );
}
