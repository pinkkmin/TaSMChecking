; ModuleID = 'struct_simple.c'
source_filename = "struct_simple.c"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

%struct.struct_simple = type { i8, i32, i32 }
%struct.struct_has_ptr = type { i8*, i32 }
%struct.struct_has_ptr2 = type { i32, %struct.struct_has_array* }
%struct.struct_has_array = type { i32, [100 x i32] }

@struct_1 = dso_local constant %struct.struct_simple { i8 97, i32 12, i32 0 }, align 4
@struct_2 = dso_local constant { i32, <{ i32, i32, i32, i32, [96 x i32] }> } { i32 11, <{ i32, i32, i32, i32, [96 x i32] }> <{ i32 1, i32 2, i32 3, i32 4, [96 x i32] zeroinitializer }> }, align 4
@struct_1_array = dso_local global <{ %struct.struct_simple, [14 x %struct.struct_simple] }> <{ %struct.struct_simple { i8 97, i32 1, i32 0 }, [14 x %struct.struct_simple] zeroinitializer }>, align 16
@.str = private unnamed_addr constant [7 x i8] c"abcdef\00", align 1
@struct_3 = dso_local constant %struct.struct_has_ptr { i8* getelementptr inbounds ([7 x i8], [7 x i8]* @.str, i32 0, i32 0), i32 12 }, align 8
@struct_4 = dso_local constant %struct.struct_has_ptr2 { i32 12, %struct.struct_has_array* inttoptr (i64 97 to %struct.struct_has_array*) }, align 8
@p1 = dso_local global i32* bitcast (i8* getelementptr (i8, i8* getelementptr inbounds (%struct.struct_simple, %struct.struct_simple* @struct_1, i32 0, i32 0), i64 4) to i32*), align 8
@ch_ptr1 = dso_local global   getelementptr (i8, i8* getelementptr inbounds (%struct.struct_simple, %struct.struct_simple* @struct_1, i32 0, i32 0), i64 4), align 8
@p2 = dso_local global i32* bitcast (i8* getelementptr (i8, i8* bitcast ({ i32, <{ i32, i32, i32, i32, [96 x i32] }> }* @struct_2 to i8*), i64 636) to i32*), align 8
@ch_ptr2 = dso_local global i8* getelementptr (i8, i8* bitcast ({ i32, <{ i32, i32, i32, i32, [96 x i32] }> }* @struct_2 to i8*), i64 4), align 8
@array = dso_local global <{ i32, i32, i32, i32, [96 x i32] }> <{ i32 0, i32 12, i32 3, i32 4, [96 x i32] zeroinitializer }>, align 16
@ptr_to_a = dso_local constant i32 1, align 4
@arrat_int_ptr = dso_local global [100 x i32*] zeroinitializer, align 16
@ptrArray = dso_local global [1212 x i32*] zeroinitializer, align 16
@array1 = dso_local global [123 x %struct.struct_simple] zeroinitializer, align 16

; Function Attrs: noinline nounwind optnone uwtable
define dso_local void @test() #0 {
  %1 = load i32*, i32** getelementptr inbounds ([100 x i32*], [100 x i32*]* @arrat_int_ptr, i64 0, i64 11), align 8
  store i32 12, i32* %1, align 4
  ret void
}

attributes #0 = { noinline nounwind optnone uwtable "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "frame-pointer"="all" "less-precise-fpmad"="false" "min-legal-vector-width"="0" "no-infs-fp-math"="false" "no-jump-tables"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }

!llvm.module.flags = !{!0}
!llvm.ident = !{!1}

!0 = !{i32 1, !"wchar_size", i32 4}
!1 = !{!"clang version 11.0.0"}
