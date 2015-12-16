	.cpu arm9tdmi
	.fpu softvfp
	.eabi_attribute 20, 1
	.eabi_attribute 21, 1
	.eabi_attribute 23, 3
	.eabi_attribute 24, 1
	.eabi_attribute 25, 1
	.eabi_attribute 26, 2
	.eabi_attribute 30, 2
	.eabi_attribute 18, 4
	.file	"monte.c"
	.global	__aeabi_i2d
	.global	__aeabi_dmul
	.global	__aeabi_dsub
	.global	__aeabi_dadd
	.global	__aeabi_dcmplt
	.global	__aeabi_ui2d
	.global	__aeabi_ddiv
	.text
	.align	2
	.global	calcPi
	.type	calcPi, %function
calcPi:
	@ Function supports interworking.
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	stmfd	sp!, {r4, r5, r6, r7, r8, sl, fp, lr} /* save reg.file state to stack */
	subs	r7, r0, #0 /* === mov r7, r0; r0 === N => r7 === N */
	moveq	r8, r7
	beq	.L3
	mov	r4, #0 /* cycle counter */
	mov	r8, r4 /* r8 == M */
.L6:
	bl	rand /* r0 <- rand() */
	bl	__aeabi_i2d /* cast int2double */
	/* r0,r1 -- (double)rand()
	r2,r3 -- const */
	mov	r2, #2097152
	ldr	r3, .L11 
	bl	__aeabi_dmul
	/* r0, r1 -- mul. result 
	r2, r3 -- 1.0 */
	mov	r2, #0
	ldr	r3, .L11+4
	bl	__aeabi_dsub
	/* r0, r1 -- x value */
	mov	r6, r1
	mov	r5, r0 /* copy x value to r5,r6 */
	bl	rand
	bl	__aeabi_i2d
	mov	r2, #2097152
	ldr	r3, .L11
	bl	__aeabi_dmul
	mov	r2, #0
	ldr	r3, .L11+4
	bl	__aeabi_dsub
	/* r0, r1 -- y value
	   r5, r6 -- x value */

    /* backup y value */
	mov	sl, r0
	mov	fp, r1

    /* x*x */
	mov	r2, r5
	mov	r3, r6
	mov	r0, r5
	mov	r1, r6
	bl	__aeabi_dmul

	mov	r5, r0
	mov	r6, r1
    /* r5,r6 == x*x */

	mov	r2, sl
	mov	r3, fp
	mov	r0, sl
	mov	r1, fp
	bl	__aeabi_dmul
	/* r0,r1 == y*y */

	mov	r2, r0
	mov	r3, r1
	mov	r0, r5
	mov	r1, r6
	bl	__aeabi_dadd
	/* r0, r1 == x*x+y*y */

	mov	r2, #0
	ldr	r3, .L11+4
	/* r0,r1 == xx+yy
	r2,r3 == 1.0 */
	bl	__aeabi_dcmplt /* compare less than */
	add	r4, r4, #1 /* r4 -- cycle counter */
	cmp	r0, #0 /* r0 -- comare result, 0 === Not less than */
	addne	r8, r8, #1 /* inc M */
	cmp	r7, r4 /* compare N and cycle counter */
	bhi	.L6 /* next iteration if N higher than cycle counter */
.L3:
	mov	r0, r8
	bl	__aeabi_ui2d
	mov	r2, #0
	ldr	r3, .L11+8
	bl	__aeabi_dmul
    /* r0,r1 -- 4.0 * (double)M */

	mov	r4, r0
	mov	r0, r7
	mov	r5, r1
	bl	__aeabi_ui2d
	/* r0,r1 -- (double)N
	r4,r5 -- 4.0 * (double)M */

	mov	r2, r0
	mov	r3, r1
	mov	r0, r4
	mov	r1, r5
	bl	__aeabi_ddiv
	/* r0,r1 -- result (PI) */
	ldmfd	sp!, {r4, r5, r6, r7, r8, sl, fp, lr} /* restore reg.file state from stack */
	bx	lr
.L12:
	.align	2
.L11:
	.word	1041235968
	.word	1072693248
	.word	1074790400
	.size	calcPi, .-calcPi
	.align	2
	.global	main
	.type	main, %function
main:
	@ Function supports interworking.
	@ args = 0, pretend = 0, frame = 24
	@ frame_needed = 0, uses_anonymous_args = 0
	stmfd	sp!, {r4, r5, r6, r7, r8, sl, fp, lr}
	cmp	r0, #3 /* r0 == argc */
	sub	sp, sp, #32 /* резервируем место под лок. переменные в стеке */
	mov	r4, r1 /* r4 == argv */
	bne	.L23 /* branch if 3 != argc */
	ldr	r5, .L24 /* &stdout */
	ldr	r1, .L24+4 /* "w" */
	ldr	r2, [r5, #0] /* *(&stdout) */
	ldr	r0, [r4, #8] /* argv[2] */
	bl	freopen /* bl -- пишем в lr адрес возврата */
	add	r2, sp, #28 /* [sp, #28] ==  N */
	ldr	r0, [r4, #4] /* argv[1] */
	ldr	r1, .L24+8 /* "%u" */
	add	r4, sp, #20 /* [sp, #20] == start */
	bl	sscanf
	mov	r1, r4
	mov	r0, #4
	bl	clock_getres
	ldr	r1, [sp, #24] /* [sp, #24] == start.tv_nsec */
	ldr	r0, .L24+12
	bl	printf
	ldr	r0, [r5, #0]
	bl	fflush
	mov	r1, r4
	mov	r0, #4
	bl	clock_gettime
	ldr	r7, [sp, #28]
	cmp	r7, #0
	moveq	r8, r7
	beq	.L16 /* N == 0 => goto .L16 */
	mov	r4, #0
	mov	r8, r4
.L19:
	bl	rand
	bl	__aeabi_i2d
	mov	r2, #2097152
	ldr	r3, .L24+16
	bl	__aeabi_dmul
	mov	r2, #0
	ldr	r3, .L24+20
	bl	__aeabi_dsub
	mov	r6, r1
	mov	r5, r0
	bl	rand
	bl	__aeabi_i2d
	mov	r2, #2097152
	ldr	r3, .L24+16
	bl	__aeabi_dmul
	mov	r2, #0
	ldr	r3, .L24+20
	bl	__aeabi_dsub
	mov	r2, r5
	mov	sl, r0
	mov	fp, r1
	mov	r3, r6
	mov	r0, r5
	mov	r1, r6
	bl	__aeabi_dmul
	mov	r2, sl
	mov	r5, r0
	mov	r6, r1
	mov	r3, fp
	mov	r0, sl
	mov	r1, fp
	bl	__aeabi_dmul
	mov	r2, r0
	mov	r3, r1
	mov	r0, r5
	mov	r1, r6
	bl	__aeabi_dadd
	mov	r2, #0
	ldr	r3, .L24+20
	bl	__aeabi_dcmplt
	add	r4, r4, #1
	cmp	r0, #0
	addne	r8, r8, #1
	cmp	r7, r4
	bhi	.L19
.L16: mul 
	mov	r0, r8
	bl	__aeabi_ui2d
	mov	r2, #0
	ldr	r3, .L24+24
	bl	__aeabi_dmul
	mov	r4, r0
	mov	r0, r7
	mov	r5, r1
	bl	__aeabi_ui2d
	mov	r2, r0
	mov	r3, r1
	mov	r0, r4
	mov	r1, r5
	bl	__aeabi_ddiv
	mov	r4, r0
	mov	r5, r1
	mov	r0, #4
	add	r1, sp, #12
	bl	clock_gettime
	ldr	r3, [sp, #20]
	ldr	r0, [sp, #12]
	rsb	r0, r3, r0
	bl	__aeabi_i2d
	ldr	r3, [sp, #24]
	mov	r7, r0
	ldr	r0, [sp, #16]
	mov	r8, r1
	rsb	r0, r3, r0
	bl	__aeabi_i2d
	ldr	r2, .L24+28
	ldr	r3, .L24+32
	bl	__aeabi_dmul
	mov	r2, r0
	mov	r3, r1
	mov	r0, r7
	mov	r1, r8
	bl	__aeabi_dadd
	mov	r2, r4
	stmia	sp, {r0-r1}
	mov	r3, r5
	ldr	r0, .L24+36
	bl	printf
	mov	r2, r4
	mov	r3, r5
	ldr	r0, .L24+40
	bl	printf
	mov	r0, #0
	add	sp, sp, #32
	ldmfd	sp!, {r4, r5, r6, r7, r8, sl, fp, lr}
	bx	lr
.L23:
	ldr	r3, .L24+44 ; stderr
	ldr	r2, [r1, #0]
	ldr	r0, [r3, #0]
	ldr	r1, .L24+48 ; usage string
	bl	fprintf 
	mov	r0, #1
	bl	exit
.L25:
	.align	2
.L24:
	.word	stdout
	.word	.LC1
	.word	.LC2
	.word	.LC3
	.word	1041235968
	.word	1072693248
	.word	1074790400
	.word	-400107883
	.word	1041313291
	.word	.LC4
	.word	.LC5
	.word	stderr
	.word	.LC0
	.size	main, .-main
	.section	.rodata.str1.4,"aMS",%progbits,1
	.align	2
.LC0:
	.ascii	"Usage: %s <N> <out file>\012\000"
	.space	2
.LC1:
	.ascii	"w\000"
	.space	2
.LC2:
	.ascii	"%u\000"
	.space	1
.LC3:
	.ascii	"Resolution: %d ns\012\012\000"
.LC4:
	.ascii	"Pi: %lf\012Time: %lf s\012\012\000"
	.space	2
.LC5:
	.ascii	"%lf\012\000"
	.ident	"GCC: (Debian 4.4.5-8) 4.4.5"
	.section	.note.GNU-stack,"",%progbits
