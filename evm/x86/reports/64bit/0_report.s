	.file	"monte.c"
	.intel_syntax noprefix
	.text
	.globl	calcPi
	.type	calcPi, @function
calcPi:
.LFB0:
    /* пролог функции */
	.cfi_startproc
	push	rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	mov	rbp, rsp
	.cfi_def_cfa_register 6
	/* выделяем место под локальные переменные, инициализируем */
	sub	rsp, 48
	mov	DWORD PTR [rbp-36], edi
	mov	DWORD PTR [rbp-8], 0
	mov	DWORD PTR [rbp-4], 0
	jmp	.L2
.L5:
	call	rand
	cvtsi2sd	xmm0, eax /* cast rand() result to double */
	/* movsd c xmm-аргументами -- это не строковая операция,
	а пересылка single double */
	/* x64 использует SSE вместо сопроцессора x87  */
	movsd	xmm1, QWORD PTR .LC0[rip] /* 2.0 * (double)RAND_MAX */
	mulsd	xmm0, xmm1 /* xmm0 == rand() * (2.0 / (double)RAND_MAX */
	movsd	xmm1, QWORD PTR .LC1[rip]
	subsd	xmm0, xmm1 /* xmm0 == x */
	movsd	QWORD PTR [rbp-16], xmm0 /* сохраним х в стеке */
	/* вычисляем y */
	call	rand
	cvtsi2sd	xmm0, eax
	movsd	xmm1, QWORD PTR .LC0[rip]
	mulsd	xmm0, xmm1
	movsd	xmm1, QWORD PTR .LC1[rip]
	subsd	xmm0, xmm1
	movsd	QWORD PTR [rbp-24], xmm0 /* сохраним y в стеке */
	movsd	xmm0, QWORD PTR [rbp-16]
	movapd	xmm1, xmm0
	mulsd	xmm1, QWORD PTR [rbp-16] /* вычислили x*x, можно было короче записать*/
	movsd	xmm0, QWORD PTR [rbp-24]
	mulsd	xmm0, QWORD PTR [rbp-24] /* вычислили y*y, тут нет лишней инструкции типа movapd xmm1, xmm0 (строчка 39) */
	addsd	xmm0, xmm1 /* xmm0 == xx + yy */
	movsd	xmm1, QWORD PTR .LC1[rip] /* 1.0 */
	/* буква u -- unordered
	значит, что идет проверка на особые константы вроде NaN, inf */
	ucomisd	xmm1, xmm0
	jbe	.L3 /* на самом деле проверяется xx+yy >= 1.0, если так, то идем на .L3 */
.L11:
	add	DWORD PTR [rbp-8], 1 /* M++ */
.L3:
	add	DWORD PTR [rbp-4], 1 /* ++i */
.L2:
	mov	eax, DWORD PTR [rbp-4]
	cmp	eax, DWORD PTR [rbp-36]
	jb	.L5 /* условие цикла */
	mov	eax, DWORD PTR [rbp-8]
	test	rax, rax
	js	.L6 /* если в M установлен старший бит, идем в .L6 */
	cvtsi2sd	xmm0, rax
	jmp	.L7
.L6:
    /* хитрый алгоритм преобразования unsigned к double 
       cvti2sd преобразует signed к double, поэтому если в
       старшем бите единица, то переведенное число будет отрицательным.
       поэтому делим исходный unsigned пополам, переводим его в double,
       а потом этот double умножаем на 2 (сложением самого с собой) */
	mov	rdx, rax
	shr	rdx
	and	eax, 1
	or	rdx, rax
	cvtsi2sd	xmm0, rdx
	addsd	xmm0, xmm0 
	
	/* xmm0 == (double)M */
.L7:
	movsd	xmm1, QWORD PTR .LC2[rip]
	mulsd	xmm1, xmm0 /* xmm1 == 4.0 * (double)M */
	mov	eax, DWORD PTR [rbp-36] /* N */
	test	rax, rax /* аналогично приводим N к double */
	js	.L8 
	cvtsi2sd	xmm0, rax
	jmp	.L9
.L8:
	mov	rdx, rax
	shr	rdx
	and	eax, 1
	or	rdx, rax
	cvtsi2sd	xmm0, rdx
	addsd	xmm0, xmm0

	/* xmm0 == (double)N */
	/* xmm1 == 4.0 * (double)M */
.L9:
	movapd	xmm2, xmm1
	divsd	xmm2, xmm0
	movapd	xmm0, xmm2
	/* xmm0 == result */
	/* ненужные копирования результата, он готов еще на 97 строчке */
	movsd	QWORD PTR [rbp-48], xmm0
	mov	rax, QWORD PTR [rbp-48]
	mov	QWORD PTR [rbp-48], rax
	movsd	xmm0, QWORD PTR [rbp-48]
    /* эпилог функции */
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE0:
	.size	calcPi, .-calcPi
	.section	.rodata
.LC3:
	.string	"Usage: %s <N> <out file>\n"
.LC4:
	.string	"w"
.LC5:
	.string	"%u"
.LC6:
	.string	"Resolution: %d ns\n\n"
.LC8:
	.string	"Pi: %lf\nTime: %lf s\n\n"
	.text
	.globl	main
	.type	main, @function
main:
.LFB1:
	.cfi_startproc
	push	rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	mov	rbp, rsp
	.cfi_def_cfa_register 6
	sub	rsp, 96
	mov	DWORD PTR [rbp-68], edi
	mov	QWORD PTR [rbp-80], rsi
	cmp	DWORD PTR [rbp-68], 3
	je	.L13
	mov	rax, QWORD PTR [rbp-80]
	mov	rdx, QWORD PTR [rax]
	mov	rax, QWORD PTR stderr[rip]
	mov	esi, OFFSET FLAT:.LC3
	mov	rdi, rax
	mov	eax, 0
	call	fprintf
	mov	edi, 1
	call	exit
.L13:
	mov	rdx, QWORD PTR stdout[rip]
	mov	rax, QWORD PTR [rbp-80]
	add	rax, 16
	mov	rax, QWORD PTR [rax]
	mov	esi, OFFSET FLAT:.LC4
	mov	rdi, rax
	call	freopen
	mov	rax, QWORD PTR [rbp-80]
	add	rax, 8
	mov	rax, QWORD PTR [rax]
	lea	rdx, [rbp-20]
	mov	esi, OFFSET FLAT:.LC5
	mov	rdi, rax
	mov	eax, 0
	call	__isoc99_sscanf
	lea	rax, [rbp-48]
	mov	rsi, rax
	mov	edi, 4
	call	clock_getres
	mov	rax, QWORD PTR [rbp-40]
	mov	rsi, rax
	mov	edi, OFFSET FLAT:.LC6
	mov	eax, 0
	call	printf
	mov	rax, QWORD PTR stdout[rip]
	mov	rdi, rax
	call	fflush
	lea	rax, [rbp-48]
	mov	rsi, rax
	mov	edi, 4
	call	clock_gettime
	mov	eax, DWORD PTR [rbp-20]
	mov	edi, eax
	/* edi -- аргумент функции calcPi */
	call	calcPi
	/* результат возаращается в xmm0 */
	movsd	QWORD PTR [rbp-88], xmm0
	mov	rax, QWORD PTR [rbp-88]
	mov	QWORD PTR [rbp-8], rax
	lea	rax, [rbp-64]
	mov	rsi, rax
	mov	edi, 4
	call	clock_gettime
	mov	rdx, QWORD PTR [rbp-64]
	mov	rax, QWORD PTR [rbp-48]
	mov	rcx, rdx
	sub	rcx, rax
	mov	rax, rcx
	cvtsi2sd	xmm1, rax
	mov	rdx, QWORD PTR [rbp-56]
	mov	rax, QWORD PTR [rbp-40]
	mov	rcx, rdx
	sub	rcx, rax
	mov	rax, rcx
	cvtsi2sd	xmm0, rax
	movsd	xmm2, QWORD PTR .LC7[rip]
	mulsd	xmm0, xmm2
	addsd	xmm0, xmm1
	movsd	QWORD PTR [rbp-16], xmm0
	mov	rdx, QWORD PTR [rbp-16]
	mov	rax, QWORD PTR [rbp-8]
	mov	QWORD PTR [rbp-88], rdx
	movsd	xmm1, QWORD PTR [rbp-88]
	mov	QWORD PTR [rbp-88], rax
	movsd	xmm0, QWORD PTR [rbp-88]
	mov	edi, OFFSET FLAT:.LC8
	mov	eax, 2
	call	printf
	mov	eax, 0
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE1:
	.size	main, .-main
	.section	.rodata
	.align 8
.LC0:
	.long	2097152
	.long	1041235968
	.align 8
.LC1:
	.long	0
	.long	1072693248
	.align 8
.LC2:
	.long	0
	.long	1074790400
	.align 8
.LC7:
	.long	3894859413
	.long	1041313291
	.ident	"GCC: (Debian 4.7.2-5) 4.7.2"
	.section	.note.GNU-stack,"",@progbits
