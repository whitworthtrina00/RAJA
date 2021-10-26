#ifndef NEW_REDUCE_FORALL_OMP_TARGET_HPP
#define NEW_REDUCE_FORALL_OMP_TARGET_HPP

#if defined(RAJA_ENABLE_TARGET_OPENMP)
namespace detail {

  template <typename EXEC_POL, typename B, typename... Params>
  camp::concepts::enable_if< std::is_same< EXEC_POL, RAJA::omp_target_parallel_for_exec_nt> >
  forall_param(EXEC_POL&&, int N, B const &body, Params... params)
  {
    ForallParamPack<Params...> f_params(params...);

    //init<EXEC_POL>(f_params);

    #pragma omp declare reduction(                                              \
      combine                                                                   \
      : decltype(f_params)                                                      \
      : combine<EXEC_POL>(omp_out, omp_in) )\
      initializer(init<EXEC_POL>(omp_priv))

    #pragma omp target data if (use_dev) map(tofrom : f_params) map(to : body)
    {
      #pragma omp target if (use_dev) teams distribute parallel for reduction(combine : f_params)
      for (int i = 0; i < N; ++i) {
        invoke(f_params, body, i);
      }
    }
    resolve<EXEC_POL>(f_params);
  }

} //  namespace detail
#endif

#endif //  NEW_REDUCE_FORALL_OMP_HPP
