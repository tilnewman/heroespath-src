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
// world-factory.cpp
//
#include "npc-factory.hpp"
#include "game/game.hpp"
#include "map/level-enum.hpp"
#include "misc/random.hpp"
#include "misc/vectors.hpp"
#include "state/game-state.hpp"
#include "state/maps.hpp"
#include "state/world.hpp"

namespace heroespath
{
namespace state
{

    const NpcVec_t NpcFactory::Make(const NpcPlaceholder & PLACEHOLDER)
    {
        NpcVec_t npcs;

        auto const COUNT{ misc::random::SizeT(PLACEHOLDER.CountMin(), PLACEHOLDER.CountMax()) };

        for (std::size_t i(0); i < COUNT; ++i)
        {
            auto AVATAR_TYPE{ misc::Vector::SelectRandom(PLACEHOLDER.AvatarTypeVec()) };
            auto AVATAR{ misc::Vector::SelectRandom(avatar::Avatar::Avatars(AVATAR_TYPE)) };

            interact::talk::CategoryVec_t talkCategories;

            if (PLACEHOLDER.IsDrunk())
            {
                talkCategories.emplace_back(interact::talk::Category::Drunk);
            }
            else
            {
                talkCategories.emplace_back(avatar::Avatar::TalkCategory(AVATAR_TYPE));
            }

            if (map::LevelType::Town
                == map::Level::Type(
                       game::Game::Instance()->State().World().GetMaps().Current().Which()))
            {
                talkCategories.emplace_back(interact::talk::Category::Town);
            }

            auto const MOOD{ (
                (misc::random::Bool()) ? interact::talk::Mood::Kind : interact::talk::Mood::Mean) };

            npcs.emplace_back(Npc(AVATAR, talkCategories, MOOD, PLACEHOLDER.WalkBoundsIndex()));
        }

        return npcs;
    }

} // namespace state
} // namespace heroespath
