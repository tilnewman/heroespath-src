// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// npc-factory.cpp
//
#include "npc-factory.hpp"

#include "game/game.hpp"
#include "map/level-enum.hpp"
#include "misc/random.hpp"
#include "misc/vectors.hpp"
#include "state/game-state.hpp"
#include "state/maps.hpp"
#include "state/npc-warehouse.hpp"
#include "state/npc.hpp"
#include "state/world.hpp"

namespace heroespath
{
namespace state
{

    const NpcPVec_t NpcFactory::Make(const NpcPlaceholder & PLACEHOLDER) const
    {
        NpcPVec_t npcPtrs;

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
                       game::Game::Instance()->State().GetWorld().GetMaps().Current().Which()))
            {
                talkCategories.emplace_back(interact::talk::Category::Town);
            }

            auto const MOOD{ (
                (misc::random::Bool()) ? interact::talk::Mood::Kind : interact::talk::Mood::Mean) };

            npcPtrs.emplace_back(NpcWarehouse::Access().Store(std::make_unique<Npc>(
                AVATAR, talkCategories, MOOD, PLACEHOLDER.WalkBoundsIndex())));
        }

        return npcPtrs;
    }

} // namespace state
} // namespace heroespath
