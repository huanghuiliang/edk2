#include "EdidFind.h"
#include <Uefi.h>
//#include <Library/EdkIIGlueMemoryAllocationLib.h>
#include <Library/UefiLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/MemoryAllocationLib.h>

EFI_STATUS 
EFIAPI 
PrintEdidVersion(
	
	) 
 {
 	Print(L"EDID_VERSION = 1.3\n");
 	return EFI_SUCCESS;
 }

 EFI_EDID_FIND_PROTOCOL  EdidFindProtocol;

EFI_STATUS
EFIAPI
InitEdidFindDriver (
	IN EFI_HANDLE ImageHandle,
	IN EFI_SYSTEM_TABLE *EFI_SYSTEM_TABLE
	)
{
	//EdidFindProtocol = AllocatePool(sizeof(EFI_EDID_FIND_PROTOCOL));
	//if(EdidFindProtocol == NULL) {
	//	return EFI_OUT_OF_RESOURCES;
	//}
	EdidFindProtocol.Revision = 16;
	EdidFindProtocol.PrintEdidVersion = PrintEdidVersion;
	EFI_STATUS Status;
    	Status = gBS->InstallProtocolInterface(&ImageHandle, &gEfiEdidFindProtocolGuid, EFI_NATIVE_INTERFACE,  &EdidFindProtocol);
    	Print(L"InstallProtocolInterface ===>EdidFindProtocol\n ");
    	if(EFI_ERROR(Status)) {
		Print(L"InstallProtocolInterface ===>EdidFindProtocol failed!\n ");
    	} else {
    		Print(L"InstallProtocolInterface ===>EdidFindProtocol  successed!\n ");
    	}
    	return Status;
}