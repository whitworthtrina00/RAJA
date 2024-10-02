#ifndef NEW_REDUCE_SEQ_REDUCE_HPP
#define NEW_REDUCE_SEQ_REDUCE_HPP

#include "RAJA/pattern/params/reducer.hpp"

namespace RAJA {
namespace expt {
namespace detail {

  // Init
  template<typename EXEC_POL, typename OP, typename T, typename VType>
  camp::concepts::enable_if< std::is_same< EXEC_POL, RAJA::seq_exec> >
  init(Reducer<OP, T, VType>& red) {
    red.valop_m.val = OP::identity();
  }

  // Combine
  template<typename EXEC_POL, typename OP, typename T, typename VType>
  camp::concepts::enable_if< std::is_same< EXEC_POL, RAJA::seq_exec> >
  combine(Reducer<OP, T, VType>& out, const Reducer<OP, T, VType>& in) {
    out.valop_m.val = OP{}(out.valop_m.val, in.valop_m.val);
  }

  // Resolve
  template<typename EXEC_POL, typename OP, typename T, typename VType>
  camp::concepts::enable_if< std::is_same< EXEC_POL, RAJA::seq_exec> >
  resolve(Reducer<OP, T, VType>& red) {
    red.setTarget(OP{}(*red.target, red.valop_m.val));
  }

} //  namespace detail
} //  namespace expt
} //  namespace RAJA

#endif //  NEW_REDUCE_SEQ_REDUCE_HPP
