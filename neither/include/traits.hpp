#ifndef NEITHER_TRAITS_HPP
#define NEITHER_TRAITS_HPP

#include <tuple>

namespace neither {
template <typename T>
struct Maybe;

template <typename L, typename R>
struct Either;

template <typename W, typename A>
struct Writer;

template <typename L, typename... Xs>
auto isCopyable(L l, Xs...) -> L {
  return l;
}

template <typename... T>
auto ensureTuple(std::tuple<T...> const &e) -> std::tuple<T...> {
  return e;
};

template <typename A, typename B>
auto ensurePair(std::pair<A, B> const &e) -> std::pair<A, B> {
  return e;
};

template <typename T>
auto ensureMaybe(Maybe<T> const &e) -> Maybe<T> {
  return e;
}

template <typename W, typename A>
auto ensureWriter(Writer<W, A> const &e) -> Writer<W, A> {
  return e;
}

template <typename L, typename R>
auto ensureEither(Either<L, R> const &e) -> Either<L, R> {
  return e;
}

template <typename L, typename R>
auto ensureEither(Either<L, R> &&e) -> Either<L, R> {
  return e;
}

template <typename L, typename R>
auto ensureEitherRight(Either<L, R> const &e, R) -> Either<L, R> {
  return e;
}

template <typename L, typename R>
auto ensureEitherRight(Either<L, R> &&e, R &&) -> Either<L, R> {
  return e;
}

template <typename L, typename R>
auto ensureEitherLeft(Either<L, R> const &e, L) -> Either<L, R> {
  return e;
}

template <typename L, typename R>
auto ensureEitherLeft(Either<L, R> &&e, L &&) -> Either<L, R> {
  return e;
}

}  // namespace neither

#endif
