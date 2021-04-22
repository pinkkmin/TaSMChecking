; ModuleID = 'test_1.c'
source_filename = "test_1.c"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

@array = dso_local global <{ i32, i32, i32, i32, [96 x i32] }> <{ i32 0, i32 12, i32 3, i32 4, [96 x i32] zeroinitializer }>, align 16
@ptr = dso_local global i32* bitcast (i8* getelementptr (i8, i8* bitcast (<{ i32, i32, i32, i32, [96 x i32] }>* @array to i8*), i64 4) to i32*), align 8
@arrat_int_ptr = dso_local global [100 x i32*] zeroinitializer, align 16

; Function Attrs: noinline nounwind optnone uwtable
define dso_local void @test(i8 signext %0, i32* %1, i32* %2, i32* %3, i32 %4) #0 {
  %6 = alloca i8, align 1
  %7 = alloca i32*, align 8
  %8 = alloca i32*, align 8
  %9 = alloca i32*, align 8
  %10 = alloca i32, align 4
  %11 = alloca i32*, align 8
  store i8 %0, i8* %6, align 1
  store i32* %1, i32** %7, align 8
  store i32* %2, i32** %8, align 8
  store i32* %3, i32** %9, align 8
  store i32 %4, i32* %10, align 4
  %12 = call noalias i8* @malloc(i64 4) #2
  %13 = bitcast i8* %12 to i32*
  store i32* %13, i32** %11, align 8
  ret void
}

; Function Attrs: nounwind
declare dso_local noalias i8* @malloc(i64) #1

; Function Attrs: noinline nounwind optnone uwtable
define dso_local i32 @main() #0 {
  %1 = alloca i32, align 4
  store i32 0, i32* %1, align 4
  %2 = load i32*, i32** @ptr, align 8
  call void @test(i8 signext 97, i32* getelementptr inbounds ([100 x i32], [100 x i32]* bitcast (<{ i32, i32, i32, i32, [96 x i32] }>* @array to [100 x i32]*), i64 0, i64 0), i32* %2, i32* getelementptr inbounds ([100 x i32], [100 x i32]* bitcast (<{ i32, i32, i32, i32, [96 x i32] }>* @array to [100 x i32]*), i64 0, i64 0), i32 1213)
  ret i32 0
}

attributes #0 = { noinline nounwind optnone uwtable "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "frame-pointer"="all" "less-precise-fpmad"="false" "min-legal-vector-width"="0" "no-infs-fp-math"="false" "no-jump-tables"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #1 = { nounwind "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "frame-pointer"="all" "less-precise-fpmad"="false" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #2 = { nounwind }

!llvm.module.flags = !{!0}
!llvm.ident = !{!1}

!0 = !{i32 1, !"wchar_size", i32 4}
!1 = !{!"clang version 11.0.0"}
