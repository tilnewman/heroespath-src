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
#ifndef GAME_SONG_SONGS_HPP_INCLUDED
#define GAME_SONG_SONGS_HPP_INCLUDED
//
// songs.hpp
//
#include "game/song/song-enum.hpp"
#include "game/song/song-type-enum.hpp"
#include "game/song/song.hpp"
#include "game/stats/types.hpp"
#include "game/phase-enum.hpp"
#include "game/target-enum.hpp"
#include "game/creature/condition.hpp"
#include "game/name-position-enum.hpp"

#include <string>


namespace game
{

//forward declarations
namespace creature
{
    class Creature;
    using CreaturePtr_t = Creature *;
}

namespace item
{
    class Item;
    using ItemPtr_t = Item *;
}

namespace song
{

    class RallyDrum : public Song
    {
    public:
        RallyDrum() : Song(Songs::RallyDrum,
                           SongType::Drum,
                           EffectType::CreatureHelpMisc,
                           static_cast<Phase::Enum>(Phase::Combat | Phase::Inventory),
                           1,
                           1,
                           TargetType::AllCompanions)
        {}

        virtual ~RallyDrum() {}

        virtual bool EffectCreature(
            creature::CreaturePtr_t   creaturePlayingPtr,
            creature::CreaturePtr_t   creatureListeningPtr,
            stats::Health_t &         healthAdj,
            creature::CondEnumVec_t & condsAddedVec,
            creature::CondEnumVec_t & condsRemovedVec,
            ContentAndNamePos &       actionPhraseCNP) const;
    };


    class SpiritResonance : public Song
    {
    public:
        SpiritResonance() : Song(Songs::SpiritResonance,
                                 SongType::Guitar,
                                 EffectType::CreatureHelpMisc,
                                 static_cast<Phase::Enum>(Phase::Combat | Phase::Inventory),
                                 5,
                                 1,
                                 TargetType::AllCompanions)
        {}

        virtual ~SpiritResonance() {}

        virtual bool EffectCreature(
            creature::CreaturePtr_t   creaturePlayingPtr,
            creature::CreaturePtr_t   creatureListeningPtr,
            stats::Health_t &         healthAdj,
            creature::CondEnumVec_t & condsAddedVec,
            creature::CondEnumVec_t & condsRemovedVec,
            ContentAndNamePos &       actionPhraseCNP) const;
    };


    class RousingRhythm : public Song
    {
    public:
        RousingRhythm() : Song(Songs::RousingRhythm,
                               SongType::Drum,
                               EffectType::CreatureHelpMisc,
                               static_cast<Phase::Enum>(Phase::Combat | Phase::Inventory),
                               3,
                               1,
                               TargetType::AllCompanions)
        {}

        virtual ~RousingRhythm() {}

        virtual bool EffectCreature(
            creature::CreaturePtr_t   creaturePlayingPtr,
            creature::CreaturePtr_t   creatureListeningPtr,
            stats::Health_t &         healthAdj,
            creature::CondEnumVec_t & condsAddedVec,
            creature::CondEnumVec_t & condsRemovedVec,
            ContentAndNamePos &       actionPhraseCNP) const;
    };


    class TripBeat : public Song
    {
    public:
        TripBeat() : Song(Songs::TripBeat,
                          SongType::Drum,
                          EffectType::CreatureHarmMisc,
                          Phase::Combat,
                          1,
                          1,
                          TargetType::AllOpponents)
        {}

        virtual ~TripBeat() {}

        virtual bool EffectCreature(
            creature::CreaturePtr_t   creaturePlayingPtr,
            creature::CreaturePtr_t   creatureListeningPtr,
            stats::Health_t &         healthAdj,
            creature::CondEnumVec_t & condsAddedVec,
            creature::CondEnumVec_t & condsRemovedVec,
            ContentAndNamePos &       actionPhraseCNP) const;
    };


    class PanicStrings : public Song
    {
    public:
        PanicStrings() : Song(Songs::PanicStrings,
                              SongType::Guitar,
                              EffectType::CreatureHarmMisc,
                              Phase::Combat,
                              2,
                              1,
                              TargetType::AllOpponents)
        {}

        virtual ~PanicStrings() {}

        virtual bool EffectCreature(
            creature::CreaturePtr_t   creaturePlayingPtr,
            creature::CreaturePtr_t   creatureListeningPtr,
            stats::Health_t &         healthAdj,
            creature::CondEnumVec_t & condsAddedVec,
            creature::CondEnumVec_t & condsRemovedVec,
            ContentAndNamePos &       actionPhraseCNP) const;
    };


    class Lullaby : public Song
    {
    public:
        Lullaby() : Song(Songs::Lullaby,
                         SongType::Guitar,
                         EffectType::CreatureHarmMisc,
                         Phase::Combat,
                         2,
                         1,
                         TargetType::AllOpponents)
        {}

        virtual ~Lullaby() {}

        virtual bool EffectCreature(
            creature::CreaturePtr_t   creaturePlayingPtr,
            creature::CreaturePtr_t   creatureListeningPtr,
            stats::Health_t &         healthAdj,
            creature::CondEnumVec_t & condsAddedVec,
            creature::CondEnumVec_t & condsRemovedVec,
            ContentAndNamePos &       actionPhraseCNP) const;
    };

}
}

#endif //GAME_SONG_SONGS_HPP_INCLUDED
