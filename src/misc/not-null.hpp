///////////////////////////////////////////////////////////////////////////////
//
// Heroes' Path - Open-source, non-commercial, simple, game in the RPG style.
// Copyright (C) 2017 Ziesche Til Newman (tilnewman@gmail.com)
//
// This software is provided 'as-is', without any express or implied warranty.
// In no event will the authors be held liable for any damages arising from
// the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely, subject to the following restrictions:
//
//  1. The origin of this software must not be misrepresented; you must not
//     claim that you wrote the original software.  If you use this software
//     in a product, an acknowledgment in the product documentation would be
//     appreciated but is not required.
//
//  2. Altered source versions must be plainly marked as such, and must not
//     be misrepresented as being the original software.
//
//  3. This notice may not be removed or altered from any source distribution.
//
///////////////////////////////////////////////////////////////////////////////
#ifndef HEROESPATH_MISC_NOT_NULL_HPP_INCLUDED
#define HEROESPATH_MISC_NOT_NULL_HPP_INCLUDED
//
// not-null.hpp
//
#include "misc/assertlogandthrow.hpp"
#include <algorithm>
#include <iosfwd>
#include <type_traits>
#include <typeinfo>

namespace heroespath
{
namespace misc
{

    // Responsible for wrapping a pointer that is guaranteed to never be null.
    // Based on gsl::NotNull
    template <typename T>
    class NotNull
    {
    public:
        static_assert(std::is_pointer<T>::value, "misc::NotNull<T> T must be pointer type.");

        static_assert(
            std::is_assignable<T &, std::nullptr_t>::value,
            "misc::NotNull<T> cannot be assigned nullptr.");

        template <typename U, typename = std::enable_if_t<std::is_convertible<U, T>::value>>
        NotNull(U && u)
            : ptr_(std::forward<U>(u))
        {
            M_ASSERT_OR_LOGANDTHROW_SS(
                (ptr_ != nullptr),
                "misc::NotNull<" << typeid(U).name() << ">::Constructor(&&) was given a nullptr.");
        }

        template <typename U, typename = std::enable_if_t<std::is_convertible<U, T>::value>>
        NotNull(const NotNull<U> & other)
            : NotNull(other.Ptr())
        {}

        NotNull(NotNull && other) = default;
        NotNull(const NotNull & other) = default;
        NotNull & operator=(const NotNull & other) = default;

        T Ptr() const
        {
            M_ASSERT_OR_LOGANDTHROW_SS(
                (ptr_ != nullptr),
                "misc::NotNull<" << typeid(decltype(ptr_)).name()
                                 << ">::Ptr() found ptr_==nullptr.");

            return ptr_;
        }

        decltype(auto) Obj() { return *Ptr(); }
        decltype(auto) operator-> () { return Ptr(); }
        decltype(auto) operator*() { return *Ptr(); }

        decltype(auto) Obj() const { return *Ptr(); }
        decltype(auto) operator-> () const { return Ptr(); }
        decltype(auto) operator*() const { return *Ptr(); }

        operator T() const = delete;

        // prevents compilation when someone attempts to assign a null pointer constant
        NotNull(std::nullptr_t) = delete;
        NotNull & operator=(std::nullptr_t) = delete;

        // unwanted operators...pointers only point to single objects!
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

    template <typename T, typename U>
    auto operator==(const NotNull<T> & L, const NotNull<U> & R) -> decltype(L.Ptr() == R.Ptr())
    {
        return (L.Ptr() == R.Ptr());
    }

    template <typename T, typename U>
    auto operator!=(const NotNull<T> & L, const NotNull<U> & R) -> decltype(L.Ptr() != R.Ptr())
    {
        return (L.Ptr() != R.Ptr());
    }

    template <typename T, typename U>
    auto operator<(const NotNull<T> & L, const NotNull<U> & R) -> decltype(L.Ptr() < R.Ptr())
    {
        return (L.Ptr() < R.Ptr());
    }

    template <typename T>
    bool operator==(const NotNull<T> & L, const T R)
    {
        return L.Ptr() == R;
    }

    template <typename T>
    bool operator!=(const NotNull<T> & L, const T R)
    {
        return !(L == R);
    }

    template <typename T>
    bool operator==(const T L, const NotNull<T> & R)
    {
        return L == R.Ptr();
    }

    template <typename T>
    bool operator!=(const T L, const NotNull<T> & R)
    {
        return !(L == R);
    }

    // more unwanted operators
    template <typename T, typename U>
    std::ptrdiff_t operator-(const NotNull<T> &, const NotNull<U> &) = delete;

    template <typename T>
    NotNull<T> operator-(const NotNull<T> &, std::ptrdiff_t) = delete;

    template <typename T>
    NotNull<T> operator+(const NotNull<T> &, std::ptrdiff_t) = delete;

    template <typename T>
    NotNull<T> operator+(std::ptrdiff_t, const NotNull<T> &) = delete;
}
}

#endif // HEROESPATH_MISC_NOT_NULL_HPP_INCLUDED
