# usermode-and-kernel-source-r6-
rainbow six usermode and kernel 


Hey, guys, this is a special/forced release this source was used by someone I did not allow/give permission to. so, therefore, I am releasing it as a punishment
 
I recommend using this to learn and not paste and sell or else you will be gassed pretty quickly I removed a few private features like outline and some other things but everything is working as of the update pushed today.
(code is very messy) 

There are a few things I recommend doing if you plan on using this "long term"
I will first post a list of the known detection vectors

Using ImGui ( not that big of a problem ) 
Using CreateThreads
Hooking/Rendering method will probably get you gassed
Directly changes the present pointer in the swapchain.
Doesn't hide pages in away.
Doesn't Encrypt strings (xorstrg)
Maps with KDmapper


Credits:
KDmapper: z175
Driver Help: 464627
Pattern Scanner: Paracord

Driver Calls:

Entry
[CODE]NTSTATUS DriverEntry(struct _DRIVER_OBJECT* Titan_Object, PUNICODE_STRING Titan_Registry_Patch)
{
	UNREFERENCED_PARAMETER(Titan_Object);
	UNREFERENCED_PARAMETER(Titan_Registry_Patch);

	EXP_2_MAGIC_USSS( );
	change_iddb_table_entry(TIMESTAMP_IQW64E);
	clear_titan_pidb( );

	return Remove_Titan_Hook_( );
}
[/CODE]

Read
[CODE]NTSTATUS read_kernel_memory( PVOID address, PVOID buffer, SIZE_T size )
{
	DebugPrint( "[-] MmCopyMemory address=%p buffer=%p size=%lu\n", address, buffer, size );

	PVOID non_paged_buffer = ExAllocatePool( NonPagedPool, size );

	if ( !non_paged_buffer )
	{
		return STATUS_MEMORY_NOT_ALLOCATED;
	}

	SIZE_T size_copied = 0;

	MM_COPY_ADDRESS mm_address = { 0 };
	mm_address.VirtualAddress = address;

	NTSTATUS status =
		MmCopyMemory( non_paged_buffer, mm_address, size, MM_COPY_MEMORY_VIRTUAL, &size_copied );

	DebugPrint( "[-] MmCopyMemory status = %lx\n", status );

	if ( NT_SUCCESS( status ) )
	{
		RtlCopyMemory( buffer, non_paged_buffer, size_copied );
	}

	ExFreePool( non_paged_buffer );

	return status;
}[/CODE]

Write
[CODE]NTSTATUS write_kernel_memory( PVOID address, PVOID buffer, ULONG size )
{
	// PVOID aligned_to_page = ( address & ~( 4096 - 1 ) );

	PMDL mdl = IoAllocateMdl( address, size, FALSE, FALSE, NULL );

	if ( !mdl )
	{
		return STATUS_UNSUCCESSFUL;
	}

	MmProbeAndLockPages( mdl, KernelMode, IoReadAccess );

	PVOID mapped_page = MmMapLockedPagesSpecifyCache( mdl, KernelMode, MmNonCached, NULL, FALSE, NormalPagePriority );

	if ( !mapped_page )
	{
		MmUnlockPages( mdl );
		IoFreeMdl( mdl );
		return STATUS_UNSUCCESSFUL;
	}

	NTSTATUS status = MmProtectMdlSystemAddress( mdl, PAGE_READWRITE );

	if ( !NT_SUCCESS( status ) )
	{
		MmUnmapLockedPages( mapped_page, mdl );
		MmUnlockPages( mdl );
		IoFreeMdl( mdl );
		return STATUS_UNSUCCESSFUL;
	}

	status = MmProtectMdlSystemAddress( mdl, PAGE_READWRITE );

	RtlCopyMemory( mapped_page, buffer, size );

	MmUnmapLockedPages( mapped_page, mdl );
	MmUnlockPages( mdl );
	IoFreeMdl( mdl );

	return STATUS_SUCCESS;
}[/CODE]

Also thanks for the whole UC thread in anti-cheats bypassing <3 


Features
Visuals -
     Max Distance Config
     Show Distance
     Show Teammates
     Player Names
     HP Bars
     Vertical HP Bars
     Player Boxes
     Player Head Dot
     2D Box
     2D Filled
     Cav ESP
    snaplines
Aimbot -
      FOV
      Aimbot Distance
      Aimbot Smooth
      Aim Bone
      AimKey
      Recoil Modifier 
      Spread Modifier
      anti-switch
      FOV circle
      Target Selector
           Friendly
           Enemies
           Drones
      Hit Selector
           Head
           Chest
           Stomach
 Misc -
      Glow
      Speed
      Freeze Lobby
      Crosshair
      FOV Player
      FOV Weapon
      unlock all
      Config Loader/Saver 

Simple ImGui Menu
Using Sigs

Instructions: open cmd and map the driver, open the game, launch the user-mode


