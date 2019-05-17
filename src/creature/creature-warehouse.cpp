// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// creature-warehouse.cpp
//
#include "creature-warehouse.hpp"

#include "creature/creature.hpp"
#include "misc/log-macros.hpp"

namespace heroespath
{
namespace creature
{

    std::unique_ptr<CreatureWarehouse> CreatureWarehouse::instanceUPtr_;

    CreatureWarehouse::CreatureWarehouse()
        : warehouse_(100) // found by experiment to be a good upper bound for the game
    {
        M_HP_LOG_DBG("Subsystem Construction: Non-Player CreatureWarehouse");
    }

    CreatureWarehouse::~CreatureWarehouse()
    {
        M_HP_LOG_DBG("Subsystem Destruction: Non-Player CreatureWarehouse");
    }

    misc::NotNull<CreatureWarehouse *> CreatureWarehouse::Instance()
    {
        if (!instanceUPtr_)
        {
            M_HP_LOG_ERR("Subsystem Instance() called but instanceUPtr_ was null: Non-Player "
                         "CreatureWarehouse");
            Acquire();
        }

        return misc::NotNull<CreatureWarehouse *>(instanceUPtr_.get());
    }

    void CreatureWarehouse::Acquire()
    {
        if (!instanceUPtr_)
        {
            instanceUPtr_ = std::make_unique<CreatureWarehouse>();
        }
        else
        {
            M_HP_LOG_ERR("Subsystem Acquire() after Construction: Non-Player CreatureWarehouse");
        }
    }

    void CreatureWarehouse::Release() { instanceUPtr_.reset(); }

} // namespace creature
} // namespace heroespath
