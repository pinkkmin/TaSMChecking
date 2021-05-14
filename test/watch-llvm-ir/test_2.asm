	.text
	.file	"test_2.c"
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
	pushq	%r15
	pushq	%r14
	pushq	%r13
	pushq	%r12
	pushq	%rbx
	subq	$120, %rsp
	.cfi_offset %rbx, -56
	.cfi_offset %r12, -48
	.cfi_offset %r13, -40
	.cfi_offset %r14, -32
	.cfi_offset %r15, -24
	movl	$1, %eax
	movl	%edi, -60(%rbp)                 # 4-byte Spill
	movl	%eax, %edi
	movl	%esi, -64(%rbp)                 # 4-byte Spill
	callq	_f_allocateFunctionKey
	xorl	%ecx, %ecx
	movl	%ecx, %edx
	addq	$4, %rdx
	leaq	-48(%rbp), %r8
	leaq	-56(%rbp), %r9
	leaq	-52(%rbp), %r10
	leaq	-44(%rbp), %r11
	movq	%r11, %rbx
	addq	$4, %r11
	movq	%r8, %r14
	movq	%r8, %r15
	addq	$4, %r15
	movq	%r10, %r12
	addq	$4, %r10
	movq	%r9, %r13
	addq	$4, %r9
	movq	%r8, %rdi
	movq	%r14, %rsi
	movq	%rdx, -72(%rbp)                 # 8-byte Spill
	movq	%r15, %rdx
	movq	-72(%rbp), %rcx                 # 8-byte Reload
	movq	%rax, -80(%rbp)                 # 8-byte Spill
	movq	%rbx, -88(%rbp)                 # 8-byte Spill
	movq	%r11, -96(%rbp)                 # 8-byte Spill
	movq	%r14, -104(%rbp)                # 8-byte Spill
	movq	%r15, -112(%rbp)                # 8-byte Spill
	movq	%r12, -120(%rbp)                # 8-byte Spill
	movq	%r10, -128(%rbp)                # 8-byte Spill
	movq	%r13, -136(%rbp)                # 8-byte Spill
	movq	%r9, -144(%rbp)                 # 8-byte Spill
	callq	_f_checkSpatialStorePtr
	xorl	%eax, %eax
	movl	%eax, %ecx
	addq	$4, %rcx
	movl	-60(%rbp), %eax                 # 4-byte Reload
	movl	%eax, -48(%rbp)
	leaq	-52(%rbp), %rdx
	movq	%rdx, %rdi
	movq	-120(%rbp), %rsi                # 8-byte Reload
	movq	-128(%rbp), %rdx                # 8-byte Reload
	callq	_f_checkSpatialStorePtr
	xorl	%eax, %eax
	movl	%eax, %ecx
	addq	$4, %rcx
	movl	-64(%rbp), %eax                 # 4-byte Reload
	movl	%eax, -52(%rbp)
	leaq	-56(%rbp), %rdx
	movq	%rdx, %rdi
	movq	-136(%rbp), %rsi                # 8-byte Reload
	movq	-144(%rbp), %rdx                # 8-byte Reload
	callq	_f_checkSpatialStorePtr
	xorl	%eax, %eax
	movl	%eax, %ecx
	addq	$4, %rcx
	movl	$123, -56(%rbp)
	leaq	-48(%rbp), %rdx
	movq	%rdx, %rdi
	movq	-104(%rbp), %rsi                # 8-byte Reload
	movq	-112(%rbp), %rdx                # 8-byte Reload
	callq	_f_checkSpatialStorePtr
	xorl	%eax, %eax
	movl	%eax, %ecx
	addq	$4, %rcx
	movl	$12, -48(%rbp)
	leaq	-44(%rbp), %rdx
	movq	%rdx, %rdi
	movq	-88(%rbp), %rsi                 # 8-byte Reload
	movq	-96(%rbp), %rdx                 # 8-byte Reload
	callq	_f_checkSpatialLoadPtr
	movl	-44(%rbp), %eax
	movl	$1, %edi
	movl	%eax, -148(%rbp)                # 4-byte Spill
	callq	_f_deallocaFunctionKey
	movl	-148(%rbp), %eax                # 4-byte Reload
	addq	$120, %rsp
	popq	%rbx
	popq	%r12
	popq	%r13
	popq	%r14
	popq	%r15
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
	pushq	%rbx
	subq	$40, %rsp
	.cfi_offset %rbx, -24
	movl	$2, %edi
	callq	_f_allocateFunctionKey
	xorl	%ecx, %ecx
	movl	%ecx, %edx
	addq	$4, %rdx
	leaq	-12(%rbp), %rsi
	leaq	-16(%rbp), %r8
	movq	%rsi, %r9
	movq	%rsi, %r10
	addq	$4, %r10
	movq	%r8, %r11
	addq	$4, %r8
	movq	%rsi, %rdi
	movq	%r9, %rsi
	movq	%rdx, -24(%rbp)                 # 8-byte Spill
	movq	%r10, %rdx
	movq	-24(%rbp), %rcx                 # 8-byte Reload
	movq	%rax, -32(%rbp)                 # 8-byte Spill
	movq	%r11, -40(%rbp)                 # 8-byte Spill
	movq	%r8, -48(%rbp)                  # 8-byte Spill
	callq	_f_checkSpatialStorePtr
	xorl	%ebx, %ebx
	movl	%ebx, %eax
	addq	$4, %rax
	movl	$0, -12(%rbp)
	leaq	-16(%rbp), %rcx
	movq	%rcx, %rdi
	movq	-40(%rbp), %rsi                 # 8-byte Reload
	movq	-48(%rbp), %rdx                 # 8-byte Reload
	movq	%rax, %rcx
	callq	_f_checkSpatialStorePtr
	movl	$12, -16(%rbp)
	movl	$2, %edi
	callq	_f_deallocaFunctionKey
	xorl	%eax, %eax
	addq	$40, %rsp
	popq	%rbx
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
	addq	$616, %rcx                      # imm = 0x268
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
	.bss
	.globl	array
	.p2align	4
array:
	.zero	616
	.size	array, 616

	.type	ptr,@object                     # @ptr
	.data
	.globl	ptr
	.p2align	3
ptr:
	.quad	array
	.size	ptr, 8

	.section	.init_array.0,"aw",@init_array
	.p2align	3
	.quad	_tasmc_global_init
	.ident	"clang version 11.0.0"
	.section	".note.GNU-stack","",@progbits
	.addrsig
	.addrsig_sym _f_checkSpatialLoadPtr
	.addrsig_sym _f_checkSpatialStorePtr
	.addrsig_sym _f_storeMetaData
	.addrsig_sym _f_allocateFunctionKey
	.addrsig_sym _f_deallocaFunctionKey
	.addrsig_sym _tasmc_global_init
	.addrsig_sym _initTaSMC
	.addrsig_sym array
	.addrsig_sym ptr
