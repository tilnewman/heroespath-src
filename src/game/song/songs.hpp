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

    class SleepMelody : public Song
    {
    public:
        SleepMelody() : Song(Songs::SleepMelody,
                             SongType::Guitar,
                             EffectType::CreatureHarmMisc,
                             Phase::Combat,
                             2,
                             1,
                             TargetType::AllOpponents)
        {}

        virtual ~SleepMelody() {}

        virtual const std::string ActionPhrase(
            creature::CreaturePtr_t castingCreaturePtr,
            creature::CreaturePtr_t effectedCreaturePtr) const;

        virtual const std::string EffectCreature(
            creature::CreaturePtr_t castingCreaturePtr,
            creature::CreaturePtr_t effectedCreaturePtr,
            creature::ConditionEnumVec_t & conditionsAddedVec) const;
    };

}
}

#endif //GAME_SONG_SONGS_HPP_INCLUDED
