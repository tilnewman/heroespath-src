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
#include "misc/type-helpers.hpp"

#include <boost/serialization/serialization.hpp>

#include <ostream>

namespace heroespath
{
namespace misc
{

    // Responsible for wrapping types with a phantom tag Parameter_t to make them stronger.
    template <typename Value_t, typename Parameter_t>
    class StrongType
    {
    protected:
        explicit StrongType(const Value_t & X)
            : m_value(X)
        {}

    public:
        using value_type = Value_t;

        StrongType()
            : m_value()
        {
            if constexpr (are_arithmetic_v<Value_t>)
            {
                m_value = Value_t(0);
            }
        }

        virtual ~StrongType() = default;

        Value_t & Get() { return m_value; }
        const Value_t & Get() const { return m_value; }

        void Set(const Value_t & X) { m_value = X; }

        template <typename Prop_t>
        static std::enable_if_t<are_same_v<Value_t, Prop_t>, const StrongType<Value_t, Parameter_t>>
            Make(const Prop_t & X)
        {
            return StrongType<Value_t, Parameter_t>(X);
        }

        template <typename V, typename P>
        friend bool operator==(const StrongType<V, P> & L, const StrongType<V, P> & R);

        template <typename V, typename P>
        friend bool operator<(const StrongType<V, P> & L, const StrongType<V, P> & R);

    protected:
        Value_t m_value;

    private:
        friend class boost::serialization::access;
        template <typename Archive>
        void serialize(Archive & ar, const unsigned int)
        {
            ar & m_value;
        }
    };

    template <typename V, typename P>
    std::ostream & operator<<(std::ostream & os, const StrongType<V, P> & RHS)
    {
        os << RHS.Get();
        return os;
    }

    template <typename V, typename P>
    bool operator==(const StrongType<V, P> & L, const StrongType<V, P> & R)
    {
        return (L.m_value == R.m_value);
    }

    template <typename V, typename P>
    bool operator!=(const StrongType<V, P> & L, const StrongType<V, P> & R)
    {
        return !(L == R);
    }

    template <typename V, typename P>
    bool operator<(const StrongType<V, P> & L, const StrongType<V, P> & R)
    {
        return (L.m_value < R.m_value);
    }

    template <typename V, typename P>
    bool operator>(const StrongType<V, P> & L, const StrongType<V, P> & R)
    {
        return (R < L);
    }

    template <typename V, typename P>
    bool operator<=(const StrongType<V, P> & L, const StrongType<V, P> & R)
    {
        return !(L > R);
    }

    template <typename V, typename P>
    bool operator>=(const StrongType<V, P> & L, const StrongType<V, P> & R)
    {
        return !(L < R);
    }

} // namespace misc
} // namespace heroespath

#endif // HEROESPATH_MISC_STRONGTYPE_HPP_INCLUDED
