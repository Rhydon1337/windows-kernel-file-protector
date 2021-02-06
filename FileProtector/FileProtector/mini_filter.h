#pragma once

#include <fltKernel.h>

extern PFLT_FILTER g_minifilter_handle;

extern  "C" FLT_PREOP_CALLBACK_STATUS FLTAPI pre_createfile(
	_Inout_ PFLT_CALLBACK_DATA Data,
	_In_ PCFLT_RELATED_OBJECTS FltObjects,
	_Flt_CompletionContext_Outptr_ PVOID* CompletionContext);

extern  "C" FLT_PREOP_CALLBACK_STATUS FLTAPI pre_writefile(
	_Inout_ PFLT_CALLBACK_DATA Data,
	_In_ PCFLT_RELATED_OBJECTS FltObjects,
	_Flt_CompletionContext_Outptr_ PVOID * CompletionContext);

extern  "C" FLT_PREOP_CALLBACK_STATUS pre_set_information(
	_Inout_ PFLT_CALLBACK_DATA Data, _In_ PCFLT_RELATED_OBJECTS FltObjects, PVOID*);

// Constant FLT_REGISTRATION structure for our filter.
// This initializes the callback routines our filter wants to register for.
CONST FLT_OPERATION_REGISTRATION g_callbacks[] = {
	{
		IRP_MJ_CREATE,
		0,
		pre_createfile,
		0
	},
	{
		IRP_MJ_WRITE,
		0,
		pre_writefile,
		0
	},
	{
		IRP_MJ_SET_INFORMATION,
		0,
		pre_set_information,
		0
	},
	{
		IRP_MJ_OPERATION_END
	}
};

extern  "C" NTSTATUS FLTAPI InstanceFilterUnloadCallback(_In_ FLT_FILTER_UNLOAD_FLAGS Flags);


extern  "C" NTSTATUS FLTAPI InstanceSetupCallback(
	_In_ PCFLT_RELATED_OBJECTS  FltObjects,
	_In_ FLT_INSTANCE_SETUP_FLAGS  Flags,
	_In_ DEVICE_TYPE  VolumeDeviceType,
	_In_ FLT_FILESYSTEM_TYPE  VolumeFilesystemType);

extern  "C" NTSTATUS FLTAPI InstanceQueryTeardownCallback(
	_In_ PCFLT_RELATED_OBJECTS FltObjects,
	_In_ FLT_INSTANCE_QUERY_TEARDOWN_FLAGS Flags
);

const FLT_REGISTRATION g_filter_registration = {
	sizeof(FLT_REGISTRATION), //  Size
	FLT_REGISTRATION_VERSION, //  Version
	0, //  Flags
	nullptr, //  Context registration
	g_callbacks, //  Operation callbacks
	InstanceFilterUnloadCallback, //  FilterUnload
	InstanceSetupCallback, //  InstanceSetup
	InstanceQueryTeardownCallback, //  InstanceQueryTeardown
	nullptr, //  InstanceTeardownStart
	nullptr, //  InstanceTeardownComplete
	nullptr, //  GenerateFileName
	nullptr, //  GenerateDestinationFileName
	nullptr //  NormalizeNameComponent
};
