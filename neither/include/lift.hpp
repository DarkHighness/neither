#include "either.hpp"

namespace neither {

template <typename L, typename R>
constexpr bool isPresent(Either<L, R> const &e) {
  return e;
}

template <typename T>
constexpr bool isPresent(Maybe<T> const &m) {
  return m;
}

template <typename T>
constexpr bool isPresent(T) {
  return true;
}

template <typename L, typename R>
constexpr R unpack(Either<L, R> const &e) {
  return e.rightValue;
}

template <typename T>
constexpr T unpack(Maybe<T> const &m) {
  return m.value;
}

template <typename T>
constexpr T unpack(T const &x) {
  return x;
}

constexpr auto all(bool x = true, bool y = true) { return x && y; }

template <typename X, typename... Xs>
auto all(X x, Xs... xs) {
  return all(x, all(xs...));
}

constexpr auto any(bool x = true, bool y = true) { return x || y; }

template <typename X, typename... Xs>
auto any(X x, Xs... xs) {
  return any(x, any(xs...));
}

template <typename F>
auto lift(F const &f) {
  return [f](auto... x) -> decltype(maybe(f(unpack(x)...))) {
    if (all(isPresent(x)...)) return f(unpack(x)...);

    return maybe();
  };
}

}  // namespace neither
