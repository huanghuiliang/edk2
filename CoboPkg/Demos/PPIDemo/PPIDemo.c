#include <PiPei.h>
#include <Library/DebugLib.h>
#include <Library/PeimEntryPoint.h>
#include <Library/PeiServicesLib.h>
#include <Library/BaseMemoryLib.h>



EFI_STATUS
EFIAPI
MySimplePPI(VOID);

EFI_PEI_SIMPLE_PPI mSimplePPI = {
	MySimplePPI
};


EFI_PEI_PPI_DESCRIPTOR mSimplePPIList[] = {
		{
			EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST,
			&gMyPpiTestGuid,
			&mSimplePPI
		}
};

EFI_STATUS
EFIAPI
MySimplePPI(VOID)
{
	DEBUG((EFI_D_INFO, "\n\n Invoke MySimplePPI\n\n"));
	return EFI_SUCCESS;
}

EFI_STATUS
EFIAPI
PpiProducerInit(
	IN EFI_PEI_FILE_HANDLE FileHandle,
	IN CONST EFI_PEI_SERVICES **PeiServices
)
{
	EFI_STATUS Status;
	DEBUG((EFI_D_INFO, "\n\n Produce mSimplePPIList\n\n"));
	Status = PeiServicesInstallPpi(mSimplePPIList);
	if(EFI_ERROR(Status) {
		DEBUG((EFI_D_INFO, "\n\n Produce mSimplePPIList Failed\n\n"));
	}
	return EFI_SUCCESS;

}



































