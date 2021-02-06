#include "mini_filter.h"

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

	PFLT_FILE_NAME_INFORMATION file_name_information;
	auto status = FltGetFileNameInformation(Data, FLT_FILE_NAME_NORMALIZED | FLT_FILE_NAME_QUERY_DEFAULT, &file_name_information);
	if (NT_SUCCESS(status)) {
		status = FltParseFileNameInformation(file_name_information);
		if (NT_SUCCESS(status)) {
			if (Data->Iopb->Parameters.Create.Options & FILE_DELETE_ON_CLOSE) {
				KdPrint(("Delete operation: %wZ\n", &file_name_information->Name));
				if (should_protect(file_name_information->Name.Buffer)) {
					Data->IoStatus.Status = STATUS_ACCESS_DENIED;
					return FLT_PREOP_COMPLETE;
				}
			}
		}
		FltReleaseFileNameInformation(file_name_information);
	}
	
	return FLT_PREOP_SUCCESS_NO_CALLBACK;
}

FLT_PREOP_CALLBACK_STATUS pre_writefile(PFLT_CALLBACK_DATA Data, PCFLT_RELATED_OBJECTS, PVOID*) {
	KdPrint(("Write operation: %wZ\n", &Data->Iopb->TargetFileObject->FileName));
	PFLT_FILE_NAME_INFORMATION file_name_information;
	auto status = FltGetFileNameInformation(Data, FLT_FILE_NAME_NORMALIZED | FLT_FILE_NAME_QUERY_DEFAULT, &file_name_information);
	if (NT_SUCCESS(status)) {
		status = FltParseFileNameInformation(file_name_information);
		if (NT_SUCCESS(status)) {
			KdPrint(("Write operation: %wZ\n", &file_name_information->Name));
			if (should_protect(file_name_information->Name.Buffer)) {
				Data->IoStatus.Status = STATUS_ACCESS_DENIED;
				return FLT_PREOP_COMPLETE;
			}
		}
		FltReleaseFileNameInformation(file_name_information);
	}
	return FLT_PREOP_SUCCESS_NO_CALLBACK;
}

FLT_PREOP_CALLBACK_STATUS pre_set_information(
	_Inout_ PFLT_CALLBACK_DATA Data, _In_ PCFLT_RELATED_OBJECTS, PVOID*) {
	
	auto params = Data->Iopb->Parameters.SetFileInformation;
	
	if (params.FileInformationClass != FileDispositionInformation &&
		params.FileInformationClass != FileDispositionInformationEx) {
		if (params.FileInformationClass == FileRenameInformation) {
			PFLT_FILE_NAME_INFORMATION file_name_information;
			auto status = FltGetFileNameInformation(Data, FLT_FILE_NAME_NORMALIZED | FLT_FILE_NAME_QUERY_DEFAULT, &file_name_information);
			if (NT_SUCCESS(status)) {
				status = FltParseFileNameInformation(file_name_information);
				if (NT_SUCCESS(status)) {
					KdPrint(("Rename operation: %wZ\n", &file_name_information->Name));
					if (should_protect(file_name_information->Name.Buffer)) {
						Data->IoStatus.Status = STATUS_ACCESS_DENIED;
						return FLT_PREOP_COMPLETE;
					}
				}
				FltReleaseFileNameInformation(file_name_information);
			}
		}
		return FLT_PREOP_SUCCESS_NO_CALLBACK;
	}

	auto info = static_cast<FILE_DISPOSITION_INFORMATION*>(params.InfoBuffer);
	if (!info->DeleteFile) {
		return FLT_PREOP_SUCCESS_NO_CALLBACK;
	}
	// If we got till here it means that it delete operation
	PFLT_FILE_NAME_INFORMATION file_name_information;
	auto status = FltGetFileNameInformation(Data, FLT_FILE_NAME_NORMALIZED | FLT_FILE_NAME_QUERY_DEFAULT, &file_name_information);
	if (NT_SUCCESS(status)) {
		status = FltParseFileNameInformation(file_name_information);
		if (NT_SUCCESS(status)) {
			KdPrint(("Delete operation: %wZ\n", &file_name_information->Name));
			if (should_protect(file_name_information->Name.Buffer)) {
				Data->IoStatus.Status = STATUS_ACCESS_DENIED;
				return FLT_PREOP_COMPLETE;
			}
		}
		FltReleaseFileNameInformation(file_name_information);
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
