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
#ifndef HEROESPATH_SONG_SONGS_HPP_INCLUDED
#define HEROESPATH_SONG_SONGS_HPP_INCLUDED
//
// songs.hpp
//
#include "combat/name-position-enum.hpp"
#include "combat/target-enum.hpp"
#include "creature/condition.hpp"
#include "game/phase-enum.hpp"
#include "misc/types.hpp"
#include "song/song-enum.hpp"
#include "song/song-type-enum.hpp"
#include "song/song.hpp"
#include "stats/trait.hpp"

#include <string>

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

    class RallyDrum : public Song
    {
    public:
        RallyDrum()
            : Song(
                  Songs::RallyDrum,
                  SongType::Drum,
                  combat::EffectType::CreatureHelpMisc,
                  static_cast<game::Phase::Enum>(game::Phase::Combat | game::Phase::Inventory),
                  1_mana,
                  1_rank,
                  combat::TargetType::AllCompanions,
                  "emboldens",
                  "emboldened")
        {}

        virtual ~RallyDrum() {}

        virtual bool EffectCreature(
            creature::CreaturePtr_t creaturePlayingPtr,
            creature::CreaturePtr_t creatureListeningPtr,
            Health_t & healthAdj,
            creature::CondEnumVec_t & condsAddedVec,
            creature::CondEnumVec_t & condsRemovedVec,
            combat::ContentAndNamePos & actionPhraseCNP) const;
    };

    class SpiritResonance : public Song
    {
    public:
        SpiritResonance()
            : Song(
                  Songs::SpiritResonance,
                  SongType::Guitar,
                  combat::EffectType::CreatureHelpMisc,
                  static_cast<game::Phase::Enum>(game::Phase::Combat | game::Phase::Inventory),
                  5_mana,
                  1_rank,
                  combat::TargetType::AllCompanions,
                  "recharges",
                  "recharged")
        {}

        virtual ~SpiritResonance() {}

        virtual bool EffectCreature(
            creature::CreaturePtr_t creaturePlayingPtr,
            creature::CreaturePtr_t creatureListeningPtr,
            Health_t & healthAdj,
            creature::CondEnumVec_t & condsAddedVec,
            creature::CondEnumVec_t & condsRemovedVec,
            combat::ContentAndNamePos & actionPhraseCNP) const;
    };

    class RousingRhythm : public Song
    {
    public:
        RousingRhythm()
            : Song(
                  Songs::RousingRhythm,
                  SongType::Drum,
                  combat::EffectType::CreatureHelpMisc,
                  static_cast<game::Phase::Enum>(game::Phase::Combat | game::Phase::Inventory),
                  3_mana,
                  1_rank,
                  combat::TargetType::AllCompanions,
                  "rouses",
                  "roused")
        {}

        virtual ~RousingRhythm() {}

        virtual bool EffectCreature(
            creature::CreaturePtr_t creaturePlayingPtr,
            creature::CreaturePtr_t creatureListeningPtr,
            Health_t & healthAdj,
            creature::CondEnumVec_t & condsAddedVec,
            creature::CondEnumVec_t & condsRemovedVec,
            combat::ContentAndNamePos & actionPhraseCNP) const;
    };

    class TripBeat : public Song
    {
    public:
        TripBeat()
            : Song(
                  Songs::TripBeat,
                  SongType::Drum,
                  combat::EffectType::CreatureHarmMisc,
                  game::Phase::Combat,
                  1_mana,
                  1_rank,
                  combat::TargetType::AllOpponents,
                  "trips",
                  "tripped")
        {}

        virtual ~TripBeat() {}

        virtual bool EffectCreature(
            creature::CreaturePtr_t creaturePlayingPtr,
            creature::CreaturePtr_t creatureListeningPtr,
            Health_t & healthAdj,
            creature::CondEnumVec_t & condsAddedVec,
            creature::CondEnumVec_t & condsRemovedVec,
            combat::ContentAndNamePos & actionPhraseCNP) const;
    };

    class PanicStrings : public Song
    {
    public:
        PanicStrings()
            : Song(
                  Songs::PanicStrings,
                  SongType::Guitar,
                  combat::EffectType::CreatureHarmMisc,
                  game::Phase::Combat,
                  2_mana,
                  1_rank,
                  combat::TargetType::AllOpponents,
                  "panics",
                  "panicked")
        {}

        virtual ~PanicStrings() {}

        virtual bool EffectCreature(
            creature::CreaturePtr_t creaturePlayingPtr,
            creature::CreaturePtr_t creatureListeningPtr,
            Health_t & healthAdj,
            creature::CondEnumVec_t & condsAddedVec,
            creature::CondEnumVec_t & condsRemovedVec,
            combat::ContentAndNamePos & actionPhraseCNP) const;
    };

    class Lullaby : public Song
    {
    public:
        Lullaby()
            : Song(
                  Songs::Lullaby,
                  SongType::Guitar,
                  combat::EffectType::CreatureHarmMisc,
                  game::Phase::Combat,
                  2_mana,
                  1_rank,
                  combat::TargetType::AllOpponents,
                  "sleeps",
                  "slept")
        {}

        virtual ~Lullaby() {}

        virtual bool EffectCreature(
            creature::CreaturePtr_t creaturePlayingPtr,
            creature::CreaturePtr_t creatureListeningPtr,
            Health_t & healthAdj,
            creature::CondEnumVec_t & condsAddedVec,
            creature::CondEnumVec_t & condsRemovedVec,
            combat::ContentAndNamePos & actionPhraseCNP) const;
    };
} // namespace song
} // namespace heroespath

#endif // HEROESPATH_SONG_SONGS_HPP_INCLUDED
