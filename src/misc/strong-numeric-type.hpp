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
#ifndef HEROESPATH_MISC_STRONGNUMERICTYPE_HPP_INCLUDED
#define HEROESPATH_MISC_STRONGNUMERICTYPE_HPP_INCLUDED
//
// strong-numeric-type.hpp
//
#include "misc/real.hpp"
#include "strong-type.hpp"
#include <cmath>
#include <ostream>
#include <type_traits>

namespace heroespath
{
namespace misc
{

    // Responsible for wrapping StrongType with functions useful for numeric types.
    template <typename T, typename Parameter>
    struct StrongNumericType : public StrongType<T, Parameter>
    {
        explicit StrongNumericType(const T & VALUE = T(0))
            : StrongType<T, Parameter>(VALUE)
        {}

        virtual ~StrongNumericType() = default;

        template <typename AsType_t>
        constexpr AsType_t As() const
        {
            return static_cast<AsType_t>(this->Get());
        }

        constexpr bool IsZero() const { return misc::IsRealZero(this->m_value); }

        constexpr bool IsNonZero() const { return !IsZero(); }

        const StrongNumericType Abs() const
        {
            return StrongNumericType<T, Parameter>(std::abs(this->m_value));
        }

        StrongNumericType & operator=(const StrongNumericType & RHS)
        {
            this->Get() = RHS.Get();
            return *this;
        }

        StrongNumericType & operator+=(const StrongNumericType & RHS)
        {
            this->Get() += RHS.Get();
            return *this;
        }

        StrongNumericType & operator-=(const StrongNumericType & RHS)
        {
            this->Get() -= RHS.Get();
            return *this;
        }

        StrongNumericType & operator*=(const StrongNumericType & RHS)
        {
            this->Get() *= RHS.Get();
            return *this;
        }

        StrongNumericType & operator/=(const StrongNumericType & RHS)
        {
            this->Get() /= RHS.Get();
            return *this;
        }

        StrongNumericType operator+(const StrongNumericType & RHS) const
        {
            return StrongNumericType(*this) += RHS;
        }

        StrongNumericType operator-(const StrongNumericType & RHS) const
        {
            return StrongNumericType(*this) -= RHS;
        }

        StrongNumericType operator*(const StrongNumericType & RHS) const
        {
            return StrongNumericType(*this) *= RHS;
        }

        StrongNumericType operator/(const StrongNumericType & RHS) const
        {
            return StrongNumericType(*this) /= RHS;
        }

        inline bool operator==(const StrongNumericType & RHS) const
        {
            return this->Get() == RHS.Get();
        }

        inline bool operator!=(const StrongNumericType & RHS) const
        {
            return this->Get() != RHS.Get();
        }

        inline bool operator<(const StrongNumericType & RHS) const
        {
            return this->Get() < RHS.Get();
        }

        inline bool operator<=(const StrongNumericType & RHS) const
        {
            return this->Get() <= RHS.Get();
        }

        inline bool operator>(const StrongNumericType & RHS) const
        {
            return this->Get() > RHS.Get();
        }

        inline bool operator>=(const StrongNumericType & RHS) const
        {
            return this->Get() >= RHS.Get();
        }

        StrongNumericType & operator++()
        {
            ++(this->Get());
            return *this;
        }

        StrongNumericType operator++(int)
        {
            StrongNumericType temp{ *this };
            operator++();
            return temp;
        }

        StrongNumericType & operator--()
        {
            --(this->Get());
            return *this;
        }

        StrongNumericType operator--(int)
        {
            StrongNumericType temp{ *this };
            operator--();
            return temp;
        }
    };

    template <typename T, typename Parameter>
    std::ostream & operator<<(std::ostream & os, const StrongNumericType<T, Parameter> & RHS)
    {
        os << RHS.Get();
        return os;
    }

    template <typename T, typename Parameter>
    inline bool operator==(const int LEFT_INT, const StrongNumericType<T, Parameter> & RIGHT_SNT)
    {
        return (LEFT_INT == RIGHT_SNT.template As<int>());
    }

    template <typename T, typename Parameter>
    inline bool operator==(const StrongNumericType<T, Parameter> & LEFT_SNT, const int RIGHT_INT)
    {
        return (LEFT_SNT.template As<int>() == RIGHT_INT);
    }

    template <typename T, typename Parameter>
    inline bool operator!=(const int LEFT_INT, const StrongNumericType<T, Parameter> & RIGHT_SNT)
    {
        return (LEFT_INT != RIGHT_SNT.template As<int>());
    }

    template <typename T, typename Parameter>
    inline bool operator!=(const StrongNumericType<T, Parameter> & LEFT_SNT, const int RIGHT_INT)
    {
        return (LEFT_SNT.template As<int>() != RIGHT_INT);
    }
} // namespace misc
} // namespace heroespath

#endif // HEROESPATH_MISC_STRONGNUMERICTYPE_HPP_INCLUDED
