#include <Uefi.h>  

#include <Library/UefiBootServicesTableLib.h>

#include <Base.h>
#include <Library/UefiLib.h>
#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/PrintLib.h>
#include <Protocol/DiskIo.h>
#include <Protocol/BlockIo.h>
#include <Protocol/DevicePath.h>

#include <Protocol/DevicePathToText.h>
#include <Uefi/UefiGpt.h>
#include <Library/DevicePathLib.h>

#include <Protocol/EdidFind.h>


EFI_GUID gEfiEdidFindProtocolGuid = { 0x1c0c34f6, 0xd381, 0x41fa, {0xa0, 0x49, 0x8a, 0xd0, 0x6c, 0x1a, 0x66, 0xab }  };

EFI_STATUS EFIAPI UefiMain(IN EFI_HANDLE ImageHandle, IN EFI_SYSTEM_TABLE *SystemTable)
{
	EFI_STATUS Status;
	UINTN HandleIndex;
	UINTN HandleCount;
	EFI_HANDLE *EdidFindHandles = NULL;
	EFI_EDID_FIND_PROTOCOL *EdidFindProtocol;

	//EFI_DISK_IO_PROTOCOL *DiskIo;
	Status = gBS->LocateHandleBuffer(ByProtocol, &gEfiEdidFindProtocolGuid, NULL, &HandleCount, &EdidFindHandles);
	if (!EFI_ERROR(Status)) {
		SystemTable->ConOut->OutputString(SystemTable->ConOut, L"LocateHandleBuffer SUCCESS\n");
		for(HandleIndex = 0; HandleIndex < HandleCount; HandleIndex ++) {
			Status = gBS->OpenProtocol(EdidFindHandles[HandleIndex],
						          &gEfiEdidFindProtocolGuid,
						          ((VOID**)&EdidFindProtocol),
						          gImageHandle,
						          NULL,
						          EFI_OPEN_PROTOCOL_GET_PROTOCOL
						          );
			if(EFI_ERROR(Status)) {
				Print(L"ERROR: Open Edid Find Protocol failed!\n");
				return FALSE;
			} else {
				Print(L"SUCCESS: Open Edid Find Protocol SUCCESS!\n");
			}
			Print(L"EdidFindProtocol.Revision = %d\n", EdidFindProtocol->Revision);
			EdidFindProtocol->PrintEdidVersion();
			
			Status = gBS->CloseProtocol(EdidFindHandles[HandleIndex], 
						        &gEfiEdidFindProtocolGuid,gImageHandle, NULL);
			if(EFI_ERROR(Status)) {
				Print(L"ERROR: close Edid Find Protocol failed!\n");
				return FALSE;
			} else {
				Print(L"SUCCESS: close Edid Find Protocol SUCCESS!\n");
			}
			return Status;
		}
	}else {
		SystemTable->ConOut->OutputString(SystemTable->ConOut, L"LocateHandleBuffer ERROR\n");
		
	}
	return Status;
}