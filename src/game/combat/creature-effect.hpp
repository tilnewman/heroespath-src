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
//
// creature-effect.hpp
//
#include "hit-info.hpp"

#include "game/stats/types.hpp"
#include "game/creature/condition-enum.hpp"


namespace game
{
namespace spell
{
    class Spell;
    using SpellPtr_t = Spell *;
}
namespace creature
{
    class Creature;
    using CreaturePtr_t = Creature *;
    using CreaturePVec_t = std::vector<CreaturePtr_t>;
}

namespace combat
{

    //Everything required to describe all the effects a fight had on a creature.
    class CreatureEffect
    {
    public:
        explicit CreatureEffect(
            const creature::CreaturePtr_t        CREATURE_PTR   = nullptr,
            const HitInfoVec_t &                 HIT_INFO_VEC   = HitInfoVec_t(),
            const spell::SpellPtr_t              SPELL_PTR      = nullptr,
            const creature::ConditionEnumVec_t & CONDITIONS_VEC = creature::ConditionEnumVec_t(),
            const bool                           WAS_POUNCED    = false);

        CreatureEffect(const CreatureEffect &);
        CreatureEffect & operator=(const CreatureEffect &);

        inline std::size_t                  GetCount() const           { return hitInfoVec_.size(); }
        inline HitInfoVec_t                 GetHitInfoVec() const      { return hitInfoVec_; }
        inline spell::SpellPtr_t            GetSpell() const           { return spellPtr_; }
        inline creature::CreaturePtr_t      GetCreature() const        { return creaturePtr_; }
        inline bool                         GetWasPounced() const      { return wasPounced_; }

        bool GetWasHit() const;
        stats::Health_t GetDamageTotal() const;

        const creature::ConditionEnumVec_t          GetHitConditions() const;
        inline const creature::ConditionEnumVec_t   GetNonHitConditions() const { return conditionsVec_; }
        const creature::ConditionEnumVec_t          GetAllConditions() const;
        bool                                        ContainsCondition(const creature::Conditions::Enum) const;
        bool                                        RemoveCondition(const creature::Conditions::Enum);

        inline bool WasKill() const { return ContainsCondition(creature::Conditions::Dead); }

        friend bool operator<(const CreatureEffect & L, const CreatureEffect & R);
        friend bool operator==(const CreatureEffect & L, const CreatureEffect & R);

    private:
        HitInfoVec_t hitInfoVec_;
        spell::SpellPtr_t spellPtr_;
        creature::CreaturePtr_t creaturePtr_;
        creature::ConditionEnumVec_t conditionsVec_;
        bool wasPounced_;
    };

    using CreatureEffectVec_t = std::vector<CreatureEffect>;


    bool operator<(const CreatureEffect & L, const CreatureEffect & R);

    bool operator==(const CreatureEffect & L, const CreatureEffect & R);

    inline bool operator!=(const CreatureEffect & L, const CreatureEffect & R)
    {
        return ! (L == R);
    }

}
}
