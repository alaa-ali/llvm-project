//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

// UNSUPPORTED: c++03, c++11, c++14
// TODO: Change to XFAIL once https://github.com/llvm/llvm-project/issues/40340 is fixed
// UNSUPPORTED: availability-pmr-missing

// <memory_resource>

// class synchronized_pool_resource

#include <cassert>
#include <cstddef>
#include <memory> // std::align
#include <memory_resource>

#include "count_new.h"
#include "test_macros.h"

bool is_aligned_to(void* p, std::size_t alignment) {
  void* p2     = p;
  std::size_t space = 1;
  void* result = std::align(alignment, 1, p2, space);
  return (result == p);
}

int main(int, char**) {
  globalMemCounter.reset();
  std::pmr::pool_options opts{1, 1024};
  std::pmr::synchronized_pool_resource sync1(opts, std::pmr::new_delete_resource());
  std::pmr::memory_resource& r1 = sync1;

  constexpr std::size_t big_alignment = 8 * alignof(std::max_align_t);
  static_assert(big_alignment > 4);

  assert(globalMemCounter.checkNewCalledEq(0));

  void* ret = r1.allocate(2048, big_alignment);
  assert(ret != nullptr);
  assert(is_aligned_to(ret, big_alignment));
  ASSERT_WITH_LIBRARY_INTERNAL_ALLOCATIONS(globalMemCounter.checkNewCalledGreaterThan(0));

  ret = r1.allocate(16, 4);
  assert(ret != nullptr);
  assert(is_aligned_to(ret, 4));
  ASSERT_WITH_LIBRARY_INTERNAL_ALLOCATIONS(globalMemCounter.checkNewCalledGreaterThan(1));

  return 0;
}
