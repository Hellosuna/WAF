; NOTE: Assertions have been autogenerated by utils/update_test_checks.py
; Tests to make sure elimination of casts is working correctly
; RUN: opt < %s -passes=instcombine -S | FileCheck %s

target datalayout = "p:32:32-p1:32:32-p2:16:16"

@global = global i8 0

; This shouldn't convert to getelementptr because the relationship
; between the arithmetic and the layout of allocated memory is
; entirely unknown.

define ptr @test1(ptr %t) {
; CHECK-LABEL: @test1(
; CHECK-NEXT:    [[TC:%.*]] = ptrtoint ptr [[T:%.*]] to i32
; CHECK-NEXT:    [[TA:%.*]] = add i32 [[TC]], 32
; CHECK-NEXT:    [[TV:%.*]] = inttoptr i32 [[TA]] to ptr
; CHECK-NEXT:    ret ptr [[TV]]
;
  %tc = ptrtoint ptr %t to i32
  %ta = add i32 %tc, 32
  %tv = inttoptr i32 %ta to ptr
  ret ptr %tv
}

; These casts should be folded away.

define i1 @test2(ptr %a, ptr %b) {
; CHECK-LABEL: @test2(
; CHECK-NEXT:    [[R:%.*]] = icmp eq ptr [[A:%.*]], [[B:%.*]]
; CHECK-NEXT:    ret i1 [[R]]
;
  %ta = ptrtoint ptr %a to i32
  %tb = ptrtoint ptr %b to i32
  %r = icmp eq i32 %ta, %tb
  ret i1 %r
}

; These casts should be folded away.

define i1 @test2_as2_same_int(ptr addrspace(2) %a, ptr addrspace(2) %b) {
; CHECK-LABEL: @test2_as2_same_int(
; CHECK-NEXT:    [[R:%.*]] = icmp eq ptr addrspace(2) [[A:%.*]], [[B:%.*]]
; CHECK-NEXT:    ret i1 [[R]]
;
  %ta = ptrtoint ptr addrspace(2) %a to i16
  %tb = ptrtoint ptr addrspace(2) %b to i16
  %r = icmp eq i16 %ta, %tb
  ret i1 %r
}

; These casts should be folded away.

define i1 @test2_as2_larger(ptr addrspace(2) %a, ptr addrspace(2) %b) {
; CHECK-LABEL: @test2_as2_larger(
; CHECK-NEXT:    [[R:%.*]] = icmp eq ptr addrspace(2) [[A:%.*]], [[B:%.*]]
; CHECK-NEXT:    ret i1 [[R]]
;
  %ta = ptrtoint ptr addrspace(2) %a to i32
  %tb = ptrtoint ptr addrspace(2) %b to i32
  %r = icmp eq i32 %ta, %tb
  ret i1 %r
}

; These casts should not be folded away.

define i1 @test2_diff_as(ptr %p, ptr addrspace(1) %q) {
; CHECK-LABEL: @test2_diff_as(
; CHECK-NEXT:    [[I0:%.*]] = ptrtoint ptr [[P:%.*]] to i32
; CHECK-NEXT:    [[I1:%.*]] = ptrtoint ptr addrspace(1) [[Q:%.*]] to i32
; CHECK-NEXT:    [[R0:%.*]] = icmp sge i32 [[I0]], [[I1]]
; CHECK-NEXT:    ret i1 [[R0]]
;
  %i0 = ptrtoint ptr %p to i32
  %i1 = ptrtoint ptr addrspace(1) %q to i32
  %r0 = icmp sge i32 %i0, %i1
  ret i1 %r0
}

; These casts should not be folded away.

define i1 @test2_diff_as_global(ptr addrspace(1) %q) {
; CHECK-LABEL: @test2_diff_as_global(
; CHECK-NEXT:    [[I1:%.*]] = ptrtoint ptr addrspace(1) [[Q:%.*]] to i32
; CHECK-NEXT:    [[R0:%.*]] = icmp sge i32 [[I1]], ptrtoint (ptr @global to i32)
; CHECK-NEXT:    ret i1 [[R0]]
;
  %i0 = ptrtoint ptr @global to i32
  %i1 = ptrtoint ptr addrspace(1) %q to i32
  %r0 = icmp sge i32 %i1, %i0
  ret i1 %r0
}

; These casts should also be folded away.

define i1 @test3(ptr %a) {
; CHECK-LABEL: @test3(
; CHECK-NEXT:    [[R:%.*]] = icmp eq ptr [[A:%.*]], @global
; CHECK-NEXT:    ret i1 [[R]]
;
  %ta = ptrtoint ptr %a to i32
  %r = icmp eq i32 %ta, ptrtoint (ptr @global to i32)
  ret i1 %r
}

define i1 @test4(i32 %A) {
; CHECK-LABEL: @test4(
; CHECK-NEXT:    [[C:%.*]] = icmp eq i32 [[A:%.*]], 0
; CHECK-NEXT:    ret i1 [[C]]
;
  %B = inttoptr i32 %A to ptr
  %C = icmp eq ptr %B, null
  ret i1 %C
}

define i1 @test4_as2(i16 %A) {
; CHECK-LABEL: @test4_as2(
; CHECK-NEXT:    [[C:%.*]] = icmp eq i16 [[A:%.*]], 0
; CHECK-NEXT:    ret i1 [[C]]
;
  %B = inttoptr i16 %A to ptr addrspace(2)
  %C = icmp eq ptr addrspace(2) %B, null
  ret i1 %C
}


; Pulling the cast out of the load allows us to eliminate the load, and then
; the whole array.

  %op = type { float }
  %unop = type { i32 }
@Array = internal constant [1 x ptr] [ ptr @foo ]

declare ptr @foo(ptr %X)

define ptr @test5(ptr %O) {
; CHECK-LABEL: @test5(
; CHECK-NEXT:    [[T_2:%.*]] = call ptr @foo(ptr [[O:%.*]])
; CHECK-NEXT:    ret ptr [[T_2]]
;
  %t = load ptr, ptr @Array; <ptr> [#uses=1]
  %t.2 = call ptr %t( ptr %O )
  ret ptr %t.2
}



; InstCombine can not 'load (cast P)' -> cast (load P)' if the cast changes
; the address space.

define i8 @test6(ptr addrspace(1) %source) {
; CHECK-LABEL: @test6(
; CHECK-NEXT:  entry:
; CHECK-NEXT:    [[ARRAYIDX223:%.*]] = addrspacecast ptr addrspace(1) [[SOURCE:%.*]] to ptr
; CHECK-NEXT:    [[T4:%.*]] = load i8, ptr [[ARRAYIDX223]], align 1
; CHECK-NEXT:    ret i8 [[T4]]
;
entry:
  %arrayidx223 = addrspacecast ptr addrspace(1) %source to ptr
  %t4 = load i8, ptr %arrayidx223
  ret i8 %t4
}

define <2 x i32> @insertelt(<2 x i32> %x, ptr %p, i133 %index) {
; CHECK-LABEL: @insertelt(
; CHECK-NEXT:    [[TMP1:%.*]] = ptrtoint ptr [[P:%.*]] to i32
; CHECK-NEXT:    [[R:%.*]] = insertelement <2 x i32> [[X:%.*]], i32 [[TMP1]], i133 [[INDEX:%.*]]
; CHECK-NEXT:    ret <2 x i32> [[R]]
;
  %v = inttoptr <2 x i32> %x to <2 x ptr>
  %i = insertelement <2 x ptr> %v, ptr %p, i133 %index
  %r = ptrtoint <2 x ptr> %i to <2 x i32>
  ret <2 x i32> %r
}

define <2 x i32> @insertelt_intptr_trunc(<2 x i64> %x, ptr %p) {
; CHECK-LABEL: @insertelt_intptr_trunc(
; CHECK-NEXT:    [[TMP1:%.*]] = trunc <2 x i64> [[X:%.*]] to <2 x i32>
; CHECK-NEXT:    [[TMP2:%.*]] = ptrtoint ptr [[P:%.*]] to i32
; CHECK-NEXT:    [[R:%.*]] = insertelement <2 x i32> [[TMP1]], i32 [[TMP2]], i64 0
; CHECK-NEXT:    ret <2 x i32> [[R]]
;
  %v = inttoptr <2 x i64> %x to <2 x ptr>
  %i = insertelement <2 x ptr> %v, ptr %p, i32 0
  %r = ptrtoint <2 x ptr> %i to <2 x i32>
  ret <2 x i32> %r
}

define <2 x i32> @insertelt_intptr_zext(<2 x i8> %x, ptr %p) {
; CHECK-LABEL: @insertelt_intptr_zext(
; CHECK-NEXT:    [[TMP1:%.*]] = zext <2 x i8> [[X:%.*]] to <2 x i32>
; CHECK-NEXT:    [[TMP2:%.*]] = ptrtoint ptr [[P:%.*]] to i32
; CHECK-NEXT:    [[R:%.*]] = insertelement <2 x i32> [[TMP1]], i32 [[TMP2]], i64 1
; CHECK-NEXT:    ret <2 x i32> [[R]]
;
  %v = inttoptr <2 x i8> %x to <2 x ptr>
  %i = insertelement <2 x ptr> %v, ptr %p, i32 1
  %r = ptrtoint <2 x ptr> %i to <2 x i32>
  ret <2 x i32> %r
}

define <2 x i64> @insertelt_intptr_zext_zext(<2 x i8> %x, ptr %p) {
; CHECK-LABEL: @insertelt_intptr_zext_zext(
; CHECK-NEXT:    [[TMP1:%.*]] = zext <2 x i8> [[X:%.*]] to <2 x i32>
; CHECK-NEXT:    [[TMP2:%.*]] = ptrtoint ptr [[P:%.*]] to i32
; CHECK-NEXT:    [[TMP3:%.*]] = insertelement <2 x i32> [[TMP1]], i32 [[TMP2]], i64 0
; CHECK-NEXT:    [[R:%.*]] = zext <2 x i32> [[TMP3]] to <2 x i64>
; CHECK-NEXT:    ret <2 x i64> [[R]]
;
  %v = inttoptr <2 x i8> %x to <2 x ptr>
  %i = insertelement <2 x ptr> %v, ptr %p, i32 0
  %r = ptrtoint <2 x ptr> %i to <2 x i64>
  ret <2 x i64> %r
}

declare void @use(<2 x ptr>)

define <2 x i32> @insertelt_extra_use1(<2 x i32> %x, ptr %p) {
; CHECK-LABEL: @insertelt_extra_use1(
; CHECK-NEXT:    [[V:%.*]] = inttoptr <2 x i32> [[X:%.*]] to <2 x ptr>
; CHECK-NEXT:    call void @use(<2 x ptr> [[V]])
; CHECK-NEXT:    [[TMP1:%.*]] = ptrtoint ptr [[P:%.*]] to i32
; CHECK-NEXT:    [[R:%.*]] = insertelement <2 x i32> [[X]], i32 [[TMP1]], i64 0
; CHECK-NEXT:    ret <2 x i32> [[R]]
;
  %v = inttoptr <2 x i32> %x to <2 x ptr>
  call void @use(<2 x ptr> %v)
  %i = insertelement <2 x ptr> %v, ptr %p, i32 0
  %r = ptrtoint <2 x ptr> %i to <2 x i32>
  ret <2 x i32> %r
}

define <2 x i32> @insertelt_extra_use2(<2 x i32> %x, ptr %p) {
; CHECK-LABEL: @insertelt_extra_use2(
; CHECK-NEXT:    [[V:%.*]] = inttoptr <2 x i32> [[X:%.*]] to <2 x ptr>
; CHECK-NEXT:    [[I:%.*]] = insertelement <2 x ptr> [[V]], ptr [[P:%.*]], i64 0
; CHECK-NEXT:    call void @use(<2 x ptr> [[I]])
; CHECK-NEXT:    [[R:%.*]] = ptrtoint <2 x ptr> [[I]] to <2 x i32>
; CHECK-NEXT:    ret <2 x i32> [[R]]
;
  %v = inttoptr <2 x i32> %x to <2 x ptr>
  %i = insertelement <2 x ptr> %v, ptr %p, i32 0
  call void @use(<2 x ptr> %i)
  %r = ptrtoint <2 x ptr> %i to <2 x i32>
  ret <2 x i32> %r
}