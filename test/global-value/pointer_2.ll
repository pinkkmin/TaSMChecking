; ModuleID = 'pointer_2.c'
source_filename = "pointer_2.c"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

@array = dso_local global [123 x i32] zeroinitializer, align 16
@ptr = dso_local global i32* getelementptr inbounds ([123 x i32], [123 x i32]* @array, i32 0, i32 0), align 8

!llvm.module.flags = !{!0}
!llvm.ident = !{!1}

!0 = !{i32 1, !"wchar_size", i32 4}
!1 = !{!"clang version 11.0.0"}
