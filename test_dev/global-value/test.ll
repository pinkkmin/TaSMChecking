; ModuleID = 'test.c'
source_filename = "test.c"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

%struct.node_1 = type { i8, i32 }
%struct.node_3 = type { %struct.node_2, i32 }
%struct.node_2 = type { [100 x %struct.node_1], i32 }
%struct.node_4 = type { %struct.node_3*, i32 }

@array_struct = dso_local constant <{ %struct.node_1, %struct.node_1, %struct.node_1, %struct.node_1, %struct.node_1, %struct.node_1, [94 x %struct.node_1] }> <{ %struct.node_1 { i8 97, i32 122 }, %struct.node_1 { i8 98, i32 100 }, %struct.node_1 { i8 97, i32 122 }, %struct.node_1 { i8 98, i32 100 }, %struct.node_1 { i8 97, i32 122 }, %struct.node_1 { i8 98, i32 100 }, [94 x %struct.node_1] zeroinitializer }>, align 16
@test = dso_local constant <{ { <{ %struct.node_1, %struct.node_1, [98 x %struct.node_1] }>, i32 }, [99 x { <{ %struct.node_1, %struct.node_1, [98 x %struct.node_1] }>, i32 }] }> <{ { <{ %struct.node_1, %struct.node_1, [98 x %struct.node_1] }>, i32 } { <{ %struct.node_1, %struct.node_1, [98 x %struct.node_1] }> <{ %struct.node_1 { i8 97, i32 122 }, %struct.node_1 { i8 98, i32 100 }, [98 x %struct.node_1] zeroinitializer }>, i32 1 }, [99 x { <{ %struct.node_1, %struct.node_1, [98 x %struct.node_1] }>, i32 }] zeroinitializer }>, align 16
@ads = dso_local constant %struct.node_3 zeroinitializer, align 4
@asasa = dso_local constant [10 x %struct.node_1] zeroinitializer, align 16
@ad12s = dso_local global %struct.node_3 zeroinitializer, align 4
@array3 = dso_local global %struct.node_4 zeroinitializer, align 8
@.str = private unnamed_addr constant [14 x i8] c"dsdsdsdsdsdsd\00", align 1
@p = dso_local global i8* getelementptr inbounds ([14 x i8], [14 x i8]* @.str, i32 0, i32 0), align 8
@aq = dso_local constant i32 100, align 4
@bq = dso_local constant i32 100, align 4
@array1 = dso_local constant <{ i32, i32, [98 x i32] }> <{ i32 100, i32 199, [98 x i32] zeroinitializer }>, align 16
@pa = dso_local global i32* bitcast (i8* getelementptr (i8, i8* getelementptr inbounds (<{ %struct.node_1, %struct.node_1, %struct.node_1, %struct.node_1, %struct.node_1, %struct.node_1, [94 x %struct.node_1] }>, <{ %struct.node_1, %struct.node_1, %struct.node_1, %struct.node_1, %struct.node_1, %struct.node_1, [94 x %struct.node_1] }>* @array_struct, i32 0, i32 0, i32 0), i64 680) to i32*), align 8
@.str.1 = private unnamed_addr constant [4 x i8] c"%zu\00", align 1

; Function Attrs: noinline nounwind optnone uwtable
define dso_local i32 @main() #0 {
  %1 = alloca i32, align 4
  %2 = alloca i64, align 8
  store i32 0, i32* %1, align 4
  %3 = load i32*, i32** @pa, align 8
  %4 = ptrtoint i32* %3 to i64
  store i64 %4, i64* %2, align 8
  %5 = load i64, i64* %2, align 8
  %6 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @.str.1, i64 0, i64 0), i64 %5)
  ret i32 0
}

declare dso_local i32 @printf(i8*, ...) #1

attributes #0 = { noinline nounwind optnone uwtable "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "frame-pointer"="all" "less-precise-fpmad"="false" "min-legal-vector-width"="0" "no-infs-fp-math"="false" "no-jump-tables"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #1 = { "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "frame-pointer"="all" "less-precise-fpmad"="false" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }

!llvm.module.flags = !{!0}
!llvm.ident = !{!1}

!0 = !{i32 1, !"wchar_size", i32 4}
!1 = !{!"clang version 11.0.0"}
