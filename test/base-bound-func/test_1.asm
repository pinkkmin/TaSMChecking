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
	subq	$176, %rsp
                                        # kill: def $dil killed $dil killed $edi
	movl	$3, %eax
	movb	%dil, -57(%rbp)                 # 1-byte Spill
	movl	%eax, %edi
	movq	%rcx, -72(%rbp)                 # 8-byte Spill
	movq	%rdx, -80(%rbp)                 # 8-byte Spill
	movq	%rsi, -88(%rbp)                 # 8-byte Spill
	callq	_f_loadBaseOfShadowStack
	movl	$3, %edi
	movq	%rax, -96(%rbp)                 # 8-byte Spill
	callq	_f_loadBoundOfShadowStack
	movl	$2, %edi
	movq	%rax, -104(%rbp)                # 8-byte Spill
	callq	_f_loadBaseOfShadowStack
	movl	$2, %edi
	movq	%rax, -112(%rbp)                # 8-byte Spill
	callq	_f_loadBoundOfShadowStack
	movl	$1, %edi
	movq	%rax, -120(%rbp)                # 8-byte Spill
	callq	_f_loadBaseOfShadowStack
	movl	$1, %edi
	movq	%rax, -128(%rbp)                # 8-byte Spill
	callq	_f_loadBoundOfShadowStack
	movb	-57(%rbp), %r8b                 # 1-byte Reload
	movb	%r8b, -1(%rbp)
	movq	-88(%rbp), %rcx                 # 8-byte Reload
	movq	%rcx, -16(%rbp)
	movq	-80(%rbp), %rdx                 # 8-byte Reload
	movq	%rdx, -24(%rbp)
	movq	-72(%rbp), %rsi                 # 8-byte Reload
	movq	%rsi, -32(%rbp)
	movl	$1, %edi
	movq	%rax, -136(%rbp)                # 8-byte Spill
	callq	_f_allocateShadowStackMetadata
	movl	$4, %edi
	movq	%rax, -144(%rbp)                # 8-byte Spill
	callq	malloc
	xorl	%edi, %edi
	movq	%rax, -152(%rbp)                # 8-byte Spill
	callq	_f_loadBaseOfShadowStack
	xorl	%edi, %edi
	movq	%rax, -160(%rbp)                # 8-byte Spill
	callq	_f_loadBoundOfShadowStack
	movq	%rax, -168(%rbp)                # 8-byte Spill
	callq	_f_deallocateShadowStackMetaData
	movq	-152(%rbp), %rax                # 8-byte Reload
	movq	%rax, -40(%rbp)
	movq	-40(%rbp), %rax
	movq	%rax, -48(%rbp)
	movq	-32(%rbp), %rax
	movq	%rax, -56(%rbp)
	addq	$176, %rsp
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
	movabsq	$array, %rax
	movq	%rax, %rcx
	addq	$400, %rcx                      # imm = 0x190
	movl	$0, -4(%rbp)
	movq	ptr, %rsi
	movabsq	$.L.str, %rdi
	movq	%rax, %rdx
	movabsq	$ptr, %r8
	movb	$0, %al
	callq	printf
	xorl	%r9d, %r9d
	movl	%eax, -8(%rbp)                  # 4-byte Spill
	movl	%r9d, %eax
	addq	$16, %rsp
	popq	%rbp
	.cfi_def_cfa %rsp, 8
	retq
.Lfunc_end1:
	.size	_f_pseudoMain, .Lfunc_end1-_f_pseudoMain
	.cfi_endproc
                                        # -- End function
	.p2align	4, 0x90                         # -- Begin function _tasmc_global_init
	.type	_tasmc_global_init,@function
_tasmc_global_init:                     # @_tasmc_global_init
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
	.size	_tasmc_global_init, .Lfunc_end2-_tasmc_global_init
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

	.type	.L.str,@object                  # @.str
	.section	.rodata.str1.1,"aMS",@progbits,1
.L.str:
	.asciz	"ptr : %zx, base: %zx, bound : %zx, addr_of_ptr : %zx\n"
	.size	.L.str, 54

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
	.addrsig_sym malloc
	.addrsig_sym printf
	.addrsig_sym _f_loadBaseOfShadowStack
	.addrsig_sym _f_loadBoundOfShadowStack
	.addrsig_sym _f_allocateShadowStackMetadata
	.addrsig_sym _f_deallocateShadowStackMetaData
	.addrsig_sym _f_storeMetaData
	.addrsig_sym _tasmc_global_init
	.addrsig_sym _initTaSMC
	.addrsig_sym array
	.addrsig_sym ptr
