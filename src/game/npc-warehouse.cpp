// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// npc-warehouse.cpp
//
#include "npc-warehouse.hpp"

#include "game/npc.hpp"
#include "misc/log-macros.hpp"

namespace heroespath
{
namespace game
{

    std::unique_ptr<NpcWarehouse> NpcWarehouse::instanceUPtr_;

    NpcWarehouse::NpcWarehouse()
        : warehouse_(100) // found by experiment to be a good upper bound for the game
    {
        M_HP_LOG_DBG("Subsystem Construction: NpcWarehouse");
    }

    NpcWarehouse::~NpcWarehouse() { M_HP_LOG_DBG("Subsystem Destruction: NpcWarehouse"); }

    misc::NotNull<NpcWarehouse *> NpcWarehouse::Instance()
    {
        if (!instanceUPtr_)
        {
            M_HP_LOG_ERR("Subsystem Instance() called but instanceUPtr_ was null: NpcWarehouse");
            Create();
        }

        return misc::NotNull<NpcWarehouse *>(instanceUPtr_.get());
    }

    void NpcWarehouse::Create()
    {
        if (!instanceUPtr_)
        {
            instanceUPtr_ = std::make_unique<NpcWarehouse>();
        }
        else
        {
            M_HP_LOG_ERR("Subsystem Create() after Construction: NpcWarehouse");
        }
    }

    void NpcWarehouse::Destroy() { instanceUPtr_.reset(); }

} // namespace game
} // namespace heroespath
