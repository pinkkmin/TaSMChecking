; ModuleID = 'test_1.c'
source_filename = "test_1.c"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

@array = dso_local global <{ i32, i32, i32, i32, [96 x i32] }> <{ i32 0, i32 12, i32 3, i32 4, [96 x i32] zeroinitializer }>, align 16
@ptr = dso_local global i32* bitcast (i8* getelementptr (i8, i8* bitcast (<{ i32, i32, i32, i32, [96 x i32] }>* @array to i8*), i64 1600) to i32*), align 8
@arrat_int_ptr = dso_local global [100 x i32*] zeroinitializer, align 16

; Function Attrs: noinline nounwind optnone uwtable
define dso_local i32 @main() #0 {
  %1 = alloca i32, align 4
  %2 = alloca i32, align 4
  store i32 0, i32* %1, align 4
  %3 = load i32*, i32** @ptr, align 8
  %4 = load i32, i32* %3, align 4
  store i32 %4, i32* %2, align 4
  ret i32 0
}

attributes #0 = { noinline nounwind optnone uwtable "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "frame-pointer"="all" "less-precise-fpmad"="false" "min-legal-vector-width"="0" "no-infs-fp-math"="false" "no-jump-tables"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }

!llvm.module.flags = !{!0}
!llvm.ident = !{!1}

!0 = !{i32 1, !"wchar_size", i32 4}
!1 = !{!"clang version 11.0.0"}
