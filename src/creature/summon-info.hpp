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

#include <cstddef> //for std::size_t
#include <string>

namespace heroespath
{
namespace creature
{

    // Responsible for wrapping all the information about how a magical items can summon creatures.
    class SummonInfo
    {
    public:
        explicit SummonInfo(
            const origin_type::Enum TYPE = origin_type::Count,
            const race::Enum RACE = creature::race::Count,
            const role::Enum ROLE = creature::role::Count);

        origin_type::Enum Type() const { return type_; }

        race::Enum Race() const { return race_; }

        role::Enum Role() const { return role_; }

        std::size_t Count() const { return count_; }

        bool IsValid() const
        {
            return (
                (type_ != origin_type::Count) && (race_ != race::Count) && (role_ != role::Count));
        }

        bool CanSummon() const { return (IsValid() && (count_ > 0)); }

        bool IsDefaultAndCompletelyInvalid() const { return (*this == SummonInfo()); }

        const std::string ToString() const;

        friend bool operator==(const SummonInfo & L, const SummonInfo & R);
        friend bool operator<(const SummonInfo & L, const SummonInfo & R);

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

    inline bool operator==(const SummonInfo & L, const SummonInfo & R)
    {
        return (
            std::tie(L.type_, L.race_, L.role_, L.count_)
            == std::tie(R.type_, R.race_, R.role_, R.count_));
    }

    inline bool operator!=(const SummonInfo & L, const SummonInfo & R) { return !(L == R); }

    inline bool operator<(const SummonInfo & L, const SummonInfo & R)
    {
        return (
            std::tie(L.type_, L.race_, L.role_, L.count_)
            < std::tie(R.type_, R.race_, R.role_, R.count_));
    }

} // namespace creature
} // namespace heroespath

#endif // HEROESPATH_CREATURE_SUMMON_INFO_HPP_INCLUDED
