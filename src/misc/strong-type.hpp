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
#ifndef HEROESPATH_MISC_STRONGTYPE_HPP_INCLUDED
#define HEROESPATH_MISC_STRONGTYPE_HPP_INCLUDED
//
// strong-type.hpp
//
#include "misc/boost-serialize-includes.hpp"

#include <boost/type_index.hpp>

#include <ostream>
#include <sstream>
#include <type_traits>


namespace heroespath
{
namespace misc
{

    //Responsible for wrapping types with a phantom tag parameter to make them stronger.
    template <typename T, typename Parameter>
    struct StrongType
    {
        explicit StrongType(const T & VALUE = T())
        :
            m_value(VALUE)
        {}

        virtual ~StrongType()
        {}

        using type = T;

        //this move constructor allows for reference types
        template<typename T_ = T>
        explicit StrongType(
            T && value,
            typename std::enable_if< ! std::is_reference<T_>::value, std::nullptr_t>::type = nullptr)
        :
            m_value(std::move(value))
        {}

        T & Get() { return m_value; }
        const T & Get() const { return m_value; }

        const std::string ToString() const
        {
            std::ostringstream ss;
            ss << m_value;
            return ss.str();
        }

        const std::string TypeName() const
        {
            return boost::typeindex::type_id<T>().pretty_name();
        }

        bool operator==(const StrongType & RHS) const
        {
            return this->m_value == RHS.m_value;
        }

        bool operator!=(const StrongType & RHS) const
        {
            return this->m_value != RHS.m_value;
        }

        bool operator<(const StrongType & RHS) const
        {
            return this->m_value < RHS.m_value;
        }

        bool operator<=(const StrongType & RHS) const
        {
            return this->m_value <= RHS.m_value;
        }

        bool operator>(const StrongType & RHS) const
        {
            return this->m_value > RHS.m_value;
        }

        bool operator>=(const StrongType & RHS) const
        {
            return this->m_value >= RHS.m_value;
        }

    protected:
        T m_value;

    private:
        friend class boost::serialization::access;
        template<typename Archive>
        void serialize(Archive & ar, const unsigned int)
        {
            ar & m_value;
        }
    };


    template<typename T, typename Parameter>
    std::ostream & operator<<(
        std::ostream & os,
        const StrongType<T, Parameter> & RHS)
    {
        os << RHS.Get();
        return os;
    }

}
}

#endif //HEROESPATH_MISC_STRONGTYPE_HPP_INCLUDED
