//
// Created by Twiliness on 2019/3/12.
//

#ifndef TWILINESS_WRITER_H
#define TWILINESS_WRITER_H

#include <memory>

#include "traits.hpp"

namespace neither {

template <typename T>
T mappend(T a, T b);

template <typename T>
std::string mappend(std::string a, std::string b) {
  return a + b;
}

template <typename A, typename W>
struct Writer {
  A a;
  W w;

  constexpr Writer(A const &value, W const &writer) : a{value}, w{writer} {}

  constexpr Writer(A &&value, W &&writer)
      : a{std::move(value)}, w{std::move(writer)} {}

  constexpr Writer(A &&value, W const &writer)
      : a{std::move(value)}, w{writer} {}

  constexpr Writer(A const &value, W &&writer)
      : a{value}, w{std::move(writer)} {}

  constexpr Writer(Writer<W, A> const &o)
      : a{new (&a) A(o.a)}, w{new (&w) W(o.w)} {}

  constexpr Writer(Writer<W, A> &&o) : a{std::move(o.a)}, w{std::move(o.w)} {}

  template <typename F>
  constexpr auto map(F const &f)
      -> Writer<decltype(ensurePair(f(isCopyable(a))).first), W> {
    using ResultType = decltype(f(isCopyable(a)).first);
    auto r = f(a);
    return Writer<ResultType, W>(r.first, mappend<W>(w, r.second));
  }

  template <typename F>
  auto map(
      F const &f) && -> Writer<decltype(ensurePair(f(std::move(a))).first), W> {
    using ResultType = decltype(ensurePair(f(std::move(a))).first);
    auto r = f(std::move(a));
    return Writer<ResultType, W>(r.first, mappend<W>(w, r.second));
  }

  template <typename F>
  constexpr auto flatMap(F const &f) const & -> decltype(ensureWriter(f(a))) {
    return f(a);
  }

  template <typename F>
  constexpr auto flatMap(F const &f) && -> decltype(
      ensureWriter(f(std::move(a)))) {
    return f(a);
  }
};

template <typename A, typename W>
auto runWriter(A a, W w) -> Writer<A, W> {
  return {a, w};
}
}  // namespace neither

#endif  // TWILINESS_WRITER_H
