; ModuleID = 'pointer_1.c'
source_filename = "pointer_1.c"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

%struct.struct_has_ptr = type { i8, i32, i32* }

@a = dso_local global i32 10, align 4
@st_has_ptr = dso_local global %struct.struct_has_ptr { i8 97, i32 10, i32* @a }, align 8

!llvm.module.flags = !{!0}
!llvm.ident = !{!1}

!0 = !{i32 1, !"wchar_size", i32 4}
!1 = !{!"clang version 11.0.0"}
