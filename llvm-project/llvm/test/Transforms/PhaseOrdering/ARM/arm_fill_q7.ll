; NOTE: Assertions have been autogenerated by utils/update_test_checks.py
; RUN: opt < %s -O3 -S                                        | FileCheck --check-prefix=OLDPM %s
; RUN: opt < %s -passes='default<O3>' -S | FileCheck --check-prefix=NEWPM %s

target datalayout = "e-m:e-p:32:32-Fi8-i64:64-v128:64:128-a:0:32-n32-S64"
target triple = "thumbv6m-none-none-eabi"

; Not only should we be able to to form memsets here, the original loops
; should be deleted, too.

; Function Attrs: nounwind
define dso_local void @arm_fill_q7(i8 signext %value, ptr %pDst, i32 %blockSize) #0 {
; OLDPM-LABEL: @arm_fill_q7(
; OLDPM-NEXT:  entry:
; OLDPM-NEXT:    [[CMP_NOT20:%.*]] = icmp ult i32 [[BLOCKSIZE:%.*]], 4
; OLDPM-NEXT:    br i1 [[CMP_NOT20]], label [[WHILE_END:%.*]], label [[WHILE_BODY_PREHEADER:%.*]]
; OLDPM:       while.body.preheader:
; OLDPM-NEXT:    [[TMP0:%.*]] = and i32 [[BLOCKSIZE]], -4
; OLDPM-NEXT:    call void @llvm.memset.p0.i32(ptr align 1 [[PDST:%.*]], i8 [[VALUE:%.*]], i32 [[TMP0]], i1 false)
; OLDPM-NEXT:    [[SCEVGEP:%.*]] = getelementptr i8, ptr [[PDST]], i32 [[TMP0]]
; OLDPM-NEXT:    br label [[WHILE_END]]
; OLDPM:       while.end:
; OLDPM-NEXT:    [[PDST_ADDR_0_LCSSA:%.*]] = phi ptr [ [[PDST]], [[ENTRY:%.*]] ], [ [[SCEVGEP]], [[WHILE_BODY_PREHEADER]] ]
; OLDPM-NEXT:    [[REM:%.*]] = and i32 [[BLOCKSIZE]], 3
; OLDPM-NEXT:    [[CMP14_NOT17:%.*]] = icmp eq i32 [[REM]], 0
; OLDPM-NEXT:    br i1 [[CMP14_NOT17]], label [[WHILE_END18:%.*]], label [[WHILE_BODY16_PREHEADER:%.*]]
; OLDPM:       while.body16.preheader:
; OLDPM-NEXT:    call void @llvm.memset.p0.i32(ptr align 1 [[PDST_ADDR_0_LCSSA]], i8 [[VALUE]], i32 [[REM]], i1 false)
; OLDPM-NEXT:    br label [[WHILE_END18]]
; OLDPM:       while.end18:
; OLDPM-NEXT:    ret void
;
; NEWPM-LABEL: @arm_fill_q7(
; NEWPM-NEXT:  entry:
; NEWPM-NEXT:    [[CMP_NOT17:%.*]] = icmp ult i32 [[BLOCKSIZE:%.*]], 4
; NEWPM-NEXT:    br i1 [[CMP_NOT17]], label [[WHILE_END:%.*]], label [[WHILE_BODY_PREHEADER:%.*]]
; NEWPM:       while.body.preheader:
; NEWPM-NEXT:    [[TMP0:%.*]] = and i32 [[BLOCKSIZE]], -4
; NEWPM-NEXT:    call void @llvm.memset.p0.i32(ptr align 1 [[PDST:%.*]], i8 [[VALUE:%.*]], i32 [[TMP0]], i1 false)
; NEWPM-NEXT:    [[SCEVGEP:%.*]] = getelementptr i8, ptr [[PDST]], i32 [[TMP0]]
; NEWPM-NEXT:    br label [[WHILE_END]]
; NEWPM:       while.end:
; NEWPM-NEXT:    [[PDST_ADDR_0_LCSSA:%.*]] = phi ptr [ [[PDST]], [[ENTRY:%.*]] ], [ [[SCEVGEP]], [[WHILE_BODY_PREHEADER]] ]
; NEWPM-NEXT:    [[REM:%.*]] = and i32 [[BLOCKSIZE]], 3
; NEWPM-NEXT:    [[CMP14_NOT20:%.*]] = icmp eq i32 [[REM]], 0
; NEWPM-NEXT:    br i1 [[CMP14_NOT20]], label [[WHILE_END18:%.*]], label [[WHILE_BODY16_PREHEADER:%.*]]
; NEWPM:       while.body16.preheader:
; NEWPM-NEXT:    call void @llvm.memset.p0.i32(ptr align 1 [[PDST_ADDR_0_LCSSA]], i8 [[VALUE]], i32 [[REM]], i1 false)
; NEWPM-NEXT:    br label [[WHILE_END18]]
; NEWPM:       while.end18:
; NEWPM-NEXT:    ret void
;
entry:
  %value.addr = alloca i8, align 1
  %pDst.addr = alloca ptr, align 4
  %blockSize.addr = alloca i32, align 4
  %blkCnt = alloca i32, align 4
  %packedValue = alloca i32, align 4
  store i8 %value, ptr %value.addr, align 1, !tbaa !3
  store ptr %pDst, ptr %pDst.addr, align 4, !tbaa !6
  store i32 %blockSize, ptr %blockSize.addr, align 4, !tbaa !8
  call void @llvm.lifetime.start.p0(i64 4, ptr %blkCnt) #3
  call void @llvm.lifetime.start.p0(i64 4, ptr %packedValue) #3
  %0 = load i8, ptr %value.addr, align 1, !tbaa !3
  %conv = sext i8 %0 to i32
  %shl = shl i32 %conv, 0
  %and = and i32 %shl, 255
  %1 = load i8, ptr %value.addr, align 1, !tbaa !3
  %conv1 = sext i8 %1 to i32
  %shl2 = shl i32 %conv1, 8
  %and3 = and i32 %shl2, 65280
  %or = or i32 %and, %and3
  %2 = load i8, ptr %value.addr, align 1, !tbaa !3
  %conv4 = sext i8 %2 to i32
  %shl5 = shl i32 %conv4, 16
  %and6 = and i32 %shl5, 16711680
  %or7 = or i32 %or, %and6
  %3 = load i8, ptr %value.addr, align 1, !tbaa !3
  %conv8 = sext i8 %3 to i32
  %shl9 = shl i32 %conv8, 24
  %and10 = and i32 %shl9, -16777216
  %or11 = or i32 %or7, %and10
  store i32 %or11, ptr %packedValue, align 4, !tbaa !8
  %4 = load i32, ptr %blockSize.addr, align 4, !tbaa !8
  %shr = lshr i32 %4, 2
  store i32 %shr, ptr %blkCnt, align 4, !tbaa !8
  br label %while.cond

while.cond:                                       ; preds = %while.body, %entry
  %5 = load i32, ptr %blkCnt, align 4, !tbaa !8
  %cmp = icmp ugt i32 %5, 0
  br i1 %cmp, label %while.body, label %while.end

while.body:                                       ; preds = %while.cond
  %6 = load i32, ptr %packedValue, align 4, !tbaa !8
  call void @write_q7x4_ia(ptr %pDst.addr, i32 %6)
  %7 = load i32, ptr %blkCnt, align 4, !tbaa !8
  %dec = add i32 %7, -1
  store i32 %dec, ptr %blkCnt, align 4, !tbaa !8
  br label %while.cond, !llvm.loop !10

while.end:                                        ; preds = %while.cond
  %8 = load i32, ptr %blockSize.addr, align 4, !tbaa !8
  %rem = urem i32 %8, 4
  store i32 %rem, ptr %blkCnt, align 4, !tbaa !8
  br label %while.cond13

while.cond13:                                     ; preds = %while.body16, %while.end
  %9 = load i32, ptr %blkCnt, align 4, !tbaa !8
  %cmp14 = icmp ugt i32 %9, 0
  br i1 %cmp14, label %while.body16, label %while.end18

while.body16:                                     ; preds = %while.cond13
  %10 = load i8, ptr %value.addr, align 1, !tbaa !3
  %11 = load ptr, ptr %pDst.addr, align 4, !tbaa !6
  %incdec.ptr = getelementptr inbounds i8, ptr %11, i32 1
  store ptr %incdec.ptr, ptr %pDst.addr, align 4, !tbaa !6
  store i8 %10, ptr %11, align 1, !tbaa !3
  %12 = load i32, ptr %blkCnt, align 4, !tbaa !8
  %dec17 = add i32 %12, -1
  store i32 %dec17, ptr %blkCnt, align 4, !tbaa !8
  br label %while.cond13, !llvm.loop !12

while.end18:                                      ; preds = %while.cond13
  call void @llvm.lifetime.end.p0(i64 4, ptr %packedValue) #3
  call void @llvm.lifetime.end.p0(i64 4, ptr %blkCnt) #3
  ret void
}

; Function Attrs: argmemonly nofree nosync nounwind willreturn
declare void @llvm.lifetime.start.p0(i64 immarg, ptr nocapture) #1

; Function Attrs: alwaysinline nounwind
define internal void @write_q7x4_ia(ptr %pQ7, i32 %value) #2 {
entry:
  %pQ7.addr = alloca ptr, align 4
  %value.addr = alloca i32, align 4
  %val = alloca i32, align 4
  store ptr %pQ7, ptr %pQ7.addr, align 4, !tbaa !6
  store i32 %value, ptr %value.addr, align 4, !tbaa !8
  call void @llvm.lifetime.start.p0(i64 4, ptr %val) #3
  %0 = load i32, ptr %value.addr, align 4, !tbaa !8
  store i32 %0, ptr %val, align 4, !tbaa !8
  %1 = load i32, ptr %val, align 4, !tbaa !8
  %and = and i32 %1, 255
  %conv = trunc i32 %and to i8
  %2 = load ptr, ptr %pQ7.addr, align 4, !tbaa !6
  %3 = load ptr, ptr %2, align 4, !tbaa !6
  store i8 %conv, ptr %3, align 1, !tbaa !3
  %4 = load i32, ptr %val, align 4, !tbaa !8
  %shr = ashr i32 %4, 8
  %and1 = and i32 %shr, 255
  %conv2 = trunc i32 %and1 to i8
  %5 = load ptr, ptr %pQ7.addr, align 4, !tbaa !6
  %6 = load ptr, ptr %5, align 4, !tbaa !6
  %arrayidx3 = getelementptr inbounds i8, ptr %6, i32 1
  store i8 %conv2, ptr %arrayidx3, align 1, !tbaa !3
  %7 = load i32, ptr %val, align 4, !tbaa !8
  %shr4 = ashr i32 %7, 16
  %and5 = and i32 %shr4, 255
  %conv6 = trunc i32 %and5 to i8
  %8 = load ptr, ptr %pQ7.addr, align 4, !tbaa !6
  %9 = load ptr, ptr %8, align 4, !tbaa !6
  %arrayidx7 = getelementptr inbounds i8, ptr %9, i32 2
  store i8 %conv6, ptr %arrayidx7, align 1, !tbaa !3
  %10 = load i32, ptr %val, align 4, !tbaa !8
  %shr8 = ashr i32 %10, 24
  %and9 = and i32 %shr8, 255
  %conv10 = trunc i32 %and9 to i8
  %11 = load ptr, ptr %pQ7.addr, align 4, !tbaa !6
  %12 = load ptr, ptr %11, align 4, !tbaa !6
  %arrayidx11 = getelementptr inbounds i8, ptr %12, i32 3
  store i8 %conv10, ptr %arrayidx11, align 1, !tbaa !3
  %13 = load ptr, ptr %pQ7.addr, align 4, !tbaa !6
  %14 = load ptr, ptr %13, align 4, !tbaa !6
  %add.ptr = getelementptr inbounds i8, ptr %14, i32 4
  store ptr %add.ptr, ptr %13, align 4, !tbaa !6
  call void @llvm.lifetime.end.p0(i64 4, ptr %val) #3
  ret void
}

; Function Attrs: argmemonly nofree nosync nounwind willreturn
declare void @llvm.lifetime.end.p0(i64 immarg, ptr nocapture) #1

attributes #0 = { nounwind "disable-tail-calls"="false" "frame-pointer"="all" "less-precise-fpmad"="false" "min-legal-vector-width"="0" "no-infs-fp-math"="false" "no-jump-tables"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="cortex-m0plus" "target-features"="+armv6-m,+strict-align,+thumb-mode,-aes,-bf16,-cdecp0,-cdecp1,-cdecp2,-cdecp3,-cdecp4,-cdecp5,-cdecp6,-cdecp7,-crc,-crypto,-dotprod,-dsp,-fp16fml,-fullfp16,-hwdiv,-hwdiv-arm,-i8mm,-lob,-mve,-mve.fp,-ras,-sb,-sha2" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #1 = { argmemonly nofree nosync nounwind willreturn }
attributes #2 = { alwaysinline nounwind "disable-tail-calls"="false" "frame-pointer"="all" "less-precise-fpmad"="false" "min-legal-vector-width"="0" "no-infs-fp-math"="false" "no-jump-tables"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="cortex-m0plus" "target-features"="+armv6-m,+strict-align,+thumb-mode,-aes,-bf16,-cdecp0,-cdecp1,-cdecp2,-cdecp3,-cdecp4,-cdecp5,-cdecp6,-cdecp7,-crc,-crypto,-dotprod,-dsp,-fp16fml,-fullfp16,-hwdiv,-hwdiv-arm,-i8mm,-lob,-mve,-mve.fp,-ras,-sb,-sha2" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #3 = { nounwind }

!llvm.module.flags = !{!0, !1}
!llvm.ident = !{!2}

!0 = !{i32 1, !"wchar_size", i32 4}
!1 = !{i32 1, !"min_enum_size", i32 4}
!2 = !{!"clang version 12.0.0 (https://github.com/llvm/llvm-project.git 07f234be1ccbce131704f580aa3f117083a887f7)"}
!3 = !{!4, !4, i64 0}
!4 = !{!"omnipotent char", !5, i64 0}
!5 = !{!"Simple C/C++ TBAA"}
!6 = !{!7, !7, i64 0}
!7 = !{!"any pointer", !4, i64 0}
!8 = !{!9, !9, i64 0}
!9 = !{!"int", !4, i64 0}
!10 = distinct !{!10, !11}
!11 = !{!"llvm.loop.mustprogress"}
!12 = distinct !{!12, !11}