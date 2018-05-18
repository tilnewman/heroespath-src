// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// world-factory.cpp
//
#include "world-factory.hpp"
#include "game/level.hpp"
#include "game/world.hpp"

namespace heroespath
{
namespace game
{

    WorldUPtr_t WorldFactory::MakeForNewGame()
    {
        auto worldUPtr{ std::make_unique<World>() };
        worldUPtr->GetMaps().SetupForNewGame();
        return worldUPtr;
    }

    void WorldFactory::SetupLevelForNewGame(Level & level)
    {
        if ((level.Which() == map::Level::Thornberry_GuardPostWest)
            || (level.Which() == map::Level::Thornberry_GuardPostEast))
        {
            level.AddRandomNpcPlaceholder(NpcPlaceholder(
                1,
                1,
                { avatar::Avatar::NameEnum::Private,
                  avatar::Avatar::NameEnum::Private1st,
                  avatar::Avatar::NameEnum::Private2nd,
                  avatar::Avatar::NameEnum::Corporal },
                0,
                false));
        }

        if (level.Which() == map::Level::Thornberry)
        {
            level.IsDoorLocked(map::Level::Thornberry_GuardPostWest, true);
        }
    }

} // namespace game
} // namespace heroespath
