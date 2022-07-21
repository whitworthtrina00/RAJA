//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
// Copyright (c) 2016-22, Lawrence Livermore National Security, LLC
// and RAJA project contributors. See the RAJA/LICENSE file for details.
//
// SPDX-License-Identifier: (BSD-3-Clause)
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//

#include <cstdlib>
#include <cstring>
#include <iostream>

#include "memoryManager.hpp"

#include "RAJA/RAJA.hpp"

/*
 *  Vector Dot Product Exercise
 *
 *  Computes dot = (a,b), where a, b are vectors of 
 *  doubles and dot is a scalar double. It illustrates how RAJA
 *  supports a portable parallel reduction opertion in a way that 
 *  the code looks like it does in a sequential implementation.
 *
 *  RAJA features shown:
 *    - `forall` loop iteration template method
 *    -  Index range segment
 *    -  Execution policies
 *    -  Reduction types
 *
 * If CUDA is enabled, CUDA unified memory is used.
 */

/*
  CUDA_BLOCK_SIZE - specifies the number of threads in a CUDA thread block
*/
#if defined(RAJA_ENABLE_CUDA)
const int CUDA_BLOCK_SIZE = 256;
#endif

#if defined(RAJA_ENABLE_HIP)
const int HIP_BLOCK_SIZE = 256;
#endif

#if defined(RAJA_ENABLE_SYCL)
const int SYCL_BLOCK_SIZE = 256;
#endif

//
//  Function to check dot product result.
//
void checkResult(double compdot, double refdot);

int main(int RAJA_UNUSED_ARG(argc), char **RAJA_UNUSED_ARG(argv[]))
{

  std::cout << "\n\nRAJA vector dot product example...\n";

#if defined(RAJA_ENABLE_SYCL)
  memoryManager::sycl_res = new camp::resources::Resource{camp::resources::Sycl()};
  ::RAJA::sycl::detail::setQueue(memoryManager::sycl_res);
#endif

//
// Define vector length
//
  const int N = 1000000;

//
// Allocate and initialize vector data
//
  double *a = memoryManager::allocate<double>(N);
  double *b = memoryManager::allocate<double>(N);

  for (int i = 0; i < N; ++i) {
    a[i] = 1.0;
    b[i] = 1.0;
  }

//----------------------------------------------------------------------------//

//
// C-style dot product operation.
//
  std::cout << "\n Running C-version of dot product...\n";

  // _csytle_dotprod_start
  double dot = 0.0;

  for (int i = 0; i < N; ++i) {
    dot += a[i] * b[i];
  }

  std::cout << "\t (a, b) = " << dot << std::endl;
  // _csytle_dotprod_end

  double dot_ref = dot;

//----------------------------------------------------------------------------//

  std::cout << "\n Running RAJA sequential dot product...\n";

  ///
  /// TODO...
  ///
  /// EXERCISE: Implement the dot product kernel using a RAJA::seq_exec
  ///           execution policy type and RAJA::seq_reduce. 
  ///
  /// NOTE: We've done this one for you to help you get started...
  ///

  RAJA::ReduceSum<RAJA::seq_reduce, double> seqdot(0.0);

  RAJA::forall<RAJA::seq_exec>(RAJA::RangeSegment(0, N), [=] (int i) { 
    seqdot += a[i] * b[i]; 
  });

  dot = seqdot.get();

  std::cout << "\t (a, b) = " << dot << std::endl;

  checkResult(dot, dot_ref);


//----------------------------------------------------------------------------//

#if defined(RAJA_ENABLE_OPENMP)
  std::cout << "\n Running RAJA OpenMP dot product...\n";

  ///
  /// TODO...
  ///
  /// EXERCISE: Implement the dot product kernel using a RAJA::omp_parallel_for_exec
  ///           execution policy type and RAJA::omp_reduce. 
  ///

  std::cout << "\t (a, b) = " << dot << std::endl;

  checkResult(dot, dot_ref);
#endif


//----------------------------------------------------------------------------//

#if defined(RAJA_ENABLE_CUDA)
  std::cout << "\n Running RAJA CUDA dot product...\n";

  ///
  /// TODO...
  ///
  /// EXERCISE: Implement the dot product kernel using a RAJA::cuda_exec
  ///           execution policy type and RAJA::cuda_reduce. 
  ///

  std::cout << "\t (a, b) = " << dot << std::endl;

  checkResult(dot, dot_ref);
#endif

//----------------------------------------------------------------------------//

#if defined(RAJA_ENABLE_HIP)
  std::cout << "\n Running RAJA HIP dot product...\n";

  int *d_a = memoryManager::allocate_gpu<int>(N);
  int *d_b = memoryManager::allocate_gpu<int>(N);

  hipErrchk(hipMemcpy( d_a, a, N * sizeof(int), hipMemcpyHostToDevice ));
  hipErrchk(hipMemcpy( d_b, b, N * sizeof(int), hipMemcpyHostToDevice ));

  ///
  /// TODO...
  ///
  /// EXERCISE: Implement the dot product kernel using a RAJA::hip_exec
  ///           execution policy type and RAJA::hip_reduce. 
  ///

  std::cout << "\t (a, b) = " << dot << std::endl;

  checkResult(dot, dot_ref);

  memoryManager::deallocate_gpu(d_a);
  memoryManager::deallocate_gpu(d_b);
#endif

//----------------------------------------------------------------------------//

#if defined(RAJA_ENABLE_SYCL)
  std::cout << "\n Running RAJA SYCL dot product...\n";

  ///
  /// TODO...
  ///
  /// EXERCISE: Implement the dot product kernel using a RAJA::sycl_exec
  ///           execution policy type and RAJA::sycl_reduce. 
  ///

  std::cout << "\t (a, b) = " << dot << std::endl;

  checkResult(dot, dot_ref);

#endif

//----------------------------------------------------------------------------//


  memoryManager::deallocate(a);
  memoryManager::deallocate(b);

  std::cout << "\n DONE!...\n";

  return 0;
}

//
//  Function to check computed dot product and report P/F.
//
void checkResult(double compdot, double refdot)
{
  if ( compdot == refdot ) {
    std::cout << "\n\t result -- PASS\n";
  } else {
    std::cout << "\n\t result -- FAIL\n";
  }
}

