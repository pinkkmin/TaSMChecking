; ModuleID = 'struct_simple.c'
source_filename = "struct_simple.c"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

%struct.struct_simple = type { i8, i32 }
%struct.struct_has_ptr = type { i8*, i32 }
%struct.struct_has_ptr2 = type { i32, %struct.struct_has_array* }
%struct.struct_has_array = type { i32, [100 x i32] }

@struct_1 = dso_local constant %struct.struct_simple { i8 97, i32 12 }, align 4
@struct_2 = dso_local constant { i32, <{ i32, i32, i32, i32, [96 x i32] }> } { i32 97, <{ i32, i32, i32, i32, [96 x i32] }> <{ i32 1, i32 2, i32 3, i32 4, [96 x i32] zeroinitializer }> }, align 4
@struct_1_array = dso_local constant <{ %struct.struct_simple, [14 x %struct.struct_simple] }> <{ %struct.struct_simple { i8 97, i32 1 }, [14 x %struct.struct_simple] zeroinitializer }>, align 16
@.str = private unnamed_addr constant [7 x i8] c"abcdef\00", align 1
@struct_3 = dso_local constant %struct.struct_has_ptr { i8* getelementptr inbounds ([7 x i8], [7 x i8]* @.str, i32 0, i32 0), i32 12 }, align 8
@struct_4 = dso_local constant %struct.struct_has_ptr2 { i32 12, %struct.struct_has_array* inttoptr (i64 97 to %struct.struct_has_array*) }, align 8
@p1 = dso_local global i32* bitcast (i8* getelementptr (i8, i8* getelementptr inbounds (%struct.struct_simple, %struct.struct_simple* @struct_1, i32 0, i32 0), i64 4) to i32*), align 8
@ch_ptr1 = dso_local global i8* getelementptr (i8, i8* getelementptr inbounds (%struct.struct_simple, %struct.struct_simple* @struct_1, i32 0, i32 0), i64 4), align 8
@p2 = dso_local global i32* bitcast (i8* getelementptr (i8, i8* bitcast ({ i32, <{ i32, i32, i32, i32, [96 x i32] }> }* @struct_2 to i8*), i64 64) to i32*), align 8
@ch_ptr2 = dso_local global i8* getelementptr (i8, i8* bitcast ({ i32, <{ i32, i32, i32, i32, [96 x i32] }> }* @struct_2 to i8*), i64 4), align 8
@array = dso_local constant [100 x i32] zeroinitializer, align 16

!llvm.module.flags = !{!0}
!llvm.ident = !{!1}

!0 = !{i32 1, !"wchar_size", i32 4}
!1 = !{!"clang version 11.0.0"}
