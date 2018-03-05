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
#ifndef HEROESPATH_NONPLAYER_CHARACTER_HPP_INCLUDED
#define HEROESPATH_NONPLAYER_CHARACTER_HPP_INCLUDED
//
// character.hpp (non-player)
//  A character NOT under the control of the user.
//
#include "misc/boost-serialize-includes.hpp"

#include "creature/creature.hpp"
#include "creature/race-enum.hpp"
#include "creature/role-enum.hpp"
#include "creature/sex-enum.hpp"
#include "creature/title-enum.hpp"
#include "misc/types.hpp"
#include "song/song-enum.hpp"
#include "spell/spell-enum.hpp"
#include "stats/trait.hpp"

#include <memory>
#include <set>
#include <string>
#include <vector>

namespace heroespath
{
namespace sfml_util
{
    class DateTime;
}

namespace item
{
    class Inventory;
}
namespace creature
{
    class BodyType;
}
namespace stats
{
    class StateSet;
}

namespace non_player
{

    class Character : public creature::Creature
    {
        Character(const Character &) = delete;
        Character & operator=(const Character &) = delete;

    public:
        explicit Character(
            const std::string & NAME = "no_name_error",
            const creature::sex::Enum SEX = creature::sex::Unknown,
            const creature::BodyType & BODY_TYPE = creature::BodyType(),
            const creature::race::Enum & RACE = creature::race::Human,
            const creature::role::Enum & ROLE = creature::role::Archer,
            const stats::StatSet & STATS = stats::StatSet(),
            const Health_t & HEALTH = 0_health,
            const Rank_t & RANK = 1_rank,
            const Experience_t & EXPERIENCE = 0_exp,
            const creature::CondEnumVec_t & CONDITIONS_VEC = creature::CondEnumVec_t(),
            const creature::TitleEnumVec_t & TITLES_VEC = creature::TitleEnumVec_t(),
            const item::Inventory & INVENTORY = item::Inventory(),
            const sfml_util::DateTime & DATE_TIME = sfml_util::DateTime(),
            const std::string & IMAGE_FILENAME = "",
            const spell::SpellVec_t & SPELLS_VEC = spell::SpellVec_t(),
            const Mana_t & MANA = 0_mana,
            const song::SongVec_t & SONG_VEC = song::SongVec_t());

        virtual ~Character();

        friend bool operator==(const Character & L, const Character & R);
        friend bool operator<(const Character & L, const Character & R);

    private:
        friend class boost::serialization::access;
        template <typename Archive>
        void serialize(Archive & ar, const unsigned int)
        {
            ar & boost::serialization::base_object<Creature>(*this);
        }
    };

    using CharacterPtr_t = Character *;
    using CharacterCPtr_t = const Character *;
    using CharacterPtrC_t = Character * const;
    using CharacerCPtrC_t = const Character * const;

    using CharacterPVec_t = std::vector<CharacterPtr_t>;
    using CharacterPSet_t = std::set<CharacterPtr_t>;

    bool operator==(const Character & L, const Character & R);

    inline bool operator!=(const Character & L, const Character & R) { return !(L == R); }

    bool operator<(const Character & L, const Character & R);
}
}

#endif // HEROESPATH_NONPLAYER_CHARACTER_HPP_INCLUDED
