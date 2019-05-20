// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_CREATURE_SUMMON_INFO_HPP_INCLUDED
#define HEROESPATH_CREATURE_SUMMON_INFO_HPP_INCLUDED
//
// summon-info.hpp
//
#include "creature/race-enum.hpp"

#include <cstddef>
#include <string>

namespace heroespath
{
namespace creature
{

    // Responsible for wrapping all the information about how a magical items can summon creatures.
    class SummonInfo
    {
    public:
        constexpr explicit SummonInfo(
            const origin_type::Enum TYPE = origin_type::Count,
            const race::Enum RACE = creature::race::Count,
            const role::Enum ROLE = creature::role::Count) noexcept
            : type_(TYPE)
            , race_(RACE)
            , role_(ROLE)
            , count_((TYPE < origin_type::Count) ? origin_type::UseCount(TYPE) : 0)
        {}

        constexpr SummonInfo(const SummonInfo &) noexcept = default;
        constexpr SummonInfo(SummonInfo &&) noexcept = default;
        constexpr SummonInfo & operator=(const SummonInfo &) noexcept = default;
        constexpr SummonInfo & operator=(SummonInfo &&) noexcept = default;

        constexpr origin_type::Enum Type() const noexcept { return type_; }
        constexpr race::Enum Race() const noexcept { return race_; }
        constexpr role::Enum Role() const noexcept { return role_; }
        constexpr std::size_t Count() const noexcept { return count_; }

        constexpr bool IsValid() const noexcept
        {
            return !(
                (type_ >= origin_type::Count) || (race_ >= race::Count) || (role_ >= role::Count));
        }

        constexpr bool IsDefaultAndCompletelyInvalid() const noexcept
        {
            return (!IsValid() && (0 == count_));
        }

        constexpr bool CanSummon() const noexcept { return (IsValid() && (count_ > 0)); }

        const std::string ToString() const
        {
            std::string str;
            str.reserve(255);
            str += "SummonInfo{";

            if (IsDefaultAndCompletelyInvalid())
            {
                str += "None}";
            }
            else
            {
                str += "SummonInfo{type=";
                str += NAMEOF_ENUM(type_);
                str += ", race=";
                str += NAMEOF_ENUM(race_);
                str += ", role=";
                str += NAMEOF_ENUM(role_);
                str += ", charges_remaining=";
                str += std::to_string(count_);
                str += "}";
            }

            return str;
        }

        friend constexpr bool operator==(const SummonInfo & L, const SummonInfo & R) noexcept;
        friend constexpr bool operator<(const SummonInfo & L, const SummonInfo & R) noexcept;

    private:
        origin_type::Enum type_;
        race::Enum race_;
        role::Enum role_;
        std::size_t count_;

    private:
        friend class boost::serialization::access;
        template <typename Archive>
        void serialize(Archive & ar, const unsigned int)
        {
            ar & type_;
            ar & race_;
            ar & role_;
            ar & count_;
        }
    };

    constexpr bool operator==(const SummonInfo & L, const SummonInfo & R) noexcept
    {
        return !(
            (L.type_ != R.type_) || (L.race_ != R.race_) || (L.role_ != R.role_)
            || (L.count_ != R.count_));
    }

    constexpr bool operator!=(const SummonInfo & L, const SummonInfo & R) noexcept
    {
        return !(L == R);
    }

    constexpr bool operator<(const SummonInfo & L, const SummonInfo & R) noexcept
    {
        return (
            std::tie(L.type_, L.race_, L.role_, L.count_)
            < std::tie(R.type_, R.race_, R.role_, R.count_));
    }

} // namespace creature
} // namespace heroespath

#endif // HEROESPATH_CREATURE_SUMMON_INFO_HPP_INCLUDED
