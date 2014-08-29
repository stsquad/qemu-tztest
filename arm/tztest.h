#ifndef _TZTEST_H
#define _TZTEST_H

#include "tztest_asm.h"
#include "tztest_builtins.h"
#include "tztest_mmu.h"
#include "sm.h"
#include "libcflat.h"

typedef struct {
    uint32_t (*func)(uint32_t);
    uint32_t arg;
    uint32_t ret;
} tztest_dispatch_t;

typedef struct {
    union {
        tztest_dispatch_t dispatch;
        struct {
            uint32_t reg;
            uint32_t val;
        } reg_read;
        struct {
            uint32_t state;
        } secure_state;
    };
} tztest_svc_desc_t;

#define CLEAR_SVC_DESC(_desc) memset(&(_desc), sizeof(tztest_svc_desc_t), 0)

typedef struct {
    union {
        tztest_dispatch_t dispatch;
    };
} tztest_smc_desc_t;

#ifdef DEBUG
#define DEBUG_MSG(_str, ...) \
    printf("\n[DEBUG] %s: " _str, __FUNCTION__, ##__VA_ARGS__)
#define DEBUG_ARG
#else
#define DEBUG_MSG(_str, ...)
#define DEBUG_ARG __attribute__ ((unused))
#endif

#define MODE_STR(_mode)             \
    ((_mode == MON) ? "MON" :        \
     (_mode == SVC) ? "SVC" :        \
     (_mode == SYS) ? "SYS" :        \
     (_mode == USR) ? "USR" : "Unknown")


#define FAULT_STR(_s) \
    ((0x01 == (_s)) ? "Alignment fault" :                   \
     (0x02 == (_s)) ? "Debug event" :                       \
     (0x03 == (_s)) ? "Access flag fault" :                 \
     (0x04 == (_s)) ? "Instruction cache maintenance" :     \
     (0x05 == (_s)) ? "Translation fault" :                 \
     (0x06 == (_s)) ? "Access flag fault" :                 \
     (0x07 == (_s)) ? "Translation fault" :                 \
     (0x08 == (_s)) ? "Synchronous external abort" :        \
     (0x09 == (_s)) ? "Domain fault" :                      \
     (0x0b == (_s)) ? "Domain fault" :                      \
     (0x0c == (_s)) ? "External abort on table walk" :      \
     (0x0d == (_s)) ? "Permisison fault" :                  \
     (0x0e == (_s)) ? "Synchronous external abort" :        \
     (0x0f == (_s)) ? "Permisison fault" :                  \
     (0x10 == (_s)) ? "TLB conflict abort" :                \
     (0x1c == (_s)) ? "External parity err on table walk" : \
     (0x1e == (_s)) ? "External parity err on table walk" : \
     "Unknown")

#define CALL(_f)  __svc(0, _f)
#define RETURN(_r)  __svc(0,(_r))

#define DISPATCH(_op, _func, _arg, _ret)    \
    do {                                    \
        tztest_svc_desc_t _desc;            \
        _desc.dispatch.func = (_func);      \
        _desc.dispatch.arg = (_arg);        \
        __svc((_op), &_desc);               \
        (_ret) = _desc.dispatch.ret;        \
    } while(0)

#define SECURE_USR_FUNC(_func)  \
    uint32_t _func##_wrapper(uint32_t arg) { RETURN(_func(arg)); return 0; }

#define DISPATCH_SECURE_USR(_func, _arg, _ret)            \
        DISPATCH(SVC_DISPATCH_SECURE_USR, (_func##_wrapper), (_arg), (_ret))
#define DISPATCH_SECURE_SVC(_func, _arg, _ret)            \
        DISPATCH(SVC_DISPATCH_SECURE_SVC, (_func), (_arg), (_ret))
#define DISPATCH_MONITOR(_func, _arg, _ret)            \
        DISPATCH(SVC_DISPATCH_MONITOR, (_func), (_arg), (_ret))
#define DISPATCH_NONSECURE_SVC(_func, _arg, _ret)            \
        DISPATCH(SVC_DISPATCH_NONSECURE_SVC, (_func), (_arg), (_ret))

#define INC_TEST_COUNT()    (*tztest_test_count += 1)
#define INC_FAIL_COUNT()    (*tztest_fail_count += 1)

#define TEST_CONDITION(_cond)                           \
    do {                                                \
        if (!(_cond)) {                                 \
            printf("FAILED\n");                         \
            INC_FAIL_COUNT();                           \
        } else {                                        \
            printf("PASSED\n");                         \
        }                                               \
        INC_TEST_COUNT();                               \
    } while(0)

#define TEST_FUNCTION(_fn, _cond)                       \
    do {                                                \
        _fn;                                            \
        TEST_CONDITION(_cond);                          \
    } while(0)

#define TEST_EXCEPTION(_fn, _excp)                      \
    do {                                                \
        TEST_FUNCTION(_fn, *tztest_exception == (_excp));   \
        *tztest_exception = 0;                          \
    } while (0)

#define TZTEST_SVAL 0xaaaaaaaa
#define TZTEST_NSVAL ~TZTEST_SVAL
#define TZTEST_GET_REG_SECURE_BANK(_reg, _val)      \
    do {                                            \
        _write_scr(scr & ~SCR_NS);                  \
        (_val) = _read_##_reg();                    \
    } while(0)

#define TZTEST_GET_REG_NONSECURE_BANK(_reg, _val)   \
    do {                                            \
        _write_scr(scr | SCR_NS);                   \
        (_val) = _read_##_reg();                    \
    } while(0)

#define TZTEST_SET_REG_SECURE_BANK(_reg, _val)      \
    do {                                            \
        _write_scr(scr & ~SCR_NS);                  \
        _write_##_reg(_val);                        \
    } while(0)

#define TZTEST_SET_REG_NONSECURE_BANK(_reg, _val)   \
    do {                                            \
        _write_scr(scr | SCR_NS);                   \
        _write_##_reg(_val);                        \
    } while(0)

#define TZTEST_GET_REG_BANKS(_reg, _sval, _nsval)   \
    do {                                            \
        TZTEST_GET_REG_SECURE_BANK(_reg, _sval);    \
        TZTEST_GET_REG_NONSECURE_BANK(_reg, _nsval);\
    } while(0)

#define TZTEST_SET_REG_BANKS(_reg, _sval, _nsval)   \
    do {                                            \
        TZTEST_SET_REG_SECURE_BANK(_reg, _sval);    \
        TZTEST_SET_REG_NONSECURE_BANK(_reg, _nsval);\
    } while(0)

#define VERIFY_REGISTER_CUSTOM(_reg, _mask, _sval, _nsval)              \
    do {                                                                \
        uint32_t sval = 0, nsval = 0;                                   \
        uint32_t _reg[2] = {0,0};                                       \
        printf("\tChecking %s banks... ", #_reg);                       \
        TZTEST_GET_REG_BANKS(_reg, _reg[!SCR_NS], _reg[SCR_NS]);        \
        TZTEST_SET_REG_BANKS(_reg, (_sval), (_nsval));                  \
        TZTEST_GET_REG_SECURE_BANK(_reg, sval);                         \
        TZTEST_GET_REG_NONSECURE_BANK(_reg, nsval);                     \
        TEST_CONDITION(((sval & (_mask)) != (nsval & (_mask))) &&       \
                       (((_sval) & (_mask)) == (sval & (_mask))) &&     \
                       (((_nsval) & (_mask)) == (nsval & (_mask))));    \
        TZTEST_SET_REG_BANKS(_reg, _reg[!SCR_NS], _reg[SCR_NS]);        \
    } while(0)

#define VERIFY_REGISTER(_reg)    \
    VERIFY_REGISTER_CUSTOM(_reg, 0xFFFFFFFF, TZTEST_SVAL, TZTEST_NSVAL)

extern volatile int *tztest_exception;
extern volatile int *tztest_exception_addr;
extern volatile int *tztest_exception_status;
extern volatile int *tztest_fail_count;
extern volatile int *tztest_test_count;
extern void validate_state(uint32_t, uint32_t);
extern uint32_t _shared_memory_heap_base;
extern uint32_t *nsec_l1_page_table;
extern uint32_t *nsec_l2_page_table;

#endif
