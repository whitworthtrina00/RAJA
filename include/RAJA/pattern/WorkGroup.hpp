/*!
 ******************************************************************************
 *
 * \file
 *
 * \brief   Header file providing RAJA WorkPool and WorkGroup declarations.
 *
 ******************************************************************************
 */

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
// Copyright (c) 2016-20, Lawrence Livermore National Security, LLC
// and RAJA project contributors. See the RAJA/COPYRIGHT file for details.
//
// SPDX-License-Identifier: (BSD-3-Clause)
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//

#ifndef RAJA_PATTERN_WorkGroup_HPP
#define RAJA_PATTERN_WorkGroup_HPP

#include "RAJA/config.hpp"

#include "RAJA/pattern/WorkGroup/WorkStorage.hpp"
#include "RAJA/pattern/WorkGroup/WorkRunner.hpp"

namespace RAJA
{

/*!
 ******************************************************************************
 *
 * \brief  xargs alias.
 *
 * Usage example:
 *
 * \verbatim

   WorkPool<WorkGroup_policy, Index_type, xargs<int*, int>, Allocator> pool(allocator);

   pool.enqueue(..., [=] (Index_type i, int* xarg0, int xarg1) {
      xarg0[i] = xarg1;
   });

   WorkGroup<WorkGroup_policy, Index_type, xargs<int*, int>, Allocator> group = pool.instantiate();

   int* xarg0 = ...;
   int xarg1 = ...;
   WorkSite<WorkGroup_policy, Index_type, xargs<int*, int>, Allocator> site = group.run(xarg0, xarg1);

 * \endverbatim
 *
 ******************************************************************************
 */
template < typename ... Args >
using xargs = camp::list<Args...>;

//
// Forward declarations for WorkPool and WorkGroup templates.
// Actual classes appear in forall_*.hxx header files.
//

/*!
 ******************************************************************************
 *
 * \brief  WorkPool class template.
 *
 * Usage example:
 *
 * \verbatim

   WorkPool<WorkGroup_policy, Index_type, xargs<>, Allocator> pool(allocator);

   Real_ptr data = ...;

   pool.enqueue( ..., [=] (Index_type i) {
      data[i] = 1;
   });

   WorkGroup<WorkGroup_policy, Index_type, xargs<>, Allocator> group = pool.instantiate();

 * \endverbatim
 *
 ******************************************************************************
 */
template <typename WORKGROUP_POLICY_T,
          typename INDEX_T,
          typename EXTRA_ARGS_T,
          typename ALLOCATOR_T>
struct WorkPool;

/*!
 ******************************************************************************
 *
 * \brief  WorkGroup class template. Owns loops from an instantiated WorkPool.
 *
 * Usage example:
 *
 * \verbatim

   WorkGroup<WorkGroup_policy, Index_type, xargs<>, Allocator> group = pool.instantiate();

   WorkSite<WorkGroup_policy, Index_type, xargs<>, Allocator> site = group.run();

 * \endverbatim
 *
 ******************************************************************************
 */
template <typename WORKGROUP_POLICY_T,
          typename INDEX_T,
          typename EXTRA_ARGS_T,
          typename ALLOCATOR_T>
struct WorkGroup;

/*!
 ******************************************************************************
 *
 * \brief  WorkSite class template. Owns per run objects from a single run of a WorkGroup.
 *
 * Usage example:
 *
 * \verbatim

   WorkSite<WorkGroup_policy, Index_type, xargs<>, Allocator> site = group.run();

   site.synchronize();

 * \endverbatim
 *
 ******************************************************************************
 */
template <typename WORKGROUP_POLICY_T,
          typename INDEX_T,
          typename EXTRA_ARGS_T,
          typename ALLOCATOR_T>
struct WorkSite;


template <typename EXEC_POLICY_T,
          typename ORDER_POLICY_T,
          typename STORAGE_POLICY_T,
          typename INDEX_T,
          typename ... Args,
          typename ALLOCATOR_T>
struct WorkPool<WorkGroupPolicy<EXEC_POLICY_T,
                                ORDER_POLICY_T,
                                STORAGE_POLICY_T>,
                INDEX_T,
                xargs<Args...>,
                ALLOCATOR_T>
{
  using exec_policy = EXEC_POLICY_T;
  using order_policy = ORDER_POLICY_T;
  using storage_policy = STORAGE_POLICY_T;
  using policy = WorkGroupPolicy<exec_policy, order_policy, storage_policy>;
  using index_type = INDEX_T;
  using xarg_type = xargs<Args...>;
  using Allocator = ALLOCATOR_T;

  using workgroup_type = WorkGroup<policy, index_type, xarg_type, Allocator>;
  using worksite_type = WorkSite<policy, index_type, xarg_type, Allocator>;

  WorkPool(Allocator aloc)
    : m_storage(std::forward<Allocator>(aloc))
  { }

  WorkPool(WorkPool const&) = delete;
  WorkPool& operator=(WorkPool const&) = delete;

  WorkPool(WorkPool&&) = default;
  WorkPool& operator=(WorkPool&&) = default;

  void reserve(size_t num_loops, size_t storage_bytes)
  {
    m_storage.reserve(num_loops, storage_bytes);
  }

  template < typename segment_T, typename loop_T >
  inline void enqueue(segment_T&& seg, loop_T&& loop)
  {
    if (m_storage.begin() == m_storage.end()) {
      // perform auto-reserve on reuse
      reserve(m_max_num_loops, m_max_storage_bytes);
    }

    m_runner.enqueue(
        m_storage, std::forward<segment_T>(seg), std::forward<loop_T>(loop));
  }

  inline workgroup_type instantiate();

  ~WorkPool()
  {
    // storage is about to be destroyed
    // but it was never used so no synchronization necessary
  }

private:
  using workrunner_type = detail::WorkRunner<
      exec_policy, order_policy, Allocator, index_type, Args...>;
  using storage_type = detail::WorkStorage<
      storage_policy, Allocator, typename workrunner_type::vtable_type>;

  friend workgroup_type;
  friend worksite_type;

  storage_type m_storage;
  size_t m_max_num_loops = 0;
  size_t m_max_storage_bytes = 0;

  workrunner_type m_runner;
};

template <typename EXEC_POLICY_T,
          typename ORDER_POLICY_T,
          typename STORAGE_POLICY_T,
          typename INDEX_T,
          typename ... Args,
          typename ALLOCATOR_T>
struct WorkGroup<WorkGroupPolicy<EXEC_POLICY_T,
                                 ORDER_POLICY_T,
                                 STORAGE_POLICY_T>,
                 INDEX_T,
                 xargs<Args...>,
                 ALLOCATOR_T>
{
  using exec_policy = EXEC_POLICY_T;
  using order_policy = ORDER_POLICY_T;
  using storage_policy = STORAGE_POLICY_T;
  using policy = WorkGroupPolicy<exec_policy, order_policy, storage_policy>;
  using index_type = INDEX_T;
  using xarg_type = xargs<Args...>;
  using Allocator = ALLOCATOR_T;

  using workpool_type = WorkPool<policy, index_type, xarg_type, Allocator>;
  using worksite_type = WorkSite<policy, index_type, xarg_type, Allocator>;

  WorkGroup(WorkGroup const&) = delete;
  WorkGroup& operator=(WorkGroup const&) = delete;

  WorkGroup(WorkGroup&&) = default;
  WorkGroup& operator=(WorkGroup&&) = default;

  inline worksite_type run(Args...);

  ~WorkGroup()
  {
    // storage is about to be destroyed
    // TODO: synchronize
  }

private:
  using storage_type = typename workpool_type::storage_type;
  using workrunner_type = typename workpool_type::workrunner_type;

  friend workpool_type;
  friend worksite_type;

  storage_type m_storage;
  workrunner_type m_runner;

  WorkGroup(storage_type&& storage, workrunner_type&& runner)
    : m_storage(std::move(storage))
    , m_runner(std::move(runner))
  { }
};

template <typename EXEC_POLICY_T,
          typename ORDER_POLICY_T,
          typename STORAGE_POLICY_T,
          typename INDEX_T,
          typename ... Args,
          typename ALLOCATOR_T>
struct WorkSite<WorkGroupPolicy<EXEC_POLICY_T,
                                ORDER_POLICY_T,
                                STORAGE_POLICY_T>,
                INDEX_T,
                xargs<Args...>,
                ALLOCATOR_T>
{
  using exec_policy = EXEC_POLICY_T;
  using order_policy = ORDER_POLICY_T;
  using storage_policy = STORAGE_POLICY_T;
  using policy = WorkGroupPolicy<exec_policy, order_policy, storage_policy>;
  using index_type = INDEX_T;
  using xarg_type = xargs<Args...>;
  using Allocator = ALLOCATOR_T;

  using workpool_type = WorkPool<policy, index_type, xarg_type, Allocator>;
  using workgroup_type = WorkGroup<policy, index_type, xarg_type, Allocator>;

  WorkSite(WorkSite const&) = delete;
  WorkSite& operator=(WorkSite const&) = delete;

  WorkSite(WorkSite&&) = default;
  WorkSite& operator=(WorkSite&&) = default;

  ~WorkSite()
  {
    // resources is about to be released
    // TODO: synchronize
  }

private:
  using workrunner_type = typename workgroup_type::workrunner_type;
  using per_run_storage = typename workrunner_type::per_run_storage;

  friend workpool_type;
  friend workgroup_type;

  per_run_storage m_run_storage;

  WorkSite(per_run_storage&& run_storage)
    : m_run_storage(std::move(run_storage))
  { }
};


template <typename EXEC_POLICY_T,
          typename ORDER_POLICY_T,
          typename STORAGE_POLICY_T,
          typename INDEX_T,
          typename ... Args,
          typename ALLOCATOR_T>
inline
typename WorkPool<
    WorkGroupPolicy<EXEC_POLICY_T, ORDER_POLICY_T, STORAGE_POLICY_T>,
    INDEX_T,
    xargs<Args...>,
    ALLOCATOR_T>::workgroup_type
WorkPool<
    WorkGroupPolicy<EXEC_POLICY_T, ORDER_POLICY_T, STORAGE_POLICY_T>,
    INDEX_T,
    xargs<Args...>,
    ALLOCATOR_T>::instantiate()
{
  // update max sizes to auto-reserve on reuse
  m_max_num_loops = std::max(m_storage.size(), m_max_num_loops);
  m_max_storage_bytes = std::max(m_storage.storage_size(), m_max_storage_bytes);

  // move storage into workgroup
  return workgroup_type{std::move(m_storage), std::move(m_runner)};
}

template <typename EXEC_POLICY_T,
          typename ORDER_POLICY_T,
          typename STORAGE_POLICY_T,
          typename INDEX_T,
          typename ... Args,
          typename ALLOCATOR_T>
inline
typename WorkGroup<
    WorkGroupPolicy<EXEC_POLICY_T, ORDER_POLICY_T, STORAGE_POLICY_T>,
    INDEX_T,
    xargs<Args...>,
    ALLOCATOR_T>::worksite_type
WorkGroup<
    WorkGroupPolicy<EXEC_POLICY_T, ORDER_POLICY_T, STORAGE_POLICY_T>,
    INDEX_T,
    xargs<Args...>,
    ALLOCATOR_T>::run(Args... args)
{
  // move any per run storage into worksite
  return worksite_type{m_runner.run(m_storage, std::forward<Args>(args)...)};
}

}  // namespace RAJA

#endif  // closing endif for header file include guard
