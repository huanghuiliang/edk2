#
#  Copyright (c) 2014, ARM Limited. All rights reserved.
#
#  This program and the accompanying materials are licensed and made available
#  under the terms and conditions of the BSD License which accompanies this
#  distribution. The full text of the license may be found at
#  http://opensource.org/licenses/bsd-license.php
#
#  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
#  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.
#
#

#include <AsmMacroIoLibV8.h>

#if !defined(__clang__)

//
// Clang versions before v3.6 do not support the GNU extension that allows
// system registers outside of the IMPLEMENTATION DEFINED range to be specified
// using the generic notation below. However, clang knows these registers by
// their architectural names, so it has no need for these aliases anyway.
//
#define ICC_SRE_EL1             S3_0_C12_C12_5
#define ICC_SRE_EL2             S3_4_C12_C9_5
#define ICC_SRE_EL3             S3_6_C12_C12_5
#define ICC_IGRPEN1_EL1         S3_0_C12_C12_7
#define ICC_EOIR1_EL1           S3_0_C12_C12_1
#define ICC_IAR1_EL1            S3_0_C12_C12_0
#define ICC_PMR_EL1             S3_0_C4_C6_0
#define ICC_BPR1_EL1            S3_0_C12_C12_3

#endif

//UINT32
//EFIAPI
//ArmGicV3GetControlSystemRegisterEnable (
//  VOID
//  );
ASM_FUNC(ArmGicV3GetControlSystemRegisterEnable)
        EL1_OR_EL2_OR_EL3(x1)
1:  mrs x0, ICC_SRE_EL1
        b       4f
2:  mrs x0, ICC_SRE_EL2
        b       4f
3:  mrs x0, ICC_SRE_EL3
4:  ret

//VOID
//EFIAPI
//ArmGicV3SetControlSystemRegisterEnable (
//  IN UINT32         ControlSystemRegisterEnable
//  );
ASM_FUNC(ArmGicV3SetControlSystemRegisterEnable)
        EL1_OR_EL2_OR_EL3(x1)
1:  msr ICC_SRE_EL1, x0
        b       4f
2:  msr ICC_SRE_EL2, x0
        b       4f
3:  msr ICC_SRE_EL3, x0
4:  isb
        ret

//VOID
//ArmGicV3EnableInterruptInterface (
//  VOID
//  );
ASM_FUNC(ArmGicV3EnableInterruptInterface)
        mov     x0, #1
        msr     ICC_IGRPEN1_EL1, x0
        ret

//VOID
//ArmGicV3DisableInterruptInterface (
//  VOID
//  );
ASM_FUNC(ArmGicV3DisableInterruptInterface)
        mov     x0, #0
        msr     ICC_IGRPEN1_EL1, x0
        ret

//VOID
//ArmGicV3EndOfInterrupt (
//  IN UINTN          InterruptId
//  );
ASM_FUNC(ArmGicV3EndOfInterrupt)
        msr     ICC_EOIR1_EL1, x0
        ret

//UINTN
//ArmGicV3AcknowledgeInterrupt (
//  VOID
//  );
ASM_FUNC(ArmGicV3AcknowledgeInterrupt)
        mrs     x0, ICC_IAR1_EL1
        ret

//VOID
//ArmGicV3SetPriorityMask (
//  IN UINTN          Priority
//  );
ASM_FUNC(ArmGicV3SetPriorityMask)
        msr     ICC_PMR_EL1, x0
        ret

//VOID
//ArmGicV3SetBinaryPointer (
//  IN UINTN          BinaryPoint
//  );
ASM_FUNC(ArmGicV3SetBinaryPointer)
        msr     ICC_BPR1_EL1, x0
        ret
