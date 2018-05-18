// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// world.cpp
//
#include "world.hpp"

namespace heroespath
{
namespace game
{

    World::World()
        : maps_()
        , encounterCount_(0)
    {}

    void World::HandleLevelLoad(const map::Level::Enum LEVEL) { maps_.HandleLevelLoad(LEVEL); }

    void World::HandleLevelUnload(const map::Level::Enum LEVEL) { maps_.HandleLevelUnload(LEVEL); }

    void World::BeforeSerialize() { maps_.BeforeSerialize(); }

    void World::AfterSerialize() { maps_.AfterSerialize(); }

    void World::AfterDeserialize() { maps_.AfterDeserialize(); }

} // namespace game
} // namespace heroespath
