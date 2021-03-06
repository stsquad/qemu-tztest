/*
 * Copyright (c) 2015 Linaro Limited
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms and conditions of the GNU General Public License,
 * version 2 or later, as published by the Free Software Foundation.
 *
 * This program is distributed in the hope it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#define __ASSEMBLY__
#include "cpu.h"
#include "exception.h"
#undef __ASSEMBLY__

.section .vectors
.align 12
.global el1_vectors
el1_vectors:
	b	.					/* Unused			*/
	b	el1_undef_vect	    /* Undefined instr	*/
	b	el1_svc_vect 		/* System call	    */
	b	el1_iabort_vect 	/* Prefetch abort 	*/
	b	el1_dabort_vect 	/* Data abort 		*/
	b	.					/* Unused			*/
	b	.					/* IRQ				*/
	b	.       			/* FIQ				*/

el1_svc_vect:
	srsdb sp!, #CPSR_M_SVC
    push {r1-r3}
	mov r2, #0                      /* There is no FAR on an SVC */
	ldr r3, [sp, #16]               /* Jump back over the saved args for LR */
    bl el1_handle_svc
    pop {r1-r3}
    rfefd sp!

el1_iabort_vect:
	srsdb sp!, #CPSR_M_ABT
    push {r0-r3}
	mov r0, #EC_IABORT				/* Treat ARMv7 iaborts as at same level */
	mrc p15, 0, r1, c5, c0, 1		/* ISS = IFSR on ARMv7 */
	mrc p15, 0, r2, c6, c0, 2		/* IFAR */
	ldr r3, [sp, #16]               /* Jump back over the saved args for LR */
    bl el1_handle_exception
    pop {r0-r3}
    rfefd sp!

el1_dabort_vect:
	srsdb sp!, #CPSR_M_ABT
    push {r0-r3}
	mov r0, #EC_DABORT				/* Treat ARMv7 iaborts as at same level */
	mrc p15, 0, r1, c5, c0, 0		/* ISS = DFSR on ARMv7 */
	mrc p15, 0, r2, c6, c0, 0		/* DFAR */
	ldr r3, [sp, #16]               /* Jump back over the saved args for LR */
    bl el1_handle_exception
    pop {r0-r3}
    rfefd sp!

el1_undef_vect:
	srsdb sp!, #CPSR_M_UND
    push {r0-r3}
	mov r0, #EC_UNKNOWN				/* Treat ARMv7 iaborts as at same level */
	mrc p15, 0, r1, c5, c0, 0		/* ISS = DFSR on ARMv7 */
	mrc p15, 0, r2, c6, c0, 0		/* DFAR */
	ldr r3, [sp, #16]               /* Jump back over the saved args for LR */
    bl el1_handle_exception
    pop {r0-r3}
    rfefd sp!
