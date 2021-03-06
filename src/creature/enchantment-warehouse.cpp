// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// enchantment-warehouse.cpp
//
#include "enchantment-warehouse.hpp"

#include "creature/enchantment.hpp"
#include "misc/log-macros.hpp"

namespace heroespath
{
namespace creature
{

    std::unique_ptr<EnchantmentWarehouse> EnchantmentWarehouse::instanceUPtr_;

    EnchantmentWarehouse::EnchantmentWarehouse()
        : warehouse_(500) // found by experiment to be a good upper bound for the game
    {
        M_HP_LOG_DBG("Subsystem Construction: EnchantmentWarehouse");
    }

    EnchantmentWarehouse::~EnchantmentWarehouse()
    {
        M_HP_LOG_DBG("Subsystem Destruction: EnchantmentWarehouse");
    }

    misc::NotNull<EnchantmentWarehouse *> EnchantmentWarehouse::Instance()
    {
        if (!instanceUPtr_)
        {
            M_HP_LOG_ERR(
                "Subsystem Instance() called but instanceUPtr_ was null: EnchantmentWarehouse");
            Create();
        }

        return misc::NotNull<EnchantmentWarehouse *>(instanceUPtr_.get());
    }

    void EnchantmentWarehouse::Create()
    {
        if (!instanceUPtr_)
        {
            instanceUPtr_ = std::make_unique<EnchantmentWarehouse>();
        }
        else
        {
            M_HP_LOG_ERR("Subsystem Create() after Construction: EnchantmentWarehouse");
        }
    }

    void EnchantmentWarehouse::Destroy() { instanceUPtr_.reset(); }

} // namespace creature
} // namespace heroespath
