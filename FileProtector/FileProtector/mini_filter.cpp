#include "mini_filter.h"

#include "FileNameInfo.h"

PFLT_FILTER g_minifilter_handle = nullptr;

bool should_protect(const wchar_t* file_name) {
	if (nullptr == file_name) {
		return false;
	}
	if (nullptr != wcsstr(file_name,L"_protected")) {
		return true;
	}
	return false;
}

FLT_PREOP_CALLBACK_STATUS pre_createfile(PFLT_CALLBACK_DATA Data, PCFLT_RELATED_OBJECTS,
	PVOID*) {

	if (Data->Iopb->Parameters.Create.Options & FILE_DELETE_ON_CLOSE) {
		FileNameInfo file_name(Data);
		if (nullptr == file_name.get()) {
			return FLT_PREOP_SUCCESS_NO_CALLBACK;
		}
		KdPrint(("Delete operation: %wZ\n", file_name.get()));
		if (should_protect(file_name.get()->Buffer)) {
			Data->IoStatus.Status = STATUS_ACCESS_DENIED;
			return FLT_PREOP_COMPLETE;
		}
	}
	
	return FLT_PREOP_SUCCESS_NO_CALLBACK;
}

FLT_PREOP_CALLBACK_STATUS pre_writefile(PFLT_CALLBACK_DATA Data, PCFLT_RELATED_OBJECTS, PVOID*) {
	FileNameInfo file_name(Data);
	if (nullptr == file_name.get()) {
		return FLT_PREOP_SUCCESS_NO_CALLBACK;
	}
	KdPrint(("Write operation: %wZ\n", file_name.get()));
	if (should_protect(file_name.get()->Buffer)) {
		Data->IoStatus.Status = STATUS_ACCESS_DENIED;
		return FLT_PREOP_COMPLETE;
	}
	return FLT_PREOP_SUCCESS_NO_CALLBACK;
}

FLT_PREOP_CALLBACK_STATUS pre_set_information(
	PFLT_CALLBACK_DATA Data, _In_ PCFLT_RELATED_OBJECTS, PVOID*) {
	
	auto params = Data->Iopb->Parameters.SetFileInformation;
	
	if (params.FileInformationClass != FileDispositionInformation &&
		params.FileInformationClass != FileDispositionInformationEx) {
		if (params.FileInformationClass == FileRenameInformation) {
			FileNameInfo file_name(Data);
			if (nullptr == file_name.get()) {
				return FLT_PREOP_SUCCESS_NO_CALLBACK;
			}
			KdPrint(("Rename operation: %wZ\n", file_name.get()));
			if (should_protect(file_name.get()->Buffer)) {
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
	FileNameInfo file_name(Data);
	if (nullptr == file_name.get()) {
		return FLT_PREOP_SUCCESS_NO_CALLBACK;
	}
	KdPrint(("Delete operation: %wZ\n", file_name.get()));
	if (should_protect(file_name.get()->Buffer)) {
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
