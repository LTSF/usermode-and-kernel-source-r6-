#pragma once

typedef void(__fastcall* titan_Call_Ex_Back)(_In_ unsigned int SystemCallIndex, _Inout_ void** SystemCallFunction);

NTSTATUS InitTitan_EX_Hook(
	_In_ titan_Call_Ex_Back titanB_Call);

void xTitanx_ex_Release();