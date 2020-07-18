// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_CREATURE_TRAITS_SET_HPP_INCLUDED
#define HEROESPATH_CREATURE_TRAITS_SET_HPP_INCLUDED
//
// traits-set.hpp
//
#include "game/strong-types.hpp"
#include "misc/boost-serialize-includes.hpp"
#include "trait-enum.hpp"
#include "trait.hpp"

#include <string>
#include <vector>

namespace heroespath
{
namespace creature
{

    class TraitSet
    {
    public:
        explicit TraitSet(const TraitValueVec_t & TRAITS_VEC = TraitValueVec_t());

        Trait & Get(const Traits::Enum);
        const Trait & GetCopy(const Traits::Enum ENUM) const;

        const std::string ToString(
            const bool WILL_WRAP,
            const bool WILL_ABBR,
            const bool WILL_PREVENT_NEGATIVE,
            const bool WILL_PREFIX_PERCENT) const;

        const std::string StatsString(const bool WILL_WRAP) const;

        Score_t Score() const;

        friend bool operator==(const TraitSet & L, const TraitSet & R);
        friend bool operator<(const TraitSet & L, const TraitSet & R);

    private:
        const std::string StatStringHelper(const Traits::Enum, const bool WILL_PREFIX = true) const;

    private:
        TraitVec_t traitVec_;

    private:
        friend class boost::serialization::access;
        template <typename Archive>
        void serialize(Archive & ar, const unsigned int)
        {
            ar & traitVec_;
        }
    };

    inline bool operator==(const TraitSet & L, const TraitSet & R)
    {
        return (L.traitVec_ == R.traitVec_);
    }

    inline bool operator!=(const TraitSet & L, const TraitSet & R) { return !(L == R); }

    inline bool operator<(const TraitSet & L, const TraitSet & R)
    {
        return (L.traitVec_ < R.traitVec_);
    }

} // namespace creature
} // namespace heroespath

#endif // HEROESPATH_CREATURE_TRAITS_SET_HPP_INCLUDED
