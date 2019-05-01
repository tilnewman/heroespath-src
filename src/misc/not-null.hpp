// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_MISC_NOT_NULL_HPP_INCLUDED
#define HEROESPATH_MISC_NOT_NULL_HPP_INCLUDED
//
// not-null.hpp
//
#include <memory>
#include <stdexcept>

namespace heroespath
{
namespace misc
{

    // wraps a pointer that can never be null
    template <typename T>
    class NotNull
    {
    public:
        static_assert(
            std::is_assignable<T &, std::nullptr_t>::value,
            "NotNull<T> cannot be assigned nullptr.");

        template <typename U, typename = std::enable_if_t<std::is_convertible_v<U, T>>>
        constexpr explicit NotNull(U && thing)
            : ptr_(std::forward<U>(thing))
        {
            if (nullptr == ptr_)
            {
                throw std::runtime_error("NotNull(U && u) constructor given nullptr.");
            }
        }

        template <typename = std::enable_if_t<!std::is_same_v<std::nullptr_t, T>>>
        constexpr explicit NotNull(T p)
            : ptr_(p)
        {
            if (nullptr == ptr_)
            {
                throw std::runtime_error("NotNull(T p) constructor given nullptr.");
            }
        }

        template <typename U, typename = std::enable_if_t<std::is_convertible_v<U, T>>>
        constexpr NotNull(const NotNull<U> & NOT_NULL)
            : NotNull(NOT_NULL.get())
        {}

        NotNull(NotNull &&) = default;
        NotNull(const NotNull &) = default;
        NotNull & operator=(const NotNull &) = default;

        constexpr T get() const
        {
            if (nullptr == ptr_)
            {
                throw std::runtime_error("NotNull::get() called when it was holding a nullptr.");
            }

            return ptr_;
        }

        constexpr operator T() const { return get(); }
        constexpr T operator->() const { return get(); }
        constexpr decltype(auto) operator*() const { return *get(); }

        NotNull(std::nullptr_t) = delete;
        NotNull & operator=(std::nullptr_t) = delete;

        NotNull & operator++() = delete;
        NotNull & operator--() = delete;
        NotNull operator++(int) = delete;
        NotNull operator--(int) = delete;
        NotNull & operator+=(std::ptrdiff_t) = delete;
        NotNull & operator-=(std::ptrdiff_t) = delete;
        void operator[](std::ptrdiff_t) const = delete;

    private:
        T ptr_;
    };

    template <typename T>
    auto MakeNotNull(T && thing)
    {
        return NotNull<std::remove_cv_t<std::remove_reference_t<T>>> { std::forward<T>(thing) };
    }

    template <typename T, class U>
    bool operator==(const NotNull<T> & LHS, const NotNull<U> & RHS)
    {
        return (LHS.get() == RHS.get());
    }

    template <typename T, class U>
    bool operator!=(const NotNull<T> & LHS, const NotNull<U> & RHS)
    {
        return (LHS.get() != RHS.get());
    }

    template <typename T, class U>
    bool operator<(const NotNull<T> & LHS, const NotNull<U> & RHS)
    {
        return (LHS.get() < RHS.get());
    }

    template <typename T, class U>
    bool operator<=(const NotNull<T> & LHS, const NotNull<U> & RHS)
    {
        return (LHS.get() <= RHS.get());
    }

    template <typename T, class U>
    bool operator>(const NotNull<T> & LHS, const NotNull<U> & RHS)
    {
        return (LHS.get() > RHS.get());
    }

    template <typename T, class U>
    bool operator>=(const NotNull<T> & LHS, const NotNull<U> & RHS)
    {
        return (LHS.get() >= RHS.get());
    }

    template <typename T, class U>
    std::ptrdiff_t operator-(const NotNull<T> &, const NotNull<U> &) = delete;

    template <typename T>
    NotNull<T> operator-(const NotNull<T> &, std::ptrdiff_t) = delete;

    template <typename T>
    NotNull<T> operator+(const NotNull<T> &, std::ptrdiff_t) = delete;

    template <typename T>
    NotNull<T> operator+(std::ptrdiff_t, const NotNull<T> &) = delete;

} // namespace misc
} // namespace heroespath

#endif // HEROESPATH_MISC_NOT_NULL_HPP_INCLUDED
