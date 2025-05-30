/* SPDX-License-Identifier: GPL-2.0 */

#ifndef __ASM_SYSCALL_H
#define __ASM_SYSCALL_H

#include <linux/sched.h>
#include <linux/err.h>
#include <abi/regdef.h>
#include <uapi/linux/audit.h>

extern void *sys_call_table[];

static inline int
syscall_get_nr(struct task_struct *task, struct pt_regs *regs)
{
	return regs_syscallid(regs);
}

static inline void
syscall_set_nr(struct task_struct *task, struct pt_regs *regs,
	       int sysno)
{
	regs_syscallid(regs) = sysno;
}

static inline void
syscall_rollback(struct task_struct *task, struct pt_regs *regs)
{
	regs->a0 = regs->orig_a0;
}

static inline long
syscall_get_error(struct task_struct *task, struct pt_regs *regs)
{
	unsigned long error = regs->a0;

	return IS_ERR_VALUE(error) ? error : 0;
}

static inline long
syscall_get_return_value(struct task_struct *task, struct pt_regs *regs)
{
	return regs->a0;
}

static inline void
syscall_set_return_value(struct task_struct *task, struct pt_regs *regs,
		int error, long val)
{
	regs->a0 = (long) error ?: val;
}

static inline void
syscall_get_arguments(struct task_struct *task, struct pt_regs *regs,
		      unsigned long *args)
{
	args[0] = regs->orig_a0;
	args++;
	memcpy(args, &regs->a1, 5 * sizeof(args[0]));
}

static inline void
syscall_set_arguments(struct task_struct *task, struct pt_regs *regs,
		      const unsigned long *args)
{
	memcpy(&regs->a0, args, 6 * sizeof(regs->a0));
	/*
	 * Also copy the first argument into orig_a0
	 * so that syscall_get_arguments() would return it
	 * instead of the previous value.
	 */
	regs->orig_a0 = regs->a0;
}

static inline int
syscall_get_arch(struct task_struct *task)
{
	return AUDIT_ARCH_CSKY;
}

#endif	/* __ASM_SYSCALL_H */
