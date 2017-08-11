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

#include "game/log-macros.hpp"
#include "game/item/item.hpp"
#include "game/item/item-warehouse.hpp"
#include "game/item/item-profile.hpp"
#include "game/item/weapon-info.hpp"
#include "game/item/weapon-details.hpp"
#include "game/creature/enchantment-factory.hpp"

#include "misc/boost-string-includes.hpp"
#include "misc/assertlogandthrow.hpp"

#include <sstream>
#include <exception>


namespace game
{
namespace item
{
namespace misc
{

    std::unique_ptr<MiscItemFactory> MiscItemFactory::instanceUPtr_{ nullptr };


    MiscItemFactory::MiscItemFactory()
    {
        M_HP_LOG_DBG("Singleton Construction: MiscItemFactory");
    }


    MiscItemFactory::~MiscItemFactory()
    {
        M_HP_LOG_DBG("Singleton Destruction: MiscItemFactory");
    }


    MiscItemFactory * MiscItemFactory::Instance()
    {
        if (instanceUPtr_.get() == nullptr)
        {
            M_HP_LOG_WRN("Singleton Instance() before Acquire(): MiscItemFactory");
            Acquire();
        }

        return instanceUPtr_.get();
    }


    void MiscItemFactory::Acquire()
    {
        if (instanceUPtr_.get() == nullptr)
        {
            instanceUPtr_.reset(new MiscItemFactory);
        }
        else
        {
            M_HP_LOG_WRN("Singleton Acquire() after Construction: MiscItemFactory");
        }
    }


    void MiscItemFactory::Release()
    {
        M_ASSERT_OR_LOGANDTHROW_SS((instanceUPtr_.get() != nullptr),
            "game::item::misc::MiscItemFactory::Release() found instanceUPtr that was null.");

        instanceUPtr_.reset();
    }


    ItemPtr_t MiscItemFactory::Make(const ItemProfile & PROFILE)
    {
        if (PROFILE.MiscType() == misc_type::Balm_Pot)          { return Make_BalmPot(PROFILE); }
        if (PROFILE.MiscType() == misc_type::Bust)              { return Make_Bust(PROFILE); }
        if (PROFILE.MiscType() == misc_type::Devil_Horn)        { return Make_DevilHorn(PROFILE); }
        if (PROFILE.MiscType() == misc_type::Doll_Blessed)      { return Make_DollBlessed(PROFILE); }
        if (PROFILE.MiscType() == misc_type::Doll_Cursed)       { return Make_DollCursed(PROFILE); }
        if (PROFILE.MiscType() == misc_type::Dried_Head)        { return Make_DriedHead(PROFILE); }
        if (PROFILE.MiscType() == misc_type::DrumLute)          { return Make_DrumLute(PROFILE); }
        if (PROFILE.MiscType() == misc_type::Egg)               { return Make_Egg(PROFILE); }
        if (PROFILE.MiscType() == misc_type::Embryo)            { return Make_Embryo(PROFILE); }
        if (PROFILE.MiscType() == misc_type::Figurine_Blessed)  { return Make_FigurineBlessed(PROFILE); }
        if (PROFILE.MiscType() == misc_type::Figurine_Cursed)   { return Make_FigurineCursed(PROFILE); }
        if (PROFILE.MiscType() == misc_type::Finger)            { return Make_Finger(PROFILE); }
        if (PROFILE.MiscType() == misc_type::Goblet)            { return Make_Goblet(PROFILE); }
        if (PROFILE.MiscType() == misc_type::Hurdy_Gurdy)       { return Make_HurdyGurdy(PROFILE); }
        if (PROFILE.MiscType() == misc_type::Icicle)            { return Make_Icicle(PROFILE); }
        if (PROFILE.MiscType() == misc_type::LockPicks)         { return Make_LockPicks(PROFILE); }
        if (PROFILE.MiscType() == misc_type::Lyre)              { return Make_Lyre(PROFILE); }
        if (PROFILE.MiscType() == misc_type::Mummy_Hand)        { return Make_MummyHand(PROFILE); }
        if (PROFILE.MiscType() == misc_type::Orb)               { return Make_Orb(PROFILE); }
        if (PROFILE.MiscType() == misc_type::Petrified_Snake)   { return Make_PetrifiedSnake(PROFILE); }
        if (PROFILE.MiscType() == misc_type::Pipe_And_Tabor)    { return Make_PipeAndTabor(PROFILE); }
        if (PROFILE.MiscType() == misc_type::Puppet)            { return Make_Puppet(PROFILE); }
        if (PROFILE.MiscType() == misc_type::Recorder)          { return Make_Recorder(PROFILE); }
        if (PROFILE.MiscType() == misc_type::Ring)              { return Make_Ring(PROFILE); }
        if (PROFILE.MiscType() == misc_type::Scepter)           { return Make_Scepter(PROFILE); }
        if (PROFILE.MiscType() == misc_type::Seeds)             { return Make_Seeds(PROFILE); }
        if (PROFILE.MiscType() == misc_type::Shard)             { return Make_Shard(PROFILE); }
        if (PROFILE.MiscType() == misc_type::Spider_Eggs)       { return Make_SpiderEggs(PROFILE); }
        if (PROFILE.MiscType() == misc_type::Staff)             { return Make_Staff(PROFILE); }
        if (PROFILE.MiscType() == misc_type::Unicorn_Horn)      { return Make_UnicornHorn(PROFILE); }
        if (PROFILE.MiscType() == misc_type::Viol)              { return Make_Viol(PROFILE); }
        if (PROFILE.MiscType() == misc_type::Wand)              { return Make_Wand(PROFILE); }
        
        M_HP_LOG_ERR("game::item::misc::MiscItemFactory::Make(profile=[" << PROFILE.ToString()
            << "]) was unable to make an item with that profile.");

        return nullptr;
    }


    ItemPtr_t MiscItemFactory::Make_BalmPot(const ItemProfile & PROFILE)
    {
        return Make_Helper(PROFILE, 746, 233);
    }


    ItemPtr_t MiscItemFactory::Make_Bust(const ItemProfile & PROFILE)
    {
        return Make_Helper(PROFILE, 231, 188);
    }


    ItemPtr_t MiscItemFactory::Make_DevilHorn(const ItemProfile & PROFILE)
    {
        return Make_Helper(PROFILE, PROFILE.TreasureScore(), 85);
    }


    ItemPtr_t MiscItemFactory::Make_DollBlessed(const ItemProfile & PROFILE)
    {
        return Make_Helper(PROFILE, PROFILE.TreasureScore(), 38);
    }


    ItemPtr_t MiscItemFactory::Make_DollCursed(const ItemProfile & PROFILE)
    {
        return Make_Helper(PROFILE, PROFILE.TreasureScore(), 38);
    }


    ItemPtr_t MiscItemFactory::Make_DriedHead(const ItemProfile & PROFILE)
    {
        return Make_Helper(PROFILE, PROFILE.TreasureScore(), 38);
    }


    ItemPtr_t MiscItemFactory::Make_DrumLute(const ItemProfile & PROFILE)
    {
        return Make_Helper(PROFILE,
                           1013,
                           364,
                           "An acoustic guitar with a round hollow body that can be drummed");
    }


    ItemPtr_t MiscItemFactory::Make_Egg(const ItemProfile & PROFILE)
    {
        return Make_Helper(PROFILE, PROFILE.TreasureScore(), 153);
    }


    ItemPtr_t MiscItemFactory::Make_Embryo(const ItemProfile & PROFILE)
    {
        return Make_Helper(PROFILE, PROFILE.TreasureScore(), 41);
    }


    ItemPtr_t MiscItemFactory::Make_FigurineBlessed(const ItemProfile & PROFILE)
    {
        return Make_Helper(PROFILE, PROFILE.TreasureScore(), 38);
    }


    ItemPtr_t MiscItemFactory::Make_FigurineCursed(const ItemProfile & PROFILE)
    {
        return Make_Helper(PROFILE, PROFILE.TreasureScore(), 38);
    }


    ItemPtr_t MiscItemFactory::Make_Finger(const ItemProfile & PROFILE)
    {
        return Make_Helper(PROFILE, PROFILE.TreasureScore(), 51);
    }


    ItemPtr_t MiscItemFactory::Make_Goblet(const ItemProfile & PROFILE)
    {
        return Make_Helper(PROFILE, PROFILE.TreasureScore(), 44);
    }


    ItemPtr_t MiscItemFactory::Make_HurdyGurdy(const ItemProfile & PROFILE)
    {
        return Make_Helper(PROFILE, PROFILE.TreasureScore(), 446);
    }


    ItemPtr_t MiscItemFactory::Make_Icicle(const ItemProfile & PROFILE)
    {
        return Make_Helper(PROFILE, PROFILE.TreasureScore(), 26);
    }


    ItemPtr_t MiscItemFactory::Make_LockPicks(const ItemProfile & PROFILE)
    {
        return Make_Helper(PROFILE, PROFILE.TreasureScore(), 33);
    }


    ItemPtr_t MiscItemFactory::Make_Lyre(const ItemProfile & PROFILE)
    {
        return Make_Helper(PROFILE, PROFILE.TreasureScore(), 54);
    }


    ItemPtr_t MiscItemFactory::Make_MummyHand(const ItemProfile & PROFILE)
    {
        return Make_Helper(PROFILE, PROFILE.TreasureScore(), 38);
    }


    ItemPtr_t MiscItemFactory::Make_Orb(const ItemProfile & PROFILE)
    {
        return Make_Helper(PROFILE, PROFILE.TreasureScore(), 87);
    }


    ItemPtr_t MiscItemFactory::Make_PetrifiedSnake(const ItemProfile & PROFILE)
    {
        return Make_Helper(PROFILE, PROFILE.TreasureScore(), 32);
    }


    ItemPtr_t MiscItemFactory::Make_PipeAndTabor(const ItemProfile & PROFILE)
    {
        return Make_Helper(PROFILE, PROFILE.TreasureScore(), 127);
    }


    ItemPtr_t MiscItemFactory::Make_Puppet(const ItemProfile & PROFILE)
    {
        return Make_Helper(PROFILE, PROFILE.TreasureScore(), 38);
    }


    ItemPtr_t MiscItemFactory::Make_Recorder(const ItemProfile & PROFILE)
    {
        return Make_Helper(PROFILE, PROFILE.TreasureScore(), 36);
    }


    ItemPtr_t MiscItemFactory::Make_Ring(const ItemProfile & PROFILE)
    {
        return Make_Helper(PROFILE, PROFILE.TreasureScore(), 20);
    }


    ItemPtr_t MiscItemFactory::Make_Scepter(const ItemProfile & PROFILE)
    {
        return Make_Helper(PROFILE, PROFILE.TreasureScore(), 48);
    }


    ItemPtr_t MiscItemFactory::Make_Seeds(const ItemProfile & PROFILE)
    {
        return Make_Helper(PROFILE, PROFILE.TreasureScore(), 12);
    }


    ItemPtr_t MiscItemFactory::Make_Shard(const ItemProfile & PROFILE)
    {
        return Make_Helper(PROFILE, PROFILE.TreasureScore(), 49);
    }


    ItemPtr_t MiscItemFactory::Make_SpiderEggs(const ItemProfile & PROFILE)
    {
        return Make_Helper(PROFILE, PROFILE.TreasureScore(), 12);
    }


    ItemPtr_t MiscItemFactory::Make_Staff(const ItemProfile & PROFILE)
    {
        auto price{ PROFILE.TreasureScore() };
        AdjustPrice(price,
                    PROFILE.MaterialPrimary(),
                    PROFILE.MaterialSecondary(),
                    PROFILE.IsPixie());

        auto weight{ 47 };
        AdjustWeight(weight,
                     PROFILE.MaterialPrimary(),
                     PROFILE.MaterialSecondary());

        auto const BASE_NAME{ item::misc_type::Name(PROFILE.MiscType()) };

        auto const DESC_PREFIX{ "A " + boost::to_lower_copy(BASE_NAME) + " " };

        weapon::WeaponInfo weaponInfo(weapon_type::Staff);
        weaponInfo.is_staff = true;
        weaponInfo.is_quarterstaff = false;
        auto const DETAILS{ weapon::WeaponDetailLoader::Instance()->LookupWeaponDetails("Staff") };

        auto itemPtr{ ItemWarehouse::Instance()->Store( new Item(
            Make_Name(BASE_NAME,
                      PROFILE.MaterialPrimary(),
                      PROFILE.MaterialSecondary(),
                      PROFILE.IsPixie()),
            Make_Desc(DESC_PREFIX,
                      PROFILE.MaterialPrimary(),
                      PROFILE.MaterialSecondary(),
                      "",
                      PROFILE.IsPixie()),
            PROFILE.Category(),
            PROFILE.MiscType(),
            PROFILE.WeaponType(),
            armor_type::NotArmor,
            PROFILE.MaterialPrimary(),
            PROFILE.MaterialSecondary(),
            "",
            price,
            weight,
            DETAILS.damage_min,
            DETAILS.damage_max,
            0,
            creature::role::Count,
            weaponInfo,
            armor::ArmorInfo(),
            PROFILE.IsPixie(),
            PROFILE.UniqueType(),
            PROFILE.SetType(),
            PROFILE.NamedType(),
            PROFILE.ElementType()) ) };

        itemPtr->ImageFilename(sfml_util::gui::ItemImageManager::Instance()->
            GetImageFilename(itemPtr));

        creature::EnchantmentFactory::Instance()->MakeStoreAttachReturn(itemPtr);
        
        return itemPtr;
    }


    ItemPtr_t MiscItemFactory::Make_UnicornHorn(const ItemProfile & PROFILE)
    {
        return Make_Helper(PROFILE, PROFILE.TreasureScore(), 28);
    }


    ItemPtr_t MiscItemFactory::Make_Viol(const ItemProfile & PROFILE)
    {
        return Make_Helper(PROFILE, PROFILE.TreasureScore(), 233);
    }


    ItemPtr_t MiscItemFactory::Make_Wand(const ItemProfile & PROFILE)
    {
        return Make_Helper(PROFILE, PROFILE.TreasureScore(), 30);
    }


    ItemPtr_t MiscItemFactory::Make_Ring(const material::Enum MATERIAL_PRI,
                                         const material::Enum MATERIAL_SEC)
    {
        stats::Trait_t price(5);
        AdjustPrice(price, MATERIAL_PRI, MATERIAL_SEC);

        stats::Trait_t weight(20);
        AdjustWeight(weight, MATERIAL_PRI, MATERIAL_SEC);

        auto itemPtr{ ItemWarehouse::Instance()->Store( new Item(
            Make_Name("Ring", MATERIAL_PRI, MATERIAL_SEC),
            Make_Desc("A ring", MATERIAL_PRI, MATERIAL_SEC, ""),
            static_cast<category::Enum>(category::Equippable | category::Wearable),
            misc_type::Ring,
            weapon_type::NotAWeapon,
            armor_type::NotArmor,
            MATERIAL_PRI,
            MATERIAL_SEC,
            "",
            price,
            weight) ) };

        itemPtr->ImageFilename(sfml_util::gui::ItemImageManager::Instance()->
             GetImageFilename(itemPtr));
 
        return itemPtr;
    }


    ItemPtr_t MiscItemFactory::Make_Wand(const material::Enum MATERIAL_PRI,
                                         const material::Enum MATERIAL_SEC)
    {
        stats::Trait_t price(437);
        AdjustPrice(price, MATERIAL_PRI, MATERIAL_SEC);

        stats::Trait_t weight(40);
        AdjustWeight(weight, MATERIAL_PRI, MATERIAL_SEC);

        auto itemPtr{ ItemWarehouse::Instance()->Store( new Item(
            Make_Name("Wand", MATERIAL_PRI, MATERIAL_SEC),
            Make_Desc("A wand", MATERIAL_PRI, MATERIAL_SEC),
            static_cast<category::Enum>(category::Equippable),
            misc_type::Wand,
            weapon_type::NotAWeapon,
            armor_type::NotArmor,
            MATERIAL_PRI,
            MATERIAL_SEC,
            "",
            price,
            weight) ) };

        itemPtr->ImageFilename(sfml_util::gui::ItemImageManager::Instance()->
            GetImageFilename(itemPtr));
        
        return itemPtr;
    }


    ItemPtr_t MiscItemFactory::Make_DrumLute(const bool IS_PIXIE_ITEM)
    {
        stats::Trait_t price(1000);
        stats::Trait_t weight(350);
        std::string desc("An acoustic guitar with a round hollow body that can be drummed.");
        
        AdjustPrice(price, material::Wood, material::Rope, IS_PIXIE_ITEM);
        AdjustWeight(weight, material::Wood, material::Rope);

        auto itemPtr{ ItemWarehouse::Instance()->Store(new Item(
            Make_Name("DrumLute", material::Wood, material::Rope, IS_PIXIE_ITEM),
            Make_Desc(desc, material::Wood, material::Rope, "", IS_PIXIE_ITEM),
            static_cast<category::Enum>(category::Equippable |
                                        category::Useable |
                                        category::TwoHanded),
            misc_type::DrumLute,
            weapon_type::NotAWeapon,
            armor_type::NotArmor,
            material::Wood,
            material::Rope,
            "",
            price,
            weight,
            0,
            0,
            0,
            creature::role::Bard,
            weapon::WeaponInfo(),
            armor::ArmorInfo(),
            IS_PIXIE_ITEM) ) };

        itemPtr->ImageFilename(sfml_util::gui::ItemImageManager::Instance()->
             GetImageFilename(itemPtr));
 
        creature::EnchantmentFactory::Instance()->MakeStoreAttachReturn(itemPtr);
        
        return itemPtr;
    }


    ItemPtr_t MiscItemFactory::Make_Helper(const ItemProfile &  PROFILE,
                                           const stats::Trait_t BASE_PRICE,
                                           const stats::Trait_t BASE_WEIGHT,
                                           const std::string &  BASE_DESC)
    {
        auto price{ BASE_PRICE };
        AdjustPrice(price,
                    PROFILE.MaterialPrimary(),
                    PROFILE.MaterialSecondary(),
                    PROFILE.IsPixie());

        auto weight{ BASE_WEIGHT };
        AdjustWeight(weight,
                     PROFILE.MaterialPrimary(),
                     PROFILE.MaterialSecondary());

        auto const BASE_NAME{ item::misc_type::Name(PROFILE.MiscType()) };

        auto const DESC_PREFIX{ ((BASE_DESC.empty()) ?
            ("A " + boost::to_lower_copy(BASE_NAME) + " ") :
            BASE_DESC) };

        auto itemPtr{ ItemWarehouse::Instance()->Store( new Item(
            Make_Name(BASE_NAME,
                      PROFILE.MaterialPrimary(),
                      PROFILE.MaterialSecondary(),
                      PROFILE.IsPixie()),
            Make_Desc(DESC_PREFIX,
                      PROFILE.MaterialPrimary(),
                      PROFILE.MaterialSecondary(),
                      "",
                      PROFILE.IsPixie()),
            PROFILE.Category(),
            PROFILE.MiscType(),
            weapon_type::NotAWeapon,
            armor_type::NotArmor,
            PROFILE.MaterialPrimary(),
            PROFILE.MaterialSecondary(),
            "",
            price,
            weight,
            0,
            0,
            0,
            PROFILE.Role(),
            weapon::WeaponInfo(),
            armor::ArmorInfo(),
            PROFILE.IsPixie(),
            PROFILE.UniqueType(),
            PROFILE.SetType(),
            PROFILE.NamedType(),
            PROFILE.ElementType()) ) };

        itemPtr->ImageFilename(sfml_util::gui::ItemImageManager::Instance()->
            GetImageFilename(itemPtr));

        creature::EnchantmentFactory::Instance()->MakeStoreAttachReturn(itemPtr);
        
        return itemPtr;
    }

}
}
}
