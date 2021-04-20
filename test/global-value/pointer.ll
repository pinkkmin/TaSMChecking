; ModuleID = 'pointer.c'
source_filename = "pointer.c"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

@a = dso_local constant i32 10, align 4
@ptr_const_a = dso_local global i32* @a, align 8
@array = dso_local constant [100 x i32] zeroinitializer, align 16
@ptr_to_const = dso_local global i32* getelementptr inbounds ([100 x i32], [100 x i32]* @array, i32 0, i32 0), align 8
@ptr_array = dso_local global [10 x i32*] zeroinitializer, align 16
@ptr_null = dso_local global i32* null, align 8
@ptr_const_null = dso_local global i32* null, align 8
@ptr_to_const_null = dso_local global i32* null, align 8

; Function Attrs: noinline nounwind optnone uwtable
define dso_local void @test(i32 %0, i32* %1) #0 {
  %3 = alloca i32, align 4
  %4 = alloca i32*, align 8
  store i32 %0, i32* %3, align 4
  store i32* %1, i32** %4, align 8
  %5 = load i32*, i32** getelementptr inbounds ([10 x i32*], [10 x i32*]* @ptr_array, i64 1, i64 2), align 16
  store i32 12, i32* %5, align 4
  ret void
}

attributes #0 = { noinline nounwind optnone uwtable "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "frame-pointer"="all" "less-precise-fpmad"="false" "min-legal-vector-width"="0" "no-infs-fp-math"="false" "no-jump-tables"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }

!llvm.module.flags = !{!0}
!llvm.ident = !{!1}

!0 = !{i32 1, !"wchar_size", i32 4}
!1 = !{!"clang version 11.0.0"}
