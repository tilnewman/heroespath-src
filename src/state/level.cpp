// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// level.cpp
//
#include "level.hpp"

#include "misc/serialize-helpers.hpp"
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

    Level::~Level()
    {
        NpcWarehouse::Access().Free(specificNpcs_);
        NpcWarehouse::Access().Free(randomNpcs_);
    }

    bool Level::IsDoorLocked(const map::Level::Enum LEVEL) const
    {
        auto isLocked{ false };
        doorLockMap_.Find(LEVEL, isLocked);
        return isLocked;
    }

    void Level::HandleLevelLoad()
    {
        CreateRandomNPCs();

        for (auto & npcPtr : randomNpcs_)
        {
            npcPtr->ResetConversation();
        }
    }

    void Level::HandleLevelUnload() { NpcWarehouse::Access().Free(randomNpcs_); }

    void Level::BeforeSerialize()
    {
        misc::SerializeHelp::BeforeSerialize(specificNpcs_, specificNpcsToSerialize_);
        misc::SerializeHelp::BeforeSerialize(randomNpcs_, randomNpcsToSerialize_);
    }

    void Level::AfterSerialize()
    {
        misc::SerializeHelp::AfterSerialize(
            specificNpcsToSerialize_, state::NpcWarehouse::Access());

        misc::SerializeHelp::AfterSerialize(randomNpcsToSerialize_, state::NpcWarehouse::Access());
    }

    void Level::AfterDeserialize()
    {
        misc::SerializeHelp::AfterDeserialize(
            specificNpcs_, specificNpcsToSerialize_, state::NpcWarehouse::Access());

        misc::SerializeHelp::AfterDeserialize(
            randomNpcs_, randomNpcsToSerialize_, state::NpcWarehouse::Access());
    }

    void Level::CreateRandomNPCs()
    {
        NpcFactory npcFactory;
        for (auto const & NPC_PLACEHOLDER : randomNPCPlaceholders_)
        {
            for (auto const & NPC_PTR : npcFactory.Make(NPC_PLACEHOLDER))
            {
                AddRandomNpc(NPC_PTR);
            }
        }
    }

} // namespace state
} // namespace heroespath
