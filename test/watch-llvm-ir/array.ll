; ModuleID = 'array.c'
source_filename = "array.c"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

%struct.array_ele = type { i8, i32 }
%struct.array_2 = type { %struct.array_1, i32 }
%struct.array_1 = type { [100 x %struct.array_ele], i32 }

@array_struct = dso_local constant <{ %struct.array_ele, %struct.array_ele, %struct.array_ele, %struct.array_ele, %struct.array_ele, %struct.array_ele, [94 x %struct.array_ele] }> <{ %struct.array_ele { i8 97, i32 122 }, %struct.array_ele { i8 98, i32 100 }, %struct.array_ele { i8 97, i32 122 }, %struct.array_ele { i8 98, i32 100 }, %struct.array_ele { i8 97, i32 122 }, %struct.array_ele { i8 98, i32 100 }, [94 x %struct.array_ele] zeroinitializer }>, align 16
@test = dso_local constant <{ { <{ %struct.array_ele, %struct.array_ele, [98 x %struct.array_ele] }>, i32 }, [99 x { <{ %struct.array_ele, %struct.array_ele, [98 x %struct.array_ele] }>, i32 }] }> <{ { <{ %struct.array_ele, %struct.array_ele, [98 x %struct.array_ele] }>, i32 } { <{ %struct.array_ele, %struct.array_ele, [98 x %struct.array_ele] }> <{ %struct.array_ele { i8 97, i32 122 }, %struct.array_ele { i8 98, i32 100 }, [98 x %struct.array_ele] zeroinitializer }>, i32 1 }, [99 x { <{ %struct.array_ele, %struct.array_ele, [98 x %struct.array_ele] }>, i32 }] zeroinitializer }>, align 16
@ads = dso_local constant %struct.array_2 zeroinitializer, align 4

; Function Attrs: noinline nounwind optnone uwtable
define dso_local void @a() #0 {
  ret void
}

attributes #0 = { noinline nounwind optnone uwtable "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "frame-pointer"="all" "less-precise-fpmad"="false" "min-legal-vector-width"="0" "no-infs-fp-math"="false" "no-jump-tables"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }

!llvm.module.flags = !{!0}
!llvm.ident = !{!1}

!0 = !{i32 1, !"wchar_size", i32 4}
!1 = !{!"clang version 11.0.0"}
