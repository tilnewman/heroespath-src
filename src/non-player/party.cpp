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
// party.hpp (non-player)
//
#include "party.hpp"

#include "creature/creature.hpp"
#include "log/log-macros.hpp"
#include "misc/boost-string-includes.hpp"
#include "misc/vector-map.hpp"
#include "non-player/character-warehouse.hpp"

#include <sstream>
#include <utility>

namespace heroespath
{
namespace non_player
{

    Party::Party(const creature::CreaturePVec_t & CHARACTERS_PVEC)
        : charactersPVec_()
    {
        for (auto const & NEXT_CHARACTER_PTR : CHARACTERS_PVEC)
        {
            charactersPVec_.emplace_back(CharacterWarehouse::Instance()->Store(NEXT_CHARACTER_PTR));
        }
    }

    Party::~Party() { CharacterWarehouse::Instance()->Free(charactersPVec_); }

    void Party::Add(const creature::CreaturePtr_t CHARACTER_PTR, const bool WILL_STORE)
    {
        if (WILL_STORE)
        {
            charactersPVec_.emplace_back(CharacterWarehouse::Instance()->Store(CHARACTER_PTR));
        }
        else
        {
            charactersPVec_.emplace_back(CHARACTER_PTR);
        }
    }

    bool Party::Remove(const creature::CreaturePtr_t CHARACTER_PTR, const bool WILL_FREE)
    {
        auto const FOUND_ITER{ std::find(
            charactersPVec_.begin(), charactersPVec_.end(), CHARACTER_PTR) };

        if (FOUND_ITER == charactersPVec_.end())
        {
            M_HP_LOG_ERR(
                "non-player::Party::Remove(will_free=" << std::boolalpha << WILL_FREE
                                                       << ", creature=" << CHARACTER_PTR->ToString()
                                                       << ") was not found in the party.");

            return false;
        }
        else
        {
            if (WILL_FREE)
            {
                CharacterWarehouse::Instance()->Free(CHARACTER_PTR);
            }

            charactersPVec_.erase(FOUND_ITER);
            return true;
        }
    }

    const std::string Party::Summary() const
    {
        using RaceRolePair_t = std::pair<std::string, std::string>;
        misc::VectorMap<RaceRolePair_t, std::size_t> raceRoleMap;

        // count all race/role combinations
        for (auto const & NEXT_CHAR_PTR : charactersPVec_)
        {
            raceRoleMap[std::make_pair(NEXT_CHAR_PTR->RaceName(), NEXT_CHAR_PTR->RoleName())]++;
        }

        // make a single string summary of all race/role combinations
        std::ostringstream ss;
        for (auto itr(raceRoleMap.begin()); itr != raceRoleMap.end(); ++itr)
        {
            auto const NEXT_RACEROLECOUNT_PAIR{ *itr };

            if (itr != raceRoleMap.begin())
            {
                ss << ", ";

                auto nextItr{ itr };
                std::advance(nextItr, 1);
                if (nextItr == raceRoleMap.end())
                {
                    ss << "and ";
                }
            }

            ss << NEXT_RACEROLECOUNT_PAIR.second << " " << NEXT_RACEROLECOUNT_PAIR.first.first
               << " " << NEXT_RACEROLECOUNT_PAIR.first.second
               << ((NEXT_RACEROLECOUNT_PAIR.second > 1) ? "s" : "");
        }

        return ss.str();
    }

    bool Party::Contains(const creature::CreaturePtr_t CREATURE_PTR) const
    {
        return (
            std::find(std::begin(charactersPVec_), std::end(charactersPVec_), CREATURE_PTR)
            != std::end(charactersPVec_));
    }

} // namespace non_player
} // namespace heroespath
