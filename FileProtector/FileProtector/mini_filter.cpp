#include "mini_filter.h"

PFLT_FILTER g_minifilter_handle = nullptr;

bool should_protect(const wchar_t* file_name) {
	if (nullptr != wcsstr(file_name,L"_protected")) {
		return true;
	}
	if (nullptr != wcsstr(file_name, L"_PROT~1")) {
		return true;
	}
	return false;
}

FLT_PREOP_CALLBACK_STATUS pre_createfile(PFLT_CALLBACK_DATA Data, PCFLT_RELATED_OBJECTS,
	PVOID*) {
	
	if (Data->Iopb->Parameters.Create.Options & FILE_DELETE_ON_CLOSE) {
		KdPrint(("Delete operation: %wZ\n", &Data->Iopb->TargetFileObject->FileName));
		if (should_protect(Data->Iopb->TargetFileObject->FileName.Buffer)) {
			Data->IoStatus.Status = STATUS_ACCESS_DENIED;
			return FLT_PREOP_COMPLETE;
		}
	}
	return FLT_PREOP_SUCCESS_NO_CALLBACK;
}

FLT_PREOP_CALLBACK_STATUS pre_set_information(
	_Inout_ PFLT_CALLBACK_DATA Data, _In_ PCFLT_RELATED_OBJECTS, PVOID*) {
	
	auto params = Data->Iopb->Parameters.SetFileInformation;
	
	if (params.FileInformationClass != FileDispositionInformation &&
		params.FileInformationClass != FileDispositionInformationEx) {
		if (params.FileInformationClass == FileRenameInformation) {
			KdPrint(("Rename operation: %wZ\n", &Data->Iopb->TargetFileObject->FileName));
			if (should_protect(Data->Iopb->TargetFileObject->FileName.Buffer)) {
				Data->IoStatus.Status = STATUS_ACCESS_DENIED;
				return FLT_PREOP_COMPLETE;
			}
		}
		return FLT_PREOP_SUCCESS_NO_CALLBACK;
	}

	auto info = static_cast<FILE_DISPOSITION_INFORMATION*>(params.InfoBuffer);
	if (!info->DeleteFile) {
		return FLT_PREOP_SUCCESS_NO_CALLBACK;
	}
	// If we got till here it means that it delete operation
	KdPrint(("Delete operation: %wZ\n", &Data->Iopb->TargetFileObject->FileName));
	if (should_protect(Data->Iopb->TargetFileObject->FileName.Buffer)) {
		Data->IoStatus.Status = STATUS_ACCESS_DENIED;
		return FLT_PREOP_COMPLETE;
	}

	return FLT_PREOP_SUCCESS_NO_CALLBACK;
}


NTSTATUS InstanceFilterUnloadCallback(FLT_FILTER_UNLOAD_FLAGS Flags) {
	UNREFERENCED_PARAMETER(Flags);

	if (nullptr != g_minifilter_handle) {
        FltUnregisterFilter(g_minifilter_handle);
    }
    return STATUS_SUCCESS;
}

NTSTATUS InstanceSetupCallback(PCFLT_RELATED_OBJECTS FltObjects, FLT_INSTANCE_SETUP_FLAGS Flags, ULONG VolumeDeviceType,
	FLT_FILESYSTEM_TYPE VolumeFilesystemType) {
	UNREFERENCED_PARAMETER(FltObjects);
	UNREFERENCED_PARAMETER(Flags);
	UNREFERENCED_PARAMETER(VolumeDeviceType);
	UNREFERENCED_PARAMETER(VolumeFilesystemType);
	
	return STATUS_SUCCESS;
}

NTSTATUS InstanceQueryTeardownCallback(PCFLT_RELATED_OBJECTS FltObjects, FLT_INSTANCE_QUERY_TEARDOWN_FLAGS Flags) {
	UNREFERENCED_PARAMETER(FltObjects);
	UNREFERENCED_PARAMETER(Flags);
	
	return STATUS_SUCCESS;
}
