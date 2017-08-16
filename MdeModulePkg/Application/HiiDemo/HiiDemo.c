#include "HiiDemo.h"
#include "HiiDemoData.h"

EFI_GUID   mFormSetGuid = HII_DEMO_FORMSET_GUID;

EFI_STATUS
EFIAPI
HiiDemoMain (
  IN EFI_HANDLE         ImageHandle,
  IN EFI_SYSTEM_TABLE   *SystemTable
  )
{
  EFI_STATUS                      Status;
  EFI_HII_HANDLE                  HiiHandle;
  EFI_SCREEN_DESCRIPTOR           Screen;
  EFI_FORM_BROWSER2_PROTOCOL      *FormBrowser2;
  
   SystemTable->ConOut->ClearScreen(SystemTable->ConOut);  /* Clear screen */
   ZeroMem (&Screen, sizeof (EFI_SCREEN_DESCRIPTOR));
   gST->ConOut->QueryMode (gST->ConOut, gST->ConOut->Mode->Mode, &Screen.RightColumn, &Screen.BottomRow);
   Screen.TopRow     = 3;
   Screen.BottomRow  = Screen.BottomRow - 3;
   
   /* . */
   //
   // Locate Formbrowser2 protocol
   //
   Status = gBS->LocateProtocol (&gEfiFormBrowser2ProtocolGuid, NULL, (VOID **) &FormBrowser2);
   if (EFI_ERROR (Status)) {
     return Status;
   } 
   
   HiiHandle = HiiAddPackages(
   									&mFormSetGuid,
   									NULL, /*Device Handlle*/
   									HiiDemoStrings,
   									HiiDemoVfrBin,
   									NULL
   									);
   									
   	ASSERT (HiiHandle != NULL);
   	
   	/**
   	SendForm (
  IN  CONST EFI_FORM_BROWSER2_PROTOCOL *This,
  IN  EFI_HII_HANDLE                   *Handles,
  IN  UINTN                            HandleCount,
  IN  EFI_GUID                         *FormSetGuid, OPTIONAL
  IN  UINT16                           FormId, OPTIONAL
  IN  CONST EFI_SCREEN_DESCRIPTOR      *ScreenDimensions, OPTIONAL
  OUT EFI_BROWSER_ACTION_REQUEST       *ActionRequest  OPTIONAL
  )**/
   	Status = FormBrowser2->SendForm(
   														FormBrowser2,
   														&HiiHandle,
   														1,
   														&mFormSetGuid,
   														0,
   														&Screen,
   														NULL   														
   														);
   														
   	HiiRemovePackages(HiiHandle);
   	return EFI_SUCCESS;
   	
}