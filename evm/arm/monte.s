	.cpu arm9tdmi
	.fpu softvfp
	.eabi_attribute 20, 1
	.eabi_attribute 21, 1
	.eabi_attribute 23, 3
	.eabi_attribute 24, 1
	.eabi_attribute 25, 1
	.eabi_attribute 26, 2
	.eabi_attribute 30, 6
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
	@ args = 0, pretend = 0, frame = 32
	@ frame_needed = 1, uses_anonymous_args = 0
	stmfd	sp!, {r4, r5, r6, fp, lr} 
	add	fp, sp, #16
	sub	sp, sp, #36
	str	r0, [fp, #-48] /* local variable N */
	mov	r3, #0
	str	r3, [fp, #-40] /* local variable M */
	mov	r3, #0
	str	r3, [fp, #-44] /* local variable i */
	b	.L2 /* jump */
.L6:
	bl	rand
	mov	r3, r0
	mov	r0, r3
	bl	__aeabi_i2d
	mov	r3, r0
	mov	r4, r1
	mov	r0, r3
	mov	r1, r4
	mov	r2, #2097152
	mov	r3, #1040187392
	add	r3, r3, #1048576
	bl	__aeabi_dmul
	mov	r3, r0
	mov	r4, r1
	mov	r0, r3
	mov	r1, r4
	mov	r2, #0
	mov	r3, #1069547520
	add	r3, r3, #3145728
	bl	__aeabi_dsub
	mov	r3, r0
	mov	r4, r1
	/* r0,r1 -- x value
	r3,r4 -- x value */
	str	r3, [fp, #-36]
	str	r4, [fp, #-32]
	bl	rand
	mov	r3, r0
	mov	r0, r3
	bl	__aeabi_i2d
	mov	r3, r0
	mov	r4, r1
	mov	r0, r3
	mov	r1, r4
	mov	r2, #2097152
	mov	r3, #1040187392
	add	r3, r3, #1048576
	bl	__aeabi_dmul
	mov	r3, r0
	mov	r4, r1
	mov	r0, r3
	mov	r1, r4
	mov	r2, #0
	mov	r3, #1069547520
	add	r3, r3, #3145728
	bl	__aeabi_dsub
	mov	r3, r0
	mov	r4, r1
	str	r3, [fp, #-28]
	str	r4, [fp, #-24]
	sub	r1, fp, #36
	ldmia	r1, {r0-r1} /* see lines 61,62 (load x to r0,r1)*/
	sub	r3, fp, #36
	ldmia	r3, {r2-r3} /* again load x to r0,r1 */
	bl	__aeabi_dmul /* square x*x */
	mov	r3, r0
	mov	r4, r1
	mov	r5, r3
	mov	r6, r4
	sub	r1, fp, #28 /* see lines 85,86 */
	ldmia	r1, {r0-r1}
	sub	r3, fp, #28
	ldmia	r3, {r2-r3}
	bl	__aeabi_dmul /* square y*y */
	mov	r3, r0
	mov	r4, r1
	mov	r0, r5
	mov	r1, r6
	mov	r2, r3
	mov	r3, r4
	/* r0,r1 -- x*x
	r2,r3 -- y*y */
	bl	__aeabi_dadd
	mov	r3, r0
	mov	r4, r1
	mov	r2, #0
	mov	r5, r2
	mov	r0, r3
	mov	r1, r4
	mov	r2, #0
	mov	r3, #1069547520
	add	r3, r3, #3145728
	bl	__aeabi_dcmplt
	mov	r3, r0
	cmp	r3, #0
	beq	.L3
.L4:
	mov	r3, #1
	mov	r5, r3
.L3:
	and	r3, r5, #255
	cmp	r3, #0
	/* очень сильно размазано сравнение xx+yy < 1.0 */
	beq	.L5 /* если if не сработал */
	ldr	r3, [fp, #-40]
	add	r3, r3, #1
	str	r3, [fp, #-40]
.L5:
	ldr	r3, [fp, #-44]
	add	r3, r3, #1
	str	r3, [fp, #-44]
.L2:
	ldr	r2, [fp, #-44]
	ldr	r3, [fp, #-48]
	cmp	r2, r3
	bcc	.L6 /* loop condition */
	ldr	r0, [fp, #-40]
	bl	__aeabi_ui2d
	mov	r3, r0
	mov	r4, r1
	mov	r0, r3
	mov	r1, r4
	mov	r2, #0
	mov	r3, #1073741824
	add	r3, r3, #1048576
	bl	__aeabi_dmul
	mov	r3, r0
	mov	r4, r1
	mov	r5, r3
	mov	r6, r4
	ldr	r0, [fp, #-48]
	bl	__aeabi_ui2d
	mov	r3, r0
	mov	r4, r1
	mov	r0, r5
	mov	r1, r6
	mov	r2, r3
	mov	r3, r4
	bl	__aeabi_ddiv
	mov	r3, r0
	mov	r4, r1
	mov	r0, r3
	mov	r1, r4
	sub	sp, fp, #16
	ldmfd	sp!, {r4, r5, r6, fp, lr}
	bx	lr
	.size	calcPi, .-calcPi
	.section	.rodata
	.align	2
.LC0:
	.ascii	"Usage: %s <N> <out file>\012\000"
	.align	2
.LC1:
	.ascii	"w\000"
	.align	2
.LC2:
	.ascii	"%u\000"
	.align	2
.LC3:
	.ascii	"Resolution: %d ns\012\012\000"
	.align	2
.LC4:
	.ascii	"Pi: %lf\012Time: %lf s\012\012\000"
	.align	2
.LC5:
	.ascii	"%lf\012\000"
	.text
	.align	2
	.global	main
	.type	main, %function
main:
	@ Function supports interworking.
	@ args = 0, pretend = 0, frame = 48
	@ frame_needed = 1, uses_anonymous_args = 0
	stmfd	sp!, {r4, r5, r6, fp, lr}
	add	fp, sp, #16
	sub	sp, sp, #60
	str	r0, [fp, #-64]
	str	r1, [fp, #-68]
	ldr	r3, [fp, #-64]
	cmp	r3, #3
	beq	.L9
	ldr	r3, .L11+8
	ldr	r2, [r3, #0]
	ldr	r3, [fp, #-68]
	ldr	r3, [r3, #0]
	mov	r0, r2
	ldr	r1, .L11+12
	mov	r2, r3
	bl	fprintf
	mov	r0, #1
	bl	exit
.L9:
	ldr	r3, [fp, #-68]
	add	r3, r3, #8
	ldr	r2, [r3, #0]
	ldr	r3, .L11+16
	ldr	r3, [r3, #0]
	mov	r0, r2
	ldr	r1, .L11+20
	mov	r2, r3
	bl	freopen
	ldr	r3, [fp, #-68]
	add	r3, r3, #4
	ldr	r2, [r3, #0]
	sub	r3, fp, #44
	mov	r0, r2
	ldr	r1, .L11+24
	mov	r2, r3
	bl	sscanf
	sub	r3, fp, #52
	mov	r0, #4
	mov	r1, r3
	bl	clock_getres
	ldr	r3, [fp, #-48]
	ldr	r0, .L11+28
	mov	r1, r3
	bl	printf
	ldr	r3, .L11+16
	ldr	r3, [r3, #0]
	mov	r0, r3
	bl	fflush
	sub	r3, fp, #52
	mov	r0, #4
	mov	r1, r3
	bl	clock_gettime
	ldr	r3, [fp, #-44]
	mov	r0, r3
	bl	calcPi
	mov	r3, r0
	mov	r4, r1
	str	r3, [fp, #-28]
	str	r4, [fp, #-24]
	sub	r3, fp, #60
	mov	r0, #4
	mov	r1, r3
	bl	clock_gettime
	ldr	r2, [fp, #-60]
	ldr	r3, [fp, #-52]
	rsb	r3, r3, r2
	mov	r0, r3
	bl	__aeabi_i2d
	mov	r5, r0
	mov	r6, r1
	ldr	r2, [fp, #-56]
	ldr	r3, [fp, #-48]
	rsb	r3, r3, r2
	mov	r0, r3
	bl	__aeabi_i2d
	mov	r3, r0
	mov	r4, r1
	mov	r0, r3
	mov	r1, r4
	adr	r3, .L11
	ldmia	r3, {r2-r3}
	bl	__aeabi_dmul
	mov	r3, r0
	mov	r4, r1
	mov	r0, r5
	mov	r1, r6
	mov	r2, r3
	mov	r3, r4
	bl	__aeabi_dadd
	mov	r3, r0
	mov	r4, r1
	str	r3, [fp, #-36]
	str	r4, [fp, #-32]
	sub	r4, fp, #36
	ldmia	r4, {r3-r4}
	stmia	sp, {r3-r4}
	ldr	r0, .L11+32
	sub	r3, fp, #28
	ldmia	r3, {r2-r3}
	bl	printf
	ldr	r0, .L11+36
	sub	r3, fp, #28
	ldmia	r3, {r2-r3}
	bl	printf
	mov	r3, #0
	mov	r0, r3
	sub	sp, fp, #16
	ldmfd	sp!, {r4, r5, r6, fp, lr}
	bx	lr
.L12:
	.align	3
.L11:
	.word	3894859413
	.word	1041313291
	.word	stderr
	.word	.LC0
	.word	stdout
	.word	.LC1
	.word	.LC2
	.word	.LC3
	.word	.LC4
	.word	.LC5
	.size	main, .-main
	.ident	"GCC: (Debian 4.4.5-8) 4.4.5"
	.section	.note.GNU-stack,"",%progbits
