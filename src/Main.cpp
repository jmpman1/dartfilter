/*

                                                      _       
                                          __   ___.--'_`.    
                                         ( _`.'. -   'o` )   
                                         _\.'_'      _.-'    
                                        ( \`. )    //\`         
                                         \_`-'`---'\\__,      
                                          \`        `-\        
                                           `
                                            DartFilter
                                            by jmpman1
                         

    * A Windows Kernel Filesystem Minifilter Driver that detects ransomware activity, monitoring a Honeypot file's IRPs
    * TESTED ON
            // Windows 10 Education Build 19041
            // Test Mode (Because the driver is unsigned xd)

    * Project Structure
      |
      |-> Main.cpp (DriverEntry)
      |
      |-> DartFilter.cpp (Routines for our Callback Preoperations)
      |   |
      |   |-> DartFilter.h (Header for our Minifilter Callbacks, Registration and Handle)
      |
      |-> FileNameInfo.cpp (Provides functions for us to work with the FileNameInfo, mainly to retrieve the name of the file)
          |
          |-> FileNameInfo.h (Creation of the class to store the File Information, which will be later used to check if IRPs are happening on our honeypot)

*/

#include "DartFilter.h"
 
extern "C" NTSTATUS DriverEntry(PDRIVER_OBJECT DriverObject, PUNICODE_STRING RegistryPath)
{
    UNREFERENCED_PARAMETER(RegistryPath);

    DbgPrint("DartFilter: Hello from the driver entry!\n");

    auto status = FltRegisterFilter(DriverObject, &FilterRegistration, &FilterHandle);

    if (!NT_SUCCESS(status))
    {
        return status;
    }

    status = FltStartFiltering(FilterHandle);

    if (!NT_SUCCESS(status))
    {
        FltUnregisterFilter(FilterHandle);
    }

    return status;
}