#include "DartFilter.h"
#include "FileNameInfo.h"

PFLT_FILTER FilterHandle = nullptr;

bool ishoneypot(const wchar_t* filename)
{
	if (nullptr == filename)
	{
		return false;
	}

	if (nullptr != wcsstr(filename, L"_darthoney")) // Checks if the file name contains the _darthoney extension
	{
		return true;
	}

	return false;
}

NTSTATUS terminateProcess(ULONG processID)
{
	NTSTATUS status;
	HANDLE processH;
	PEPROCESS processPE;

	// procura o processo pelo PID
	status = PsLookupProcessByProcessId(ULongToHandle(processID), &processPE);
	
	if (!NT_SUCCESS(status))
	{
		DbgPrint("PsLookupProcessbyProcessId: Failed to look for process (%lu)", processID);
		return status;
	}

	// abre o processo (no caso objeto) pelo ponteiro
	status = ObOpenObjectByPointer(processPE, 0, nullptr, 0, NULL, KernelMode, &processH);

	if (!NT_SUCCESS(status))
	{
		DbgPrint("ObOpenObjectByPointer: Could not open object by his pointer.");
		return status;
	}

	// termina o processo encontrado e aberto pelo seu ponteiro
	status = ZwTerminateProcess(processH, STATUS_ACCESS_DENIED);

	if (!NT_SUCCESS(status))
	{
		DbgPrint("ZwTerminateProcess: Could not termminate process.");
		return status;
	}

	ZwClose(processH);
	return status;
}

FLT_PREOP_CALLBACK_STATUS pre_create(PFLT_CALLBACK_DATA Data, PCFLT_RELATED_OBJECTS, PVOID*)
{
	NTSTATUS status;
	auto& parameters = Data->Iopb->Parameters.Create;
	auto Success = FLT_PREOP_SUCCESS_NO_CALLBACK;
	auto Fail = FLT_PREOP_COMPLETE;

	ULONG processID = FltGetRequestorProcessId(Data);

	if (Data->RequestorMode == KernelMode)
	{
		return Success;
	}

	if (parameters.Options & FILE_DELETE_ON_CLOSE)
	{
		FileNameInfo file_name(Data);

		if (nullptr == file_name.get())
		{
			return Success;
		}

		KdPrint(("DartFilter: Process (%lu) delete operation.", processID));

		if (!ishoneypot(file_name.get()->Buffer))
		{
			return Success;
		}
		else
		{
			KdPrint(("DartFilter: Attempt of delete operation on honeypot."));

			status = terminateProcess(processID);

			if (!NT_SUCCESS(status))
			{
				KdPrint(("Failed to kill process (%lu) (0x%08X).\n", processID, status));
			}
			else 
			{
				KdPrint(("if looks could kill... (%lu)", processID));
			}

			Data->IoStatus.Status = STATUS_ACCESS_DENIED;
			return Fail;
		}
	}

	return Success;
}

FLT_PREOP_CALLBACK_STATUS pre_write(PFLT_CALLBACK_DATA Data, PCFLT_RELATED_OBJECTS, PVOID*)
{
	NTSTATUS status;

	auto Success = FLT_PREOP_SUCCESS_NO_CALLBACK;
	auto Fail = FLT_PREOP_COMPLETE;

	ULONG processID = FltGetRequestorProcessId(Data);
	FileNameInfo file_name(Data);

	if (Data->RequestorMode == KernelMode)
	{
		return Success;
	}

	if (nullptr == file_name.get())
	{
		return Success;
	}

	KdPrint(("DartFilter: Process (%lu) write operation.", processID));

	if (!ishoneypot(file_name.get()->Buffer))
	{
		return Success;
	}
	else
	{
		KdPrint(("DartFilter: Attempt of write operation on honeypot."));

		status = terminateProcess(processID);

		if (!NT_SUCCESS(status))
		{
			KdPrint(("Failed to kill process (%lu) (0x%08X).\n", processID, status));
		}
		else
		{
			KdPrint(("if looks could kill... (%lu)", processID));
		}
		
		Data->IoStatus.Status = STATUS_ACCESS_DENIED;
		return Fail;
	}

	return FLT_PREOP_SUCCESS_NO_CALLBACK;
}

FLT_PREOP_CALLBACK_STATUS pre_set(PFLT_CALLBACK_DATA Data, PCFLT_RELATED_OBJECTS, PVOID*)
{
	NTSTATUS status;

	auto& parameters = Data->Iopb->Parameters.SetFileInformation;
	auto Success = FLT_PREOP_SUCCESS_NO_CALLBACK;
	auto Fail = FLT_PREOP_COMPLETE;

	ULONG processID = FltGetRequestorProcessId(Data);

	if (Data->RequestorMode == KernelMode)
	{
		return Success;
	}

	if (parameters.FileInformationClass != FileDispositionInformation && parameters.FileInformationClass != FileDispositionInformationEx)
	{
		if (parameters.FileInformationClass == FileRenameInformation)
		{
			FileNameInfo file_name(Data);

			if (nullptr == file_name.get())
			{
				return Success;
			}

			KdPrint(("DartFilter: Process (%lu) set file information operation.", processID));

			if (!ishoneypot(file_name.get()->Buffer))
			{
				return Success;
			}
			else
			{
				KdPrint(("DartFilter: Attempt of write operation on honeypot."));

				status = terminateProcess(processID);

				if (!NT_SUCCESS(status))
				{
					KdPrint(("Failed to kill process (%lu) (0x%08X).\n", processID, status));
				}
				else
				{
					KdPrint(("if looks could kill... (%lu)", processID));
				}

				Data->IoStatus.Status = STATUS_ACCESS_DENIED;
				return Fail;
			}
		}

		return Success;
	}

	auto info = static_cast<FILE_DISPOSITION_INFORMATION*>(parameters.InfoBuffer);
	if (!info->DeleteFile)
	{
		return Success;
	}

	FileNameInfo file_name(Data);

	if (nullptr == file_name.get())
	{
		return Success;
	}

	if (!ishoneypot(file_name.get()->Buffer))
	{
		return Success;
	}
	else
	{
		status = terminateProcess(processID);

		KdPrint(("DartFilter: Attempt of delete operation on honeypot."));

		if (!NT_SUCCESS(status))
		{
			KdPrint(("Failed to kill process (%lu) (0x%08X).\n", processID, status));
		}
		else
		{
			KdPrint(("if looks could kill... (%lu)", processID));
		}

		Data->IoStatus.Status = STATUS_ACCESS_DENIED;
		return Fail;
	}

	return FLT_PREOP_SUCCESS_NO_CALLBACK;
}

NTSTATUS DartFilterUnloadCallback(FLT_FILTER_UNLOAD_FLAGS Flags)
{
	UNREFERENCED_PARAMETER(Flags);

	if (nullptr != FilterHandle)
	{
		FltUnregisterFilter(FilterHandle);
	}

	return STATUS_SUCCESS;
}

NTSTATUS DartFilterSetupCallback(PCFLT_RELATED_OBJECTS FltObjects, FLT_INSTANCE_SETUP_FLAGS Flags, ULONG VolumeDeviceType, FLT_FILESYSTEM_TYPE VolumeFilesystemType)
{
	UNREFERENCED_PARAMETER(FltObjects);
	UNREFERENCED_PARAMETER(Flags);
	UNREFERENCED_PARAMETER(VolumeDeviceType);
	UNREFERENCED_PARAMETER(VolumeFilesystemType);

	return STATUS_SUCCESS;
}

NTSTATUS DartFilterQueryTeardownCallback(PCFLT_RELATED_OBJECTS FltObjects, FLT_INSTANCE_QUERY_TEARDOWN_FLAGS Flags)
{
	UNREFERENCED_PARAMETER(FltObjects);
	UNREFERENCED_PARAMETER(Flags);

	return STATUS_SUCCESS;
}