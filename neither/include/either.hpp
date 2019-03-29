#ifndef NEITHER_EITHER_HPP
#define NEITHER_EITHER_HPP

#include <memory>
#include <type_traits>

#include "maybe.hpp"
#include "traits.hpp"

namespace neither {
template <typename T>
constexpr T max(T x, T y) {
  return x > y ? x : y;
}

template <typename T>
struct Left {
  T value;
};

template <typename T>
constexpr Left<T> left(T const &x) {
  return {x};
}

template <typename T>
constexpr Left<T> left(T &&x) {
  return {std::move(x)};
}

template <typename T>
struct Right {
  T value;
};

template <typename T>
constexpr Right<T> right(T const &x) {
  return {x};
}

template <typename T>
constexpr Right<T> right(T &&x) {
  return {std::move(x)};
}

template <typename L, typename R>
struct Either {
  union {
    L leftValue;
    R rightValue;
  };

  bool const isLeft = false;

  constexpr Either(Left<L> const &l) : leftValue{l.value}, isLeft{true} {}

  constexpr Either(Right<R> const &r) : rightValue{r.value}, isLeft{false} {}

  explicit Either(Left<L> &&l) : leftValue{std::move(l.value)}, isLeft{true} {}

  explicit Either(Right<R> &&r)
      : rightValue{std::move(r.value)}, isLeft{false} {}

  constexpr Either(Either<L, R> const &e) : isLeft{e.isLeft} {
    isLeft ? new (&leftValue) L(e.leftValue)
           : new (&rightValue) R(e.rightValue);
  }

  Either(Either<L, R> &&e) noexcept : isLeft{e.isLeft} {
    isLeft ? new (&leftValue) L(std::move(e.leftValue))
           : new (&rightValue) R(std::move(e.rightValue));
  }

  ~Either() { isLeft ? leftValue.~L() : rightValue.~R(); }

  constexpr auto left() const -> Maybe<L> {
    return isLeft ? maybe(leftValue) : maybe();
  }

  constexpr auto right() const -> Maybe<R> {
    return isLeft ? maybe() : maybe(rightValue);
  }

  static constexpr auto leftOf(L const &l) {
    return Either<L, R>(neither::left(l));
  }

  static constexpr auto rightOf(R const &r) {
    return Either<L, R>(neither::right(r));
  }

  static constexpr auto leftOf(L &&l) {
    return Either<L, R>(neither::left(std::move(l)));
  }

  static constexpr auto rightOf(R &&r) {
    return Either<L, R>(neither::right(std::move(r)));
  }

  template <typename L2 = L, typename R2 = R>
  constexpr auto join() const
      -> decltype(isCopyable((L2)leftValue, (R2)rightValue),
                  std::declval<std::common_type_t<L2, R2>>()) {
    return isLeft ? leftValue : rightValue;
  }

  template <typename L2 = L, typename R2 = R>
  auto join() && -> std::common_type_t<L2, R2> {
    return isLeft ? std::move(leftValue) : std::move(rightValue);
  }

  template <typename LeftF, typename RightF>
  constexpr auto join(LeftF const &leftCase, RightF const &rightCase) const
      -> decltype(isLeft ? leftCase(leftValue) : rightCase(rightValue)) {
    return isLeft ? leftCase(leftValue) : rightCase(rightValue);
  }

  template <typename F, typename L2 = L, typename R2 = R>
  constexpr auto leftMap(F const &leftCase) const & -> Either<
      decltype(leftCase(isCopyable((L2)leftValue, (R2)rightValue))), R2> {
    using NextEither = Either<decltype(leftCase(leftValue)), R2>;
    return isLeft ? NextEither::leftOf(leftCase(leftValue))
                  : NextEither::rightOf(rightValue);
  }

  template <typename F, typename L2 = L, typename R2 = R>
  auto leftMap(F const &leftCase)
      && -> Either<decltype(leftCase(std::move(leftValue))), R2> {
    using NextEither = Either<decltype(leftCase(std::move(leftValue))), R2>;
    return isLeft ? NextEither::leftOf(leftCase(std::move(leftValue)))
                  : NextEither::rightOf(std::move(rightValue));
  }

  template <typename F, typename L2 = L, typename R2 = R>
  constexpr auto rightMap(F const &rightCase) const & -> Either<
      L, decltype(rightCase(isCopyable((R2)rightValue, (L2)leftValue)))> {
    using NextEither = Either<L, decltype(rightCase(rightValue))>;
    return isLeft ? NextEither::leftOf(leftValue)
                  : NextEither::rightOf(rightCase(rightValue));
  }

  template <typename F, typename L2 = L, typename R2 = R>
  auto rightMap(F const &rightCase)
      && -> Either<L2, decltype(rightCase(std::move(rightValue)))> {
    using NextEither = Either<L, decltype(rightCase(std::move(rightValue)))>;
    return isLeft ? NextEither::leftOf(std::move(leftValue))
                  : NextEither::rightOf(rightCase(std::move(rightValue)));
  }

  template <typename LeftCase, typename L2 = L, typename R2 = R>
  constexpr auto leftFlatMap(LeftCase const &leftCase) const & -> decltype(
      ensureEitherRight(leftCase(isCopyable((L2)leftValue)),
                        isCopyable((R2)rightValue))) {
    using NextEither = decltype(leftCase(leftValue));

    return *this ? NextEither::rightOf(rightValue) : leftCase(leftValue);
  }

  template <class RightCase, class L2 = L, class R2 = R>
  constexpr auto rightFlatMap(RightCase const &rightCase) const & -> decltype(
      ensureEitherLeft(rightCase(isCopyable((R2)rightValue)),
                       isCopyable((L2)leftValue))) {
    using NextEither = decltype(rightCase(rightValue));

    return *this ? rightCase(rightValue) : NextEither::leftOf(leftValue);
  }

  template <class LeftCase, class L2 = L, class R2 = R>
  auto leftFlatMap(LeftCase const &leftCase) && -> decltype(ensureEitherRight(
      leftCase(std::move(leftValue)), std::move(rightValue))) {
    using NextEither = decltype(leftCase(std::move(leftValue)));

    return *this ? NextEither::rightOf(std::move(rightValue))
                 : leftCase(std::move(leftValue));
  }

  template <class RightCase, class L2 = L, class R2 = R>
  auto rightFlatMap(RightCase const &rightCase) && -> decltype(ensureEitherLeft(
      rightCase(std::move(rightValue)), std::move(leftValue))) {
    using NextEither = decltype(rightCase(std::move(rightValue)));

    return *this ? rightCase(std::move(rightValue))
                 : NextEither::leftOf(std::move(leftValue));
  }

  constexpr explicit operator bool() const { return !isLeft; }
};

template <typename L, typename R>
bool operator==(Either<L, R> const &a, Either<L, R> const &b) {
  if (a.isLeft) {
    if (b.isLeft) return a.left() == b.left();
  } else if (!b.isLeft)
    return a.right() == b.right();
  return false;
}

template <typename L, typename R>
bool operator!=(Either<L, R> const &a, Either<L, R> const &b) {
  return !(a == b);
}
}  // namespace neither

#endif
