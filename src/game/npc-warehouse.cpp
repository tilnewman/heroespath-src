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
            Acquire();
        }

        return misc::NotNull<NpcWarehouse *>(instanceUPtr_.get());
    }

    void NpcWarehouse::Acquire()
    {
        if (!instanceUPtr_)
        {
            instanceUPtr_ = std::make_unique<NpcWarehouse>();
        }
        else
        {
            M_HP_LOG_ERR("Subsystem Acquire() after Construction: NpcWarehouse");
        }
    }

    void NpcWarehouse::Release()
    {
        M_HP_ASSERT_OR_LOG_AND_THROW(
            (instanceUPtr_), "creature::NpcWarehouse::Release() found instanceUPtr that was null.");

        instanceUPtr_.reset();
    }

} // namespace game
} // namespace heroespath
