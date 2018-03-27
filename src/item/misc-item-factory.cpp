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

#include "creature/enchantment-factory.hpp"
#include "item/armor-details.hpp"
#include "item/item-profile.hpp"
#include "item/item-warehouse.hpp"
#include "item/item.hpp"
#include "item/weapon-details.hpp"
#include "item/weapon-info.hpp"
#include "log/log-macros.hpp"
#include "misc/assertlogandthrow.hpp"
#include "misc/boost-string-includes.hpp"
#include "misc/random.hpp"
#include "sfml-util/gui/item-image-manager.hpp"

#include <exception>
#include <sstream>

namespace heroespath
{
namespace item
{

    std::unique_ptr<MiscItemFactory> MiscItemFactory::instanceUPtr_{ nullptr };

    MiscItemFactory::MiscItemFactory() { M_HP_LOG_DBG("Singleton Construction: MiscItemFactory"); }

    MiscItemFactory::~MiscItemFactory() { M_HP_LOG_DBG("Singleton Destruction: MiscItemFactory"); }

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
            instanceUPtr_ = std::make_unique<MiscItemFactory>();
        }
        else
        {
            M_HP_LOG_WRN("Singleton Acquire() after Construction: MiscItemFactory");
        }
    }

    void MiscItemFactory::Release()
    {
        M_ASSERT_OR_LOGANDTHROW_SS(
            (instanceUPtr_.get() != nullptr),
            "item::MiscItemFactory::Release() found instanceUPtr that was null.");

        instanceUPtr_.reset();
    }

    const ItemPtr_t MiscItemFactory::Make(const ItemProfile & PROFILE)
    {
        auto const MISC_ENUM{ PROFILE.MiscType() };

        if (MISC_ENUM == misc_type::Amulet)
        {
            return Make_Amulet(PROFILE);
        }
        if (MISC_ENUM == misc_type::Armband)
        {
            return Make_Armband(PROFILE);
        }
        if (MISC_ENUM == misc_type::Veil)
        {
            return Make_Veil(PROFILE);
        }
        if (MISC_ENUM == misc_type::Balm_Pot)
        {
            return Make_BalmPot(PROFILE);
        }
        if (MISC_ENUM == misc_type::Braid)
        {
            return Make_Braid(PROFILE);
        }
        if (MISC_ENUM == misc_type::Cape)
        {
            return Make_Cape(PROFILE);
        }
        if (MISC_ENUM == misc_type::Devil_Horn)
        {
            return Make_DevilHorn(PROFILE);
        }
        if (MISC_ENUM == misc_type::Doll)
        {
            return Make_Doll(PROFILE);
        }
        if (MISC_ENUM == misc_type::DrumLute)
        {
            return Make_DrumLute(PROFILE);
        }
        if (MISC_ENUM == misc_type::Egg)
        {
            return Make_Egg(PROFILE);
        }
        if (MISC_ENUM == misc_type::Embryo)
        {
            return Make_Embryo(PROFILE);
        }
        if (MISC_ENUM == misc_type::Finger)
        {
            return Make_Finger(PROFILE);
        }
        if (MISC_ENUM == misc_type::Fingerclaw)
        {
            return Make_Fingerclaw(PROFILE);
        }
        if (MISC_ENUM == misc_type::Goblet)
        {
            return Make_Goblet(PROFILE);
        }
        if (MISC_ENUM == misc_type::Hurdy_Gurdy)
        {
            return Make_HurdyGurdy(PROFILE);
        }
        if (MISC_ENUM == misc_type::Icicle)
        {
            return Make_Icicle(PROFILE);
        }
        if (MISC_ENUM == misc_type::Litch_Hand)
        {
            return Make_Litchhand(PROFILE);
        }
        if (MISC_ENUM == misc_type::LockPicks)
        {
            return Make_LockPicks(PROFILE);
        }
        if (MISC_ENUM == misc_type::Lyre)
        {
            return Make_Lyre(PROFILE);
        }
        if (MISC_ENUM == misc_type::Mummy_Hand)
        {
            return Make_MummyHand(PROFILE);
        }
        if (MISC_ENUM == misc_type::Orb)
        {
            return Make_Orb(PROFILE);
        }
        if (MISC_ENUM == misc_type::Petrified_Snake)
        {
            return Make_PetrifiedSnake(PROFILE);
        }
        if (MISC_ENUM == misc_type::Pipe_And_Tabor)
        {
            return Make_PipeAndTabor(PROFILE);
        }
        if (MISC_ENUM == misc_type::Recorder)
        {
            return Make_Recorder(PROFILE);
        }
        if (MISC_ENUM == misc_type::Ring)
        {
            return Make_Ring(PROFILE);
        }
        if (MISC_ENUM == misc_type::Scepter)
        {
            return Make_Scepter(PROFILE);
        }
        if (MISC_ENUM == misc_type::Seeds)
        {
            return Make_Seeds(PROFILE);
        }
        if (MISC_ENUM == misc_type::Shard)
        {
            return Make_Shard(PROFILE);
        }
        if (MISC_ENUM == misc_type::Spider_Eggs)
        {
            return Make_SpiderEggs(PROFILE);
        }
        if (MISC_ENUM == misc_type::Staff)
        {
            return Make_Staff(PROFILE);
        }
        if (MISC_ENUM == misc_type::Summoning_Statue)
        {
            return Make_SummoningStatue(PROFILE);
        }
        if (MISC_ENUM == misc_type::Unicorn_Horn)
        {
            return Make_UnicornHorn(PROFILE);
        }
        if (MISC_ENUM == misc_type::Viol)
        {
            return Make_Viol(PROFILE);
        }
        if (MISC_ENUM == misc_type::Wand)
        {
            return Make_Wand(PROFILE);
        }

        if (misc_type::IsBlessed(MISC_ENUM) || misc_type::IsCursed(MISC_ENUM))
        {
            return Make_MiscBlessedOrCursed(PROFILE);
        }

        std::ostringstream ss;
        ss << "item::MiscItemFactory::Make(profile=" << PROFILE.ToString()
           << ") was unable to make an item with that profile.";

        throw std::runtime_error(ss.str());
    }

    const ItemPtr_t MiscItemFactory::Make_Amulet(const ItemProfile & PROFILE)
    {
        return Make_Helper(PROFILE, TreasureScoreToCoins(PROFILE.TreasureScore()), 42_weight);
    }

    const ItemPtr_t MiscItemFactory::Make_Armband(const ItemProfile & PROFILE)
    {
        return Make_Helper(PROFILE, TreasureScoreToCoins(PROFILE.TreasureScore()), 62_weight);
    }

    const ItemPtr_t MiscItemFactory::Make_BalmPot(const ItemProfile & PROFILE)
    {
        return Make_Helper(PROFILE, TreasureScoreToCoins(PROFILE.TreasureScore()), 233_weight);
    }

    const ItemPtr_t MiscItemFactory::Make_Braid(const ItemProfile & PROFILE)
    {
        return Make_Helper(PROFILE, TreasureScoreToCoins(PROFILE.TreasureScore()), 16_weight);
    }

    const ItemPtr_t MiscItemFactory::Make_Cape(const ItemProfile & PROFILE)
    {
        Coin_t price{ TreasureScoreToCoins(PROFILE.TreasureScore()) };
        AdjustPrice(
            price, PROFILE.MaterialPrimary(), PROFILE.MaterialSecondary(), PROFILE.IsPixie());

        Weight_t weight{ 111_weight };
        AdjustWeight(weight, PROFILE.MaterialPrimary(), PROFILE.MaterialSecondary());

        auto const BASE_NAME{ [PROFILE]() {
            if ((PROFILE.UniqueType() == unique_type::Count)
                || (PROFILE.UniqueType() == unique_type::NotUnique))
            {
                return misc_type::Name(PROFILE.MiscType());
            }
            else
            {
                return unique_type::Name(PROFILE.UniqueType());
            }
        }() };

        auto const DESC_PREFIX{ "A " + boost::to_lower_copy(BASE_NAME) + " " };

        armor::ArmorInfo armorInfo(armor_type::Covering);
        armorInfo.cover = armor::cover_type::Cape;

        auto const DETAILS{ armor::ArmorDetailLoader::Instance()->LookupArmorDetails(
            armor::cover_type::ToString(armor::cover_type::Cape)) };

        auto itemPtr{ ItemWarehouse::Instance()->Store(new Item(
            Make_Name(
                BASE_NAME,
                PROFILE.MaterialPrimary(),
                PROFILE.MaterialSecondary(),
                PROFILE.IsPixie()),
            Make_Desc(
                DESC_PREFIX,
                PROFILE.MaterialPrimary(),
                PROFILE.MaterialSecondary(),
                "",
                PROFILE.IsPixie()),
            PROFILE.Category(),
            PROFILE.MiscType(),
            weapon_type::NotAWeapon,
            PROFILE.ArmorType(),
            PROFILE.MaterialPrimary(),
            PROFILE.MaterialSecondary(),
            "",
            price,
            weight,
            0_health,
            0_health,
            DETAILS.armor_rating,
            creature::role::Count,
            weapon::WeaponInfo(),
            armorInfo,
            PROFILE.IsPixie(),
            PROFILE.UniqueType(),
            PROFILE.SetType(),
            PROFILE.NamedType(),
            PROFILE.ElementType())) };

        itemPtr->ImageFilename(
            sfml_util::gui::ItemImageManager::Instance()->GetImageFilename(itemPtr));

        creature::EnchantmentFactory::Instance()->MakeStoreAttachReturn(itemPtr);

        return itemPtr;
    }

    const ItemPtr_t MiscItemFactory::Make_DevilHorn(const ItemProfile & PROFILE)
    {
        return Make_Helper(PROFILE, TreasureScoreToCoins(PROFILE.TreasureScore()), 85_weight);
    }

    const ItemPtr_t MiscItemFactory::Make_Doll(const ItemProfile & PROFILE)
    {
        return Make_Helper(PROFILE, TreasureScoreToCoins(PROFILE.TreasureScore()), 38_weight);
    }

    const ItemPtr_t MiscItemFactory::Make_DrumLute(const ItemProfile & PROFILE)
    {
        return Make_Helper(
            PROFILE,
            1013_coin,
            364_weight,
            "An acoustic guitar with a round hollow body that can be drummed");
    }

    const ItemPtr_t MiscItemFactory::Make_Egg(const ItemProfile & PROFILE)
    {
        return Make_Helper(PROFILE, TreasureScoreToCoins(PROFILE.TreasureScore()), 153_weight);
    }

    const ItemPtr_t MiscItemFactory::Make_Embryo(const ItemProfile & PROFILE)
    {
        return Make_Helper(PROFILE, TreasureScoreToCoins(PROFILE.TreasureScore()), 41_weight);
    }

    const ItemPtr_t MiscItemFactory::Make_Finger(const ItemProfile & PROFILE)
    {
        return Make_Helper(PROFILE, TreasureScoreToCoins(PROFILE.TreasureScore()), 51_weight);
    }

    const ItemPtr_t MiscItemFactory::Make_Fingerclaw(const ItemProfile & PROFILE)
    {
        return Make_Helper(PROFILE, TreasureScoreToCoins(PROFILE.TreasureScore()), 30_weight);
    }

    const ItemPtr_t MiscItemFactory::Make_Goblet(const ItemProfile & PROFILE)
    {
        return Make_Helper(PROFILE, TreasureScoreToCoins(PROFILE.TreasureScore()), 44_weight);
    }

    const ItemPtr_t MiscItemFactory::Make_HurdyGurdy(const ItemProfile & PROFILE)
    {
        return Make_Helper(PROFILE, TreasureScoreToCoins(PROFILE.TreasureScore()), 446_weight);
    }

    const ItemPtr_t MiscItemFactory::Make_Icicle(const ItemProfile & PROFILE)
    {
        return Make_Helper(PROFILE, TreasureScoreToCoins(PROFILE.TreasureScore()), 26_weight);
    }

    const ItemPtr_t MiscItemFactory::Make_Litchhand(const ItemProfile & PROFILE)
    {
        return Make_Helper(PROFILE, TreasureScoreToCoins(PROFILE.TreasureScore()), 37_weight);
    }

    const ItemPtr_t MiscItemFactory::Make_LockPicks(const ItemProfile & PROFILE)
    {
        return Make_Helper(PROFILE, TreasureScoreToCoins(PROFILE.TreasureScore()), 33_weight);
    }

    const ItemPtr_t MiscItemFactory::Make_Lyre(const ItemProfile & PROFILE)
    {
        return Make_Helper(PROFILE, TreasureScoreToCoins(PROFILE.TreasureScore()), 54_weight);
    }

    const ItemPtr_t MiscItemFactory::Make_MummyHand(const ItemProfile & PROFILE)
    {
        return Make_Helper(PROFILE, TreasureScoreToCoins(PROFILE.TreasureScore()), 38_weight);
    }

    const ItemPtr_t MiscItemFactory::Make_Orb(const ItemProfile & PROFILE)
    {
        return Make_Helper(PROFILE, TreasureScoreToCoins(PROFILE.TreasureScore()), 87_weight);
    }

    const ItemPtr_t MiscItemFactory::Make_PetrifiedSnake(const ItemProfile & PROFILE)
    {
        return Make_Helper(PROFILE, TreasureScoreToCoins(PROFILE.TreasureScore()), 32_weight);
    }

    const ItemPtr_t MiscItemFactory::Make_PipeAndTabor(const ItemProfile & PROFILE)
    {
        return Make_Helper(PROFILE, TreasureScoreToCoins(PROFILE.TreasureScore()), 127_weight);
    }

    const ItemPtr_t MiscItemFactory::Make_Recorder(const ItemProfile & PROFILE)
    {
        return Make_Helper(PROFILE, TreasureScoreToCoins(PROFILE.TreasureScore()), 36_weight);
    }

    const ItemPtr_t MiscItemFactory::Make_Ring(const ItemProfile & PROFILE)
    {
        return Make_Helper(PROFILE, TreasureScoreToCoins(PROFILE.TreasureScore()), 20_weight);
    }

    const ItemPtr_t MiscItemFactory::Make_Scepter(const ItemProfile & PROFILE)
    {
        return Make_Helper(PROFILE, TreasureScoreToCoins(PROFILE.TreasureScore()), 48_weight);
    }

    const ItemPtr_t MiscItemFactory::Make_Seeds(const ItemProfile & PROFILE)
    {
        return Make_Helper(PROFILE, TreasureScoreToCoins(PROFILE.TreasureScore()), 12_weight);
    }

    const ItemPtr_t MiscItemFactory::Make_Shard(const ItemProfile & PROFILE)
    {
        return Make_Helper(PROFILE, TreasureScoreToCoins(PROFILE.TreasureScore()), 49_weight);
    }

    const ItemPtr_t MiscItemFactory::Make_SpiderEggs(const ItemProfile & PROFILE)
    {
        return Make_Helper(PROFILE, TreasureScoreToCoins(PROFILE.TreasureScore()), 12_weight);
    }

    const ItemPtr_t MiscItemFactory::Make_Staff(const ItemProfile & PROFILE)
    {
        Coin_t price{ TreasureScoreToCoins(PROFILE.TreasureScore()) };
        AdjustPrice(
            price, PROFILE.MaterialPrimary(), PROFILE.MaterialSecondary(), PROFILE.IsPixie());

        Weight_t weight{ 47_weight };
        AdjustWeight(weight, PROFILE.MaterialPrimary(), PROFILE.MaterialSecondary());

        auto const BASE_NAME{ item::misc_type::Name(PROFILE.MiscType()) };

        auto const DESC_PREFIX{ "A " + boost::to_lower_copy(BASE_NAME) + " " };

        weapon::WeaponInfo weaponInfo(weapon_type::Staff);
        weaponInfo.is_staff = true;
        weaponInfo.is_quarterstaff = false;
        auto const DETAILS{ weapon::WeaponDetailLoader::Instance()->LookupWeaponDetails("Staff") };

        auto itemPtr{ ItemWarehouse::Instance()->Store(new Item(
            Make_Name(
                BASE_NAME,
                PROFILE.MaterialPrimary(),
                PROFILE.MaterialSecondary(),
                PROFILE.IsPixie()),
            Make_Desc(
                DESC_PREFIX,
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
            0_armor,
            creature::role::Count,
            weaponInfo,
            armor::ArmorInfo(),
            PROFILE.IsPixie(),
            PROFILE.UniqueType(),
            PROFILE.SetType(),
            PROFILE.NamedType(),
            PROFILE.ElementType())) };

        itemPtr->ImageFilename(
            sfml_util::gui::ItemImageManager::Instance()->GetImageFilename(itemPtr));

        creature::EnchantmentFactory::Instance()->MakeStoreAttachReturn(itemPtr);

        return itemPtr;
    }

    const ItemPtr_t MiscItemFactory::Make_SummoningStatue(const ItemProfile & PROFILE)
    {
        return Make_Helper(PROFILE, TreasureScoreToCoins(PROFILE.TreasureScore()), 38_weight);
    }

    const ItemPtr_t MiscItemFactory::Make_UnicornHorn(const ItemProfile & PROFILE)
    {
        return Make_Helper(PROFILE, TreasureScoreToCoins(PROFILE.TreasureScore()), 28_weight);
    }

    const ItemPtr_t MiscItemFactory::Make_Veil(const ItemProfile & PROFILE)
    {
        return Make_Helper(PROFILE, TreasureScoreToCoins(PROFILE.TreasureScore()), 39_weight);
    }

    const ItemPtr_t MiscItemFactory::Make_Viol(const ItemProfile & PROFILE)
    {
        return Make_Helper(PROFILE, TreasureScoreToCoins(PROFILE.TreasureScore()), 233_weight);
    }

    const ItemPtr_t MiscItemFactory::Make_Wand(const ItemProfile & PROFILE)
    {
        return Make_Helper(PROFILE, TreasureScoreToCoins(PROFILE.TreasureScore()), 8_weight);
    }

    const ItemPtr_t MiscItemFactory::Make_MiscBlessedOrCursed(const ItemProfile & PROFILE)
    {
        auto const MISC_ENUM{ PROFILE.MiscType() };
        auto const IS_BLESSED{ misc_type::IsBlessed(MISC_ENUM) };

        auto const MATERIAL_PRI{ PROFILE.MaterialPrimary() };
        auto const MATERIAL_SEC{ PROFILE.MaterialSecondary() };

        Coin_t price{ TreasureScoreToCoins(PROFILE.TreasureScore()) };
        AdjustPrice(price, MATERIAL_PRI, MATERIAL_SEC);

        auto weight{ [MISC_ENUM]() {
            if (MISC_ENUM == misc_type::Bust)
            {
                return 200_weight;
            }
            else if (MISC_ENUM == misc_type::Dried_Head)
            {
                return 65_weight;
            }
            else
            {
                return 38_weight;
            }
        }() };

        AdjustWeight(weight, MATERIAL_PRI, MATERIAL_SEC);

        std::ostringstream ssName;
        ssName << ((IS_BLESSED) ? "Blessed" : "Cursed") << " " << [MISC_ENUM]() {
            if ((MISC_ENUM == misc_type::Doll_Blessed) || (MISC_ENUM == misc_type::Doll_Cursed))
            {
                return ((misc::random::Bool()) ? "Doll" : "Effigy");
            }
            else if (
                (MISC_ENUM == misc_type::Figurine_Blessed)
                || (MISC_ENUM == misc_type::Figurine_Cursed))
            {
                return ((misc::random::Bool()) ? "Figurine" : "Statuette");
            }
            else if (
                (MISC_ENUM == misc_type::Puppet_Blessed) || (MISC_ENUM == misc_type::Puppet_Cursed))
            {
                return ((misc::random::Bool()) ? "Puppet" : "Marionette");
            }
            else if (MISC_ENUM == misc_type::Bust)
            {
                return "Bust";
            }
            else
            {
                return "Dried Head";
            }
        }();

        std::ostringstream ssDesc;
        ssDesc << "A " << boost::algorithm::to_lower_copy(ssName.str()) << " made of "
               << material::ToString(MATERIAL_PRI);

        if ((MATERIAL_PRI != MATERIAL_SEC) && (MATERIAL_SEC != material::Nothing))
        {
            ssDesc << " with " << material::ToString(MATERIAL_SEC) << " eyes";
        }

        ssDesc << ".";

        auto itemPtr{ ItemWarehouse::Instance()->Store(new Item(
            ssName.str(),
            ssDesc.str(),
            static_cast<category::Enum>(category::Equippable | category::Wearable),
            misc_type::Ring,
            weapon_type::NotAWeapon,
            armor_type::NotArmor,
            MATERIAL_PRI,
            MATERIAL_SEC,
            "",
            price,
            weight)) };

        itemPtr->ImageFilename(
            sfml_util::gui::ItemImageManager::Instance()->GetImageFilename(itemPtr));

        return itemPtr;
    }

    const ItemPtr_t MiscItemFactory::Make_Ring(
        const material::Enum MATERIAL_PRI, const material::Enum MATERIAL_SEC)
    {
        auto price{ 5_coin };
        AdjustPrice(price, MATERIAL_PRI, MATERIAL_SEC);

        auto weight{ 20_weight };
        AdjustWeight(weight, MATERIAL_PRI, MATERIAL_SEC);

        auto itemPtr{ ItemWarehouse::Instance()->Store(new Item(
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
            weight)) };

        itemPtr->ImageFilename(
            sfml_util::gui::ItemImageManager::Instance()->GetImageFilename(itemPtr));

        return itemPtr;
    }

    const ItemPtr_t MiscItemFactory::Make_Wand(
        const material::Enum MATERIAL_PRI, const material::Enum MATERIAL_SEC)
    {
        auto price{ 437_coin };
        AdjustPrice(price, MATERIAL_PRI, MATERIAL_SEC);

        auto weight{ 40_weight };
        AdjustWeight(weight, MATERIAL_PRI, MATERIAL_SEC);

        auto itemPtr{ ItemWarehouse::Instance()->Store(new Item(
            Make_Name("Wand", MATERIAL_PRI, MATERIAL_SEC),
            Make_Desc("A wand", MATERIAL_PRI, MATERIAL_SEC),
            static_cast<category::Enum>(category::Equippable | category::AllowsCasting),
            misc_type::Wand,
            weapon_type::NotAWeapon,
            armor_type::NotArmor,
            MATERIAL_PRI,
            MATERIAL_SEC,
            "",
            price,
            weight)) };

        itemPtr->ImageFilename(
            sfml_util::gui::ItemImageManager::Instance()->GetImageFilename(itemPtr));

        return itemPtr;
    }

    const ItemPtr_t MiscItemFactory::Make_DrumLute(const bool IS_PIXIE_ITEM)
    {
        auto price{ 1000_coin };
        auto weight{ 350_weight };
        std::string desc("An acoustic guitar with a round hollow body that can be drummed.");

        AdjustPrice(price, material::Wood, material::Rope, IS_PIXIE_ITEM);
        AdjustWeight(weight, material::Wood, material::Rope);

        auto itemPtr{ ItemWarehouse::Instance()->Store(new Item(
            Make_Name("DrumLute", material::Wood, material::Rope, IS_PIXIE_ITEM),
            Make_Desc(desc, material::Wood, material::Rope, "", IS_PIXIE_ITEM),
            static_cast<category::Enum>(
                category::Equippable | category::Useable | category::TwoHanded),
            misc_type::DrumLute,
            weapon_type::NotAWeapon,
            armor_type::NotArmor,
            material::Wood,
            material::Rope,
            "",
            price,
            weight,
            0_health,
            0_health,
            0_armor,
            creature::role::Bard,
            weapon::WeaponInfo(),
            armor::ArmorInfo(),
            IS_PIXIE_ITEM)) };

        itemPtr->ImageFilename(
            sfml_util::gui::ItemImageManager::Instance()->GetImageFilename(itemPtr));

        creature::EnchantmentFactory::Instance()->MakeStoreAttachReturn(itemPtr);

        return itemPtr;
    }

    const ItemPtr_t MiscItemFactory::Make_Helper(
        const ItemProfile & PROFILE,
        const Coin_t & BASE_PRICE,
        const Weight_t & BASE_WEIGHT,
        const std::string & BASE_DESC)
    {
        auto price{ BASE_PRICE };
        AdjustPrice(
            price, PROFILE.MaterialPrimary(), PROFILE.MaterialSecondary(), PROFILE.IsPixie());

        auto weight{ BASE_WEIGHT };
        AdjustWeight(weight, PROFILE.MaterialPrimary(), PROFILE.MaterialSecondary());

        auto const BASE_NAME{ [PROFILE]() {
            if ((PROFILE.UniqueType() == item::unique_type::Count)
                || (PROFILE.UniqueType() == item::unique_type::NotUnique))
            {
                return item::misc_type::Name(PROFILE.MiscType());
            }
            else
            {
                return item::unique_type::Name(PROFILE.UniqueType());
            }
        }() };

        auto const DESC_PREFIX{ (
            (BASE_DESC.empty()) ? ("A " + boost::to_lower_copy(BASE_NAME) + " ") : BASE_DESC) };

        auto itemPtr{ ItemWarehouse::Instance()->Store(new Item(
            Make_Name(
                BASE_NAME,
                PROFILE.MaterialPrimary(),
                PROFILE.MaterialSecondary(),
                PROFILE.IsPixie()),
            Make_Desc(
                DESC_PREFIX,
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
            0_health,
            0_health,
            0_armor,
            PROFILE.Role(),
            weapon::WeaponInfo(),
            armor::ArmorInfo(),
            PROFILE.IsPixie(),
            PROFILE.UniqueType(),
            PROFILE.SetType(),
            PROFILE.NamedType(),
            PROFILE.ElementType())) };

        itemPtr->ImageFilename(
            sfml_util::gui::ItemImageManager::Instance()->GetImageFilename(itemPtr));

        creature::EnchantmentFactory::Instance()->MakeStoreAttachReturn(itemPtr);

        return itemPtr;
    }

} // namespace item
} // namespace heroespath
