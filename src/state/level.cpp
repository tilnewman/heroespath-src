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
// level.cpp
//
#include "level.hpp"
#include "state/npc-factory.hpp"
#include "state/npc-warehouse.hpp"

#include <algorithm>

namespace heroespath
{
namespace state
{

    Level::Level(const map::Level::Enum LEVEL)
        : level_(LEVEL)
        , doorLockMap_()
        , randomNPCPlaceholders_()
        , specificNpcs_()
        , randomNpcs_()
        , specificNpcsToSerialize_()
        , randomNpcsToSerialize_()
    {}

    bool Level::IsDoorLocked(const map::Level::Enum LEVEL) const
    {
        auto isLocked{ false };
        doorLockMap_.Find(LEVEL, isLocked);
        return isLocked;
    }

    void Level::HandleLevelLoad()
    {
        for (auto & npcPtr : specificNpcs_)
        {
            npcPtr->HandleLevelLoad();
        }

        ResetRandomNPCs();

        for (auto & npcPtr : randomNpcs_)
        {
            npcPtr->HandleLevelLoad();
        }
    }

    void Level::AddSpecificNPC(const NpcPtr_t NPC_PTR)
    {
        specificNpcs_.emplace_back(NpcWarehouse::Instance()->Store(NPC_PTR));
    }

    void Level::ResetRandomNPCs()
    {
        randomNpcs_.clear();

        for (auto const & NPC_PLACEHOLDER : randomNPCPlaceholders_)
        {
            for (auto const & NPC_PTR : NpcFactory::Make(NPC_PLACEHOLDER))
            {
                AddRandomNpc(NPC_PTR);
            }
        }
    }

    void Level::BeforeSerialize()
    {
        specificNpcsToSerialize_.clear();
        for (auto const & NPC_PTR : specificNpcs_)
        {
            specificNpcsToSerialize_.emplace_back(NPC_PTR.Ptr());
        }
        // everything in specificNpcs_ is free'd in the destructor

        randomNpcsToSerialize_.clear();
        for (auto const & NPC_PTR : randomNpcs_)
        {
            randomNpcsToSerialize_.emplace_back(NPC_PTR.Ptr());
        }
        // everything in specificNpcs_ is free'd in the destructor
    }

    void Level::AfterDeserialize()
    {
        specificNpcs_.clear();
        for (auto const & NPC_PTR : specificNpcsToSerialize_)
        {
            AddSpecificNPC(NPC_PTR);
        }
        specificNpcsToSerialize_.clear();

        randomNpcs_.clear();
        for (auto const & NPC_PTR : randomNpcsToSerialize_)
        {
            AddRandomNpc(NPC_PTR);
        }
        randomNpcsToSerialize_.clear();
    }

    void Level::AddRandomNpc(const NpcPtr_t NPC_PTR)
    {
        randomNpcs_.emplace_back(NpcWarehouse::Instance()->Store(NPC_PTR));
    }

} // namespace state
} // namespace heroespath
