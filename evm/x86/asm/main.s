.section .rodata
argsFormat:	.string	"Usage: %s <N> <out file>\n"
writeMode:	.string	"w"
resFormat:	.string	"Resolution: %d ns\n\n"
piFormat:		.string	"Pi: %lf\nTime: %lf s\n\n"
avgFormat:	.string	"================\nAverage time: %lf s\n"
nFormat:		.string "%u"
dblOne:			.double 1.0
dblMagic:		.double 9.31322575049e-10

.section .bss
N:			.long
tStart:	.quad
tEnd:		.quad
dblOne:	.double

.section .text

.type calcPi, @function
calcPi:
	movl $N, %ecx
c1:
genRand:
	call rand
	fldl $dblMagic
	fmul
	fldl $dblOne
	fsub

	decl %ecx
	jnz c1
	ret


.globl main
.type main, @function
main:
	pushl %ebp
	movl %esp, %ebp
	cmpl $3, 4(%ebp) // argc
	je argsOk
	// Печатаем сообщение о использовании аргументов кмд. строки
	pushl 8(%ebp) // argv[0]
	pushl $argsFormat
	pushl stderr
	call fprintf
	addl $12, %esp
	movl $1, %eax
	jmp return
argsOk:
	pushl stdout
	pushl $writeMode
	pushl 8(%ebp, 2, 4)
	call freopen

	pushl $N
	pushl $nFormat
	pushl 8(%ebp, 1, 4)
	call sscanf
	addl $24, %esp // freopen, scanf

	movl $0x2b, %eax // sys_times
	subl $32, %esp
	movl %esp, %ebx
	int $0x80

	movl (%esp), tStart
	addl $32, %esp // СМОТРИ `subl $32, %esp`

	xorl %eax, %eax
return:
	movl %ebp, %esp
	popl %ebp
	ret


	.size main, .-main
