
#define EFI_EDID_FIND_PROTOCOL_GUID \
  { \
    0x1c0c34f6, 0xd381, 0x41fa, {0xa0, 0x49, 0x8a, 0xd0, 0x6c, 0x1a, 0x66, 0xab } \
  }


typedef struct _EFI_EDID_FIND_PROTOCOL EFI_EDID_FIND_PROTOCOL;
typedef EFI_EDID_FIND_PROTOCOL EFI_EDID_FIND;

//为兼容EFI 1.1而定义的Protocol GUID名字
#define EDID_FIND_PROTOCOL EFI_EDID_FIND_PROTOCOL_GUID


typedef EFI_STATUS (EFIAPI *EFI_PRINT_EDID_VERSION)(
  );

struct _EFI_EDID_FIND_PROTOCOL {
    UINTN Revision;

    EFI_PRINT_EDID_VERSION PrintEdidVersion;
   // EFI_FUNCTION2 function2;
    //EFI_FUNCTION3 function3;
};

extern EFI_GUID gEfiEdidFindProtocolGuid;
