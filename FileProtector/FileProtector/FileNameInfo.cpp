#include "FileNameInfo.h"

FileNameInfo::FileNameInfo(PFLT_CALLBACK_DATA data)
	:  m_file_name_information(nullptr) {
	auto status = FltGetFileNameInformation(data, FLT_FILE_NAME_NORMALIZED | FLT_FILE_NAME_QUERY_DEFAULT, &m_file_name_information);
	if (NT_SUCCESS(status)) {
		FltParseFileNameInformation(m_file_name_information);
	}
	else {
		m_file_name_information = nullptr;
	}
}

FileNameInfo::~FileNameInfo() {
	if (nullptr != m_file_name_information) {
		FltReleaseFileNameInformation(m_file_name_information);
	}
}

PUNICODE_STRING FileNameInfo::get() const {
	if (nullptr != m_file_name_information) {
		return &m_file_name_information->Name;
	}
	return nullptr;
}
