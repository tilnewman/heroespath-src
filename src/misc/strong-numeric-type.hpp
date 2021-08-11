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
#include "misc/real.hpp"
#include "misc/strong-type.hpp"

#include <ostream>

namespace heroespath
{
namespace misc
{

    // Responsible for wrapping StrongType with functions useful for numeric types.
    template <typename T, typename Parameter_t>
    struct StrongNumericType : public StrongType<T, Parameter_t>
    {
        explicit StrongNumericType(const T & VALUE = T(0))
            : StrongType<T, Parameter_t>(VALUE)
        {}

        virtual ~StrongNumericType() = default;

        template <typename From_t>
        static StrongNumericType<T, Parameter_t> Make(const From_t FROM_VALUE)
        {
            return StrongNumericType<T, Parameter_t>(static_cast<T>(FROM_VALUE));
        }

        template <typename From_t, typename Phantom_t>
        static StrongNumericType<T, Parameter_t>
            Make(const StrongNumericType<From_t, Phantom_t> FROM_STRONG_NUMERIC_TYPE)
        {
            return StrongNumericType<T, Parameter_t>(FROM_STRONG_NUMERIC_TYPE.template As<T>());
        }

        template <typename AsType_t>
        constexpr AsType_t As() const
        {
            return static_cast<AsType_t>(this->m_value);
        }

        constexpr bool IsZero() const { return misc::IsRealZero(this->m_value); }

        constexpr bool IsNonZero() const { return !IsZero(); }

        const StrongNumericType Abs() const
        {
            return StrongNumericType<T, Parameter_t>(std::abs(this->m_value));
        }

        StrongNumericType & operator=(const StrongNumericType & RHS)
        {
            this->m_value = RHS.m_value;
            return *this;
        }

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

        StrongNumericType & operator/=(const StrongNumericType & RHS)
        {
            this->m_value /= RHS.m_value;
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
            return this->m_value == RHS.m_value;
        }

        inline bool operator!=(const StrongNumericType & RHS) const
        {
            return this->m_value != RHS.m_value;
        }

        inline bool operator<(const StrongNumericType & RHS) const
        {
            return this->m_value < RHS.m_value;
        }

        inline bool operator<=(const StrongNumericType & RHS) const
        {
            return this->m_value <= RHS.m_value;
        }

        inline bool operator>(const StrongNumericType & RHS) const
        {
            return this->m_value > RHS.m_value;
        }

        inline bool operator>=(const StrongNumericType & RHS) const
        {
            return this->m_value >= RHS.m_value;
        }

        StrongNumericType & operator++()
        {
            ++(this->m_value);
            return *this;
        }

        StrongNumericType operator++(int)
        {
            StrongNumericType temp { *this };
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
            StrongNumericType temp { *this };
            operator--();
            return temp;
        }

    private:
        friend class boost::serialization::access;
        template <typename Archive>
        void serialize(Archive & ar, const unsigned int)
        {
            ar & boost::serialization::base_object<StrongType<T, Parameter_t>>(*this);
        }
    };

    template <typename T, typename Parameter_t>
    std::ostream & operator<<(std::ostream & os, const StrongNumericType<T, Parameter_t> & RHS)
    {
        os << RHS.Get();
        return os;
    }

    template <typename T, typename Parameter_t>
    inline bool operator==(const int LEFT_INT, const StrongNumericType<T, Parameter_t> & RIGHT_SNT)
    {
        return (LEFT_INT == RIGHT_SNT.template As<int>());
    }

    template <typename T, typename Parameter_t>
    inline bool operator==(const StrongNumericType<T, Parameter_t> & LEFT_SNT, const int RIGHT_INT)
    {
        return (LEFT_SNT.template As<int>() == RIGHT_INT);
    }

    template <typename T, typename Parameter_t>
    inline bool operator!=(const int LEFT_INT, const StrongNumericType<T, Parameter_t> & RIGHT_SNT)
    {
        return (LEFT_INT != RIGHT_SNT.template As<int>());
    }

    template <typename T, typename Parameter_t>
    inline bool operator!=(const StrongNumericType<T, Parameter_t> & LEFT_SNT, const int RIGHT_INT)
    {
        return (LEFT_SNT.template As<int>() != RIGHT_INT);
    }
} // namespace misc
} // namespace heroespath

#endif // HEROESPATH_MISC_STRONGNUMERICTYPE_HPP_INCLUDED
