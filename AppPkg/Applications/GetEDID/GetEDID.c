#include  <Uefi.h>
#include  <Library/UefiLib.h>
#include  <Library/ShellCEntryLib.h>

#include  <Protocol/EdidDiscovered.h>


extern EFI_BOOT_SERVICES         *gBS;
extern EFI_SYSTEM_TABLE			 *gST;
extern EFI_RUNTIME_SERVICES 	 *gRT;

extern EFI_HANDLE				 gImageHandle;

EFI_GUID gEfiEdidDiscoveredProtocolGuid = { 0x1C0C34F6, 0xD380, 0x41FA, { 0xA0, 0x49, 0x8A, 0xD0, 0x6C, 0x1A, 0x66, 0xAA }};

EFI_STATUS GetEDIDInfo()
{
    EFI_STATUS  Status;
    EFI_HANDLE *EDIDHandleBuffer;
    UINTN EDIDHandleCount, index, i;
    EFI_EDID_DISCOVERED_PROTOCOL * EDID;
    
    Status = gBS->LocateHandleBuffer (ByProtocol, 
                                      &gEfiEdidDiscoveredProtocolGuid, 
                                      NULL, 
                                      &EDIDHandleCount, 
                                      &EDIDHandleBuffer); 
    if (EFI_ERROR (Status))
    {
      Print(L"ERROR : Can't get EdidDiscoveredProtocol.\n");   
      return FALSE;
    }
    
    Print(L"EDID count = %d\n", EDIDHandleCount);
    for(index = 0 ; index < EDIDHandleCount ; index++)
    {
      Status = gBS->OpenProtocol( EDIDHandleBuffer[index],
                                  &gEfiEdidDiscoveredProtocolGuid,
                                  (VOID**)&EDID,
                                  gImageHandle,
                                  NULL,
                                  EFI_OPEN_PROTOCOL_GET_PROTOCOL
                                  );
      if (EFI_ERROR (Status))
      {
        Print(L"ERROR : Open EDID Protocol failed.\n");   
        return FALSE;
      }          
      
      Print(L"%d\n", EDID->SizeOfEdid);
      for(i = 0 ; i < 128 ; i++)
      {
        Print(L"%02X ", EDID->Edid[i]);
		
		if ((i+1) % 16 ==0) { 
				Print(L"\n"); }
		else	
			    if ((i+1) % 8 ==0) { Print(L"- ");}
      }
      Print(L"\n");
                
      Status = gBS->CloseProtocol(EDIDHandleBuffer[index], &gEfiEdidDiscoveredProtocolGuid, 
	  	gImageHandle, NULL); 
      
      if (EFI_ERROR (Status))
      {
        Print(L"ERROR : Close EDID device handle failed.\n");   
        return FALSE;
      }
    }    
  return EFI_SUCCESS;    
}

int
EFIAPI
main (
  IN int Argc,
  IN CHAR16 **Argv
  )
{
 GetEDIDInfo();
 return EFI_SUCCESS;
}

