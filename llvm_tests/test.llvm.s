	.text
	.file	"test.llvm"
	.globl	main
	.align	16, 0x90
	.type	main,@function
main:                                   # @main
	.cfi_startproc
# BB#0:
	pushq	%rax
.Ltmp0:
	.cfi_def_cfa_offset 16
	movw	$3841, (%rsp)           # imm = 0xF01
	movzbl	1(%rsp), %edx
	movl	(%rsp), %esi
	movl	$str, %edi
	xorl	%eax, %eax
	callq	printf
	xorl	%eax, %eax
	popq	%rcx
	retq
.Lfunc_end0:
	.size	main, .Lfunc_end0-main
	.cfi_endproc

	.type	str,@object             # @str
	.section	.rodata,"a",@progbits
	.globl	str
str:
	.asciz	"x: %d, y: %d\n"
	.size	str, 14


	.section	".note.GNU-stack","",@progbits
