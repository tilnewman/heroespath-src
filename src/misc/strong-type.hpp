// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_MISC_STRONGTYPE_HPP_INCLUDED
#define HEROESPATH_MISC_STRONGTYPE_HPP_INCLUDED
//
// strong-type.hpp
//
#include "misc/boost-serialize-includes.hpp"

#include "misc/nameof.hpp"

#include <ostream>
#include <sstream>
#include <string>

namespace heroespath
{
namespace misc
{

    // Responsible for wrapping types with a phantom tag Parameter_t to make them stronger.
    template <typename T, typename Parameter_t>
    struct StrongType
    {
        explicit StrongType(const T & VALUE = T())
            : m_value(VALUE)
        {}

        virtual ~StrongType() = default;

        using type = T;

        T & Get() { return m_value; }
        const T & Get() const { return m_value; }

        const std::string ToString() const
        {
            std::ostringstream ss;
            ss << m_value;
            return ss.str();
        }

        const std::string TypeName() const { return NAMEOF_TYPE_T_STR(T); }

        inline bool operator==(const StrongType & RHS) const
        {
            return this->m_value == RHS.m_value;
        }

        inline bool operator!=(const StrongType & RHS) const
        {
            return this->m_value != RHS.m_value;
        }

        inline bool operator<(const StrongType & RHS) const { return this->m_value < RHS.m_value; }

        inline bool operator<=(const StrongType & RHS) const
        {
            return this->m_value <= RHS.m_value;
        }

        inline bool operator>(const StrongType & RHS) const { return this->m_value > RHS.m_value; }

        inline bool operator>=(const StrongType & RHS) const
        {
            return this->m_value >= RHS.m_value;
        }

        template <typename FromType_t>
        static StrongType<T, Parameter_t> Make(const FromType_t FROM_VALUE)
        {
            return StrongType<T, Parameter_t>(static_cast<T>(FROM_VALUE));
        }

    protected:
        T m_value;

    private:
        friend class boost::serialization::access;
        template <typename Archive>
        void serialize(Archive & ar, const unsigned int)
        {
            ar & m_value;
        }
    };

    template <typename T, typename Parameter_t>
    std::ostream & operator<<(std::ostream & os, const StrongType<T, Parameter_t> & RHS)
    {
        os << RHS.Get();
        return os;
    }
} // namespace misc
} // namespace heroespath

#endif // HEROESPATH_MISC_STRONGTYPE_HPP_INCLUDED
