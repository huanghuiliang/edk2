#include <Uefi.h>
#include <Library/UefiLib.h>
#include <Library/BaseLib.h>
#include <Protocol/EdidDiscovered.h>
#include <Protocol/EdidActive.h>


#include <stdio.h>


#include <Uefi.h>
#include <Library/UefiLib.h>
#include <Library/ShellCEntryLib.h>
#include <Library/ShellLib.h>
#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/PrintLib.h>

#include <Protocol/EfiShell.h>
#include <Protocol/LoadedImage.h>
#include <Protocol/GraphicsOutput.h>
#include <Protocol/EdidActive.h>
#include <Protocol/EdidDiscovered.h>

 


extern EFI_BOOT_SERVICES *gBS;
extern EFI_SYSTEM_TABLE *gST;
extern EFI_RUNTIME_SERVICES *gRT;
extern EFI_HANDLE gImageHandle;

EFI_GUID gEfiEdidDiscoveredProtocolGuid = { 0x1C0C34F6, 0xD380, 0x41FA, { 0xA0, 0x49, 0x8A, 0xD0, 0x6C, 0x1A, 0x66, 0xAA }};
EFI_GUID gEfiHiiStringProtocolGuid = {0x0fd96974, 0x23aa, 0x4cdc, {0xb9, 0xcb, 0x98, 0xd1, 0x77, 0x50, 0x32, 0x2a}};


typedef struct {
    UINT8  Header[8];                        //EDID header "00 FF FF FF FF FF FF 00"
    UINT16 ManufactureName;                  //EISA 3-character ID
    UINT16 ProductCode;                      //Vendor assigned code
    UINT32 SerialNumber;                     //32-bit serial number
    UINT8  WeekOfManufacture;                //Week number
    UINT8  YearOfManufacture;                //Year
    UINT8  EdidVersion;                      //EDID Structure Version
    UINT8  EdidRevision;                     //EDID Structure Revision
    UINT8  VideoInputDefinition;
    UINT8  MaxHorizontalImageSize;           //cm
    UINT8  MaxVerticalImageSize;             //cm
    UINT8  DisplayGamma;
    UINT8  DpmSupport;
    UINT8  RedGreenLowBits;                  //Rx1 Rx0 Ry1 Ry0 Gx1 Gx0 Gy1Gy0
    UINT8  BlueWhiteLowBits;                 //Bx1 Bx0 By1 By0 Wx1 Wx0 Wy1 Wy0
    UINT8  RedX;                             //Red-x Bits 9 - 2
    UINT8  RedY;                             //Red-y Bits 9 - 2
    UINT8  GreenX;                           //Green-x Bits 9 - 2
    UINT8  GreenY;                           //Green-y Bits 9 - 2
    UINT8  BlueX;                            //Blue-x Bits 9 - 2
    UINT8  BlueY;                            //Blue-y Bits 9 - 2
    UINT8  WhiteX;                           //White-x Bits 9 - 2
    UINT8  WhiteY;                           //White-x Bits 9 - 2
    UINT8  EstablishedTimings[3];
    UINT8  StandardTimingIdentification[16];
    UINT8  DescriptionBlock1[18];
    UINT8  DescriptionBlock2[18];
    UINT8  DescriptionBlock3[18];
    UINT8  DescriptionBlock4[18];
    UINT8  ExtensionFlag;                    //Number of (optional) 128-byte EDID extension blocks
    UINT8  Checksum;
} __attribute__((__packed__)) EDID_DATA_BLOCK;

// most of these defines come straight out of NetBSD EDID source code
#define CHECK_BIT(var, pos)               ((var & (1 << pos)) == (1 << pos))
#define EDID_COMBINE_HI_8LO(hi, lo)       ((((unsigned)hi) << 8) | (unsigned)lo)
#define EDID_DATA_BLOCK_SIZE              0x80
#define EDID_VIDEO_INPUT_LEVEL(x)         (((x) & 0x60) >> 5)
#define EDID_DPMS_ACTIVE_OFF              (1 << 5)
#define EDID_DPMS_SUSPEND                 (1 << 6)
#define EDID_DPMS_STANDBY                 (1 << 7)
#define EDID_STD_TIMING_HRES(ptr)         ((((ptr)[0]) * 8) + 248)
#define EDID_STD_TIMING_VFREQ(ptr)        ((((ptr)[1]) & 0x3f) + 60)
#define EDID_STD_TIMING_RATIO(ptr)        ((ptr)[1] & 0xc0)
#define EDID_BLOCK_IS_DET_TIMING(ptr)     ((ptr)[0] | (ptr)[1])
#define EDID_DET_TIMING_DOT_CLOCK(ptr)    (((ptr)[0] | ((ptr)[1] << 8)) * 10000)
#define EDID_HACT_LO(ptr)                 ((ptr)[2])
#define EDID_HBLK_LO(ptr)                 ((ptr)[3])
#define EDID_HACT_HI(ptr)                 (((ptr)[4] & 0xf0) << 4)
#define EDID_HBLK_HI(ptr)                 (((ptr)[4] & 0x0f) << 8)
#define EDID_DET_TIMING_HACTIVE(ptr)      (EDID_HACT_LO(ptr) | EDID_HACT_HI(ptr))
#define EDID_DET_TIMING_HBLANK(ptr)       (EDID_HBLK_LO(ptr) | EDID_HBLK_HI(ptr))
#define EDID_VACT_LO(ptr)                 ((ptr)[5])
#define EDID_VBLK_LO(ptr)                 ((ptr)[6])
#define EDID_VACT_HI(ptr)                 (((ptr)[7] & 0xf0) << 4)
#define EDID_VBLK_HI(ptr)                 (((ptr)[7] & 0x0f) << 8)
#define EDID_DET_TIMING_VACTIVE(ptr)      (EDID_VACT_LO(ptr) | EDID_VACT_HI(ptr))
#define EDID_DET_TIMING_VBLANK(ptr)       (EDID_VBLK_LO(ptr) | EDID_VBLK_HI(ptr))
#define EDID_HOFF_LO(ptr)                 ((ptr)[8])
#define EDID_HWID_LO(ptr)                 ((ptr)[9])
#define EDID_VOFF_LO(ptr)                 ((ptr)[10] >> 4)
#define EDID_VWID_LO(ptr)                 ((ptr)[10] & 0xf)
#define EDID_HOFF_HI(ptr)                 (((ptr)[11] & 0xc0) << 2)
#define EDID_HWID_HI(ptr)                 (((ptr)[11] & 0x30) << 4)
#define EDID_VOFF_HI(ptr)                 (((ptr)[11] & 0x0c) << 2)
#define EDID_VWID_HI(ptr)                 (((ptr)[11] & 0x03) << 4)
#define EDID_DET_TIMING_HSYNC_OFFSET(ptr) (EDID_HOFF_LO(ptr) | EDID_HOFF_HI(ptr))
#define EDID_DET_TIMING_HSYNC_WIDTH(ptr)  (EDID_HWID_LO(ptr) | EDID_HWID_HI(ptr))
#define EDID_DET_TIMING_VSYNC_OFFSET(ptr) (EDID_VOFF_LO(ptr) | EDID_VOFF_HI(ptr))
#define EDID_DET_TIMING_VSYNC_WIDTH(ptr)  (EDID_VWID_LO(ptr) | EDID_VWID_HI(ptr))
#define EDID_HSZ_LO(ptr)                  ((ptr)[12])
#define EDID_VSZ_LO(ptr)                  ((ptr)[13])
#define EDID_HSZ_HI(ptr)                  (((ptr)[14] & 0xf0) << 4)
#define EDID_VSZ_HI(ptr)                  (((ptr)[14] & 0x0f) << 8)
#define EDID_DET_TIMING_HSIZE(ptr)        (EDID_HSZ_LO(ptr) | EDID_HSZ_HI(ptr))
#define EDID_DET_TIMING_VSIZE(ptr)        (EDID_VSZ_LO(ptr) | EDID_VSZ_HI(ptr))
#define EDID_DET_TIMING_HBORDER(ptr)      ((ptr)[15])
#define EDID_DET_TIMING_VBORDER(ptr)      ((ptr)[16])
#define EDID_DET_TIMING_FLAGS(ptr)        ((ptr)[17])
#define EDID_DET_TIMING_VSOBVHSPW(ptr)    ((ptr)[11])


//
// Based on code found at http://code.google.com/p/my-itoa/
//
/*
int
Integer2AsciiString(int val, char* buf)
{
    const unsigned int radix = 10;
 
    char* p = buf;
    unsigned int a; 
    int len;
    char* b;
    char temp;
    unsigned int u;
 
    if (val < 0) {
        *p++ = '-';
        val = 0 - val;
    }
    u = (unsigned int)val;
    b = p;
 
    do {
        a = u % radix;
        u /= radix;
        *p++ = a + '0';
    } while (u > 0);
 
    len = (int)(p - buf);
    *p-- = 0;
 
    // swap 
    do {
       temp = *p; *p = *b; *b = temp;
       --p; ++b;
    } while (b < p);
 
    return len;
}
 
 
//
// Based on code found on the Internet (author unknown)
// Search for ftoa implementations
//
int
Float2AsciiString(float f, char *buffer, int numdecimals)
{
    int status = 0;
    char *s = buffer;
    long mantissa, int_part, frac_part;
    short exp2;
    char *p;
    char m;
 
    typedef union {
        long L;
        float F;
    } LF_t;
    LF_t x;
 
    if (f == 0.0) {           // return 0.00
        *s++ = '0'; *s++ = '.'; *s++ = '0'; *s++ = '0'; 
        *s = 0;
       return status;
    }
 
    x.F = f;
 
    exp2 = (unsigned char)(x.L >> 23) - 127;
    mantissa = (x.L & 0xFFFFFF) | 0x800000;
    frac_part = 0;
    int_part = 0;
 
    if (exp2 >= 31 || exp2 < -23) {
        *s = 0;
        return 1;
    } 
 
    if (exp2 >= 0) {
        int_part = mantissa >> (23 - exp2);
        frac_part = (mantissa << (exp2 + 1)) & 0xFFFFFF;
    } else {
        frac_part = (mantissa & 0xFFFFFF) >> -(exp2 + 1);
    }
 
    if (int_part == 0)
       *s++ = '0';
    else {
        Integer2AsciiString(int_part, s);
        while (*s) s++;
    }
    *s++ = '.';
 
    if (frac_part == 0)
        *s++ = '0';
    else {
        for (m = 0; m < numdecimals; m++) {                       // print BCD
            frac_part = (frac_part << 3) + (frac_part << 1);      // frac_part *= 10
            *s++ = (frac_part >> 24) + '0';
            frac_part &= 0xFFFFFF;
        }
    }
    *s = 0;
 
    return status;
}
 
 
VOID
Ascii2UnicodeString(CHAR8 *String, CHAR16 *UniString)
{
    while (*String != '\0') {
        *(UniString++) = (CHAR16) *(String++);
    }
    *UniString = '\0';
}
 
 
CHAR16 *
DisplayGammaString(UINT8 gamma)
{
    char str[8];
    static CHAR16 wstr[8];
 
    float g1 = (float)gamma;
    float g2 = 1.00 + (g1/100);
 
    Float2AsciiString(g2, str, 2);         
    Ascii2UnicodeString(str, wstr);
 
    return wstr;
}
 
 
CHAR16 *
ManufacturerAbbrev(UINT16 *ManufactureName)
{
    static CHAR16 mcode[8];
    UINT8 *block = (UINT8 *)ManufactureName;
    UINT16 h = EDID_COMBINE_HI_8LO(block[0], block[1]);
 
    mcode[0] = (CHAR16)((h>>10) & 0x1f) + 'A' - 1;
    mcode[1] = (CHAR16)((h>>5) & 0x1f) + 'A' - 1;
    mcode[2] = (CHAR16)(h & 0x1f) + 'A' - 1;
    mcode[3] = (CHAR16)'\0';
  
    return mcode;
}
 
 
int
CheckForValidEdid(EFI_EDID_ACTIVE_PROTOCOL *Edid)
{
    EDID_DATA_BLOCK *EdidDataBlock = (EDID_DATA_BLOCK *)Edid->Edid; 
    UINT8 *edid = (UINT8 *)Edid->Edid;
    UINT8 i;
    UINT8 checksum = 0;
    const UINT8 EdidHeader[] = {0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00};
 
    for (i = 0; i < EDID_LENGTH; i++) {
        checksum += edid[i];
    }
    if (checksum != 0) {
        return(1);
    }
  
    if (*edid == 0x00) {
       checksum = 0;
       for (i = 0; i < 8; i++) {
           if (*edid++ == EdidHeader[i])
               checksum++;
       }
       if (checksum != 8) {
           return(1);
       }
    }
  
    if (EdidDataBlock->EdidVersion != 1 || EdidDataBlock->EdidRevision > 4) {
        return(1);
    }
  
    return(0);
}
*/


 
void
PrintDetailedTimingBlock(UINT8 *dtb)
{
    Print(L"Horizonal Image Size: %d mm\n", EDID_DET_TIMING_HSIZE(dtb));
    Print(L" Vertical Image Size: %d mm\n", EDID_DET_TIMING_VSIZE(dtb));
    Print(L"HoriImgSzByVertImgSz: %d\n", dtb[14]);
    Print(L"   Horizontal Border: %d\n", EDID_DET_TIMING_HBORDER(dtb));
    Print(L"     Vertical Border: %d\n", EDID_DET_TIMING_VBORDER(dtb));
}



CHAR16 *
ManufacturerAbbrev(UINT16 *ManufactureName)
{
    static CHAR16 Mcode[8];
    UINT8 *block = (UINT8 *)ManufactureName;
    UINT16 h = EDID_COMBINE_HI_8LO(block[0], block[1]);

    Mcode[0] = (CHAR16)((h>>10) & 0x1f) + 'A' - 1;
    Mcode[1] = (CHAR16)((h>>5) & 0x1f) + 'A' - 1;
    Mcode[2] = (CHAR16)(h & 0x1f) + 'A' - 1;
    Mcode[3] = (CHAR16)'\0';
 
    return Mcode;
}

void f2(UINT8 n) 
{
	if(n)
		f2(n/2);
	else return;
	Print(L"%d", n%2);
}

void
PrintEdid(EFI_EDID_DISCOVERED_PROTOCOL *Edid)
{
    EDID_DATA_BLOCK *EdidDataBlock = (EDID_DATA_BLOCK *)Edid->Edid; 
    UINT8 tmp;
	
 
	
	tmp = EdidDataBlock->VideoInputDefinition;

   
  
 
   /* PrintDetailedTimingBlock((UINT8 *)&(EdidDataBlock->DescriptionBlock1[0]));*/
}


void
PrintGeneralInfo(EFI_EDID_DISCOVERED_PROTOCOL *Edid)
{
	Print(L"=====================PrintGeneralInfo=====================\n");
    EDID_DATA_BLOCK *EdidDataBlock = (EDID_DATA_BLOCK *)Edid->Edid; 
	Print(L" Vendor Abbreviation: %s\n", ManufacturerAbbrev(&(EdidDataBlock->ManufactureName)));
    Print(L"          Product ID: %0X\n", EdidDataBlock->ProductCode);	
    Print(L"       Serial Number: %02X\n",  EdidDataBlock->SerialNumber);
	Print(L"    Manufacture Week: %d\n", EdidDataBlock->WeekOfManufacture);
    Print(L"    Manufacture Year: %d\n", EdidDataBlock->YearOfManufacture + 1990);
    Print(L"        EDID Version: %d\n", EdidDataBlock->EdidVersion);
    Print(L"       EDID Revision: %d\n",  EdidDataBlock->EdidRevision);
	Print(L"       EDID Checksum: %02X\n",  EdidDataBlock->Checksum);
	Print(L"     EDID Extensions: %02X\n",  EdidDataBlock->ExtensionFlag);

}

void 
PrintBasicDisplayParameters(EFI_EDID_DISCOVERED_PROTOCOL *Edid)
{
	Print(L"=====================PrintBasicDisplayParameters=====================\n");
	EDID_DATA_BLOCK *EdidDataBlock = (EDID_DATA_BLOCK *)Edid->Edid; 
	UINT8 tmp; 

	tmp = EdidDataBlock->VideoInputDefinition;
    Print(L"         Video Input: ");
    if (!CHECK_BIT(tmp, 7)) {
        Print(L"Analog\n");
		if (tmp & 0x1F) {
        	Print(L"      Syncronization: ");
        	if (CHECK_BIT(tmp, 4))
            	Print(L"BlankToBackSetup ");
        	if (CHECK_BIT(tmp, 3))
            	Print(L"SeparateSync ");
	        if (CHECK_BIT(tmp, 2))
    	        Print(L"CompositeSync ");
        	if (CHECK_BIT(tmp, 1))
            	Print(L"SyncOnGreen ");
	        if (CHECK_BIT(tmp, 0))
    	        Print(L"SerrationVSync ");
        	Print(L"\n");
    	}
    } else {
        Print(L"Digital\n");
		if (CHECK_BIT(tmp, 0))
			Print(L"                 [*]");
		else 
			Print(L"                 [ ]");
    	        Print(L"DFP 1.x compatible interface\n ");
    }
   
    Print(L"  Max Horizonal Size: %1d cm\n", EdidDataBlock->MaxHorizontalImageSize);
    Print(L"   Max Vertical Size: %1d cm\n", EdidDataBlock->MaxVerticalImageSize);

	tmp = EdidDataBlock->DpmSupport;
	Print(L"      Feature Support: ");
	if (CHECK_BIT(tmp, 7))
        Print(L"[*]Standby ");
	else 
		Print(L"[ ]Standby ");
	
   	if (CHECK_BIT(tmp, 6))
        Print(L"[*]Suspend ");
	else 
		Print(L"[ ]Suspend ");
	
	if (CHECK_BIT(tmp, 5))
    	Print(L"[*]ActiveOff ");
	else 
		Print(L"[ ]ActiveOff ");

	Print(L"\n");

	if (CHECK_BIT(tmp, 2))
        Print(L"                   ¡[ [*]sRGB ");
	else
		Print(L"                  ¡[¡¡[ ]sRGB ");
    if (CHECK_BIT(tmp, 1))
        Print(L"[*]Preferred timing mode ");
	else
		Print(L"[ ]Preferred timing mode ");
	
	if (CHECK_BIT(tmp, 0))
    	Print(L"[*]Default GTF supported ");
	else 
		Print(L"[ ]Default GTF supported ");
	Print(L"\n");
	
    Print(L"        Display Type: ");
    if (CHECK_BIT(tmp, 3) && CHECK_BIT(tmp, 4)) {
        Print(L"[ ]Monochrome  [ ]RGB color  [ ]Non-RGB multicolor  [*]Undefined  ");
    } else if (CHECK_BIT(tmp, 3)) {
        Print(L"[ ]Monochrome  [*]RGB color  [ ]Non-RGB multicolor  [ ]Undefined  ");
    } else if (CHECK_BIT(tmp, 4)) {
        Print(L"[ ]Monochrome  [ ]RGB color  [*]Non-RGB multicolor  [ ]Undefined  ");
    } else {
        Print(L"[*]Monochrome  [ ]RGB color  [ ]Non-RGB multicolor  [ ]Undefined  ");
    }
	Print(L"\n");
	
	
    Print(L"\n");
//	Print(L"               Gamma: %s\n", DisplayGammaString(EdidDataBlock->DisplayGamma));

	
}

/*
#string STR_720x400_70Hz	#language en-US "720 x 400 @ 70Hz[IBM,VGA]"
#string STR_720x400_88Hz	#language en-US "720 x 400 @ 88Hz[IBM,XGA2]"
#string STR_640x480_60Hz	#language en-US "640 x 480 @ 60Hz[IBM,VGA]"
#string STR_640x480_67Hz	#language en-US "640 x 480 @ 67Hz[Apple,mac]"
#string STR_640x480_72Hz	#language en-US "640 x 480 @ 72z[VESA]"
#string STR_640x480_75Hz	#language en-US "640 x 480 @ 75z[VESA]"
*/

UINT16 STR_720x400_70Hz[] = L"720 x 400 @ 70Hz[IBM,VGA]";
UINT16 STR_720x400_88Hz[] = L"720 x 400 @ 88Hz[IBM,XGA2]";
UINT16 STR_640x480_60Hz[] = L"640 x 480 @ 60Hz[IBM,VGA]";
UINT16 STR_640x480_67Hz[] = L"640 x 480 @ 67Hz[Apple,mac]";
UINT16 STR_640x480_72Hz[] = L"640 x 480 @ 72Hz[VESA]";
UINT16 STR_640x480_75Hz[] = L"640 x 480 @ 75Hz[VESA]";
UINT16 STR_800x600_56Hz[] = L"800 x 600 @ 56Hz[VESA]";

UINT16 STR_800x600_60Hz[] = L"800 x 600 @ 60Hz[VESA]";
UINT16 STR_800x600_72Hz[] = L"800 x 600 @ 72Hz[VESA]";
UINT16 STR_800x600_75Hz[] = L"800 x 600 @ 75Hz[VESA]";
UINT16 STR_832x624_75Hz[] = L"640 x 480 @ 75Hz[Apple,mac]";
UINT16 STR_1024x768_87Hz[] = L"1024 x 768 @ 87Hz[IBM]";
UINT16 STR_1024x768_60Hz[] = L"1024 x 768 @ 60Hz[VESA]";
UINT16 STR_1024x768_70Hz[] = L"1024 x 768 @ 70Hz[VESA]";

UINT16 STR_1024x768_75Hz[] = L"1024 x 768 @ 75Hz[VESA]";

UINT16 *STR_Timing_Array1[27] = {STR_720x400_70Hz, STR_720x400_88Hz, STR_640x480_60Hz, STR_640x480_67Hz,
								STR_640x480_72Hz, STR_640x480_75Hz, STR_800x600_56Hz};


UINT16 *STR_Timing_Array2[27] = {STR_800x600_60Hz, STR_800x600_72Hz, STR_800x600_75Hz, STR_832x624_75Hz,
								STR_1024x768_87Hz, STR_1024x768_60Hz, STR_1024x768_70Hz};


void 
PrintfSpace()
{
	Print(L"                      ");

}

void
PrintfColorAndEstablishedTimings(EFI_EDID_DISCOVERED_PROTOCOL *Edid)
{
	Print(L"=====================PrintfColorAndEstablishedTimings=====================\n");
	EDID_DATA_BLOCK *EdidDataBlock = (EDID_DATA_BLOCK *)Edid->Edid; 
	UINT8 tmp; 	
	
	Print(L"                RedX: %1d cm\n", EdidDataBlock->MaxVerticalImageSize);
	Print(L"                RedY: %1d cm\n", EdidDataBlock->MaxVerticalImageSize);
	Print(L"              GreenX: %1d cm\n", EdidDataBlock->MaxVerticalImageSize);
	Print(L"              GreenY: %1d cm\n", EdidDataBlock->MaxVerticalImageSize);
	Print(L"               BlueX: %1d cm\n", EdidDataBlock->MaxVerticalImageSize);
	Print(L"               BlueY: %1d cm\n", EdidDataBlock->MaxVerticalImageSize);
    Print(L"               WhiteX: %1d cm\n", EdidDataBlock->MaxVerticalImageSize);
    Print(L"               WhiteY: %1d cm\n", EdidDataBlock->MaxVerticalImageSize);
	
	
 	Print(L"               EstablishedTimings: %1d cm\n", EdidDataBlock->MaxVerticalImageSize);
    tmp = EdidDataBlock->EstablishedTimings[0];
	Print(L"%02X\n", tmp);

	for(UINT8 i = 0; i < 7; i ++) {
		if (CHECK_BIT(tmp, i)) {
			PrintfSpace();
			Print(STR_Timing_Array1[i]);
			Print(L"\n", tmp);
		}
	}

	tmp = EdidDataBlock->EstablishedTimings[1];
	for(UINT8 i = 0; i < 7; i ++) {
		if (CHECK_BIT(tmp, i)) {
			PrintfSpace();
			Print(STR_Timing_Array2[i]);
			Print(L"\n", tmp);
		}
	}
 	
	tmp = EdidDataBlock->EstablishedTimings[2];
	if (CHECK_BIT(tmp, 0)) {
			PrintfSpace();
			Print(STR_1024x768_75Hz);
			Print(L"\n", tmp);
	}
	///Print(L"%02X\n", tmp);
 


}


void 
PrintStandardTimings(EFI_EDID_DISCOVERED_PROTOCOL *Edid)
{
	Print(L"=====================PrintStandardTimings=====================\n");
	EDID_DATA_BLOCK *EdidDataBlock = (EDID_DATA_BLOCK *)Edid->Edid; 
	UINT8 tmp;
	Print(L"     Standard Timings: \n");
	for(UINT8 i = 0 ; i < 16; i = i + 2) {
		Print(L"     Timing ID #%d: \n", i/2 + 1);
		//H Actvie
		tmp = EdidDataBlock->StandardTimingIdentification[i];
		PrintfSpace();
		Print(L"H Active: %d\n", tmp);
		//Aspect Ratio
		PrintfSpace();
		Print(L"Aspect Ratio:");
		tmp = EdidDataBlock->StandardTimingIdentification[i+1];
		if(CHECK_BIT(tmp, 7) & CHECK_BIT(tmp, 6)) {
			Print(L"16:10");
		} else if(CHECK_BIT(tmp, 6)) {
			Print(L"4:3");
		} else if(CHECK_BIT(tmp, 7)) {
			Print(L"5:4");
		} else {
			Print(L"16:9");
		}
		Print(L"\n");
		//TODO 0~6 Refresh 
		
	}
}



UINT16 GeneralInfo[] = L"1. Show General Info\n";
UINT16 BasicDisplayParameters[] = L"2. Show Basic Display Parameters\n";
UINT16 ColorAndEstablishedTimings[] = L"3. Show Color&EstablishedTimings\n";
UINT16 StandardTimings[] = L"4. Show PrintStandardTimings\n";

void
PrintMainMenu(EFI_EDID_DISCOVERED_PROTOCOL *Edid)
{
		EFI_INPUT_KEY Key;
    	UINTN index = 0;
		EFI_STATUS Status;
	    Print(GeneralInfo);
		Print(BasicDisplayParameters);
		Print(ColorAndEstablishedTimings);
		Print(StandardTimings);
		
		Status = gBS->WaitForEvent(1, &gST->ConIn->WaitForKey, &index);
		Status = gST->ConIn->ReadKeyStroke(gST->ConIn, &Key);
		Print(L"ScanCode=%d, UnicodeChar=%c\n", Key.ScanCode, Key.UnicodeChar);
		switch(Key.UnicodeChar) {
			case '1':
				PrintGeneralInfo(Edid);
				break;
			case '2':
				PrintBasicDisplayParameters(Edid);
				break;
			case '3':
				PrintfColorAndEstablishedTimings(Edid);
				break;
			case '4':
				PrintStandardTimings(Edid);
			case 'Q':
				return;
				break;
			default:
				Print(L"Please enter the right key!\n");
				break;
	
		}

}


EFI_STATUS GetEdidInfo()
{
	EFI_STATUS Status;
	UINTN HandleCount, HandleIndex, i;
	EFI_HANDLE *EDIDHandleBuffer = NULL;
	EFI_EDID_DISCOVERED_PROTOCOL * EDID;
	

//	EFI_HANDLE HiiHandle = HiiAddPackages(IN CONST EFI_GUID * PackageListGuid,IN EFI_HANDLE DeviceHandle OPTIONAL,...)
	
	
	
	Status = gBS->LocateHandleBuffer(ByProtocol, &gEfiEdidDiscoveredProtocolGuid, NULL, &HandleCount, &EDIDHandleBuffer);
	if(!EFI_ERROR(Status)) {
		for(HandleIndex = 0; HandleIndex < HandleCount; HandleIndex ++) {
			Status = gBS->OpenProtocol(EDIDHandleBuffer[HandleIndex],
				&gEfiEdidDiscoveredProtocolGuid,
				((VOID**)&EDID),
				gImageHandle,
				NULL,
				EFI_OPEN_PROTOCOL_GET_PROTOCOL
				);
			
			if(EFI_ERROR(Status)) {
				Print(L"ERROR£ºOpen Edid Protocol failed!\n");
				return FALSE;
			}
			Print(L"EDID->SizeOfEdid=%d\n", EDID->SizeOfEdid);
			for(i = 0; i < 128; i ++) {
				Print(L"%02X ", EDID->Edid[i]);
				if((i+1) % 16 == 0)
					Print(L"\n");
				else if((i+1) % 8 ==0) 
					Print(L"- ");
			}
			Print(L"\n\n");
//			PrintEdid(EDID);
			
			//PrintGeneralInfo(EDID);
			
			//PrintBasicDisplayParameters(EDID);
			
			//PrintfColorAndEstablishedTimings(EDID);
			PrintMainMenu(EDID);
			Status = gBS->CloseProtocol(EDIDHandleBuffer[HandleIndex], 
				&gEfiEdidDiscoveredProtocolGuid, gImageHandle, NULL);
			
			if (EFI_ERROR (Status)) {
		        Print(L"ERROR : Close EDID device handle failed.\n");   
        		return FALSE;
      	    }
				
		}
		return EFI_SUCCESS;   
	} else {
		Print(L"ERROR : Can't get EdidDiscoveredProtocol.\n");   
        return FALSE;
	}
    
}

int EFIAPI main(IN int Argc, IN CHAR16 **Argv)
{
	GetEdidInfo();
	return EFI_SUCCESS;
}


































