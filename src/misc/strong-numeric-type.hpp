// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_MISC_STRONGNUMERICTYPE_HPP_INCLUDED
#define HEROESPATH_MISC_STRONGNUMERICTYPE_HPP_INCLUDED
//
// strong-numeric-type.hpp
//
#include "creature/trait-type.hpp"
#include "misc/real.hpp"
#include "misc/type-helpers.hpp"

#include <boost/serialization/serialization.hpp>

#include <iostream>

namespace heroespath
{
namespace misc
{

    // Responsible for wrapping StrongNumericType with functions useful for numeric types.
    template <
        typename Value_t,
        typename Parameter_t,
        typename = std::enable_if_t<are_integral_nobool_v<Value_t>>>
    class StrongNumericType
    {
        explicit StrongNumericType(const Value_t NUMBER)
            : m_value(NUMBER)
        {}

    public:
        using value_type = Value_t;

        StrongNumericType()
            : m_value(Value_t(0))
        {}

        const Value_t Get() const { return m_value; }

        template <typename New_t>
        std::enable_if_t<(are_arithmetic_nobool_v<New_t> || are_enum_v<New_t>), const New_t>
            GetAs() const
        {
            return static_cast<New_t>(this->m_value);
        }

        creature::Trait_t GetAsTrait() const
        {
            return static_cast<creature::Trait_t>(this->m_value);
        }

        bool IsZero() const { return (this->m_value == 0); }
        bool IsPositive() const { return (this->m_value >= 0); }
        bool IsNegative() const { return (this->m_value < 0); }

        const std::string ToString() const { return std::to_string(this->m_value); }

        template <typename Prop_t>
        static std::enable_if_t<
            (are_arithmetic_nobool_v<Prop_t> || are_enum_v<Prop_t>),
            const StrongNumericType<Value_t, Parameter_t>>
            Make(const Prop_t X)
        {
            return StrongNumericType<Value_t, Parameter_t>(static_cast<Value_t>(X));
        }

        StrongNumericType<Value_t, Parameter_t> MakePositiveCopy() const
        {
            return StrongNumericType<Value_t, Parameter_t>(Abs(this->m_value));
        }

        StrongNumericType<Value_t, Parameter_t> MakeNegativeCopy() const
        {
            static_assert(are_signed_v<Value_t>, "unsigned types forbidden");

            return StrongNumericType<Value_t, Parameter_t>(-Abs(this->m_value));
        }

        void MakePositive() { this->m_value = MakePositiveCopy().Get(); }
        void MakeNegative() { this->m_value = MakeNegativeCopy().Get(); }

        StrongNumericType<Value_t, Parameter_t> ChangeSignCopy() const
        {
            static_assert(are_signed_v<Value_t>, "unsigned types forbidden");
            return StrongNumericType<Value_t, Parameter_t>(-(this->m_value));
        }

        void ChangeSign() { this->m_value = ChangeSignCopy().Get(); }

        StrongNumericType & operator+=(const StrongNumericType & RHS)
        {
            this->m_value += RHS.m_value;
            return *this;
        }

        StrongNumericType & operator-=(const StrongNumericType & RHS)
        {
            this->m_value -= RHS.m_value;
            return *this;
        }

        StrongNumericType & operator*=(const StrongNumericType & RHS)
        {
            this->m_value *= RHS.m_value;
            return *this;
        }

        // NOTE: divide or mod by zero equals zero
        StrongNumericType & operator/=(const StrongNumericType & RHS)
        {
            if (RHS.IsZero())
            {
                this->m_value = Value_t(0);
            }
            else
            {
                this->m_value /= RHS.m_value;
            }

            return *this;
        }

        // NOTE: divide or mod by zero equals zero
        StrongNumericType & operator%=(const StrongNumericType & RHS)
        {
            if (RHS.IsZero())
            {
                this->m_value = Value_t(0);
            }
            else
            {
                this->m_value %= RHS.m_value;
            }

            return *this;
        }

        StrongNumericType & operator++()
        {
            ++(this->m_value);
            return *this;
        }

        StrongNumericType operator++(int)
        {
            StrongNumericType temp(*this);
            operator++();
            return temp;
        }

        StrongNumericType & operator--()
        {
            --(this->m_value);
            return *this;
        }

        StrongNumericType operator--(int)
        {
            StrongNumericType temp(*this);
            operator--();
            return temp;
        }

        template <typename V, typename P>
        friend std::ostream & operator<<(std::ostream & os, const StrongNumericType<V, P> & R);

        template <typename V, typename P>
        friend bool
            operator==(const StrongNumericType<V, P> & L, const StrongNumericType<V, P> & R);

        template <typename V, typename P>
        friend bool operator<(const StrongNumericType<V, P> & L, const StrongNumericType<V, P> & R);

        template <typename V, typename P>
        friend StrongNumericType<V, P>
            Max(const StrongNumericType<V, P> & L, const StrongNumericType<V, P> & R);

        template <typename V, typename P>
        friend StrongNumericType<V, P>
            Min(const StrongNumericType<V, P> & L, const StrongNumericType<V, P> & R);

        template <typename V, typename P>
        friend StrongNumericType<V, P>
            Random(const StrongNumericType<V, P> & MIN, const StrongNumericType<V, P> & MAX);

        template <typename V, typename P>
        friend StrongNumericType<V, P> Random(const StrongNumericType<V, P> & MAX);

    private:
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
    std::ostream & operator<<(std::ostream & os, const StrongNumericType<V, P> & R)
    {
        os << R.m_value;
        return os;
    }

    template <typename V, typename P>
    bool operator==(const StrongNumericType<V, P> & L, const StrongNumericType<V, P> & R)
    {
        return (L.m_value == R.m_value);
    }

    template <typename V, typename P>
    bool operator!=(const StrongNumericType<V, P> & L, const StrongNumericType<V, P> & R)
    {
        return !(L == R);
    }

    template <typename V, typename P>
    bool operator<(const StrongNumericType<V, P> & L, const StrongNumericType<V, P> & R)
    {
        return (L.m_value < R.m_value);
    }

    template <typename V, typename P>
    bool operator>(const StrongNumericType<V, P> & L, const StrongNumericType<V, P> & R)
    {
        return (R < L);
    }

    template <typename V, typename P>
    bool operator<=(const StrongNumericType<V, P> & L, const StrongNumericType<V, P> & R)
    {
        return !(L > R);
    }

    template <typename V, typename P>
    bool operator>=(const StrongNumericType<V, P> & L, const StrongNumericType<V, P> & R)
    {
        return !(L < R);
    }

    template <typename V, typename P>
    StrongNumericType<V, P>
        operator+(const StrongNumericType<V, P> & L, const StrongNumericType<V, P> & R)
    {
        return (StrongNumericType<V, P>(L) += R);
    }

    template <typename V, typename P>
    StrongNumericType<V, P>
        operator-(const StrongNumericType<V, P> & L, const StrongNumericType<V, P> & R)
    {
        return (StrongNumericType<V, P>(L) -= R);
    }

    template <typename V, typename P>
    StrongNumericType<V, P>
        operator*(const StrongNumericType<V, P> & L, const StrongNumericType<V, P> & R)
    {
        return (StrongNumericType<V, P>(L) *= R);
    }

    // NOTE: divide or mod by zero equals zero
    template <typename V, typename P>
    StrongNumericType<V, P>
        operator/(const StrongNumericType<V, P> & L, const StrongNumericType<V, P> & R)
    {
        return (StrongNumericType<V, P>(L) /= R);
    }

    // NOTE: divide or mod by zero equals zero
    template <typename V, typename P>
    StrongNumericType<V, P>
        operator%(const StrongNumericType<V, P> & L, const StrongNumericType<V, P> & R)
    {
        return (StrongNumericType<V, P>(L) %= R);
    }

    // note how this NEVER changes the sign
    template <typename V, typename P>
    StrongNumericType<V, P> operator+(const StrongNumericType<V, P> & X)
    {
        return X;
    }

    // note how this ALWAYS changes the sign
    template <typename V, typename P>
    StrongNumericType<V, P> operator-(const StrongNumericType<V, P> & X)
    {
        static_assert(are_signed_v<V>, "unsigned types forbidden");
        return X.ChangeSignCopy();
    }

    // strong numeric type utils

    template <typename V, typename P>
    StrongNumericType<V, P> Abs(const StrongNumericType<V, P> & X)
    {
        return X.MakePositiveCopy();
    }

    template <typename V, typename P>
    StrongNumericType<V, P>
        Max(const StrongNumericType<V, P> & L, const StrongNumericType<V, P> & R)
    {
        return StrongNumericType<V, P>::Make(Max(L.m_value, R.m_value));
    }

    template <typename V, typename P, typename... Ts>
    StrongNumericType<V, P> Max(const StrongNumericType<V, P> & FIRST, const Ts... OTHERS)
    {
        return Max(FIRST, Max(OTHERS...));
    }

    template <typename V, typename P>
    StrongNumericType<V, P>
        Min(const StrongNumericType<V, P> & L, const StrongNumericType<V, P> & R)
    {
        return StrongNumericType<V, P>::Make(Min(L.m_value, R.m_value));
    }

    template <typename V, typename P, typename... Ts>
    StrongNumericType<V, P> Min(const StrongNumericType<V, P> & FIRST, const Ts... OTHERS)
    {
        return Min(FIRST, Min(OTHERS...));
    }

    namespace helpers
    {
        long long strongTypeRandom(const long long MIN, const long long MAX);
    }

    template <typename V, typename P>
    StrongNumericType<V, P>
        Random(const StrongNumericType<V, P> & MIN, const StrongNumericType<V, P> & MAX)
    {
        return StrongNumericType<V, P>::Make(helpers::strongTypeRandom(
            static_cast<long long>(MIN.m_value), static_cast<long long>(MAX.m_value)));
    }

    template <typename V, typename P>
    StrongNumericType<V, P> Random(const StrongNumericType<V, P> & MAX)
    {
        return StrongNumericType<V, P>::Make(helpers::strongTypeRandom(
            static_cast<long long>(0), static_cast<long long>(MAX.m_value)));
    }

} // namespace misc
} // namespace heroespath

#endif // HEROESPATH_MISC_STRONGNUMERICTYPE_HPP_INCLUDED
