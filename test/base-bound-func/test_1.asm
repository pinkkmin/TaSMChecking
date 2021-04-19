	.text
	.file	"test_1.c"
	.globl	test                            # -- Begin function test
	.p2align	4, 0x90
	.type	test,@function
test:                                   # @test
	.cfi_startproc
# %bb.0:
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movq	arrat_int_ptr+88, %rax
	movl	$12, (%rax)
	popq	%rbp
	.cfi_def_cfa %rsp, 8
	retq
.Lfunc_end0:
	.size	test, .Lfunc_end0-test
	.cfi_endproc
                                        # -- End function
	.globl	_f_pseudoMain                   # -- Begin function _f_pseudoMain
	.p2align	4, 0x90
	.type	_f_pseudoMain,@function
_f_pseudoMain:                          # @_f_pseudoMain
	.cfi_startproc
# %bb.0:
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	subq	$16, %rsp
	movl	$0, -4(%rbp)
	movabsq	$.L.str, %rdi
	movb	$0, %al
	callq	printf
	xorl	%ecx, %ecx
	movl	%eax, -8(%rbp)                  # 4-byte Spill
	movl	%ecx, %eax
	addq	$16, %rsp
	popq	%rbp
	.cfi_def_cfa %rsp, 8
	retq
.Lfunc_end1:
	.size	_f_pseudoMain, .Lfunc_end1-_f_pseudoMain
	.cfi_endproc
                                        # -- End function
	.p2align	4, 0x90                         # -- Begin function __tasmc_global_init
	.type	__tasmc_global_init,@function
__tasmc_global_init:                    # @__tasmc_global_init
# %bb.0:
	pushq	%rax
	callq	_initTaSMC
	movabsq	$array, %rax
	movq	%rax, %rcx
	addq	$400, %rcx                      # imm = 0x190
	movabsq	$ptr, %rdx
	movq	%rdx, %rdi
	movq	%rax, %rsi
	movq	%rcx, %rdx
	callq	_f_storeMetaData
	popq	%rax
	retq
.Lfunc_end2:
	.size	__tasmc_global_init, .Lfunc_end2-__tasmc_global_init
                                        # -- End function
	.type	array,@object                   # @array
	.data
	.globl	array
	.p2align	4
array:
	.long	0                               # 0x0
	.long	12                              # 0xc
	.long	3                               # 0x3
	.long	4                               # 0x4
	.zero	384
	.size	array, 400

	.type	ptr,@object                     # @ptr
	.globl	ptr
	.p2align	3
ptr:
	.quad	array+4
	.size	ptr, 8

	.type	arrat_int_ptr,@object           # @arrat_int_ptr
	.bss
	.globl	arrat_int_ptr
	.p2align	4
arrat_int_ptr:
	.zero	800
	.size	arrat_int_ptr, 800

	.type	.L.str,@object                  # @.str
	.section	.rodata.str1.1,"aMS",@progbits,1
.L.str:
	.asciz	"hello main()\n"
	.size	.L.str, 14

	.section	.init_array.0,"aw",@init_array
	.p2align	3
	.quad	__tasmc_global_init
	.ident	"clang version 11.0.0"
	.section	".note.GNU-stack","",@progbits
	.addrsig
	.addrsig_sym printf
	.addrsig_sym _f_storeMetaData
	.addrsig_sym __tasmc_global_init
	.addrsig_sym _initTaSMC
	.addrsig_sym array
	.addrsig_sym ptr
	.addrsig_sym arrat_int_ptr
