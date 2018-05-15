// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_GAME_COMBAT_CREATUREEFFEC_HPP_INCLUDED
#define HEROESPATH_GAME_COMBAT_CREATUREEFFEC_HPP_INCLUDED
//
// creature-effect.hpp
//
#include "creature/condition-enum.hpp"
#include "hit-info.hpp"
#include "misc/not-null.hpp"
#include "stats/trait.hpp"

#include <vector>

namespace heroespath
{
namespace creature
{
    class Creature;
    using CreaturePtr_t = misc::NotNull<Creature *>;
} // namespace creature

namespace combat
{

    // Everything required to describe all the effects a fight had on a creature.
    class CreatureEffect
    {
    public:
        explicit CreatureEffect(
            const creature::CreaturePtr_t CREATURE_PTR,
            const HitInfoVec_t & HIT_INFO_VEC = HitInfoVec_t());

        std::size_t GetCount() const { return hitInfoVec_.size(); }
        const HitInfoVec_t & GetHitInfoVec() const { return hitInfoVec_; }
        const creature::CreaturePtr_t GetCreature() const { return creaturePtr_; }

        bool GetWasHit() const;

        Health_t GetDamageTotal() const;

        const creature::CondEnumVec_t GetAllCondsAdded() const;
        const creature::CondEnumVec_t GetAllCondsRemoved() const;

        bool GetAllCondsAddedContains(const creature::Conditions::Enum) const;
        bool GetAllCondsRemovedContains(const creature::Conditions::Enum) const;

        bool AllCondsAddedRemove(const creature::Conditions::Enum);
        bool AllCondsRemovedRemove(const creature::Conditions::Enum);

        bool WasKill() const { return GetAllCondsAddedContains(creature::Conditions::Dead); }

        friend bool operator<(const CreatureEffect & L, const CreatureEffect & R);
        friend bool operator==(const CreatureEffect & L, const CreatureEffect & R);

    private:
        HitInfoVec_t hitInfoVec_;
        creature::CreaturePtr_t creaturePtr_;
    };

    using CreatureEffectVec_t = std::vector<CreatureEffect>;

    bool operator<(const CreatureEffect & L, const CreatureEffect & R);

    bool operator==(const CreatureEffect & L, const CreatureEffect & R);

    inline bool operator!=(const CreatureEffect & L, const CreatureEffect & R) { return !(L == R); }

} // namespace combat
} // namespace heroespath

#endif // HEROESPATH_GAME_COMBAT_CREATUREEFFEC_HPP_INCLUDED
