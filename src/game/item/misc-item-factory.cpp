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
// misc-item-factory.cpp
//
#include "misc-item-factory.hpp"

#include "sfml-util/gui/item-image-manager.hpp"

#include "game/item/item.hpp"
#include "game/item/item-warehouse.hpp"
#include "game/log-macros.hpp"

#include "misc/boost-string-includes.hpp"
#include "misc/assertlogandthrow.hpp"

#include <exception>
#include <sstream>


namespace game
{
namespace item
{
namespace misc
{

    std::unique_ptr<ItemFactory> ItemFactory::instanceUPtr_{ nullptr };


    ItemFactory::ItemFactory()
    {
        M_HP_LOG_DBG("Singleton Construction: ItemFactory");
    }


    ItemFactory::~ItemFactory()
    {
        M_HP_LOG_DBG("Singleton Destruction: ItemFactory");
    }


    ItemFactory * ItemFactory::Instance()
    {
        if (instanceUPtr_.get() == nullptr)
        {
            M_HP_LOG_WRN("Singleton Instance() before Acquire(): ItemFactory");
            Acquire();
        }

        return instanceUPtr_.get();
    }


    void ItemFactory::Acquire()
    {
        if (instanceUPtr_.get() == nullptr)
        {
            instanceUPtr_.reset(new ItemFactory);
        }
        else
        {
            M_HP_LOG_WRN("Singleton Acquire() after Construction: ItemFactory");
        }
    }


    void ItemFactory::Release()
    {
        M_ASSERT_OR_LOGANDTHROW_SS((instanceUPtr_.get() != nullptr),
            "game::item::misc::ItemFactory::Release() found instanceUPtr that was null.");
        instanceUPtr_.reset();
    }


    ItemPtr_t ItemFactory::Make_Ring(const material::Enum MATERIAL_PRI,
                                     const material::Enum MATERIAL_SEC,
                                     const bool           IS_PIXIE_ITEM)
    {
        Coin_t price(5);
        AdjustPrice(price, MATERIAL_PRI, MATERIAL_SEC, IS_PIXIE_ITEM);

        Weight_t weight(20);
        AdjustWeight(weight, MATERIAL_PRI, MATERIAL_SEC);

        std::string imageFilename("ring-plain");
        if (material::IsPrecious(MATERIAL_SEC))
        {
            imageFilename = "ring-jeweled";
        }

        return ItemWarehouse::Instance()->Store( new Item(
            Make_Name("Ring", MATERIAL_PRI, MATERIAL_SEC, IS_PIXIE_ITEM),
            Make_Desc("A ring", MATERIAL_PRI, MATERIAL_SEC, "", IS_PIXIE_ITEM),
            static_cast<category::Enum>(category::Equippable | category::Wearable),
            misc_type::Ring,
            weapon_type::NotAWeapon,
            armor_type::NotArmor,
            MATERIAL_PRI,
            MATERIAL_SEC,
            imageFilename,
            price,
            weight,
            0,
            0,
            0,
            creature::role::Count,
            weapon::WeaponInfo(),
            armor::ArmorInfo(),
            IS_PIXIE_ITEM) );
    }


    ItemPtr_t ItemFactory::Make_Wand(const material::Enum MATERIAL_PRI,
                                     const material::Enum MATERIAL_SEC,
                                     const bool           IS_PIXIE_ITEM)
    {
        Coin_t price(437);
        AdjustPrice(price, MATERIAL_PRI, MATERIAL_SEC, IS_PIXIE_ITEM);

        Weight_t weight(40);
        AdjustWeight(weight, MATERIAL_PRI, MATERIAL_SEC);

        return ItemWarehouse::Instance()->Store( new Item(
            Make_Name("Wand", MATERIAL_PRI, MATERIAL_SEC, IS_PIXIE_ITEM),
            Make_Desc("A wand", MATERIAL_PRI, MATERIAL_SEC, "", IS_PIXIE_ITEM),
            static_cast<category::Enum>(category::Equippable |
                                        category::Magical |
                                        category::EnchantsOnlyWhenEquipped),
            misc_type::Wand,
            weapon_type::NotAWeapon,
            armor_type::NotArmor,
            MATERIAL_PRI,
            MATERIAL_SEC,
            boost::algorithm::erase_all_copy(
                sfml_util::gui::ItemImageManager::Instance()->GetImageFilename(
                    item::misc_type::Wand, material::IsJewel(MATERIAL_SEC), true),
                sfml_util::gui::ItemImageManager::FileExtension()),
            price,
            weight,
            0,
            0,
            0,
            creature::role::Count,
            weapon::WeaponInfo(),
            armor::ArmorInfo(),
            IS_PIXIE_ITEM) );
    }


    ItemPtr_t ItemFactory::Make_Instrument(const instrument_type::Enum TYPE,
                                           const bool                  IS_PIXIE_ITEM)
    {
        Coin_t price(0);
        Weight_t weight(0);
        std::string name("");
        std::string desc("");
        misc_type::Enum miscType(misc_type::Count);
        std::string imageFilename("");
        category::Enum categoryHanded(category::OneHanded);

        switch (TYPE)
        {
            case instrument_type::Panflute:
            {
                name = "Panflute";
                desc = "A set of pipes at varying length bound together";
                price = 100;
                weight = 150;
                categoryHanded = category::OneHanded;
                imageFilename = "panflute";
                miscType = misc_type::Panflute;
                break;
            }
            case instrument_type::Flute:
            {
                name = "Flute";
                desc = "A single hollow length";
                price = 500;
                weight = 200;
                categoryHanded = category::TwoHanded;
                imageFilename = "flute";
                miscType = misc_type::Flute;
                break;
            }
            case instrument_type::Lute:
            {
                name = "Lute";
                desc = "A type of guitar with a round body";
                price = 1000;
                weight = 350;
                categoryHanded = category::TwoHanded;
                imageFilename = "lute";
                miscType = misc_type::Lute;
                break;
            }
            case instrument_type::Sitar:
            {
                name = "Sitar";
                desc = "A type of guitar with a long neck";
                price = 2000;
                weight = 400;
                categoryHanded = category::TwoHanded;
                imageFilename = "sitar";
                miscType = misc_type::Sitar;
                break;
            }
            case instrument_type::Count:
            default:
            {
                std::ostringstream ss;
                ss << "herospath::item::ItemFactory::Make_Instrument("
                    << TYPE << ") -but that type is invalid.";
                throw std::range_error(ss.str());
            }
        }

        AdjustPrice(price, material::Wood, material::Rope, IS_PIXIE_ITEM);
        AdjustWeight(weight, material::Wood, material::Rope);

        return ItemWarehouse::Instance()->Store( new Item(
            Make_Name(name, material::Wood, material::Rope, IS_PIXIE_ITEM),
            Make_Desc(desc, material::Wood, material::Rope, "", IS_PIXIE_ITEM),
            static_cast<category::Enum>(category::Equippable | category::Useable | categoryHanded),
            miscType,
            weapon_type::NotAWeapon,
            armor_type::NotArmor,
            material::Wood,
            material::Rope,
            imageFilename,
            price,
            weight,
            0,
            0,
            0,
            creature::role::Bard,
            weapon::WeaponInfo(),
            armor::ArmorInfo(),
            IS_PIXIE_ITEM));
    }

}
}
}
