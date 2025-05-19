//
// Created by ramesh on 19/05/25.
//

#pragma once

#include "Concepts.h"
#include "Platform.h"

namespace RK {

namespace Detail {

    template<auto condition, typename T>
    struct ConditionallyResultType;

    template<typename T>
    struct ConditionallyResultType<true, T> {
        using Type = typename T::ResultType;
    };

    template<typename T>
    struct ConditionallyResultType<false, T> {
        using Type = T;
    };

} // DETAIL

template<auto condition, typename T>
using ConditionallyResultType = typename Detail::ConditionallyResultType<condition, T>::Type;

template<typename>
class Optional;

struct OptionalNone {
    explicit constexpr OptionalNone() = default;
};


template<typename T, typename Self = Optional<T>>
requires(!IsLvalueReference<Self>) class [[nodiscard]] OptionalBase {
public:
    using ValueType = T;

    template<SameAs<OptionalNone> V>
    ALWAYS_INLINE constexpr Self& operator=(V)
    {
        static_cast<Self&>(*this).clear();
        return static_cast<Self&>(*this);
    }

    [[nodiscard]] ALWAYS_INLINE constexpr T* ptr() &
    {
        return static_cast<Self&>(*this).has_value() ? __builtin_launder(reinterpret_cast<T*>(&static_cast<Self&>(*this).value())) : nullptr;
    }

    [[nodiscard]] ALWAYS_INLINE constexpr T const* ptr() const&
    {
        return static_cast<Self const&>(*this).has_value() ? __builtin_launder(reinterpret_cast<T const*>(&static_cast<Self const&>(*this).value())) : nullptr;
    }

    template<typename O = T, typename Fallback = O>
    [[nodiscard]] ALWAYS_INLINE constexpr O value_or(Fallback const& fallback) const&
    {
        if (static_cast<Self const&>(*this).has_value())
            return static_cast<Self const&>(*this).value();
        return fallback;
    }

    template<typename O = T, typename Fallback = O>
    requires(!IsLvalueReference<O> && !IsRvalueReference<O>)
    [[nodiscard]] ALWAYS_INLINE constexpr O value_or(Fallback&& fallback) &&
    {
        if (static_cast<Self&>(*this).has_value())
            return move(static_cast<Self&>(*this).value());
        return move(fallback);
    }

    template<typename Callback, typename O = T>
    [[nodiscard]] ALWAYS_INLINE constexpr O value_or_lazy_evaluated(Callback callback) const
    {
        if (static_cast<Self const&>(*this).has_value())
            return static_cast<Self const&>(*this).value();
        return callback();
    }

    template<typename Callback, typename O = T>
    [[nodiscard]] ALWAYS_INLINE constexpr Optional<O> value_or_lazy_evaluated_optional(Callback callback) const
    {
        if (static_cast<Self const&>(*this).has_value())
            return static_cast<Self const&>(*this).value();
        return callback();
    }

    template<typename Callback, typename O = T>
    [[nodiscard]] ALWAYS_INLINE constexpr ErrorOr<O> try_value_or_lazy_evaluated(Callback callback) const
    {
        if (static_cast<Self const&>(*this).has_value())
            return static_cast<Self const&>(*this).value();
        return TRY(callback());
    }

    template<typename Callback, typename O = T>
    [[nodiscard]] ALWAYS_INLINE constexpr ErrorOr<Optional<O>> try_value_or_lazy_evaluated_optional(Callback callback) const
    {
        if (static_cast<Self const&>(*this).has_value())
            return static_cast<Self const&>(*this).value();
        return TRY(callback());
    }

    [[nodiscard]] ALWAYS_INLINE constexpr T const& operator*() const { return static_cast<Self const&>(*this).value(); }
    [[nodiscard]] ALWAYS_INLINE constexpr T& operator*() { return static_cast<Self&>(*this).value(); }

    ALWAYS_INLINE constexpr T const* operator->() const { return &static_cast<Self const&>(*this).value(); }
    ALWAYS_INLINE constexpr T* operator->() { return &static_cast<Self&>(*this).value(); }

    template<typename F, typename MappedType = decltype(declval<F>()(declval<T&>())), auto IsErrorOr = IsSpecializationOf<MappedType, ErrorOr>, typename OptionalType = Optional<ConditionallyResultType<IsErrorOr, MappedType>>>
    ALWAYS_INLINE constexpr Conditional<IsErrorOr, ErrorOr<OptionalType>, OptionalType> map(F&& mapper)
    {
        if constexpr (IsErrorOr) {
            if (static_cast<Self&>(*this).has_value())
                return OptionalType { TRY(mapper(static_cast<Self&>(*this).value())) };
            return OptionalType {};
        } else {
            if (static_cast<Self&>(*this).has_value())
                return OptionalType { mapper(static_cast<Self&>(*this).value()) };

            return OptionalType {};
        }
    }

    template<typename F, typename MappedType = decltype(declval<F>()(declval<T&>())), auto IsErrorOr = IsSpecializationOf<MappedType, ErrorOr>, typename OptionalType = Optional<ConditionallyResultType<IsErrorOr, MappedType>>>
    ALWAYS_INLINE constexpr Conditional<IsErrorOr, ErrorOr<OptionalType>, OptionalType> map(F&& mapper) const
    {
        if constexpr (IsErrorOr) {
            if (static_cast<Self const&>(*this).has_value())
                return OptionalType { TRY(mapper(static_cast<Self const&>(*this).value())) };
            return OptionalType {};
        } else {
            if (static_cast<Self const&>(*this).has_value())
                return OptionalType { mapper(static_cast<Self const&>(*this).value()) };

            return OptionalType {};
        }
    }
};

} // RK