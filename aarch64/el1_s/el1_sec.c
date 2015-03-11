#include "platform.h"
#include "arm_builtins.h"
#include "libcflat.h"
#include <stdint.h>
#include "smc.h"

extern void el1_map_pa(uintptr_t vaddr, uintptr_t paddr);
void el1_sec_smc_loop()
{
    smc_op_desc_t *desc = smc_interop_buf;
    printf("EL1_S: In loop\n");

    while (desc->op != SMC_OP_EXIT) {
        switch (desc->op) {
        case SMC_OP_MAP:
            printf("EL1_S: Doing a MAP desc = %p\n", desc);
            el1_map_pa((uintptr_t)(desc->map.va), (uintptr_t)(desc->map.pa));
            break;
        case SMC_OP_YIELD:
            printf("EL1_S: Doing a YIELD desc = %p\n", desc);
            break;
        default:
            printf("Unrecognized SMC opcode %d.  Exiting ...\n", desc->op);
            SMC_EXIT();
            break;
        }

        SMC_YIELD();
    }

    SMC_EXIT();
}

void el1_init_el0()
{
    SMC_YIELD();

    el1_sec_smc_loop();
}
