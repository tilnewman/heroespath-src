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
// maps.cpp
//
#include "maps.hpp"
#include "state/world-factory.hpp"

namespace heroespath
{
namespace state
{

    Maps::Maps()
        : level_(map::Level::Count)
        , levels_()
    {}

    void Maps::SetupForNewGame()
    {
        level_ = map::Level::Thornberry;
        levels_.clear();

        for (int i(0); i < map::Level::Count; ++i)
        {
            levels_.emplace_back(Level(static_cast<map::Level::Enum>(i)));
            WorldFactory::SetupLevelForNewGame(levels_[levels_.size() - 1]);
        }
    }

    void Maps::HandleLevelLoad(const map::Level::Enum LEVEL)
    {
        level_ = LEVEL;
        Current().HandleLevelLoad();
    }

    void Maps::BeforeSerialize()
    {
        for (auto & level : levels_)
        {
            level.BeforeSerialize();
        }
    }

    void Maps::AfterDeserialize()
    {
        for (auto & level : levels_)
        {
            level.AfterDeserialize();
        }
    }

} // namespace state
} // namespace heroespath
