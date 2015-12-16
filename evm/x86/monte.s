	.file	"monte.c"
	.intel_syntax noprefix
	.text
	.p2align 4,,15
	.globl	calcPi
	.type	calcPi, @function
calcPi:
.LFB18: 
	.cfi_startproc
	push	edi
	.cfi_def_cfa_offset 8
	.cfi_offset 7, -8
	push	esi
	.cfi_def_cfa_offset 12
	.cfi_offset 6, -12
	push	ebx
	.cfi_def_cfa_offset 16
	.cfi_offset 3, -16
	sub	esp, 48
	.cfi_def_cfa_offset 64
	mov	esi, DWORD PTR [esp+64] ; esi == N
	test	esi, esi ; чтобы миновать цикл при нулевом N
	je	.L6 ; je == jz
	xor	edi, edi
	xor	ebx, ebx
	jmp	.L5
	.p2align 4,,7
	.p2align 3
.L3: ; промотка цикла на 1 если if не выполнился
	add	ebx, 1 
	cmp	ebx, esi
	je	.L11
.L5:
	call	rand
	mov	DWORD PTR [esp+36], eax ; временное место в памяти
	fild	DWORD PTR [esp+36]
	fmul	QWORD PTR .LC1
	fld1
	fsub	st(1), st
	fxch	st(1)
	fstp	QWORD PTR [esp+16]
	fstp	QWORD PTR [esp] ; чтобы единицу не грузить с помощью fld1, кладем в память

    call	rand
	mov	DWORD PTR [esp+36], eax
	fild	DWORD PTR [esp+36]
	fmul	QWORD PTR .LC1
	fld	QWORD PTR [esp]
	fsub	st(1), st
	fld	QWORD PTR [esp+16]
	fmul	st, st(0)
	fxch	st(2)
	fmul	st, st(0)
	faddp	st(2), st
	fucompp
	fnstsw	ax
	sahf
	jbe	.L3 ; if не сработал
	add	ebx, 1 
	add	edi, 1; if сработал, M++
	cmp	ebx, esi
	jne	.L5
.L11: ; return после цикла
	xor	ebx, ebx
	mov	DWORD PTR [esp+40], edi ; грузим M
	mov	DWORD PTR [esp+44], ebx ;  МУСОР
	fild	QWORD PTR [esp+40]
	fmul	DWORD PTR .LC3 ; M * 4
.L2:
	xor	edx, edx
	mov	DWORD PTR [esp+40], esi ; грузим N
	mov	DWORD PTR [esp+44], edx ; МУСОР
	fild	QWORD PTR [esp+40]
	add	esp, 48
	.cfi_remember_state
	.cfi_def_cfa_offset 16
	pop	ebx
	.cfi_restore 3
	.cfi_def_cfa_offset 12
	fdivp	st(1), st ; делим на N
	pop	esi
	.cfi_restore 6
	.cfi_def_cfa_offset 8
	pop	edi
	.cfi_restore 7
	.cfi_def_cfa_offset 4
	ret
.L6:
	.cfi_restore_state
	fldz ; если N == 0, возврат 0
	jmp	.L2
	.cfi_endproc
.LFE18:
	.size	calcPi, .-calcPi
	.section	.rodata.str1.1,"aMS",@progbits,1
.LC5:
	.string	"Usage: %s <N> <out file>\n"
.LC6:
	.string	"w"
.LC7:
	.string	"%u"
.LC8:
	.string	"Resolution: %d ns\n\n"
.LC10:
	.string	"Pi: %lf\nTime: %lf s\n\n"
	.section	.text.startup,"ax",@progbits
	.p2align 4,,15
	.globl	main
	.type	main, @function
main:
.LFB19:
	.cfi_startproc
	push	ebp
	.cfi_def_cfa_offset 8
	.cfi_offset 5, -8
	mov	ebp, esp
	.cfi_def_cfa_register 5
	push	ebx
	and	esp, -16
	sub	esp, 80
	.cfi_offset 3, -12
	mov	ebx, DWORD PTR [ebp+12]
	cmp	DWORD PTR [ebp+8], 3
	jne	.L15
	mov	eax, DWORD PTR stdout
	mov	DWORD PTR [esp+4], OFFSET FLAT:.LC6
	mov	DWORD PTR [esp+8], eax
	mov	eax, DWORD PTR [ebx+8]
	mov	DWORD PTR [esp], eax
	call	freopen
	lea	eax, [esp+60]
	mov	DWORD PTR [esp+8], eax
	mov	DWORD PTR [esp+4], OFFSET FLAT:.LC7
	mov	eax, DWORD PTR [ebx+4]
	lea	ebx, [esp+64]
	mov	DWORD PTR [esp], eax
	call	__isoc99_sscanf
	mov	DWORD PTR [esp+4], ebx
	mov	DWORD PTR [esp], 4
	call	clock_getres
	mov	eax, DWORD PTR [esp+68]
	mov	DWORD PTR [esp], OFFSET FLAT:.LC8
	mov	DWORD PTR [esp+4], eax
	call	printf
	mov	eax, DWORD PTR stdout
	mov	DWORD PTR [esp], eax
	call	fflush
	mov	DWORD PTR [esp+4], ebx
	mov	DWORD PTR [esp], 4
	call	clock_gettime
	mov	eax, DWORD PTR [esp+60]
	mov	DWORD PTR [esp], eax
	call	calcPi
	lea	eax, [esp+72]
	mov	DWORD PTR [esp+4], eax
	mov	DWORD PTR [esp], 4
	fstp	QWORD PTR [esp+32]
	call	clock_gettime
	mov	eax, DWORD PTR [esp+72]
	sub	eax, DWORD PTR [esp+64]
	mov	DWORD PTR [esp], OFFSET FLAT:.LC10
	mov	DWORD PTR [esp+44], eax
	mov	eax, DWORD PTR [esp+76]
	sub	eax, DWORD PTR [esp+68]
	fild	DWORD PTR [esp+44]
	mov	DWORD PTR [esp+44], eax
	fild	DWORD PTR [esp+44]
	fmul	QWORD PTR .LC9
	faddp	st(1), st
	fstp	QWORD PTR [esp+12]
	fld	QWORD PTR [esp+32]
	fstp	QWORD PTR [esp+4]
	call	printf
	xor	eax, eax
	mov	ebx, DWORD PTR [ebp-4]
	leave
	.cfi_remember_state
	.cfi_restore 5
	.cfi_restore 3
	.cfi_def_cfa 4, 4
	ret
.L15:
	.cfi_restore_state
	mov	eax, DWORD PTR [ebx]
	mov	DWORD PTR [esp+4], OFFSET FLAT:.LC5
	mov	DWORD PTR [esp+8], eax
	mov	eax, DWORD PTR stderr
	mov	DWORD PTR [esp], eax
	call	fprintf
	mov	DWORD PTR [esp], 1
	call	exit
	.cfi_endproc
.LFE19:
	.size	main, .-main
	.section	.rodata.cst8,"aM",@progbits,8
	.align 8
.LC1:
	.long	2097152
	.long	1041235968
	.section	.rodata.cst4,"aM",@progbits,4
	.align 4
.LC3:
	.long	1082130432
	.section	.rodata.cst8
	.align 8
.LC9:
	.long	3894859413
	.long	1041313291
	.ident	"GCC: (Debian 4.7.2-5) 4.7.2"
	.section	.note.GNU-stack,"",@progbits
