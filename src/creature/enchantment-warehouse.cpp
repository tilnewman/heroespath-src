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
// enchantment-warehouse.cpp
//
#include "enchantment-warehouse.hpp"

#include "creature/enchantment.hpp"
#include "log/log-macros.hpp"

namespace heroespath
{
namespace creature
{

    std::unique_ptr<EnchantmentWarehouse> EnchantmentWarehouse::instanceUPtr_{ nullptr };

    EnchantmentWarehouse::EnchantmentWarehouse()
        : warehouse_()
    {
        M_HP_LOG_DBG("Singleton Construction: EnchantmentWarehouse");
    }

    EnchantmentWarehouse::~EnchantmentWarehouse()
    {
        M_HP_LOG_DBG("Singleton Destruction: EnchantmentWarehouse");
    }

    EnchantmentWarehouse * EnchantmentWarehouse::Instance()
    {
        if (!instanceUPtr_)
        {
            M_HP_LOG_ERR("Singleton Instance() before Acquire(): EnchantmentWarehouse");
            Acquire();
        }

        return instanceUPtr_.get();
    }

    void EnchantmentWarehouse::Acquire()
    {
        if (!instanceUPtr_)
        {
            instanceUPtr_ = std::make_unique<EnchantmentWarehouse>();
        }
        else
        {
            M_HP_LOG_ERR("Singleton Acquire() after Construction: EnchantmentWarehouse");
        }
    }

    void EnchantmentWarehouse::Release()
    {
        M_ASSERT_OR_LOGANDTHROW_SS(
            (instanceUPtr_),
            "creature::EnchantmentWarehouse::Release() found instanceUPtr that was null.");

        instanceUPtr_.reset();
    }

    const EnchantmentPtr_t EnchantmentWarehouse::Store(const EnchantmentPtr_t ENCHANTMENT_PTR)
    {
        return warehouse_.Store(ENCHANTMENT_PTR);
    }

    void EnchantmentWarehouse::Free(const EnchantmentPtr_t ENCHANTMENT_PTR)
    {
        warehouse_.Free(ENCHANTMENT_PTR);
    }

    void EnchantmentWarehouse::Free(EnchantmentPVec_t & enchantmentPVec)
    {
        warehouse_.Free(enchantmentPVec);
    }

} // namespace creature
} // namespace heroespath
