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
        typename Tag_t,
        typename = std::enable_if_t<are_integral_nobool_v<Value_t>>>
    class StrongNumericType
    {
        constexpr explicit StrongNumericType(const Value_t NUMBER) noexcept
            : m_value(NUMBER)
        {}

    public:
        using value_type = Value_t;
        using tag_type = Tag_t;

        constexpr StrongNumericType() noexcept
            : m_value(Value_t(0))
        {}

        constexpr StrongNumericType(const StrongNumericType &) noexcept = default;
        constexpr StrongNumericType(StrongNumericType &&) noexcept = default;
        constexpr StrongNumericType & operator=(const StrongNumericType &) noexcept = default;
        constexpr StrongNumericType & operator=(StrongNumericType &&) noexcept = default;

        constexpr Value_t Get() const noexcept { return m_value; }

        template <typename New_t>
        constexpr std::enable_if_t<(are_arithmetic_nobool_v<New_t> || are_enum_v<New_t>), New_t>
            GetAs() const noexcept
        {
            return static_cast<New_t>(this->m_value);
        }

        template <typename NewStrongType_t>
        constexpr std::enable_if_t<
            (are_same_v<
                 Value_t,
                 typename NewStrongType_t::
                     value_type> && !are_same_v<Tag_t, typename NewStrongType_t::tag_type>),
            NewStrongType_t>
            GetAs() const noexcept
        {
            return NewStrongType_t::Make(this->m_value);
        }

        constexpr creature::Trait_t GetAsTrait() const noexcept
        {
            return static_cast<creature::Trait_t>(this->m_value);
        }

        constexpr bool IsZero() const noexcept { return (this->m_value == 0); }
        constexpr bool IsZeroOrMore() const noexcept { return (this->m_value >= 0); }
        constexpr bool IsNegative() const noexcept { return (this->m_value < 0); }

        template <typename Real_t, typename = std::enable_if_t<are_floating_point_v<Real_t>>>
        constexpr void Scale(const Real_t SCALE) noexcept
        {
            this->m_value = static_cast<Value_t>(static_cast<Real_t>(this->m_value) * SCALE);
        }

        template <typename Real_t, typename = std::enable_if_t<are_floating_point_v<Real_t>>>
        constexpr StrongNumericType ScaleCopy(const Real_t SCALE) const noexcept
        {
            auto temp(*this);
            temp.Scale(SCALE);
            return temp;
        }

        const std::string ToString() const { return std::to_string(this->m_value); }

        template <typename Prop_t>
        static constexpr std::enable_if_t<
            (are_arithmetic_nobool_v<Prop_t> || are_enum_v<Prop_t>),
            const StrongNumericType>
            Make(const Prop_t X) noexcept
        {
            return StrongNumericType(static_cast<Value_t>(X));
        }

        constexpr StrongNumericType MakePositiveCopy() const noexcept
        {
            return StrongNumericType(Abs(this->m_value));
        }

        constexpr StrongNumericType MakeNegativeCopy() const noexcept
        {
            static_assert(are_signed_v<Value_t>, "unsigned types forbidden");
            return StrongNumericType(-Abs(this->m_value));
        }

        constexpr void MakePositive() noexcept { this->m_value = Abs(this->m_value); }
        constexpr void MakeNegative() noexcept { this->m_value = -Abs(this->m_value); }

        constexpr StrongNumericType ChangeSignCopy() const noexcept
        {
            static_assert(are_signed_v<Value_t>, "unsigned types forbidden");
            return StrongNumericType(-(this->m_value));
        }

        constexpr void ChangeSign() noexcept { this->m_value = -this->m_value; }

        constexpr StrongNumericType & operator+=(const StrongNumericType & RHS) noexcept
        {
            this->m_value += RHS.m_value;
            return *this;
        }

        constexpr StrongNumericType & operator-=(const StrongNumericType & RHS) noexcept
        {
            this->m_value -= RHS.m_value;
            return *this;
        }

        constexpr StrongNumericType & operator*=(const StrongNumericType & RHS) noexcept
        {
            this->m_value *= RHS.m_value;
            return *this;
        }

        // NOTE: divide or mod by zero equals zero
        constexpr StrongNumericType & operator/=(const StrongNumericType & RHS) noexcept
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
        constexpr StrongNumericType & operator%=(const StrongNumericType & RHS) noexcept
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

        constexpr StrongNumericType & operator++() noexcept
        {
            ++(this->m_value);
            return *this;
        }

        constexpr StrongNumericType operator++(int) noexcept
        {
            StrongNumericType temp(*this);
            operator++();
            return temp;
        }

        constexpr StrongNumericType & operator--() noexcept
        {
            --(this->m_value);
            return *this;
        }

        constexpr StrongNumericType operator--(int) noexcept
        {
            StrongNumericType temp(*this);
            operator--();
            return temp;
        }

        template <typename V, typename P>
        friend std::ostream & operator<<(std::ostream & os, const StrongNumericType<V, P> & R);

        template <typename V, typename P>
        friend constexpr bool operator==(
            const StrongNumericType<V, P> & L, const StrongNumericType<V, P> & R) noexcept;

        template <typename V, typename P>
        friend constexpr bool operator<(
            const StrongNumericType<V, P> & L, const StrongNumericType<V, P> & R) noexcept;

        template <typename V, typename P>
        friend constexpr StrongNumericType<V, P>
            Max(const StrongNumericType<V, P> & L, const StrongNumericType<V, P> & R) noexcept;

        template <typename V, typename P>
        friend constexpr StrongNumericType<V, P>
            Min(const StrongNumericType<V, P> & L, const StrongNumericType<V, P> & R) noexcept;

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
    constexpr bool
        operator==(const StrongNumericType<V, P> & L, const StrongNumericType<V, P> & R) noexcept
    {
        return (L.m_value == R.m_value);
    }

    template <typename V, typename P>
    constexpr bool
        operator!=(const StrongNumericType<V, P> & L, const StrongNumericType<V, P> & R) noexcept
    {
        return !(L == R);
    }

    template <typename V, typename P>
    constexpr bool
        operator<(const StrongNumericType<V, P> & L, const StrongNumericType<V, P> & R) noexcept
    {
        return (L.m_value < R.m_value);
    }

    template <typename V, typename P>
    constexpr bool
        operator>(const StrongNumericType<V, P> & L, const StrongNumericType<V, P> & R) noexcept
    {
        return (R < L);
    }

    template <typename V, typename P>
    constexpr bool
        operator<=(const StrongNumericType<V, P> & L, const StrongNumericType<V, P> & R) noexcept
    {
        return !(L > R);
    }

    template <typename V, typename P>
    constexpr bool
        operator>=(const StrongNumericType<V, P> & L, const StrongNumericType<V, P> & R) noexcept
    {
        return !(L < R);
    }

    template <typename V, typename P>
    constexpr StrongNumericType<V, P>
        operator+(const StrongNumericType<V, P> & L, const StrongNumericType<V, P> & R) noexcept
    {
        return (StrongNumericType<V, P>(L) += R);
    }

    template <typename V, typename P>
    constexpr StrongNumericType<V, P>
        operator-(const StrongNumericType<V, P> & L, const StrongNumericType<V, P> & R) noexcept
    {
        return (StrongNumericType<V, P>(L) -= R);
    }

    template <typename V, typename P>
    constexpr StrongNumericType<V, P>
        operator*(const StrongNumericType<V, P> & L, const StrongNumericType<V, P> & R) noexcept
    {
        return (StrongNumericType<V, P>(L) *= R);
    }

    // NOTE: divide or mod by zero equals zero
    template <typename V, typename P>
    constexpr StrongNumericType<V, P>
        operator/(const StrongNumericType<V, P> & L, const StrongNumericType<V, P> & R) noexcept
    {
        return (StrongNumericType<V, P>(L) /= R);
    }

    // NOTE: divide or mod by zero equals zero
    template <typename V, typename P>
    constexpr StrongNumericType<V, P>
        operator%(const StrongNumericType<V, P> & L, const StrongNumericType<V, P> & R) noexcept
    {
        return (StrongNumericType<V, P>(L) %= R);
    }

    // note how this NEVER changes the sign
    template <typename V, typename P>
    constexpr StrongNumericType<V, P> operator+(const StrongNumericType<V, P> & X) noexcept
    {
        return X;
    }

    // note how this ALWAYS changes the sign
    template <typename V, typename P>
    constexpr StrongNumericType<V, P> operator-(const StrongNumericType<V, P> & X) noexcept
    {
        static_assert(are_signed_v<V>, "unsigned types forbidden");
        return X.ChangeSignCopy();
    }

    // strong numeric type utils

    template <typename V, typename P>
    constexpr StrongNumericType<V, P> Abs(const StrongNumericType<V, P> & X) noexcept
    {
        return X.MakePositiveCopy();
    }

    template <typename V, typename P>
    constexpr StrongNumericType<V, P>
        Max(const StrongNumericType<V, P> & L, const StrongNumericType<V, P> & R) noexcept
    {
        return StrongNumericType<V, P>::Make(Max(L.m_value, R.m_value));
    }

    template <typename V, typename P, typename... Ts>
    constexpr StrongNumericType<V, P>
        Max(const StrongNumericType<V, P> & FIRST, const Ts... OTHERS) noexcept
    {
        return Max(FIRST, Max(OTHERS...));
    }

    template <typename V, typename P>
    constexpr StrongNumericType<V, P>
        Min(const StrongNumericType<V, P> & L, const StrongNumericType<V, P> & R) noexcept
    {
        return StrongNumericType<V, P>::Make(Min(L.m_value, R.m_value));
    }

    template <typename V, typename P, typename... Ts>
    constexpr StrongNumericType<V, P>
        Min(const StrongNumericType<V, P> & FIRST, const Ts... OTHERS) noexcept
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
