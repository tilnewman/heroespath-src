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
// enchantment-factory.cpp
//
#include "enchantment-factory.hpp"

#include "game/log-macros.hpp"
#include "game/creature/enchantment-warehouse.hpp"
#include "game/item/item.hpp"

#include "misc/assertlogandthrow.hpp"


namespace game
{
namespace creature
{

    std::unique_ptr<EnchantmentFactory> EnchantmentFactory::instanceUPtr_{ nullptr };


    EnchantmentFactory::EnchantmentFactory()
    {
        M_HP_LOG_DBG("Singleton Construction: EnchantmentFactory");
    }


    EnchantmentFactory::~EnchantmentFactory()
    {
        M_HP_LOG_DBG("Singleton Destruction: EnchantmentFactory");
    }


    EnchantmentFactory * EnchantmentFactory::Instance()
    {
        if (instanceUPtr_.get() == nullptr)
        {
            M_HP_LOG_WRN("Singleton Instance() before Acquire(): EnchantmentFactory");
            Acquire();
        }

        return instanceUPtr_.get();
    }


    void EnchantmentFactory::Acquire()
    {
        if (instanceUPtr_.get() == nullptr)
        {
            instanceUPtr_.reset(new EnchantmentFactory);
        }
        else
        {
            M_HP_LOG_WRN("Singleton Acquire() after Construction: EnchantmentFactory");
        }
    }


    void EnchantmentFactory::Release()
    {
        M_ASSERT_OR_LOGANDTHROW_SS((instanceUPtr_.get() != nullptr),
            "game::creature::EnchantmentFactory::Release() found instanceUPtr that was null.");

        instanceUPtr_.reset();
    }


    void EnchantmentFactory::MakeStoreAttach(
        item::ItemPtr_t             itemPtr,
        const EnchantmentType::Enum TYPE,
        const stats::TraitSet &     TRAIT_SET,
        const UseInfo &             USE_INFO,
        const SummonInfo &          SUMMON_INFO) const
    {
        itemPtr->EnchantmentAdd( EnchantmentWarehouse::Instance()->Store( new Enchantment(
            TYPE,
            TRAIT_SET,
            USE_INFO,
            SUMMON_INFO) ) );
    }

    /*
    NotNamed,
    Wicked,
    Fiendish,
    Infernal,
    Raging,
    Diabolic,
    Dancing,
    Marauder,
    Honest,
    Noble,
    Daring,
    Elite,
    Valiant,
    Heros,
    Army,
    Gladiator,
    Soldiers,
    Wardens,
    Princes,
    Rangers,
    Samurai,
    Thors,
    Nile,
    Searing,
    Burning,
    Fiery,
    Charred,
    Icy,
    Winter,
    Frigid,
    Chill,
    Frozen,
    Arctic,
    Proud,
    Glory,
    Pure,
    Light,
    Dawn,
    Sun,
    Gloom,
    Twilight,
    Dusk,
    Sorrow,
    Woe,
    Misery,
    Moon,
    Dark,
    Betrayer,
    Imposters,
    Pickpocket,
    Burglar,
    Mountebank,
    Charlatans,
    Focus,
    Count
    */

}
}
