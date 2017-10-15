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
#ifndef MISC_STRONGNUMERICTYPE_HPP_INCLUDED
#define MISC_STRONGNUMERICTYPE_HPP_INCLUDED
//
// strong-numeric-type.hpp
//
#include "strong-type.hpp"

#include <cmath>
#include <ostream>
#include <type_traits>


namespace misc
{

    //Responsible for wrapping StrongType with functions useful for numeric types.
    template<typename T, typename Parameter>
    struct StrongNumericType : public StrongType<T, Parameter>
    {
        explicit StrongNumericType(const T & VALUE = T(0))
         :
            StrongType<T, Parameter>(VALUE)
        {}

        virtual ~StrongNumericType()
        {}

        unsigned char AsUChar() const { return static_cast<unsigned char>(this->m_value); }
        int AsInt() const { return static_cast<int>(this->m_value); }
        std::size_t AsSizeT() const { return static_cast<std::size_t>(this->m_value); }
        float AsFloat() const { return static_cast<float>(this->m_value); }
        double AsDouble() const { return static_cast<double>(this->m_value); }

        const StrongNumericType Abs() const
        {
            return StrongNumericType<T, Parameter>(std::abs(this->m_value));
        }
        
        StrongNumericType & operator+=(const StrongNumericType & RHS)
        {
            this->m_value += RHS.m_value;
            return * this;
        }

        StrongNumericType & operator-=(const StrongNumericType & RHS)
        {
            this->m_value -= RHS.m_value;
            return * this;
        }

        StrongNumericType & operator*=(const StrongNumericType & RHS)
        {
            this->m_value *= RHS.m_value;
            return * this;
        }

        StrongNumericType & operator/=(const StrongNumericType & RHS)
        {
            this->m_value /= RHS.m_value;
            return * this;
        }

        StrongNumericType operator+(const StrongNumericType & RHS) const
        {
            return StrongNumericType( * this) += RHS;
        }

        StrongNumericType operator-(const StrongNumericType & RHS) const
        {
            return StrongNumericType( * this) -= RHS;
        }

        StrongNumericType operator*(const StrongNumericType & RHS) const
        {
            return StrongNumericType( * this) *= RHS;
        }

        StrongNumericType operator/(const StrongNumericType & RHS) const
        {
            return StrongNumericType( * this) /= RHS;
        }

        
        bool operator==(const StrongNumericType & RHS) const
        {
            return this->m_value == RHS.m_value;
        }

        bool operator!=(const StrongNumericType & RHS) const
        {
            return this->m_value != RHS.m_value;
        }

        bool operator<(const StrongNumericType & RHS) const
        {
            return this->m_value < RHS.m_value;
        }

        bool operator<=(const StrongNumericType & RHS) const
        {
            return this->m_value <= RHS.m_value;
        }

        bool operator>(const StrongNumericType & RHS) const
        {
            return this->m_value > RHS.m_value;
        }

        bool operator>=(const StrongNumericType & RHS) const
        {
            return this->m_value >= RHS.m_value;
        }
    };


    template<typename T, typename Parameter>
    std::ostream & operator<<(
        std::ostream & os,
        const StrongNumericType<T, Parameter> & RHS)
    {
        os << RHS.Get();
        return os;
    }

}

#endif //MISC_STRONGNUMERICTYPE_HPP_INCLUDED