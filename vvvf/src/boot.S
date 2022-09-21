@ AArch32

.section ".text.boot"

@ Entry point for the kernel.
@ r15 = should begin execution at 0x8000
@ r0  = 0x00000000
@ r1  = 0x00000C42 = machine id
@ r2  = 0x00000100 = start of ATAGS
@ Preserve these registers as arguments for kernel main.
.global start
start:
	@ Enable L1 cache.
	@ r0 = System Control Register.
	mrcal	p15, #0, r0, c1, c0, #0
	@ Enable caches and branch prediction
	orral	r0, #0x0800	@ Enable branch prediction.
	orral	r0, #0x04	@ Enable data cache.
	orral	r0, #0x1000	@ Enable instruction cache.
	@ System Control Register = r0.
	mcral	p15, #0, r0, c1, c0, #0

	@ Enable VFP.
	@ r1 = Access Control Register
	mrcal	p15, #0, r1, c1, c0, #2
	@ Enable full access for p10, p11.
	orral	r1, r1, #(0xf << 20)
	@ Access Control Register = r1.
	mcral	p15, #0, r1, c1, c0, #2
	moval	r1, #0
	@ Flush prefetch buffer because of FMXR below and CP 10 & 11 were only just enabled.
	mcral	p15, #0, r1, c7, c5, #4
	@ Enable VFP itself.
	moval	r0, #0x40000000
	@ fpexc = r0.
	fmxral	fpexc, r0

	@ Setup a basic C environment (stack + zero'd BSS).

	@ Stack will live before the code.
	moval	sp, #0x8000

	@ Zero-out BSS.
	ldral	r4, =__bss_start
	ldral	r9, =__bss_end
	@ Prepare 4 words of 0's.
	moval	r5, #0
	moval	r6, #0
	moval	r7, #0
	moval	r8, #0
	bss_zero_next:
		@ Write 4 words at a time.
		stmia	r4!, {r5-r8}
		addal	r4, r4, #16
		cmpal	r4, r9
		bge	bss_zero_done
		bal	bss_zero_next
	bss_zero_done:

	@ Create a vector table at address per ARM of ARMv6.
	@ 1. Copy the same branch instruction for the number of vectors
		moval	r3, #64			@ Where to store vector IRQ offsets.
		moval	r4, #0			@ Vector table address in memory.
		ldral	r5, =vector_start	@ Address to the new vector table data.
		ldral	r6, =vector_end
		ldral	r7, =vector_ins
	vec_cpy_next:
		@ Copy word from new table into regs then copy that to the vector table.
		ldral	r8, [r5]		@ 1. Save IRQ handler offset close to the table.
		stral	r8, [r3]
		ldral	r8, [r7]		@ 2. Create a branch instruction that uses the handler offset.
		stral	r8, [r4]
		addal	r3, r3, #4
		addal	r4, r4, #4
		addal	r5, r5, #4
		cmpal	r5, r6
		bge		vec_cpy_done
		b		vec_cpy_next
	vec_cpy_done:

	@ Branch to C kernel.
	ldral	r3, =kernel_main
	blxal	r3
	@ Kernel should not return but if it does go to panic.
	bal	panic
