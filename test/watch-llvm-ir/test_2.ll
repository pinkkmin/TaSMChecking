; ModuleID = 'test_2.c'
source_filename = "test_2.c"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

%struct.Node = type { i32, [10 x i32] }

@test = dso_local global %struct.Node { i32 10, [10 x i32] [i32 15, i32 15, i32 7, i32 7, i32 0, i32 0, i32 0, i32 0, i32 0, i32 0] }, align 4
@ptr = dso_local global i32* bitcast (i8* getelementptr (i8, i8* bitcast (%struct.Node* @test to i8*), i64 4) to i32*), align 8
@array = dso_local global [154 x i32] zeroinitializer, align 16

; Function Attrs: noinline nounwind optnone uwtable
define dso_local i32 @main() #0 {
  %1 = alloca i32, align 4
  store i32 0, i32* %1, align 4
  %2 = load i32*, i32** @ptr, align 8
  %3 = getelementptr inbounds i32, i32* %2, i64 15
  store i32* %3, i32** @ptr, align 8
  %4 = load i32*, i32** @ptr, align 8
  store i32 100, i32* %4, align 4
  ret i32 0
}

attributes #0 = { noinline nounwind optnone uwtable "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "frame-pointer"="all" "less-precise-fpmad"="false" "min-legal-vector-width"="0" "no-infs-fp-math"="false" "no-jump-tables"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }

!llvm.module.flags = !{!0}
!llvm.ident = !{!1}

!0 = !{i32 1, !"wchar_size", i32 4}
!1 = !{!"clang version 11.0.0"}
