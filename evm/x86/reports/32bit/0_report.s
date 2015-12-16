; Литвинов Василий, группа 13201
; Лабораторная работа N3
; Ассемблерный листинг с оптимизацией -O0

    .file	"monte.c"
	.intel_syntax noprefix
	.text
	.globl	calcPi
	.type	calcPi, @function
calcPi:
.LFB0:
	.cfi_startproc
    ; вместе со следующим mov -- стандартный пролог функции, 
    ; в ebp до push хранилось начало предыдущего фрейма стека
	push	ebp 
	.cfi_def_cfa_offset 8
	.cfi_offset 5, -8
	mov	ebp, esp
	.cfi_def_cfa_register 5
	sub	esp, 56  ; резервируем место под локальные переменные
	mov	DWORD PTR [ebp-16], 0 ; local M
	mov	DWORD PTR [ebp-12], 0 ; local i
	jmp	.L2
.L5:
    ; вычисляем x == QWORD PTR [ebp-24]
	call	rand ; return in eax
	mov	DWORD PTR [ebp-52], eax 
    ; кастуем результат rand к double, кладем в стек х87
	fild	DWORD PTR [ebp-52] 
	fld	QWORD PTR .LC0 ; константа 2.0 / (double)RAND_MAX
    ; умножаем результат rand на эту константу, и тут же
    ; выталкиваем верхушку стека x87 (там лежит уже ненужная константа)
	fmulp	st(1), st 
	fld1 ; загружаем 1
	fsubp	st(1), st ; и отнимаем ее от (double)rand() * (...)
	fstp	QWORD PTR [ebp-24] ; local x

    ; вычисляем y == QWORD PTR [ebp]
	call	rand
	mov	DWORD PTR [ebp-52], eax
	fild	DWORD PTR [ebp-52]
	fld	QWORD PTR .LC0
	fmulp	st(1), st
	fld1
	fsubp	st(1), st
	fstp	QWORD PTR [ebp-32] ; local y

    ; считаем x*x + y*y
	fld	QWORD PTR [ebp-24]
	fmul	QWORD PTR [ebp-24]
	fld	QWORD PTR [ebp-32]
	fmul	QWORD PTR [ebp-32]
	faddp	st(1), st 

    ; сравниваем xx+yy с 1.0
	fld1
    ; результат сравнения записывается в флаги x87, 
    ; 2 значения (а больше и нет) выталкиваются, стек x87 свободен
	fucompp 
	; этими 2 командами копируем флаги x87 в соответствующие x86
	fnstsw	ax 
	sahf
	jbe	.L3 ; если 1.0 > xx+yy, то пропускаем наращивание M
.L7:
	add	DWORD PTR [ebp-16], 1 ; M++
.L3:
	add	DWORD PTR [ebp-12], 1 ; счетчик цикла хранится в памяти
.L2:
	mov	eax, DWORD PTR [ebp-12] 
	cmp	eax, DWORD PTR [ebp+8] ; сравниваем счетчик с N
	jb	.L5 ; i < N, тогда выполняем еще одну итерацию for

	mov	eax, DWORD PTR [ebp-16] ; M
	mov	edx, 0 ; RUBBISH
	mov	DWORD PTR [ebp-48], eax
	mov	DWORD PTR [ebp-44], edx ; RUBBISH
	fild	QWORD PTR [ebp-48] ; (double)M загрузили в стек x87
	fld	QWORD PTR .LC2 ; 4.0 туда же
	fmulp	st(1), st ; 4.0 * (double)M
	mov	eax, DWORD PTR [ebp+8] ; N
	mov	edx, 0 ; RUBBISH
	mov	DWORD PTR [ebp-48], eax 
	mov	DWORD PTR [ebp-44], edx ; RUBBISH
	fild	QWORD PTR [ebp-48] ; (double)N загрузили в стек х87
    ; (4.0 * (double)M) / (double)N, вытолкнули верх, после
    ; этой операции ответ лежит на верщине стека x87
	fdivp	st(1), st 
	leave ; эпилог функции, обратный прологу
	.cfi_restore 5
	.cfi_def_cfa 4, 4
	ret
	.cfi_endproc
.LFE0:
	.size	calcPi, .-calcPi
	.section	.rodata
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
	.text
	.globl	main
	.type	main, @function
main:
.LFB1:
	.cfi_startproc
	push	ebp
	.cfi_def_cfa_offset 8
	.cfi_offset 5, -8
	mov	ebp, esp
	.cfi_def_cfa_register 5
	and	esp, -16 ; выравнивание
	sub	esp, 96 ; резервируем место под локальные переменные
	cmp	DWORD PTR [ebp+8], 3 ; argc сравнить с 3
	je	.L9 
	; argc != 3, тогда выводим сообщение об ошибке
	mov	eax, DWORD PTR [ebp+12] ; argv[]
	mov	edx, DWORD PTR [eax] ; argv[0]
	mov	eax, DWORD PTR stderr ; stderr

	mov	DWORD PTR [esp+8], edx ; argv[0]
	mov	DWORD PTR [esp+4], OFFSET FLAT:.LC4  ; format
	mov	DWORD PTR [esp], eax ; stderr
	call	fprintf
	; и выходим с кодом 1
	mov	DWORD PTR [esp], 1
	call	exit
.L9:
	mov	edx, DWORD PTR stdout ; 
	mov	eax, DWORD PTR [ebp+12] ; argv[]
	add	eax, 8 ; 2 * (4 байта на указатель)
	mov	eax, DWORD PTR [eax] ; argv[2]
	mov	DWORD PTR [esp+8], edx
	mov	DWORD PTR [esp+4], OFFSET FLAT:.LC5
	mov	DWORD PTR [esp], eax
	call	freopen
	mov	eax, DWORD PTR [ebp+12]
	add	eax, 4
	mov	eax, DWORD PTR [eax]
	lea	edx, [esp+76] ; взятие адреса (&N)
	mov	DWORD PTR [esp+8], edx
	mov	DWORD PTR [esp+4], OFFSET FLAT:.LC6
	mov	DWORD PTR [esp], eax
	call	__isoc99_sscanf
	lea	eax, [esp+68]
	mov	DWORD PTR [esp+4], eax
	mov	DWORD PTR [esp], 4
	call	clock_getres
	mov	eax, DWORD PTR [esp+72]
	mov	DWORD PTR [esp+4], eax
	mov	DWORD PTR [esp], OFFSET FLAT:.LC7
	call	printf
	mov	eax, DWORD PTR stdout
	mov	DWORD PTR [esp], eax
	call	fflush
	lea	eax, [esp+68]
	mov	DWORD PTR [esp+4], eax
	mov	DWORD PTR [esp], 4
	call	clock_gettime
	mov	eax, DWORD PTR [esp+76] ; N
	mov	DWORD PTR [esp], eax
	call	calcPi
	; result for calcPi is in FPU stack
	fstp	QWORD PTR [esp+88] ; double PI = ...
	lea	eax, [esp+60] ; &end
	mov	DWORD PTR [esp+4], eax
	mov	DWORD PTR [esp], 4
	call	clock_gettime
	mov	edx, DWORD PTR [esp+60]
	mov	eax, DWORD PTR [esp+68]
	mov	ecx, edx
	sub	ecx, eax ; end.tv_sec - start.tv_sec
	mov	eax, ecx
	mov	DWORD PTR [esp+44], eax
	fild	DWORD PTR [esp+44]

	mov	edx, DWORD PTR [esp+64]
	mov	eax, DWORD PTR [esp+72]
	mov	ecx, edx
	sub	ecx, eax ; end.tv_nsec - start.tv_nsec
	mov	eax, ecx
	mov	DWORD PTR [esp+44], eax
	fild	DWORD PTR [esp+44]

	fld	QWORD PTR .LC8
	fmulp	st(1), st
	faddp	st(1), st
	fstp	QWORD PTR [esp+80]
	fld	QWORD PTR [esp+80] 
	fstp	QWORD PTR [esp+12] ; copy time as 3rd arg
	fld	QWORD PTR [esp+88]
	fstp	QWORD PTR [esp+4] ; copy PI as 2nd arg
	mov	DWORD PTR [esp], OFFSET FLAT:.LC9
	call	printf
	mov	eax, 0
	leave
	.cfi_restore 5
	.cfi_def_cfa 4, 4
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
.LC2:
	.long	0
	.long	1074790400
	.align 8
.LC8:
	.long	3894859413
	.long	1041313291
	.ident	"GCC: (Debian 4.7.2-5) 4.7.2"
	.section	.note.GNU-stack,"",@progbits

