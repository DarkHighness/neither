#ifndef NEITHER_MAYBE_HPP
#define NEITHER_MAYBE_HPP

#include <cassert>
#include <cstddef>
#include <memory>
#include <type_traits>

#include "traits.hpp"

namespace neither {
template <typename T>
struct Maybe;

template <>
struct Maybe<void> {};

template <typename T>
struct Maybe {
  using size_type = std::size_t;

  union {
    T value;
  };

  bool const isPresent;

  constexpr Maybe() : isPresent{false} {}

  constexpr Maybe(T const &value) : value{value}, isPresent{true} {}

  constexpr Maybe(T &&value) : value{std::move(value)}, isPresent{true} {}

  constexpr Maybe(Maybe<void>) : isPresent{false} {}

  constexpr Maybe(Maybe<T> const &o) : isPresent{o.isPresent} {
    if (o.isPresent) new (&value) T(o.value);
  }

  ~Maybe() {
    if (isPresent) value.~T();
  }

  constexpr T get(T other) { return isPresent ? value : other; }

  constexpr T unsafeGet() {
    assert(isPresent && "unsafeGet must not be called on an empty Maybe");
    return value;
  }

  constexpr size_type size() const noexcept { return isPresent ? 1 : 0; }

  constexpr bool empty() const noexcept { return !isPresent; }

  template <typename F>
  constexpr auto map(
      F const &f) const & -> Maybe<decltype(f(isCopyable(value)))> {
    using ResultType = decltype(f(value));

    return isPresent ? Maybe<ResultType>(f(value)) : Maybe<ResultType>();
  }

  template <typename F>
  auto map(F const &f) && -> Maybe<decltype(f(std::move(value)))> {
    using ResultType = decltype(f(std::move(value)));

    return isPresent ? Maybe<ResultType>(f(std::move(value)))
                     : Maybe<ResultType>();
  }

  template <typename F>
  constexpr auto flatMap(F const &f) const & -> decltype(
      ensureMaybe(f(value))) {
    using ResultType = decltype(f(value));

    return isPresent ? f(value) : ResultType();
  }

  template <typename F>
  constexpr auto flatMap(F const &f) && -> decltype(
      ensureMaybe(f(std::move(value)))) {
    using ResultType = decltype(f(std::move(value)));

    return isPresent ? f(std::move(value)) : ResultType();
  }

  constexpr operator bool() const { return isPresent; }
};

namespace {

inline bool equal(Maybe<void> const &, Maybe<void> const &) { return true; }

template <typename T>
bool equal(Maybe<T> const &a, Maybe<T> const &b) {
  if (a.isPresent) return b.isPresent && a.value == b.value;
  return !b.isPresent;
}
}  // namespace

template <typename T>
bool operator==(Maybe<T> const &a, Maybe<T> const &b) {
  return equal(a, b);
}

template <typename T>
bool operator!=(Maybe<T> const &a, Maybe<T> const &b) {
  return !(a == b);
}

template <typename T>
auto maybe(T value) -> Maybe<T> {
  return {value};
}

template <typename T = void>
auto maybe() -> Maybe<T> {
  return {};
}

static const auto nothing = maybe();

}  // namespace neither

#endif
