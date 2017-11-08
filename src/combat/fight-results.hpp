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
#ifndef HEROESPATH_COMBAT_FIGHTRESULTS_HPP_INCLUDED
#define HEROESPATH_COMBAT_FIGHTRESULTS_HPP_INCLUDED
//
// fight-results.hpp
//
#include "hit-info.hpp"
#include "creature-effect.hpp"

#include <string>
#include <vector>
#include <utility>


namespace heroespath
{
namespace creature
{
    class Creature;
    using CreaturePtr_t  = Creature *;
    using CreaturePVec_t = std::vector<CreaturePtr_t>;
}
namespace item
{
    class Item;
    using ItemPtr_t = Item *;
}

namespace combat
{

    using CreatureHitInfoPair_t = std::pair<creature::CreaturePtr_t, HitInfo>;
    using CreatureHitInfoVec_t = std::vector<CreatureHitInfoPair_t>;


    //Summarizes many HitInfo objects from a FightResult.
    struct FightResultSummary
    {
        FightResultSummary();

        FightResultSummary(const FightResultSummary &);
        FightResultSummary & operator=(const FightResultSummary &);

        HitType::Enum hit_type;
        song::SongPtr_t song_ptr;
        spell::SpellPtr_t spell_ptr;
        CreatureHitInfoVec_t effected_vec;
        CreatureHitInfoVec_t resisted_vec;
        CreatureHitInfoVec_t already_vec;
        bool areResistedNotEffected_;

        std::size_t PtrCount() const;

        bool IsValid() const;

        const std::string VerbThirdPerson() const;

        const std::string Compose(
            const std::string & FIGHTING_CREATURE_NAME,
            const std::string & VERB_PAST_TENSE) const;
    };


    //Everything required to describe the results of a fight or fight action.
    class FightResult
    {
    public:
        explicit FightResult(const CreatureEffectVec_t & CF_VEC = CreatureEffectVec_t());
        explicit FightResult(const CreatureEffect & CF);

        inline const CreatureEffectVec_t & Effects() const { return creatureEffectVec_; }
        inline std::size_t Count() const { return creatureEffectVec_.size(); }

        const HitInfo GetHitInfo(const std::size_t EFFECT_INDEX = 0,
                                 const std::size_t HIT_INDEX    = 0) const;

        std::size_t EffectedCreatures(creature::CreaturePVec_t &) const;

    private:
        CreatureEffectVec_t creatureEffectVec_;
    };

}
}

#endif //HEROESPATH_COMBAT_FIGHTRESULTS_HPP_INCLUDED
