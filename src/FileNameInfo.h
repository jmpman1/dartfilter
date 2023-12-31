#pragma once

#include <fltKernel.h>

class FileNameInfo {
public:
	FileNameInfo(PFLT_CALLBACK_DATA Data);
	~FileNameInfo();

	PUNICODE_STRING get() const;

private:
	PFLT_FILE_NAME_INFORMATION fNameInformation;
};
