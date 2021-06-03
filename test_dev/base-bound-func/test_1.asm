	.text
	.file	"test_1.c"
	.globl	_f_pseudoMain                   # -- Begin function _f_pseudoMain
	.p2align	4, 0x90
	.type	_f_pseudoMain,@function
_f_pseudoMain:                          # @_f_pseudoMain
	.cfi_startproc
# %bb.0:
	xorl	%eax, %eax
	retq
.Lfunc_end0:
	.size	_f_pseudoMain, .Lfunc_end0-_f_pseudoMain
	.cfi_endproc
                                        # -- End function
	.p2align	4, 0x90                         # -- Begin function _tasmc_global_init
	.type	_tasmc_global_init,@function
_tasmc_global_init:                     # @_tasmc_global_init
# %bb.0:
	pushq	%rax
	callq	_initTaSMC
	movl	$ptr, %edi
	movl	$array, %esi
	movl	$array+400, %edx
	callq	_f_storeMetaData
	popq	%rax
	retq
.Lfunc_end1:
	.size	_tasmc_global_init, .Lfunc_end1-_tasmc_global_init
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
	.quad	array+1600
	.size	ptr, 8

	.type	arrat_int_ptr,@object           # @arrat_int_ptr
	.bss
	.globl	arrat_int_ptr
	.p2align	4
arrat_int_ptr:
	.zero	800
	.size	arrat_int_ptr, 800

	.section	.init_array.0,"aw",@init_array
	.p2align	3
	.quad	_tasmc_global_init
	.ident	"clang version 11.0.0"
	.section	".note.GNU-stack","",@progbits
	.addrsig
	.addrsig_sym _f_storeMetaData
	.addrsig_sym _tasmc_global_init
	.addrsig_sym _initTaSMC
	.addrsig_sym array
