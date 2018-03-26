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
// statement-factory.hpp
//
#include "statement-factory.hpp"
#include "creature/creature.hpp"
#include "game/game.hpp"
#include "interact/statement-child.hpp"
#include "interact/statement-common.hpp"
#include "interact/statement-guard.hpp"
#include "interact/statement-monk.hpp"
#include "interact/statement-tavern.hpp"
#include "interact/statement-town.hpp"
#include "map/level-enum.hpp"
#include "misc/assertlogandthrow.hpp"
#include "player/party.hpp"
#include "state/game-state.hpp"
#include "state/world.hpp"

namespace heroespath
{
namespace interact
{
    namespace talk
    {

        const std::string Factory::Make(const CategoryVec_t & CATEGORIES, const Mood MOOD)
        {
            M_ASSERT_OR_LOGANDTHROW_SS(
                (CATEGORIES.empty() == false),
                "interact::talk::Factory::Make() was given an empty category vector.");

            auto const CATEGORY{ misc::Vector::SelectRandom(CATEGORIES) };

            auto const & PARTY{ game::Game::Instance()->State().Party() };

            auto const LEVEL{
                game::Game::Instance()->State().GetWorld().GetMaps().Current().Which()
            };

            switch (CATEGORY)
            {
                case Category::Town:
                    return TownTalk::Compose(MOOD, PARTY, LEVEL);
                case Category::Child:
                    return ChildTalk::Compose(MOOD, PARTY);
                case Category::Monk:
                    return MonkTalk::Compose(MOOD);
                case Category::Drunk:
                    return TavernTalk::Compose(MOOD, PARTY);
                case Category::Guard:
                    return GuardTalk::Compose(MOOD, PARTY);
                case Category::Common:
                default:
                    return CommonTalk::Compose(MOOD, PARTY);
            }
        }

    } // namespace talk
} // namespace interact
} // namespace heroespath
