#ifndef NEITHER_TRY_HPP
#define NEITHER_TRY_HPP

#include <functional>

#include "either.hpp"

namespace neither {

template <typename E, typename F, typename... X>
auto Try(F const &f, X &&... x)
    -> Either<E, decltype(f(std::forward<X>(x)...))> {
  try {
    return right(f(std::forward<X>(x)...));
  } catch (E const &e) {
    return left(e);
  }
}
}  // namespace neither

#endif
