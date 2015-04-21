#include "libcflat.h"
#include "svc.h"
#include "smc.h"
#include "syscntl.h"
#include "builtins.h"
#include "exception.h"
#include "state.h"
#include "cpu.h"
#include "debug.h"
#include "tztest_internal.h"

uint32_t el1_check_smc(uint32_t __attribute__((unused))arg)
{
    uintptr_t scr;
    TEST_HEAD("smc behavior");

    SMC_GET_REG(SCR, 3, scr);
    SMC_SET_REG(SCR, 3, scr | SCR_SMD);
#ifdef AARCH64
    /* On AArch64, SMC calls below EL3 should result in an undefined exception
     * if the SCR.SMD bit is set.  This is regardless of whether EL2 is present
     * or not.
     */
    TEST_MSG("SMC call with SCR.SMD set");
    TEST_EL3_EXCEPTION(__smc(SMC_OP_NOOP, NULL), EC_UNKNOWN);
#else
    /* On AArch32, SMC calls are undefined when SCR.SCD is set only when the
     * virtualization extensions are present.
     */
    //TEST_EL1_EXCEPTION(__smc(SMC_OP_NOOP, NULL), EC_UNKNOWN);

    /* When the virtualization extensions are not present, the SCR.SCD bit
     * setting should have no impact on SMC.
     */
    TEST_MSG("SMC call without virt ext. and  SCR.SMD set");
    TEST_NO_EXCEPTION(__smc(SMC_OP_NOOP, NULL));

#endif

    /* Restor SCR */
    SMC_SET_REG(SCR, 3, scr);

    return 0;
}

#ifdef AARCH64
uint32_t el1_check_cpacr_trap(uint32_t __attribute__((unused))arg)
{
    uint64_t cptr_el3, cpacr;

    TEST_HEAD("CPACR trapping");

    /* Get the current CPTR so we can restore it later */
    SMC_GET_REG(CPTR_EL3, 3, cptr_el3);

    /* Disable CPACR access */
    SMC_SET_REG(CPTR_EL3, 3, cptr_el3 | CPTR_TCPAC);

    /* Try to read CPACR */
    TEST_MSG("Read of disabled CPACR");
    TEST_EL3_EXCEPTION(cpacr = READ_CPACR(), EC_SYSINSN);

    /* Try to write CPACR */
    TEST_MSG("Write of disabled CPACR");
    TEST_EL3_EXCEPTION(WRITE_CPACR(cpacr), EC_SYSINSN);

#ifdef FP_TEST
    /* Disable FP access */
    TEST_MSG("Read of disabled FP reg");
    SMC_SET_REG(CPTR_EL3, 3, cptr_el3 | CPTR_TFP);
    TEST_EL3_EXCEPTION(asm volatile("fcmp s0, #0.0\n"), EC_SIMD);
#endif

    /* Restore the original CPTR */
    SMC_SET_REG(CPTR_EL3, 3, cptr_el3);

    return 0;
}

uint32_t el1_check_wfx_trap(uint32_t __attribute__((unused))arg)
{
    uint64_t sctlr, scr;

    TEST_HEAD("WFx traps");

    /* Get the current SCR so we can restore it later */
    SMC_GET_REG(SCR, 3, scr);

    /* Get the current SCTLR so we can restore it later */
    sctlr = READ_SCTLR();

    /* Clearing SCTLR.nTWE normally traps WFE to EL1 but we are already there */
    WRITE_SCTLR(sctlr & ~SCTLR_nTWE);
    SMC_SET_REG(SCR, 3, scr & ~SCR_TWE);
    TEST_MSG("WFE (SCTLR.nTWE clear, SCR.WFE clear)");
    TEST_NO_EXCEPTION(asm volatile("wfe\n"));

    /* Trap WFE instructions to EL3.  This should work regardless od the
     * SCTLR.nTWE setting.
     */
    SMC_SET_REG(SCR, 3, scr | SCR_TWE);
    TEST_MSG("WFE (SCTLR.nTWE clear, SCR.WFE set)");
    TEST_EL3_EXCEPTION(asm volatile("wfe\n"), EC_WFI_WFE);

    /* This should trap to EL3 with SCTLR.nTWE set */
    WRITE_SCTLR(sctlr | SCTLR_nTWE);
    TEST_MSG("WFE (SCTLR.nTWE set, SCR.WFE set)");
    TEST_EL3_EXCEPTION(asm volatile("wfe\n"), EC_WFI_WFE);

    /* We cannot test the effect of WFI in EL1 mode like we did with WFE as it
     * causes a hang.  It is assumed that since the exception is not trapped we
     * actually execute the instruction.
     * For this reason we don't bother to test the SCTLR bit effect or
     * precedence.
     */

    /* Trap WFI instructions to EL3.  This should work regardless od the
     * SCTLR.nTWE setting.
     */
    SMC_SET_REG(SCR, 3, scr | SCR_TWI);
    TEST_MSG("WFI (SCTLR.nTWI clear, SCR.WFI set)");
    TEST_EL3_EXCEPTION(asm volatile("wfi\n"), EC_WFI_WFE);

    /* This should trap to EL3 with SCTLR.nTWE set */
    WRITE_SCTLR(sctlr | SCTLR_nTWE);
    TEST_MSG("WFI (SCTLR.nTWE set, SCR.WFI set)");
    TEST_EL3_EXCEPTION(asm volatile("wfi\n"), EC_WFI_WFE);

    /* Restore SCTLR */
    WRITE_SCTLR(sctlr);

    /* Restore SCR */
    SMC_SET_REG(SCR, 3, scr);

    return 0;
}
#endif

