// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
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
#include "game/strong-types.hpp"
#include "misc/boost-optional-that-throws.hpp"
#include "misc/not-null.hpp"
#include "song/song-enum.hpp"
#include "song/song-type-enum.hpp"

#include <memory>
#include <string>
#include <tuple>
#include <vector>

namespace heroespath
{
namespace creature
{
    class Creature;
    using CreaturePtr_t = misc::NotNull<Creature *>;
} // namespace creature
namespace item
{
    class Item;
    using ItemPtr_t = misc::NotNull<Item *>;
} // namespace item
namespace song
{

    // Responsible for all state and operation of Songs in the game.
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
        bool EffectCreature(
            const creature::CreaturePtr_t CREATURE_PLAYING_PTR,
            const creature::CreaturePtr_t CREATURE_LISTENING_PTR,
            Health_t & healthAdj,
            creature::CondEnumVec_t & condsAddedVec,
            creature::CondEnumVec_t & condsRemovedVec,
            combat::ContentAndNamePos & actionPhraseCNP) const;

        // Allows the spell to change the target item.
        bool EffectItem(const creature::CreaturePtr_t, const item::ItemPtr_t) const;

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

    using SongPtr_t = misc::NotNull<Song *>;
    using SongPtrOPt_t = boost::optional<SongPtr_t>;
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
