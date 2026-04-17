; ModuleID = 'build/test_clean.ll'
source_filename = "test/test.c"
target datalayout = "e-m:o-i64:64-i128:128-n32:64-S128-Fn32"
target triple = "arm64-apple-macosx15.0.0"

@.str = private unnamed_addr constant [10 x i8] c"%d %u %d\0A\00", align 1

; Function Attrs: noinline nounwind ssp uwtable(sync)
define i32 @test_algebraic(i32 noundef %0) #0 {
  ret i32 %0
}

; Function Attrs: noinline nounwind ssp uwtable(sync)
define i32 @test_strength(i32 noundef %0) #0 {
  %shl_sr = shl i32 %0, 4
  %mul_sr = sub i32 %shl_sr, %0
  %udiv_sr = lshr i32 %mul_sr, 3
  ret i32 %udiv_sr
}

; Function Attrs: noinline nounwind ssp uwtable(sync)
define i32 @test_multi(i32 noundef %0) #0 {
  %2 = add nsw i32 %0, 1
  ret i32 %0
}

; Function Attrs: noinline nounwind ssp uwtable(sync)
define i32 @main() #0 {
  %1 = call i32 @test_algebraic(i32 noundef 10)
  %2 = call i32 @test_strength(i32 noundef 10)
  %3 = call i32 @test_multi(i32 noundef 10)
  %4 = call i32 (ptr, ...) @printf(ptr noundef @.str, i32 noundef %1, i32 noundef %2, i32 noundef %3)
  ret i32 0
}

declare i32 @printf(ptr noundef, ...) #1

attributes #0 = { noinline nounwind ssp uwtable(sync) "frame-pointer"="non-leaf" "no-trapping-math"="true" "probe-stack"="__chkstk_darwin" "stack-protector-buffer-size"="8" "target-cpu"="apple-m1" "target-features"="+aes,+altnzcv,+bti,+ccdp,+ccidx,+complxnum,+crc,+dit,+dotprod,+flagm,+fp-armv8,+fp16fml,+fptoint,+fullfp16,+jsconv,+lse,+neon,+pauth,+perfmon,+predres,+ras,+rcpc,+rdm,+sb,+sha2,+sha3,+specrestrict,+ssbs,+v8.1a,+v8.2a,+v8.3a,+v8.4a,+v8.5a,+v8a,+zcm,+zcz" }
attributes #1 = { "frame-pointer"="non-leaf" "no-trapping-math"="true" "probe-stack"="__chkstk_darwin" "stack-protector-buffer-size"="8" "target-cpu"="apple-m1" "target-features"="+aes,+altnzcv,+bti,+ccdp,+ccidx,+complxnum,+crc,+dit,+dotprod,+flagm,+fp-armv8,+fp16fml,+fptoint,+fullfp16,+jsconv,+lse,+neon,+pauth,+perfmon,+predres,+ras,+rcpc,+rdm,+sb,+sha2,+sha3,+specrestrict,+ssbs,+v8.1a,+v8.2a,+v8.3a,+v8.4a,+v8.5a,+v8a,+zcm,+zcz" }

!llvm.module.flags = !{!0, !1, !2, !3, !4}
!llvm.ident = !{!5}

!0 = !{i32 2, !"SDK Version", [2 x i32] [i32 15, i32 5]}
!1 = !{i32 1, !"wchar_size", i32 4}
!2 = !{i32 8, !"PIC Level", i32 2}
!3 = !{i32 7, !"uwtable", i32 1}
!4 = !{i32 7, !"frame-pointer", i32 1}
!5 = !{!"Apple clang version 17.0.0 (clang-1700.0.13.5)"}
