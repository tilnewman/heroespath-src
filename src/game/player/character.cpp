// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
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
// character.hpp (player)
//
#include "character.hpp"

#include "sfml-util/date-time.hpp"

#include "game/creature/body-type.hpp"
#include "game/spell/spell-base.hpp"
#include "game/stats/stat-set.hpp"
#include "game/creature/race.hpp"
#include "game/creature/role.hpp"
#include "game/creature/title.hpp"
#include "game/creature/condition.hpp"
#include "game/item/inventory.hpp"

#include "misc/vectors.hpp"


namespace game
{
namespace player
{

    Character::Character(const std::string &                  NAME,
                         const creature::sex::Enum            SEX,
                         const creature::BodyType &           BODY_TYPE,
                         const creature::Race &               RACE,
                         const creature::Role &               ROLE,
                         const stats::StatSet &               STATS,
                         const stats::Health_t                HEALTH,
                         const stats::Rank_t                  RANK,
                         const stats::Exp_t                   EXPERIENCE,
                         const creature::CondEnumVec_t &      CONDITIONS_VEC,
                         const creature::TitleEnumVec_t &     TITLES_VEC,
                         const item::Inventory &              INVENTORY,
                         const sfml_util::DateTime &          DATE_TIME,
                         const std::string &                  IMAGE_FILENAME,
                         const spell::SpellVec_t &            SPELLS_VEC,
                         const stats::Mana_t                  MANA,
                         const song::SongVec_t &              SONG_VEC)
    :
        Creature(NAME,
                 SEX,
                 BODY_TYPE,
                 RACE,
                 ROLE,
                 STATS,
                 HEALTH,
                 RANK,
                 EXPERIENCE,
                 CONDITIONS_VEC,
                 TITLES_VEC,
                 INVENTORY,
                 DATE_TIME,
                 IMAGE_FILENAME,
                 SPELLS_VEC,
                 MANA,
                 SONG_VEC)
    {}


    Character::~Character()
    {}


    bool operator==(const Character & L, const Character & R)
    {
        if (std::tie(L.name_,
                     L.imageFilename_,
                     L.sex_,
                     L.bodyType_,
                     L.race_,
                     L.role_,
                     L.stats_,
                     L.serialNumber_,
                     L.healthCurrent_,
                     L.healthNormal_,
                     L.rank_,
                     L.experience_,
                     L.dateTimeCreated_,
                     L.achievements_,
                     L.manaCurrent_,
                     L.manaNormal_,
                     L.lastSpellCastNum_,
                     L.lastSongPlayedNum_)
               !=
               std::tie(R.name_,
                        R.imageFilename_,
                        R.sex_,
                        R.bodyType_,
                        R.race_,
                        R.role_,
                        R.stats_,
                        R.serialNumber_,
                        R.healthCurrent_,
                        R.healthNormal_,
                        R.rank_,
                        R.experience_,
                        R.dateTimeCreated_,
                        R.achievements_,
                        R.manaCurrent_,
                        R.manaNormal_,
                        R.lastSpellCastNum_,
                        R.lastSongPlayedNum_))
        {
            return false;
        }

        if (misc::Vector::OrderlessCompareEqual(L.titlesVec_, R.titlesVec_) == false)
        {
            return false;
        }

        if (misc::Vector::OrderlessCompareEqual(L.conditionsVec_, R.conditionsVec_) == false)
        {
            return false;
        }

        if (misc::Vector::OrderlessCompareEqual(L.songsVec_, R.songsVec_) == false)
        {
            return false;
        }

        return misc::Vector::OrderlessCompareEqual(L.spellsVec_, R.spellsVec_);
    }


    bool operator<(const Character & L, const Character & R)
    {
        if (std::tie(L.name_,
                     L.imageFilename_,
                     L.sex_,
                     L.bodyType_,
                     L.race_,
                     L.role_,
                     L.stats_,
                     L.serialNumber_,
                     L.healthCurrent_,
                     L.healthNormal_,
                     L.rank_,
                     L.experience_,
                     L.dateTimeCreated_,
                     L.achievements_,
                     L.manaCurrent_,
                     L.manaNormal_,
                     L.lastSpellCastNum_,
                     L.lastSongPlayedNum_)
               <
               std::tie(R.name_,
                        R.imageFilename_,
                        R.sex_,
                        R.bodyType_,
                        R.race_,
                        R.role_,
                        R.stats_,
                        R.serialNumber_,
                        R.healthCurrent_,
                        R.healthNormal_,
                        R.rank_,
                        R.experience_,
                        R.dateTimeCreated_,
                        R.achievements_,
                        R.manaCurrent_,
                        R.manaNormal_,
                        R.lastSpellCastNum_,
                        R.lastSongPlayedNum_))
        {
            return true;
        }

        if (misc::Vector::OrderlessCompareLess(L.titlesVec_, R.titlesVec_) == true)
        {
            return true;
        }

        if (misc::Vector::OrderlessCompareLess(L.conditionsVec_, R.conditionsVec_) == true)
        {
            return true;
        }

        if (misc::Vector::OrderlessCompareLess(L.songsVec_, R.songsVec_) == true)
        {
            return true;
        }

        return misc::Vector::OrderlessCompareLess(L.spellsVec_, R.spellsVec_);
    }

}
}
