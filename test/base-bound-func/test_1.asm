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
	subq	$192, %rsp
                                        # kill: def $dil killed $dil killed $edi
	movl	$3, %eax
	movb	%dil, -81(%rbp)                 # 1-byte Spill
	movl	%eax, %edi
	movl	%r8d, -88(%rbp)                 # 4-byte Spill
	movq	%rcx, -96(%rbp)                 # 8-byte Spill
	movq	%rdx, -104(%rbp)                # 8-byte Spill
	movq	%rsi, -112(%rbp)                # 8-byte Spill
	callq	_f_loadBaseOfShadowStack
	movl	$3, %edi
	movq	%rax, -120(%rbp)                # 8-byte Spill
	callq	_f_loadBoundOfShadowStack
	movl	$2, %edi
	movq	%rax, -128(%rbp)                # 8-byte Spill
	callq	_f_loadBaseOfShadowStack
	movl	$2, %edi
	movq	%rax, -136(%rbp)                # 8-byte Spill
	callq	_f_loadBoundOfShadowStack
	movl	$1, %edi
	movq	%rax, -144(%rbp)                # 8-byte Spill
	callq	_f_loadBaseOfShadowStack
	movl	$1, %edi
	movq	%rax, -152(%rbp)                # 8-byte Spill
	callq	_f_loadBoundOfShadowStack
	movb	-81(%rbp), %r9b                 # 1-byte Reload
	movb	%r9b, -33(%rbp)
	movq	-112(%rbp), %rcx                # 8-byte Reload
	movq	%rcx, -48(%rbp)
	movq	-104(%rbp), %rdx                # 8-byte Reload
	movq	%rdx, -56(%rbp)
	movq	-96(%rbp), %rsi                 # 8-byte Reload
	movq	%rsi, -64(%rbp)
	movl	-88(%rbp), %edi                 # 4-byte Reload
	movl	%edi, -68(%rbp)
	movl	$4, %r10d
	movq	%r10, %rdi
	movq	%rax, -160(%rbp)                # 8-byte Spill
	callq	malloc
	movq	%rax, -80(%rbp)
	movabsq	$.L.str, %rdi
	movb	$0, %al
	callq	printf
	movq	-80(%rbp), %rcx
	leaq	-80(%rbp), %rdx
	movq	%rdx, %rdi
	leaq	-24(%rbp), %rsi
	leaq	-32(%rbp), %rdx
	movl	%eax, -164(%rbp)                # 4-byte Spill
	movq	%rcx, -176(%rbp)                # 8-byte Spill
	callq	_f_loadMetaData
	movq	-176(%rbp), %rcx                # 8-byte Reload
	cmpq	$0, %rcx
	je	.LBB0_2
# %bb.1:
	movq	-80(%rbp), %rsi
	leaq	-80(%rbp), %rax
	movq	%rax, %rdi
	leaq	-8(%rbp), %rax
	movq	%rsi, -184(%rbp)                # 8-byte Spill
	movq	%rax, %rsi
	leaq	-16(%rbp), %rdx
	callq	_f_loadMetaData
	movabsq	$.L.str.1, %rdi
	movq	-184(%rbp), %rsi                # 8-byte Reload
	movb	$0, %al
	callq	printf
	jmp	.LBB0_3
.LBB0_2:
	movabsq	$.L.str.2, %rdi
	movb	$0, %al
	callq	printf
.LBB0_3:
	addq	$192, %rsp
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
	subq	$112, %rsp
	movl	$0, -20(%rbp)
	movq	ptr, %rdx
	movabsq	$ptr, %rax
	movq	%rax, %rdi
	leaq	-8(%rbp), %rsi
	leaq	-16(%rbp), %rax
	movq	%rdx, -32(%rbp)                 # 8-byte Spill
	movq	%rax, %rdx
	callq	_f_loadMetaData
	movq	-8(%rbp), %rdi
	movq	-16(%rbp), %rax
	movl	$4, %ecx
	movq	%rdi, -40(%rbp)                 # 8-byte Spill
	movl	%ecx, %edi
	movq	%rax, -48(%rbp)                 # 8-byte Spill
	callq	_f_allocateShadowStackMetadata
	movabsq	$array, %rdx
	movq	%rdx, %rdi
	movl	$1, %esi
	movq	%rax, -56(%rbp)                 # 8-byte Spill
	callq	_f_storeBaseOfShadowStack
	movabsq	$array, %rdx
	addq	$400, %rdx                      # imm = 0x190
	movq	%rdx, %rdi
	movl	$1, %esi
	movq	%rax, -64(%rbp)                 # 8-byte Spill
	callq	_f_storeBoundOfShadowStack
	movq	-40(%rbp), %rdi                 # 8-byte Reload
	movl	$2, %esi
	movq	%rax, -72(%rbp)                 # 8-byte Spill
	callq	_f_storeBaseOfShadowStack
	movq	-48(%rbp), %rdi                 # 8-byte Reload
	movl	$2, %esi
	movq	%rax, -80(%rbp)                 # 8-byte Spill
	callq	_f_storeBoundOfShadowStack
	movabsq	$array, %rdx
	movq	%rdx, %rdi
	movl	$3, %esi
	movq	%rax, -88(%rbp)                 # 8-byte Spill
	callq	_f_storeBaseOfShadowStack
	movabsq	$array, %rdx
	addq	$400, %rdx                      # imm = 0x190
	movq	%rdx, %rdi
	movl	$3, %esi
	movq	%rax, -96(%rbp)                 # 8-byte Spill
	callq	_f_storeBoundOfShadowStack
	movabsq	$array, %rdx
	movl	$97, %edi
	movq	%rdx, %rsi
	movq	-32(%rbp), %r8                  # 8-byte Reload
	movq	%rdx, -104(%rbp)                # 8-byte Spill
	movq	%r8, %rdx
	movq	-104(%rbp), %rcx                # 8-byte Reload
	movl	$1213, %r9d                     # imm = 0x4BD
	movl	%r9d, %r8d
	movq	%rax, -112(%rbp)                # 8-byte Spill
	callq	test
	callq	_f_deallocateShadowStackMetaData
	xorl	%eax, %eax
	addq	$112, %rsp
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
	.asciz	"****\n"
	.size	.L.str, 6

	.type	.L.str.1,@object                # @.str.1
.L.str.1:
	.asciz	"ptr : %zxn"
	.size	.L.str.1, 11

	.type	.L.str.2,@object                # @.str.2
.L.str.2:
	.asciz	"&&&&\n"
	.size	.L.str.2, 6

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
	.addrsig_sym test
	.addrsig_sym malloc
	.addrsig_sym printf
	.addrsig_sym _f_loadBaseOfShadowStack
	.addrsig_sym _f_storeBaseOfShadowStack
	.addrsig_sym _f_storeBoundOfShadowStack
	.addrsig_sym _f_loadBoundOfShadowStack
	.addrsig_sym _f_allocateShadowStackMetadata
	.addrsig_sym _f_deallocateShadowStackMetaData
	.addrsig_sym _f_storeMetaData
	.addrsig_sym _f_loadMetaData
	.addrsig_sym _tasmc_global_init
	.addrsig_sym _initTaSMC
	.addrsig_sym array
	.addrsig_sym ptr
