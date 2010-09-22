/**
 *
 * Phantom OS
 *
 * Copyright (C) 2005-2010 Dmitry Zavalishin, dz@dz.ru
 *
 * ia32 traps support. TODO Must go to ARCH dir!
 *
**/

#include <i386/trap.h>
#include <i386/proc_reg.h>
#include <i386/eflags.h>
#include <phantom_assert.h>
#include <phantom_libc.h>

static char *trap_type[] = {
    "Divide error", "Debug trap",		"NMI",		"Breakpoint",
    "Overflow",	    "Bounds check",		"Invalid opcode","No coprocessor",
    "Double fault", "Coprocessor overrun",	"Invalid TSS",	"Segment not present",
    "Stack bounds", "General protection",	"Page fault",	"(reserved)",
    "Coprocessor error"
};
#define TRAP_TYPES (sizeof(trap_type)/sizeof(trap_type[0]))

const char *trap_name(unsigned int trapnum)
{
    return trapnum < TRAP_TYPES ? trap_type[trapnum] : "(unknown)";
}







extern int panic_reenter;



void dump_ss(struct trap_state *st)
{
    if(panic_reenter > 1)
        return;

    int from_user = (st->cs & 3) || (st->eflags & EFL_VM);

    printf("Dump of i386 state:\n", st);
    printf("EAX %08x EBX %08x ECX %08x EDX %08x\n",
           st->eax, st->ebx, st->ecx, st->edx);
    printf("ESI %08x EDI %08x EBP %08x ESP %08x\n",
           st->esi, st->edi, st->ebp,
           from_user ? st->esp : (unsigned)&st->esp
          );
    printf("CS %04x SS %04x DS %04x ES %04x FS %04x GS %04x\n",
           st->cs & 0xffff, from_user ? st->ss & 0xffff : get_ss(),
           st->ds & 0xffff, st->es & 0xffff,
           st->fs & 0xffff, st->gs & 0xffff);
    if (st->eflags & EFL_VM)
    {
        printf("v86:            DS %04x ES %04x FS %04x GS %04x\n",
               st->v86_ds & 0xffff, st->v86_es & 0xffff,
               st->v86_gs & 0xffff, st->v86_gs & 0xffff);
    }
    printf("EIP %08x EFLAGS %08x\n", st->eip, st->eflags);
    printf("trapno %d: %s, error %08x\n",
           st->trapno, trap_name(st->trapno),
           st->err);
    if (st->trapno == T_PAGE_FAULT)
        printf("page fault linear address %08x\n", st->cr2);
    //if(!from_user)
        stack_dump_ebp((void *)st->ebp);
}







static int trap_ignore(struct trap_state *ts)
{
    (void)ts;
    return 0;
}

//static
int trap_panic(struct trap_state *ts)
{
    dump_ss(ts);
    int type = ts->trapno;

    panic("Unexpected trap %s\n", trap_name(type));

    return -1;
}


/**
 *
 * If trap handler returns nonzero, panic will be called.
 *
**/

int (*phantom_trap_handlers[I386_N_TRAPS])(struct trap_state *ts) = {
	trap_panic, /* 0 */
	trap_panic,
	trap_panic,
	trap_panic,
	trap_panic,
	trap_panic,
	trap_panic,
	trap_panic, /* 7 */
	trap_panic,
	trap_panic,
	trap_panic,
	trap_panic,
	trap_panic,
	trap_panic,
	trap_panic,
	trap_ignore,  // 15 spurios traps, not really used
	trap_panic,
	trap_panic,
	trap_panic,
	trap_panic,
	trap_panic,
	trap_panic,
	trap_panic,
	trap_panic, /* 23 */
	trap_panic,
	trap_panic,
	trap_panic,
	trap_panic,
	trap_panic,
	trap_panic,
	trap_panic,
	trap_panic  /* 31 */
};

