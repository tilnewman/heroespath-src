// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// maps.cpp
//
#include "maps.hpp"
#include "game/world-factory.hpp"

namespace heroespath
{
namespace game
{

    Maps::Maps()
        : level_(map::Level::Count)
        , levels_()
    {}

    void Maps::SetupForNewGame()
    {
        level_ = map::Level::Thornberry;
        levels_.clear();

        for (misc::EnumUnderlying_t i(0); i < map::Level::Count; ++i)
        {
            levels_.emplace_back(Level(static_cast<map::Level::Enum>(i)));
            WorldFactory::SetupLevelForNewGame(levels_[levels_.size() - 1]);
        }
    }

    void Maps::HandleLevelLoad(const map::Level::Enum LEVEL)
    {
        if (LEVEL != map::Level::Count)
        {
            level_ = LEVEL;
            Current().HandleLevelLoad();
        }
    }

    void Maps::HandleLevelUnload(const map::Level::Enum LEVEL)
    {
        if (LEVEL != map::Level::Count)
        {
            levels_.at(static_cast<std::size_t>(LEVEL)).HandleLevelUnload();
        }
    }

    void Maps::BeforeSerialize()
    {
        for (auto & level : levels_)
        {
            level.BeforeSerialize();
        }
    }

    void Maps::AfterSerialize()
    {
        for (auto & level : levels_)
        {
            level.AfterSerialize();
        }
    }

    void Maps::AfterDeserialize()
    {
        for (auto & level : levels_)
        {
            level.AfterDeserialize();
        }
    }

} // namespace game
} // namespace heroespath
