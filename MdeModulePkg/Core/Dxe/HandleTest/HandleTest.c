#include <Uefi.h>
#include <Library/UefiLib.h>
#include <Library/MemoryAllocationLib.h>
#include "../Hand/Handle.h"
#include <Library/DebugLib.h>

EFI_STATUS EFIAPI UefiMain(IN EFI_HANDLE ImageHandle, IN EFI_SYSTEM_TABLE *SystemTable)
{
	UINTN Status;
	UINTN Size, index;
	EFI_HANDLE *HandleList = NULL;
	IHANDLE *CUR_IHANDLE = NULL;
	LIST_ENTRY *Protocols;
	EFI_BOOT_SERVICES *gBS = SystemTable->BootServices;

	PROTOCOL_INTERFACE *interface;
	PROTOCOL_ENTRY *entry;
	EFI_GUID *Guid;
	LIST_ENTRY *link;

	/**
	UINTN HandleCount;
	EFI_HANDLE *DiskControllerHandles = NULL;
	//EFI_DISK_IO_PROTOCOL *DiskIo;
	Status = gBS->LocateHandleBuffer(ByProtocol, &gEfiDiskIoProtocolGuid, NULL, &HandleCount, &DiskControllerHandles);
	**/
	
	Status = gBS->LocateHandleBuffer(AllHandles, NULL, NULL, &Size, &HandleList);
	if(EFI_ERROR(Status)) {
		Print(L"LocateHandleBuffer Error\n");
	}
	/*if(Status == EFI_BUFFER_TOO_SMALL) {
		HandleList = AllocateZeroPool(Size);
		Print(L"Status == EFI_BUFFER_TOO_SMALL\n");
	}
	Status = gBS->LocateHandleBuffer(AllHandles, NULL, NULL, &Size, HandleList);*/
	//Print(L"Three are %d handles in the Database\n",Size/sizeof(EFI_HANDLE));

	
	Print(L"Three are %d handles in the Database\n",Size);
	for(index = 0; index < 10; index ++) {
		//CUR_IHANDLE = &(*(IHANDLE *)HandleList[index]);		
		CUR_IHANDLE = &(*(IHANDLE *)HandleList[index]);
		//Print(L"Handle->key: %d\n", (*(IHANDLE *)HandleList[index]).Key);
		Print(L"Handle->key: %d\n", CUR_IHANDLE->Key);
		Protocols = &CUR_IHANDLE->Protocols;
		for(link = Protocols->BackLink; link != Protocols; link = link->BackLink) {
		/*if(&Protocols == NULL) {
						Print(L"Protocols == NULL\n");
						continue;
		}*/
			
			 
			if(link == NULL) {
				Print(L"link == NULL\n");
				continue;
			}
			interface = CR(link, PROTOCOL_INTERFACE, Link, PROTOCOL_INTERFACE_SIGNATURE);
			
			if(interface == NULL) {
				Print(L"interface == NULL\n");
				continue;
			}
			entry = interface->Protocol;
			if(entry == NULL) {
				Print(L"entry == NULL\n");
				continue;
			}
			Guid = &entry->ProtocolID;
			if(Guid == NULL) {
				Print(L"Guid == NULL\n");
				continue;
			}
			Print(L"Protocol Guid = %08x-%04x-%04x-%02x%02x-%02x%02x%02x%02x%02x%02x\n",
				          (UINTN)Guid->Data1,
				          (UINTN)Guid->Data2,
				          (UINTN)Guid->Data3,
				          (UINTN)Guid->Data4[0],
				          (UINTN)Guid->Data4[1],
				          (UINTN)Guid->Data4[2],
				          (UINTN)Guid->Data4[3],
				          (UINTN)Guid->Data4[4],
				          (UINTN)Guid->Data4[5],
				          (UINTN)Guid->Data4[6],
				          (UINTN)Guid->Data4[7]
				   );
          
		}
	//	Print(L"End inner for\n");
	} 

	/*for(index = 0; index < Size/sizeof(EFI_HANDLE); index ++) {
		if((*(IHANDLE*)ImageHandle).Key == (*(IHANDLE *)HandleList[index]).Key)
			Print(L"This ImageHandle->Key = %d\n", (*(IHANDLE *)HandleList[index]).Key);
	}*/

	FreePool(HandleList);
	
	//SystemTable->ConOut->OutputString(SystemTable->ConOut, L"HelloWorld2\n");
	return EFI_SUCCESS;
}










