#pragma once

#include <fltKernel.h>
#include <ntddk.h>

extern PFLT_FILTER FilterHandle;

extern "C" FLT_PREOP_CALLBACK_STATUS FLTAPI pre_create(
	PFLT_CALLBACK_DATA Data,
	PCFLT_RELATED_OBJECTS FltObjects,
	PVOID* CompletionContext
);

extern "C" FLT_PREOP_CALLBACK_STATUS FLTAPI pre_write(
	PFLT_CALLBACK_DATA Data,
	PCFLT_RELATED_OBJECTS FltObjects,
	PVOID* CompletionContext
);

extern "C" FLT_PREOP_CALLBACK_STATUS FLTAPI pre_set(
	PFLT_CALLBACK_DATA Data,
	PCFLT_RELATED_OBJECTS FltObjects,
	PVOID* CompletionContext
);

CONST FLT_OPERATION_REGISTRATION Callbacks[] = {
	{
		IRP_MJ_CREATE,
		0,
		pre_create,
		0
	},

	{
		IRP_MJ_WRITE,
		0,
		pre_write,
		0
	},

	{
		IRP_MJ_SET_INFORMATION,
		0,
		pre_set,
		0
	},

	{
		IRP_MJ_OPERATION_END
	}
};

extern "C" NTSTATUS FLTAPI DartFilterUnloadCallback(
	FLT_FILTER_UNLOAD_FLAGS Flags
);

extern  "C" NTSTATUS FLTAPI DartFilterSetupCallback(
	PCFLT_RELATED_OBJECTS  FltObjects,
	FLT_INSTANCE_SETUP_FLAGS  Flags,
	DEVICE_TYPE  VolumeDeviceType,
	FLT_FILESYSTEM_TYPE  VolumeFilesystemType);

extern  "C" NTSTATUS FLTAPI DartFilterQueryTeardownCallback(
	PCFLT_RELATED_OBJECTS FltObjects,
	FLT_INSTANCE_QUERY_TEARDOWN_FLAGS Flags
);

const FLT_REGISTRATION FilterRegistration = {
	sizeof(FLT_REGISTRATION), //			Size
	FLT_REGISTRATION_VERSION, //			Version
	0, //									Flags
	nullptr, //								Context registration
	Callbacks, //							Operations Callbacks
	DartFilterUnloadCallback, //			FilterUnload
	DartFilterSetupCallback, //				InstanceSetupCallback
	DartFilterQueryTeardownCallback, //		InstanceQueryTeardownCallback
	nullptr, //								InstanceTeardownStart
	nullptr, //								InstanceTeardownComplete
	nullptr, //								GenerateFileName
	nullptr, //								GenerateDestinationFileName
	nullptr, //								NormalizeNameComponent
};