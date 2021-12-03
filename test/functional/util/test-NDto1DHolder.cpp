//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
// Copyright (c) 2016-21, Lawrence Livermore National Security, LLC
// and RAJA project contributors. See the RAJA/LICENSE file for details.
//
// SPDX-License-Identifier: (BSD-3-Clause)
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//

///
/// Source file containing unit tests for NDto1DHolder class.
///

#include "RAJA_test-base.hpp"

#include "RAJA/util/Span.hpp"
#include "RAJA/util/NDto1DHolder.hpp"

#include "camp/resource.hpp"

#include <numeric>
#include <vector>

template < typename IndexType, typename Segment0 >
void test_NDto1DHolder_1D(Segment0 const& seg0)
{
  using std::begin; using std::end; using std::distance;
  auto seg0_begin = begin(seg0);

  size_t counter0 = 0;
  auto holder = RAJA::make_NDto1DHolder([&](IndexType i0) {
    ASSERT_EQ(seg0_begin[counter0], i0);
    counter0 += 1;
  }, seg0);

  ASSERT_EQ(holder.size(), seg0.size());

  auto range = holder.getRange();

  ASSERT_EQ(distance(begin(range), end(range)), seg0.size());

  auto range_end = end(range);
  for (auto idx = begin(range); idx != range_end; ++idx) {
    holder(*idx);
  }
}

template < typename IndexType, typename SegIndexType >
void test_types_NDto1DHolder_1D(SegIndexType ibegin0, SegIndexType iend0)
{
  RAJA::TypedRangeSegment<SegIndexType> rseg0(ibegin0, iend0);
  test_NDto1DHolder_1D<IndexType>(rseg0);

  camp::resources::Resource res{camp::resources::Host()};
  std::vector<SegIndexType> indices0(iend0-ibegin0);
  std::iota(indices0.begin(), indices0.end(), ibegin0);
  RAJA::TypedListSegment<SegIndexType> lseg0(indices0, res);
  test_NDto1DHolder_1D<IndexType>(lseg0);
}

TEST(NDto1DHolder, test1D)
{
  test_types_NDto1DHolder_1D<int, int>(0, 0);

  test_types_NDto1DHolder_1D<int, int>(0, 15);
  test_types_NDto1DHolder_1D<long, long>(-8, 16);
}


template < typename IndexType, typename Segment0, typename Segment1 >
void test_NDto1DHolder_2D(Segment0 const& seg0, Segment1 const& seg1)
{
  using std::begin; using std::end; using std::distance;
  auto seg0_begin = begin(seg0);
  auto seg1_begin = begin(seg1);
  auto seg1_len = seg1.size();

  size_t counter0 = 0;
  size_t counter1 = 0;
  auto holder = RAJA::make_NDto1DHolder([&](IndexType i0, IndexType i1) {
    ASSERT_EQ(seg0_begin[counter0], i0);
    ASSERT_EQ(seg1_begin[counter1], i1);
    counter1 += 1;
    if (counter1 == seg1_len) {
      counter1 = 0;
      counter0 += 1;
    }
  }, seg0, seg1);

  ASSERT_EQ(holder.size(), seg0.size()*seg1.size());

  auto range = holder.getRange();

  ASSERT_EQ(distance(begin(range), end(range)), seg0.size()*seg1.size());

  auto range_end = end(range);
  for (auto idx = begin(range); idx != range_end; ++idx) {
    holder(*idx);
  }
}

template < typename IndexType, typename SegIndexType0, typename SegIndexType1 >
void test_types_NDto1DHolder_2D(SegIndexType0 ibegin0, SegIndexType0 iend0,
                                SegIndexType1 ibegin1, SegIndexType1 iend1)
{
  RAJA::TypedRangeSegment<SegIndexType0> rseg0(ibegin0, iend0);
  RAJA::TypedRangeSegment<SegIndexType1> rseg1(ibegin1, iend1);
  test_NDto1DHolder_2D<IndexType>(rseg0, rseg1);

  camp::resources::Resource res{camp::resources::Host()};
  std::vector<SegIndexType0> indices0(iend0-ibegin0);
  std::iota(indices0.begin(), indices0.end(), ibegin0);
  RAJA::TypedListSegment<SegIndexType0> lseg0(indices0, res);
  std::vector<SegIndexType1> indices1(iend1-ibegin1);
  std::iota(indices1.begin(), indices1.end(), ibegin1);
  RAJA::TypedListSegment<SegIndexType1> lseg1(indices1, res);
  test_NDto1DHolder_2D<IndexType>(lseg0, lseg1);
}

TEST(NDto1DHolder, test2D)
{
  test_types_NDto1DHolder_2D<int, int, int>(0, 0, 0, 0);
  test_types_NDto1DHolder_2D<int, int, int>(0, 5, 0, 0);
  test_types_NDto1DHolder_2D<int, int, int>(0, 0, 0, 5);

  test_types_NDto1DHolder_2D<int, int, int>(0, 3, 0, 4);
  test_types_NDto1DHolder_2D<long, int, long>(-3, 5, 0, 6);
  test_types_NDto1DHolder_2D<long, long, int>(4, 13, -2, 7);
  test_types_NDto1DHolder_2D<long, long, long>(-8, -2, -5, 3);
}

template < typename IndexType, typename Segment0, typename Segment1, typename Segment2 >
void test_NDto1DHolder_3D(Segment0 const& seg0, Segment1 const& seg1, Segment2 const& seg2)
{
  using std::begin; using std::end; using std::distance;
  auto seg0_begin = begin(seg0);
  auto seg1_begin = begin(seg1);
  auto seg1_len = seg1.size();
  auto seg2_begin = begin(seg2);
  auto seg2_len = seg2.size();

  size_t counter0 = 0;
  size_t counter1 = 0;
  size_t counter2 = 0;
  auto holder = RAJA::make_NDto1DHolder([&](IndexType i0, IndexType i1, IndexType i2) {
    ASSERT_EQ(seg0_begin[counter0], i0);
    ASSERT_EQ(seg1_begin[counter1], i1);
    ASSERT_EQ(seg2_begin[counter2], i2);
    counter2 += 1;
    if (counter2 == seg2_len) {
      counter2 = 0;
      counter1 += 1;
      if (counter1 == seg1_len) {
        counter1 = 0;
        counter0 += 1;
      }
    }
  }, seg0, seg1, seg2);

  ASSERT_EQ(holder.size(), seg0.size()*seg1.size()*seg2.size());

  auto range = holder.getRange();

  ASSERT_EQ(distance(begin(range), end(range)), seg0.size()*seg1.size()*seg2.size());

  auto range_end = end(range);
  for (auto idx = begin(range); idx != range_end; ++idx) {
    holder(*idx);
  }
}

template < typename IndexType, typename SegIndexType0, typename SegIndexType1, typename SegIndexType2 >
void test_types_NDto1DHolder_3D(SegIndexType0 ibegin0, SegIndexType0 iend0,
                                SegIndexType1 ibegin1, SegIndexType1 iend1,
                                SegIndexType2 ibegin2, SegIndexType2 iend2)
{
  RAJA::TypedRangeSegment<SegIndexType0> rseg0(ibegin0, iend0);
  RAJA::TypedRangeSegment<SegIndexType1> rseg1(ibegin1, iend1);
  RAJA::TypedRangeSegment<SegIndexType2> rseg2(ibegin2, iend2);
  test_NDto1DHolder_3D<IndexType>(rseg0, rseg1, rseg2);

  camp::resources::Resource res{camp::resources::Host()};
  std::vector<SegIndexType0> indices0(iend0-ibegin0);
  std::iota(indices0.begin(), indices0.end(), ibegin0);
  RAJA::TypedListSegment<SegIndexType0> lseg0(indices0, res);
  std::vector<SegIndexType1> indices1(iend1-ibegin1);
  std::iota(indices1.begin(), indices1.end(), ibegin1);
  RAJA::TypedListSegment<SegIndexType1> lseg1(indices1, res);
  std::vector<SegIndexType2> indices2(iend2-ibegin2);
  std::iota(indices2.begin(), indices2.end(), ibegin2);
  RAJA::TypedListSegment<SegIndexType2> lseg2(indices2, res);
  test_NDto1DHolder_3D<IndexType>(lseg0, lseg1, lseg2);
}

TEST(NDto1DHolder, test3D)
{
  test_types_NDto1DHolder_3D<int, int, int, int>(0, 0, 0, 0, 0, 0);
  test_types_NDto1DHolder_3D<int, int, int, int>(0, 5, 0, 0, 0, 0);
  test_types_NDto1DHolder_3D<int, int, int, int>(0, 0, 0, 5, 0, 0);
  test_types_NDto1DHolder_3D<int, int, int, int>(0, 0, 0, 0, 0, 5);

  test_types_NDto1DHolder_3D<int, int, int, int>(0, 3, 0, 4, 0, 5);
  test_types_NDto1DHolder_3D<long, int, long, int>(-3, 5, 0, 6, 2, 5);
  test_types_NDto1DHolder_3D<long, long, int, int>(4, 13, -2, 7, -3, 0);
  test_types_NDto1DHolder_3D<long, long, long, long>(-8, -2, -5, 3, 1, 4);
}










template < typename IndexType >
void test_CombiningAdapter_1D(IndexType N0)
{
  using std::begin; using std::end; using std::distance;

  RAJA::Layout<1, IndexType, 0> layout(N0);

  IndexType counter0 = 0;

  auto holder = RAJA::make_CombiningAdapter([&](IndexType i0) {
    ASSERT_EQ(counter0, i0);
    counter0++;
  }, layout);

  ASSERT_EQ(holder.size(), N0);
  ASSERT_EQ(holder.size(), layout.size());

  auto range = holder.getRange();

  ASSERT_EQ(distance(begin(range), end(range)), N0);

  auto range_end = end(range);
  for (auto idx = begin(range); idx != range_end; ++idx) {
    holder(*idx);
  }

  ASSERT_EQ(counter0, N0);
}

TEST(CombiningAdapter, test1D)
{
  test_CombiningAdapter_1D<int>(0);

  test_CombiningAdapter_1D<int>(15);
  test_CombiningAdapter_1D<long>(16);
}
