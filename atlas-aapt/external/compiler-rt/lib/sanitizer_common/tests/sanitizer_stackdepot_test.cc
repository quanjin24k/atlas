//===-- sanitizer_stackdepot_test.cc --------------------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file is a part of ThreadSanitizer/AddressSanitizer runtime.
//
//===----------------------------------------------------------------------===//
#include "sanitizer_common/sanitizer_stackdepot.h"
#include "sanitizer_common/sanitizer_internal_defs.h"
#include "sanitizer_common/sanitizer_libc.h"
#include "gtest/gtest.h"

namespace __sanitizer {

TEST(SanitizerCommon, StackDepotBasic) {
  uptr array[] = {1, 2, 3, 4, 5};
  StackTrace s1(array, ARRAY_SIZE(array));
  u32 i1 = StackDepotPut(s1);
  StackTrace stack = StackDepotGet(i1);
  EXPECT_NE(stack.trace, (uptr*)0);
  EXPECT_EQ(ARRAY_SIZE(array), stack.size);
  EXPECT_EQ(0, internal_memcmp(stack.trace, array, sizeof(array)));
}

TEST(SanitizerCommon, StackDepotAbsent) {
  StackTrace stack = StackDepotGet((1 << 30) - 1);
  EXPECT_EQ((uptr*)0, stack.trace);
}

TEST(SanitizerCommon, StackDepotEmptyStack) {
  u32 i1 = StackDepotPut(StackTrace());
  StackTrace stack = StackDepotGet(i1);
  EXPECT_EQ((uptr*)0, stack.trace);
}

TEST(SanitizerCommon, StackDepotZeroId) {
  StackTrace stack = StackDepotGet(0);
  EXPECT_EQ((uptr*)0, stack.trace);
}

TEST(SanitizerCommon, StackDepotSame) {
  uptr array[] = {1, 2, 3, 4, 6};
  StackTrace s1(array, ARRAY_SIZE(array));
  u32 i1 = StackDepotPut(s1);
  u32 i2 = StackDepotPut(s1);
  EXPECT_EQ(i1, i2);
  StackTrace stack = StackDepotGet(i1);
  EXPECT_NE(stack.trace, (uptr*)0);
  EXPECT_EQ(ARRAY_SIZE(array), stack.size);
  EXPECT_EQ(0, internal_memcmp(stack.trace, array, sizeof(array)));
}

TEST(SanitizerCommon, StackDepotSeveral) {
  uptr array1[] = {1, 2, 3, 4, 7};
  StackTrace s1(array1, ARRAY_SIZE(array1));
  u32 i1 = StackDepotPut(s1);
  uptr array2[] = {1, 2, 3, 4, 8, 9};
  StackTrace s2(array2, ARRAY_SIZE(array2));
  u32 i2 = StackDepotPut(s2);
  EXPECT_NE(i1, i2);
}

TEST(SanitizerCommon, StackDepotReverseMap) {
  uptr array1[] = {1, 2, 3, 4, 5};
  uptr array2[] = {7, 1, 3, 0};
  uptr array3[] = {10, 2, 5, 3};
  uptr array4[] = {1, 3, 2, 5};
  u32 ids[4] = {0};
  StackTrace s1(array1, ARRAY_SIZE(array1));
  StackTrace s2(array2, ARRAY_SIZE(array2));
  StackTrace s3(array3, ARRAY_SIZE(array3));
  StackTrace s4(array4, ARRAY_SIZE(array4));
  ids[0] = StackDepotPut(s1);
  ids[1] = StackDepotPut(s2);
  ids[2] = StackDepotPut(s3);
  ids[3] = StackDepotPut(s4);

  StackDepotReverseMap map;

  for (uptr i = 0; i < 4; i++) {
    StackTrace stack = StackDepotGet(ids[i]);
    StackTrace from_map = map.Get(ids[i]);
    EXPECT_EQ(stack.size, from_map.size);
    EXPECT_EQ(stack.trace, from_map.trace);
  }
}

}  // namespace __sanitizer
