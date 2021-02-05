#include "mini_filter.h"

extern "C" NTSTATUS DriverEntry(PDRIVER_OBJECT DriverObject, PUNICODE_STRING RegistryPath) {
	UNREFERENCED_PARAMETER(RegistryPath);

	DbgPrint("Hello from driver entry\n");

	auto nt_status = FltRegisterFilter(DriverObject, &g_filter_registration, &g_minifilter_handle);
	if (!NT_SUCCESS(nt_status)) {
		return nt_status;
	}
	
	nt_status = FltStartFiltering(g_minifilter_handle);
	if (!NT_SUCCESS(nt_status))
	{
		FltUnregisterFilter(g_minifilter_handle);
	}

	return nt_status;
}