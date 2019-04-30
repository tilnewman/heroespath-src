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
#include "misc/nameof.hpp"

#include <exception>
#include <iosfwd>
#include <memory>
#include <string_view>

namespace heroespath
{
namespace misc
{

    namespace helpers
    {
        const std::runtime_error makeException(
            const std::string_view TYPE_T_NAME,
            const std::string_view MESSAGE,
            const std::string_view TYPE_U_NAME = "");

        const std::runtime_error makeException(
            const std::string_view TYPE_T_NAME,
            const std::string_view MESSAGE_1,
            const std::string_view TYPE_U_NAME,
            const std::string_view MESSAGE_2);

    } // namespace helpers

    // wraps a pointer that can never be null
    template <typename T>
    class NotNull
    {
    public:
        static_assert(
            std::is_assignable<T &, std::nullptr_t>::value,
            "NotNull<T> cannot be assigned nullptr.");

        template <typename U, typename = std::enable_if_t<std::is_convertible<U, T>::value>>
        constexpr explicit NotNull(U && thing)
            : ptr_(std::forward<U>(thing))
        {
            if (nullptr == ptr_)
            {
                throw helpers::makeException(
                    NAMEOF_TYPE_T(T), "(U && u) but u==nullptr.", NAMEOF_TYPE_T(U));
            }
        }

        template <typename = std::enable_if_t<!std::is_same<std::nullptr_t, T>::value>>
        constexpr explicit NotNull(T p)
            : ptr_(p)
        {
            if (nullptr == ptr_)
            {
                throw helpers::makeException(NAMEOF_TYPE_T(T), "(T p) but p is nullptr.");
            }
        }

        template <typename U, typename = std::enable_if_t<std::is_convertible<U, T>::value>>
        constexpr NotNull(const NotNull<U> & NOT_NULL)
            : NotNull(NOT_NULL.get())
        {}

        template <typename U, typename = std::enable_if_t<std::is_convertible<U *, T>::value>>
        constexpr NotNull(const std::unique_ptr<U> & UNIQUE_PTR)
            : ptr_(UNIQUE_PTR.get())
        {
            if (UNIQUE_PTR.get() == nullptr)
            {
                throw helpers::makeException(
                    NAMEOF_TYPE_T(T),
                    ">(const std::unique_ptr<",
                    NAMEOF_TYPE_T(U),
                    "> & uptr) but uptr.get()==nullptr.");
            }
        }

        template <typename U, typename = std::enable_if_t<std::is_convertible<U *, T>::value>>
        constexpr NotNull(const std::shared_ptr<U> & SHARED_PTR)
            : ptr_(SHARED_PTR.get())
        {
            if (SHARED_PTR.get() == nullptr)
            {
                throw helpers::makeException(
                    NAMEOF_TYPE_T(T),
                    ">(const std::shared_ptr<",
                    NAMEOF_TYPE_T(U),
                    "> & sptr) but sptr.get()==nullptr.");
            }
        }

        NotNull(NotNull &&) = default;
        NotNull(const NotNull &) = default;
        NotNull & operator=(const NotNull &) = default;

        constexpr T get() const
        {
            if (nullptr == ptr_)
            {
                throw helpers::makeException(
                    NAMEOF_TYPE_T(T), "::get() called when ptr_==nullptr.");
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

    template <typename T>
    std::ostream & operator<<(std::ostream & os, const NotNull<T> & NOT_NULL)
    {
        os << NOT_NULL.get();
        return os;
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
