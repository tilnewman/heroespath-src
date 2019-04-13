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

#include "game/game-state.hpp"
#include "game/game.hpp"
#include "game/maps.hpp"
#include "game/npc-warehouse.hpp"
#include "game/npc.hpp"
#include "game/world.hpp"
#include "map/level-enum.hpp"
#include "misc/random.hpp"
#include "misc/vectors.hpp"

namespace heroespath
{
namespace game
{

    const NpcPVec_t NpcFactory::Make(const NpcPlaceholder & PLACEHOLDER) const
    {
        NpcPVec_t npcPtrs;

        const auto COUNT { misc::Random(PLACEHOLDER.CountMin(), PLACEHOLDER.CountMax()) };

        for (std::size_t i(0); i < COUNT; ++i)
        {
            auto AVATAR_TYPE { misc::RandomSelect(PLACEHOLDER.AvatarTypeVec()) };
            auto AVATAR { misc::RandomSelect(avatar::Avatar::Avatars(AVATAR_TYPE)) };

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

            const auto MOOD { (
                (misc::RandomBool()) ? interact::talk::Mood::Kind : interact::talk::Mood::Mean) };

            npcPtrs.emplace_back(NpcWarehouse::Access().Store(std::make_unique<Npc>(
                AVATAR, talkCategories, MOOD, PLACEHOLDER.WalkBoundsIndex())));
        }

        return npcPtrs;
    }

} // namespace game
} // namespace heroespath
