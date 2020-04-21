
uintptr_t cache;
bool bad_ptr(uintptr_t ptr)
{
	static auto minimum_application_address = uintptr_t(0x10000);
	static auto maximum_application_address = uintptr_t(0xFFE00000);
	return ((ptr >= minimum_application_address || ptr < maximum_application_address) || !ptr);
}

enum E_COMMAND_CODE
{
	ID_NULL = 0,	//

	ID_READ_PROCESS_MEMORY = 5,	// 
	ID_WRITE_PROCESS_MEMORY = 6,	//

	ID_READ_KERNEL_MEMORY = 8,	// 

	ID_GET_PROCESS = 10,	//
	ID_GET_PROCESS_BASE = 11,	//
	ID_GET_PROCESS_MODULE = 12,	//

	ID_SET_PAGE_PROTECTION = 26,  //

	ID_REMOVE_HOOK = 99,	//

	ID_CONFIRM_DRIVER_LOADED = 100,	//
};

#pragma pack( push, 8 )
typedef struct _MEMORY_STRUCT
{
	UINT_PTR	process_id;
	PVOID		address;
	SIZE_T		size;
	SIZE_T		size_copied;
	PVOID		buffer;
	BOOLEAN AllocateMemory;
	ULONG		protection;
} MEMORY_STRUCT, * PMEMORY_STRUCT;
#pragma pack( pop )

#pragma pack( push, 8 )
typedef struct _MEMORY_STRUCT_PROTECTION
{
	UINT_PTR	process_id;
	PVOID		address;
	SIZE_T		size;
	ULONG		protection;
	ULONG		protection_old;
} MEMORY_STRUCT_PROTECTION, * PMEMORY_STRUCT_PROTECTION;
#pragma pack( pop )


template<typename ... A>
uint64_t call_driver_control(void* control_function, const A ... arguments)
{
	if (!control_function)
		return 0;

	using tFunction = uint64_t(__stdcall*)(A...);
	const auto control = static_cast<tFunction>(control_function);

	return control(arguments ...);
}

void* kernel_control_function()
{
	HMODULE hModule = LoadLibrary(XorString("win32u.dll"));

	if (!hModule)
		return nullptr;

	return reinterpret_cast<void*>(GetProcAddress(hModule, XorString("NtUserGetPrecisionTouchPadConfiguration")));
}

uint64_t read_kernel(void* control_function, uint64_t address, void* buffer, std::size_t size)
{
	return call_driver_control(control_function, ID_READ_KERNEL_MEMORY, address, buffer, size);
}

DWORD GetProcessID(LPCSTR lpExeName)
{
	HANDLE hSnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

	if (hSnapShot == INVALID_HANDLE_VALUE)
		return NULL;

	PROCESSENTRY32 pe = { 0 };
	pe.dwSize = sizeof(PROCESSENTRY32);

	for (BOOL    success = Process32First(hSnapShot, &pe);
		success == TRUE;
		success = Process32Next(hSnapShot, &pe))
	{
		if (strcmp(lpExeName, pe.szExeFile) == 0)
		{
			CloseHandle(hSnapShot);
			return pe.th32ProcessID;
		}
	}

	CloseHandle(hSnapShot);
	return NULL;
}

void killProcessByName(const char* filename)
{
	HANDLE hSnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPALL, NULL);
	PROCESSENTRY32 pEntry;
	pEntry.dwSize = sizeof(pEntry);
	BOOL hRes = Process32First(hSnapShot, &pEntry);
	while (hRes)
	{
		if (strcmp(pEntry.szExeFile, filename) == 0)
		{
			HANDLE hProcess = OpenProcess(PROCESS_TERMINATE, 0,
				(DWORD)pEntry.th32ProcessID);
			if (hProcess != NULL)
			{
				TerminateProcess(hProcess, 9);
				CloseHandle(hProcess);
			}
		}
		hRes = Process32Next(hSnapShot, &pEntry);
	}
	CloseHandle(hSnapShot);
}

void* m_driver_control;
DWORD64 PID;
uintptr_t baseAddress;

int Width = GetSystemMetrics(SM_CXSCREEN);
int Height = GetSystemMetrics(SM_CYSCREEN);

NTSTATUS change_protection(void* ctrl, uint64_t pid, uint64_t address, uint32_t page_protection, std::size_t size)
{
	MEMORY_STRUCT_PROTECTION protection = { 0 };
	protection.process_id = pid;
	protection.address = reinterpret_cast<void*>(address);
	protection.size = size;
	protection.protection = page_protection;

	return (NTSTATUS)(call_driver_control(ctrl, ID_SET_PAGE_PROTECTION, &protection));
}

NTSTATUS change_protection2(void* ctrl, uint64_t pid, PVOID address, uint32_t page_protection, std::size_t size)
{
	MEMORY_STRUCT_PROTECTION protection = { 0 };
	protection.process_id = pid;
	protection.address = reinterpret_cast<void*>(address);
	protection.size = size;
	protection.protection = page_protection;

	return (NTSTATUS)(call_driver_control(ctrl, ID_SET_PAGE_PROTECTION, &protection));
}

template<typename T>
__declspec(noinline) T read(uint64_t address)
{
	T buffer{};

	if (!PID)
		return buffer;

	MEMORY_STRUCT memory_struct = { 0 };
	memory_struct.process_id = PID;
	memory_struct.address = reinterpret_cast<void*>(address);
	memory_struct.size = sizeof(T);
	memory_struct.buffer = &buffer;

	NTSTATUS result
		= (NTSTATUS)(call_driver_control(m_driver_control, ID_READ_PROCESS_MEMORY, &memory_struct));

	if (result != 0)
		return buffer;

	return buffer;
}

template<typename T>
__declspec(noinline) bool write(uint64_t address, T buffer)
{
	MEMORY_STRUCT memory_struct = { 0 };
	memory_struct.process_id = PID;
	memory_struct.address = reinterpret_cast<void*>(address);
	memory_struct.size = sizeof(T);
	memory_struct.buffer = &buffer;

	NTSTATUS result
		= (NTSTATUS)(call_driver_control(m_driver_control, ID_WRITE_PROCESS_MEMORY, &memory_struct));

	if (result != 0)
		return false;

	return true;
}


bool WriteVirtual(uint64_t address, void* buffer, size_t size)
{
	MEMORY_STRUCT memory_struct = { 0 };
	memory_struct.process_id = PID;
	memory_struct.address = reinterpret_cast<void*>(address);
	memory_struct.size = size;
	memory_struct.buffer = buffer;

	uint64_t result = call_driver_control(m_driver_control, ID_WRITE_PROCESS_MEMORY, &memory_struct);

	if (result != 0)
		return false;

	return true;
}

bool WriteVirtual2(PVOID address, void* buffer, size_t size)
{
	MEMORY_STRUCT memory_struct = { 0 };
	memory_struct.process_id = PID;
	memory_struct.address = reinterpret_cast<void*>(address);
	memory_struct.size = size;
	memory_struct.buffer = buffer;

	uint64_t result = call_driver_control(m_driver_control, ID_WRITE_PROCESS_MEMORY, &memory_struct);

	if (result != 0)
		return false;

	return true;
}

template<typename T>

std::string read_string(uint64_t address)
{
	char buffer[100];

	if (!PID)
		return buffer;

	MEMORY_STRUCT memory_struct = { 0 };
	memory_struct.process_id = PID;
	memory_struct.address = reinterpret_cast<void*>(address);
	memory_struct.size = sizeof(T);
	memory_struct.buffer = &buffer;

	NTSTATUS result
		= (NTSTATUS)(call_driver_control(m_driver_control, ID_READ_PROCESS_MEMORY, &memory_struct));

	if (result != 0)
		return "";

	std::string nameString;
	for (int i = 0; i < 100; i++) {
		if (buffer[i] == 0)
			break;
		else
			nameString += buffer[i];
	};

	return nameString;
}

static UINT_PTR AllocateMemory(SIZE_T size, ULONG protection)
{
	MEMORY_STRUCT memory_struct = { 0 };
	memory_struct.process_id = PID;
	memory_struct.AllocateMemory = TRUE;
	memory_struct.address = 0;
	memory_struct.size = size;
	memory_struct.protection = protection;

	uint64_t result = call_driver_control(m_driver_control, ID_WRITE_PROCESS_MEMORY, &memory_struct);

	if (result != 0)
		return false;

	return true;
}

bool readto(uint64_t Address, void* Buffer, size_t Size)
{
	if (!PID)
		return false;

	MEMORY_STRUCT memory_struct = { 0 };
	memory_struct.process_id = PID;
	memory_struct.address = reinterpret_cast<void*>(Address);
	memory_struct.size = Size;
	memory_struct.buffer = Buffer;

	NTSTATUS result
		= (NTSTATUS)(call_driver_control(m_driver_control, ID_READ_PROCESS_MEMORY, &memory_struct));

	if (result != 0)
		return false;

	return true;
}

template <typename T = uint64_t>
T read_vmem(uint64_t address)
{
	T buffer{};
	readto(address, &buffer, sizeof(T));
	return buffer;
}
