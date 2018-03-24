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
#ifndef HEROESPATH_SONG_SONG_HPP_INCLUDED
#define HEROESPATH_SONG_SONG_HPP_INCLUDED
//
// song.hpp
//
#include "combat/effect-type-enum.hpp"
#include "combat/name-position-enum.hpp"
#include "combat/target-enum.hpp"
#include "creature/condition-enum.hpp"
#include "game/phase-enum.hpp"
#include "misc/types.hpp"
#include "song/song-enum.hpp"
#include "song/song-type-enum.hpp"

#include <memory>
#include <string>
#include <tuple>
#include <vector>

namespace heroespath
{

// forward declarations
namespace creature
{
    class Creature;
    using CreaturePtr_t = Creature *;
} // namespace creature

namespace item
{
    class Item;
    using ItemPtr_t = Item *;
} // namespace item

namespace song
{

    // common base code to all song classes
    class Song
    {
    public:
        Song(
            const Songs::Enum WHICH,
            const SongType::Enum SONG_TYPE,
            const combat::EffectType::Enum EFFECT_TYPE,
            const game::Phase::Enum VALID_PHASES,
            const Mana_t & MANA_COST,
            const Rank_t & RANK,
            const combat::TargetType::Enum TARGET_TYPE,
            const std::string & VERB_THIRD_PERSON,
            const std::string & VERB_PAST_TENSE);

        virtual ~Song();

        const std::string Name() const { return Songs::Name(which_); }

        const std::string ToString() const;

        const std::string VerbThirdPerson() const { return verbThirdPerson_; }
        const std::string VerbPastTense() const { return verbPastTense_; }

        const std::string Desc() const { return Songs::ShortDesc(which_); }
        const std::string DescExtra() const { return Songs::ExtraDesc(which_); }
        const std::string DescDetails() const;
        const std::string DescComplete() const;

        Songs::Enum Which() const { return which_; }
        SongType::Enum Type() const { return type_; }
        combat::EffectType::Enum Effect() const { return effectType_; }
        game::Phase::Enum ValidPhases() const { return validPhases_; }
        Mana_t ManaCost() const { return manaCost_; }
        Rank_t Rank() const { return rank_; }
        combat::TargetType::Enum Target() const { return targetType_; }

        // Allows the spell to change the target creature.
        virtual bool EffectCreature(
            creature::CreaturePtr_t creaturePlayingPtr,
            creature::CreaturePtr_t creatureListeningPtr,
            Health_t & healthAdj,
            creature::CondEnumVec_t & condsAddedVec,
            creature::CondEnumVec_t & condsRemovedVec,
            combat::ContentAndNamePos & actionPhraseCNP) const = 0;

        // Allows the spell to change the target item.
        virtual bool EffectItem(creature::CreaturePtr_t, item::ItemPtr_t) const;

        const std::string ActionPhrasePreamble() const;
        const std::string TypeToVerb() const;
        const std::string TypeToNoun() const;

        friend bool operator<(const Song & L, const Song & R);
        friend bool operator==(const Song & L, const Song & R);

    public:
        static const std::string RESISTED_STR_;
        static const std::string FAILED_STR_;

    protected:
        Songs::Enum which_;
        SongType::Enum type_;
        combat::EffectType::Enum effectType_;
        Rank_t rank_;
        game::Phase::Enum validPhases_;
        Mana_t manaCost_;
        combat::TargetType::Enum targetType_;
        std::string verbThirdPerson_;
        std::string verbPastTense_;
    };

    using SongPtr_t = Song *;
    using SongPVec_t = std::vector<SongPtr_t>;

    inline bool operator<(const Song & L, const Song & R)
    {
        return std::tie(
                   L.which_,
                   L.effectType_,
                   L.effectType_,
                   L.rank_,
                   L.validPhases_,
                   L.manaCost_,
                   L.targetType_,
                   L.verbThirdPerson_,
                   L.verbPastTense_)
            < std::tie(
                   R.which_,
                   R.type_,
                   R.effectType_,
                   R.rank_,
                   R.validPhases_,
                   R.manaCost_,
                   R.targetType_,
                   R.verbThirdPerson_,
                   R.verbPastTense_);
    }

    inline bool operator==(const Song & L, const Song & R)
    {
        return std::tie(
                   L.which_,
                   L.type_,
                   L.effectType_,
                   L.rank_,
                   L.validPhases_,
                   L.manaCost_,
                   L.targetType_,
                   L.verbThirdPerson_,
                   L.verbPastTense_)
            == std::tie(
                   R.which_,
                   R.type_,
                   R.effectType_,
                   R.rank_,
                   R.validPhases_,
                   R.manaCost_,
                   R.targetType_,
                   R.verbThirdPerson_,
                   R.verbPastTense_);
    }

    inline bool operator!=(const Song & L, const Song & R) { return !(L == R); }
} // namespace song
} // namespace heroespath

#endif // HEROESPATH_SONG_SONG_HPP_INCLUDED
