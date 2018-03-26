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
#include "creature-effect.hpp"
#include "hit-info.hpp"
#include "misc/boost-optional-that-throws.hpp"
#include "misc/not-null.hpp"

#include <string>
#include <utility>
#include <vector>

namespace heroespath
{
namespace creature
{
    class Creature;
    using CreaturePtr_t = Creature *;
    using CreaturePVec_t = std::vector<CreaturePtr_t>;
} // namespace creature
namespace item
{
    class Item;
    using ItemPtr_t = Item *;
} // namespace item
namespace spell
{
    class Spell;
    using SpellPtr_t = misc::NotNull<Spell *>;
    using SpellPtrOpt_t = boost::optional<SpellPtr_t>;
}
namespace song
{
    class Song;
    using SongPtr_t = misc::NotNull<Song *>;
    using SongPtrOpt_t = boost::optional<SongPtr_t>;
}
namespace combat
{

    using CreatureHitInfoPair_t = std::pair<creature::CreaturePtr_t, HitInfo>;
    using CreatureHitInfoVec_t = std::vector<CreatureHitInfoPair_t>;

    // Summarizes many HitInfo objects from a FightResult.
    struct FightResultSummary
    {
        FightResultSummary();

        HitType::Enum hit_type;
        song::SongPtrOpt_t song_ptr_opt;
        spell::SpellPtrOpt_t spell_ptr_opt;
        CreatureHitInfoVec_t effected_vec;
        CreatureHitInfoVec_t resisted_vec;
        CreatureHitInfoVec_t already_vec;
        bool areResistedNotEffected_;

        std::size_t PtrCount() const;

        bool IsValid() const;

        const std::string VerbThirdPerson() const;

        const std::string Compose(
            const std::string & FIGHTING_CREATURE_NAME, const std::string & VERB_PAST_TENSE) const;
    };

    // Everything required to describe the results of a fight or fight action.
    class FightResult
    {
    public:
        explicit FightResult(const CreatureEffectVec_t & CF_VEC = CreatureEffectVec_t());
        explicit FightResult(const CreatureEffect & CF);

        const CreatureEffectVec_t & Effects() const { return creatureEffectVec_; }
        std::size_t Count() const { return creatureEffectVec_.size(); }

        const HitInfo
            GetHitInfo(const std::size_t EFFECT_INDEX = 0, const std::size_t HIT_INDEX = 0) const;

        std::size_t EffectedCreatures(creature::CreaturePVec_t &) const;

    private:
        CreatureEffectVec_t creatureEffectVec_;
    };

} // namespace combat
} // namespace heroespath

#endif // HEROESPATH_COMBAT_FIGHTRESULTS_HPP_INCLUDED
