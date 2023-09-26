#include "FileNameInfo.h"

FileNameInfo::FileNameInfo(PFLT_CALLBACK_DATA Data):fNameInformation(nullptr) // Constructor
{
	auto status = FltGetFileNameInformation(Data, FLT_FILE_NAME_NORMALIZED | FLT_FILE_NAME_QUERY_DEFAULT, &fNameInformation);

	if (NT_SUCCESS(status))
	{
		FltParseFileNameInformation(fNameInformation);
	}
	else {
		fNameInformation = nullptr;
	}
}

FileNameInfo::~FileNameInfo() // Deconstructor (Cleanup)
{
	if (nullptr != fNameInformation)
	{
		FltReleaseFileNameInformation(fNameInformation);
	}
}

PUNICODE_STRING FileNameInfo::get() const // Gets the File name in PUNICODE_STRING format
{
	if (nullptr != fNameInformation)
	{
		return &fNameInformation->Name;
	}

	return nullptr;
}