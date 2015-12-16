	.file	"monte.c"
	.intel_syntax noprefix
	.text
	.p2align 4,,15
	.globl	calcPi
	.type	calcPi, @function
calcPi:
.LFB18:
    /* в этой оптимизации локальные переменные почти не хранятся в стеке */
	.cfi_startproc
	push	r12
	.cfi_def_cfa_offset 16
	.cfi_offset 12, -16
	xorpd	xmm0, xmm0 /* xmm0 = 0 */
	push	rbp
	.cfi_def_cfa_offset 24
	.cfi_offset 6, -24
	mov	ebp, edi /* ebp == N */
	push	rbx
	.cfi_def_cfa_offset 32
	.cfi_offset 3, -32
	sub	rsp, 16
	.cfi_def_cfa_offset 48
	test	ebp, ebp /* N == 0 => go .L2 */
	je	.L2
	xor	r12d, r12d
	xor	ebx, ebx
	jmp	.L5
	.p2align 4,,10
	.p2align 3
.L3:
	add	ebx, 1
	cmp	ebx, ebp
	je	.L15
.L5:
	call	rand
	cvtsi2sd	xmm1, eax
	mulsd	xmm1, QWORD PTR .LC1[rip]
	subsd	xmm1, QWORD PTR .LC2[rip]
	movsd	QWORD PTR [rsp], xmm1
	/* x -- [rsp] */
	call	rand
	cvtsi2sd	xmm0, eax
	movsd	xmm1, QWORD PTR [rsp]
	mulsd	xmm1, xmm1 /* x*x */
	mulsd	xmm0, QWORD PTR .LC1[rip]
	subsd	xmm0, QWORD PTR .LC2[rip]
	mulsd	xmm0, xmm0 /* y*y */
	addsd	xmm1, xmm0 /* xx+yy */

	movsd	xmm0, QWORD PTR .LC2[rip] /* 1.0 */
	ucomisd	xmm0, xmm1
	jbe	.L3
	add	ebx, 1 /* i++ */
	add	r12d, 1 /* M++ */
	cmp	ebx, ebp /* условие цикла */
	jne	.L5
.L15:
    /* тут проблем со знаком не возникает, потому что unsigned
    хранится в quad word, а не double word */
	cvtsi2sd	xmm0, r12
	mulsd	xmm0, QWORD PTR .LC3[rip] /* *4.0 */
.L2:
	cvtsi2sd	xmm1, rbp /* N */
	add	rsp, 16
	.cfi_def_cfa_offset 32
	pop	rbx
	.cfi_def_cfa_offset 24
	pop	rbp
	.cfi_def_cfa_offset 16
	pop	r12
	.cfi_def_cfa_offset 8
	divsd	xmm0, xmm1 /* Результат в xmm0 */
	ret
	.cfi_endproc
.LFE18:
	.size	calcPi, .-calcPi
	.section	.rodata.str1.1,"aMS",@progbits,1
.LC4:
	.string	"Usage: %s <N> <out file>\n"
.LC5:
	.string	"w"
.LC6:
	.string	"%u"
.LC7:
	.string	"Resolution: %d ns\n\n"
.LC9:
	.string	"Pi: %lf\nTime: %lf s\n\n"
	.section	.text.startup,"ax",@progbits
	.p2align 4,,15
	.globl	main
	.type	main, @function
main:
.LFB19:
	.cfi_startproc
	push	rbx
	.cfi_def_cfa_offset 16
	.cfi_offset 3, -16
	mov	rbx, rsi
	sub	rsp, 64
	.cfi_def_cfa_offset 80
	cmp	edi, 3
	jne	.L19
	mov	rdi, QWORD PTR [rsi+16]
	mov	rdx, QWORD PTR stdout[rip]
	mov	esi, OFFSET FLAT:.LC5
	call	freopen
	mov	rdi, QWORD PTR [rbx+8]
	lea	rdx, [rsp+28]
	mov	esi, OFFSET FLAT:.LC6
	xor	eax, eax
	call	__isoc99_sscanf
	lea	rsi, [rsp+32]
	mov	edi, 4
	call	clock_getres
	mov	rsi, QWORD PTR [rsp+40]
	mov	edi, OFFSET FLAT:.LC7
	xor	eax, eax
	call	printf
	mov	rdi, QWORD PTR stdout[rip]
	call	fflush
	lea	rsi, [rsp+32]
	mov	edi, 4
	call	clock_gettime
	mov	edi, DWORD PTR [rsp+28]
	call	calcPi
	lea	rsi, [rsp+48]
	mov	edi, 4
	movsd	QWORD PTR [rsp], xmm0
	call	clock_gettime
	mov	rax, QWORD PTR [rsp+48]
	sub	rax, QWORD PTR [rsp+32]
	mov	edi, OFFSET FLAT:.LC9
	movsd	xmm0, QWORD PTR [rsp]
	cvtsi2sd	xmm2, rax
	mov	rax, QWORD PTR [rsp+56]
	sub	rax, QWORD PTR [rsp+40]
	cvtsi2sd	xmm1, rax
	mov	eax, 2
	mulsd	xmm1, QWORD PTR .LC8[rip]
	addsd	xmm1, xmm2
	call	printf
	add	rsp, 64
	.cfi_remember_state
	.cfi_def_cfa_offset 16
	xor	eax, eax
	pop	rbx
	.cfi_def_cfa_offset 8
	ret
.L19:
	.cfi_restore_state
	mov	rdx, QWORD PTR [rsi]
	mov	rdi, QWORD PTR stderr[rip]
	mov	esi, OFFSET FLAT:.LC4
	xor	eax, eax
	call	fprintf
	mov	edi, 1
	call	exit
	.cfi_endproc
.LFE19:
	.size	main, .-main
	.section	.rodata.cst8,"aM",@progbits,8
	.align 8
.LC1:
	.long	2097152
	.long	1041235968
	.align 8
.LC2:
	.long	0
	.long	1072693248
	.align 8
.LC3:
	.long	0
	.long	1074790400
	.align 8
.LC8:
	.long	3894859413
	.long	1041313291
	.ident	"GCC: (Debian 4.7.2-5) 4.7.2"
	.section	.note.GNU-stack,"",@progbits
