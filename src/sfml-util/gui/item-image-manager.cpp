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
// item-image-manager.cpp
//
#include "item-image-manager.hpp"

#include "sfml-util/loaders.hpp"

#include "creature/dragon-class-enum.hpp"
#include "creature/wolfen-class-enum.hpp"
#include "game/loop-manager.hpp"
#include "item/armor-details.hpp"
#include "item/item.hpp"
#include "log/log-macros.hpp"
#include "non-player/ownership-profile.hpp"

#include "misc/assertlogandthrow.hpp"
#include "misc/boost-string-includes.hpp"
#include "misc/random.hpp"

#include "sfml-util/gui/creature-image-manager.hpp"

#include <boost/filesystem.hpp>

#include <exception>
#include <sstream>
#include <vector>

namespace heroespath
{
namespace sfml_util
{
    namespace gui
    {

        std::unique_ptr<ItemImageManager> ItemImageManager::instanceUPtr_;
        std::string ItemImageManager::imagesDirectoryPath_{ "" };
        const std::string ItemImageManager::FILE_EXT_STR_{ ".png" };

        ItemImageManager::ItemImageManager()
        {
            M_HP_LOG_DBG("Singleton Construction: ItemImageManager");
        }

        ItemImageManager::~ItemImageManager()
        {
            M_HP_LOG_DBG("Singleton Construction: ItemImageManager");
        }

        misc::NotNull<ItemImageManager *> ItemImageManager::Instance()
        {
            if (!instanceUPtr_)
            {
                M_HP_LOG_ERR("Singleton Instance() before Acquire(): ItemImageManager");
                Acquire();
            }

            return instanceUPtr_.get();
        }

        void ItemImageManager::Acquire()
        {
            if (!instanceUPtr_)
            {
                instanceUPtr_ = std::make_unique<ItemImageManager>();
            }
            else
            {
                M_HP_LOG_ERR("Singleton Acquire() after Construction: ItemImageManager");
            }
        }

        void ItemImageManager::Release()
        {
            M_ASSERT_OR_LOGANDTHROW_SS(
                (instanceUPtr_),
                "sfml_util::gui::ItemImageManager::Release() found instanceUPtr that was null.");
            instanceUPtr_.reset();
        }

        bool ItemImageManager::Test()
        {
            static auto hasInitialPrompt{ false };
            if (false == hasInitialPrompt)
            {
                hasInitialPrompt = true;
                game::LoopManager::Instance()->TestingStrAppend(
                    "sfml_util::gui::ItemImageManager::Test()  Starting tests...");
            }

            auto iimPtr{ ItemImageManager::Instance() };

            const std::string TEST_PRE_STR{ "ItemImageManager Test #" };

            // test weapon items
            static auto axeIndex{ 0 };
            if (axeIndex < static_cast<int>(item::weapon::axe_type::Count))
            {
                auto const ENUM{ static_cast<item::weapon::axe_type::Enum>(axeIndex) };

                auto const STR{ boost::algorithm::to_lower_copy(
                    item::weapon::axe_type::ToString(ENUM)) };

                item::weapon::WeaponInfo wi(item::weapon_type::Axe, STR);
                wi.axe = ENUM;
                sf::Texture texture;
                iimPtr->Load(texture, iimPtr->GetImageFilename(wi, false));
                game::LoopManager::Instance()->TestingImageSet(texture, true, "item", STR);
                game::LoopManager::Instance()->TestingStrIncrement(TEST_PRE_STR);
                ++axeIndex;
                return false;
            }

            static auto hasTestedBite{ false };
            if (false == hasTestedBite)
            {
                item::weapon::WeaponInfo wi(item::weapon_type::Bite, "bite");
                wi.is_bite = true;
                sf::Texture texture;
                iimPtr->Load(texture, iimPtr->GetImageFilename(wi, false));
                game::LoopManager::Instance()->TestingImageSet(texture, true, "item", "bite");
                game::LoopManager::Instance()->TestingStrIncrement(TEST_PRE_STR);
                hasTestedBite = true;
                return false;
            }

            static auto bladedStaffIndex{ 0 };
            if (bladedStaffIndex < static_cast<int>(item::weapon::bladedstaff_type::Count))
            {
                auto const ENUM{ static_cast<item::weapon::bladedstaff_type::Enum>(
                    bladedStaffIndex) };

                auto const STR{ boost::algorithm::to_lower_copy(
                    item::weapon::bladedstaff_type::ToString(ENUM)) };

                item::weapon::WeaponInfo wi(item::weapon_type::BladedStaff, STR);
                wi.bladedstaff = ENUM;
                sf::Texture texture;
                iimPtr->Load(texture, iimPtr->GetImageFilename(wi, false));
                game::LoopManager::Instance()->TestingImageSet(texture, true, "item", STR);
                game::LoopManager::Instance()->TestingStrIncrement(TEST_PRE_STR);
                ++bladedStaffIndex;
                return false;
            }

            static auto hasTestedBreath{ false };
            if (false == hasTestedBreath)
            {
                item::weapon::WeaponInfo wi(item::weapon_type::Breath, "breath");
                wi.is_breath = true;
                sf::Texture texture;
                iimPtr->Load(texture, iimPtr->GetImageFilename(wi, false));
                game::LoopManager::Instance()->TestingImageSet(texture, true, "item", "breath");
                game::LoopManager::Instance()->TestingStrIncrement(TEST_PRE_STR);
                hasTestedBreath = true;
                return false;
            }

            static auto hasTestedClaws{ false };
            if (false == hasTestedClaws)
            {
                item::weapon::WeaponInfo wi(item::weapon_type::Claws, "claws");
                wi.is_claws = true;
                sf::Texture texture;
                iimPtr->Load(texture, iimPtr->GetImageFilename(wi, false));
                game::LoopManager::Instance()->TestingImageSet(texture, true, "item", "claws");
                game::LoopManager::Instance()->TestingStrIncrement(TEST_PRE_STR);
                hasTestedClaws = true;
                return false;
            }

            static auto clubIndex{ 0 };
            if (clubIndex < static_cast<int>(item::weapon::club_type::Count))
            {
                auto const ENUM{ static_cast<item::weapon::club_type::Enum>(clubIndex) };

                auto const STR{ boost::algorithm::to_lower_copy(
                    item::weapon::club_type::ToString(ENUM)) };

                item::weapon::WeaponInfo wi(item::weapon_type::Club, STR);
                wi.club = ENUM;
                sf::Texture texture;
                iimPtr->Load(texture, iimPtr->GetImageFilename(wi, false));
                game::LoopManager::Instance()->TestingImageSet(texture, true, "item", STR);
                game::LoopManager::Instance()->TestingStrIncrement(TEST_PRE_STR);
                ++clubIndex;
                return false;
            }

            static auto hasTestedFists{ false };
            if (false == hasTestedFists)
            {
                item::weapon::WeaponInfo wi(item::weapon_type::Fists, "fists");
                wi.is_fists = true;
                sf::Texture texture;
                iimPtr->Load(texture, iimPtr->GetImageFilename(wi, false));
                game::LoopManager::Instance()->TestingImageSet(texture, true, "item", "fists");
                game::LoopManager::Instance()->TestingStrIncrement(TEST_PRE_STR);
                hasTestedFists = true;
                return false;
            }

            static auto hasTestedKnife{ false };
            if (false == hasTestedKnife)
            {
                item::weapon::WeaponInfo wi(item::weapon_type::Knife, "knife");
                wi.is_knife = true;
                sf::Texture texture;
                iimPtr->Load(texture, iimPtr->GetImageFilename(wi, false));
                game::LoopManager::Instance()->TestingImageSet(texture, true, "item", "knife");
                game::LoopManager::Instance()->TestingStrIncrement(TEST_PRE_STR);
                hasTestedKnife = true;
                return false;
            }

            static auto hasTestedDagger{ false };
            if (false == hasTestedDagger)
            {
                item::weapon::WeaponInfo wi(item::weapon_type::Knife, "dagger");
                wi.is_dagger = true;
                sf::Texture texture;
                iimPtr->Load(texture, iimPtr->GetImageFilename(wi, false));
                game::LoopManager::Instance()->TestingImageSet(texture, true, "item", "dagger");
                game::LoopManager::Instance()->TestingStrIncrement(TEST_PRE_STR);
                hasTestedDagger = true;
                return false;
            }

            static auto projIndex{ 0 };
            if (projIndex < static_cast<int>(item::weapon::projectile_type::Count))
            {
                auto const ENUM{ static_cast<item::weapon::projectile_type::Enum>(projIndex) };

                auto const STR{ boost::algorithm::to_lower_copy(
                    item::weapon::projectile_type::ToString(ENUM)) };

                item::weapon::WeaponInfo wi(item::weapon_type::Projectile, STR);
                wi.projectile = ENUM;
                sf::Texture texture;
                iimPtr->Load(texture, iimPtr->GetImageFilename(wi, false));
                game::LoopManager::Instance()->TestingImageSet(texture, true, "item", STR);
                game::LoopManager::Instance()->TestingStrIncrement(TEST_PRE_STR);
                ++projIndex;
                return false;
            }

            static auto hasTestedStaff{ false };
            if (false == hasTestedStaff)
            {
                item::weapon::WeaponInfo wi(item::weapon_type::Staff, "staff");
                wi.is_staff = true;
                sf::Texture texture;
                iimPtr->Load(texture, iimPtr->GetImageFilename(wi, false));
                game::LoopManager::Instance()->TestingImageSet(texture, true, "item", "staff");
                game::LoopManager::Instance()->TestingStrIncrement(TEST_PRE_STR);
                hasTestedStaff = true;
                return false;
            }

            static auto hasTestedQStaff{ false };
            if (false == hasTestedQStaff)
            {
                item::weapon::WeaponInfo wi(item::weapon_type::Staff, "quarterstaff");
                wi.is_quarterstaff = true;
                sf::Texture texture;
                iimPtr->Load(texture, iimPtr->GetImageFilename(wi, false));
                game::LoopManager::Instance()->TestingImageSet(
                    texture, true, "item", "quarterstaff");
                game::LoopManager::Instance()->TestingStrIncrement(TEST_PRE_STR);
                hasTestedQStaff = true;
                return false;
            }

            static auto swordIndex{ 0 };
            if (swordIndex < static_cast<int>(item::weapon::sword_type::Count))
            {
                auto const ENUM{ static_cast<item::weapon::sword_type::Enum>(swordIndex) };

                auto const STR{ boost::algorithm::to_lower_copy(
                    item::weapon::sword_type::ToString(ENUM)) };

                item::weapon::WeaponInfo wi(item::weapon_type::Sword, STR);
                wi.sword = ENUM;
                sf::Texture texture;
                iimPtr->Load(texture, iimPtr->GetImageFilename(wi, false));
                game::LoopManager::Instance()->TestingImageSet(texture, true, "item", STR);
                game::LoopManager::Instance()->TestingStrIncrement(TEST_PRE_STR);
                ++swordIndex;
                return false;
            }

            static auto hasTestedTendrils{ false };
            if (false == hasTestedTendrils)
            {
                item::weapon::WeaponInfo wi(item::weapon_type::Tendrils, "tendrils");
                wi.is_tendrils = true;
                sf::Texture texture;
                iimPtr->Load(texture, iimPtr->GetImageFilename(wi, false));
                game::LoopManager::Instance()->TestingImageSet(texture, true, "item", "tendrils");
                game::LoopManager::Instance()->TestingStrIncrement(TEST_PRE_STR);
                hasTestedTendrils = true;
                return false;
            }

            // test armor items
            static auto aventailIndex{ 0 };
            if (aventailIndex < static_cast<int>(item::armor::base_type::Count))
            {
                item::armor::ArmorInfo ai(item::armor_type::Aventail);
                ai.base = static_cast<item::armor::base_type::Enum>(aventailIndex);
                ai.is_aventail = true;
                sf::Texture texture;
                iimPtr->Load(texture, iimPtr->GetImageFilename(ai));
                game::LoopManager::Instance()->TestingImageSet(
                    texture, true, "item", "aventail", item::armor::base_type::ToString(ai.base));
                game::LoopManager::Instance()->TestingStrIncrement(TEST_PRE_STR);
                ++aventailIndex;
                return false;
            }

            static auto bootsIndex{ 0 };
            if (bootsIndex < static_cast<int>(item::armor::base_type::Count))
            {
                item::armor::ArmorInfo ai(item::armor_type::Boots);
                ai.base = static_cast<item::armor::base_type::Enum>(bootsIndex);
                ai.is_boots = true;
                sf::Texture texture;
                iimPtr->Load(texture, iimPtr->GetImageFilename(ai));
                game::LoopManager::Instance()->TestingImageSet(
                    texture, true, "item", "boots", item::armor::base_type::ToString(ai.base));
                game::LoopManager::Instance()->TestingStrIncrement(TEST_PRE_STR);
                ++bootsIndex;
                return false;
            }

            static auto bracerIndex{ 0 };
            if (bracerIndex < static_cast<int>(item::armor::base_type::Count))
            {
                item::armor::ArmorInfo ai(item::armor_type::Bracer);
                ai.base = static_cast<item::armor::base_type::Enum>(bracerIndex);
                ai.is_bracer = true;
                sf::Texture texture;
                iimPtr->Load(texture, iimPtr->GetImageFilename(ai));
                game::LoopManager::Instance()->TestingImageSet(
                    texture, true, "item", "bracer", item::armor::base_type::ToString(ai.base));
                game::LoopManager::Instance()->TestingStrIncrement(TEST_PRE_STR);
                ++bracerIndex;
                return false;
            }

            static auto coverIndex{ 0 };
            if (coverIndex < static_cast<int>(item::armor::cover_type::Count))
            {
                item::armor::ArmorInfo ai(item::armor_type::Covering);
                ai.cover = static_cast<item::armor::cover_type::Enum>(coverIndex);
                sf::Texture texture;
                iimPtr->Load(texture, iimPtr->GetImageFilename(ai));
                game::LoopManager::Instance()->TestingImageSet(
                    texture, true, "item", "cover", item::armor::cover_type::ToString(ai.cover));
                game::LoopManager::Instance()->TestingStrIncrement(TEST_PRE_STR);
                ++coverIndex;
                return false;
            }

            static auto helmIndex{ 0 };
            if (helmIndex < static_cast<int>(item::armor::helm_type::Count))
            {
                item::armor::ArmorInfo ai(item::armor_type::Helm);
                ai.helm = static_cast<item::armor::helm_type::Enum>(helmIndex);
                sf::Texture texture;
                iimPtr->Load(texture, iimPtr->GetImageFilename(ai));
                game::LoopManager::Instance()->TestingImageSet(
                    texture, true, "item", "helm", item::armor::helm_type::ToString(ai.helm));
                game::LoopManager::Instance()->TestingStrIncrement(TEST_PRE_STR);
                ++helmIndex;
                return false;
            }

            static auto pantIndex{ 0 };
            if (pantIndex < static_cast<int>(item::armor::base_type::Count))
            {
                item::armor::ArmorInfo ai(item::armor_type::Pants);
                ai.base = static_cast<item::armor::base_type::Enum>(pantIndex);
                ai.is_pants = true;
                sf::Texture texture;
                iimPtr->Load(texture, iimPtr->GetImageFilename(ai));
                game::LoopManager::Instance()->TestingImageSet(
                    texture, true, "item", "pants", item::armor::base_type::ToString(ai.base));
                game::LoopManager::Instance()->TestingStrIncrement(TEST_PRE_STR);
                ++pantIndex;
                return false;
            }

            static auto shieldIndex{ 0 };
            if (shieldIndex < static_cast<int>(item::armor::shield_type::Count))
            {
                item::armor::ArmorInfo ai(item::armor_type::Sheild);
                ai.shield = static_cast<item::armor::shield_type::Enum>(shieldIndex);
                sf::Texture texture;
                iimPtr->Load(texture, iimPtr->GetImageFilename(ai));
                game::LoopManager::Instance()->TestingImageSet(
                    texture, true, "item", "shield", item::armor::shield_type::ToString(ai.shield));
                game::LoopManager::Instance()->TestingStrIncrement(TEST_PRE_STR);
                ++shieldIndex;
                return false;
            }

            static auto shirtIndex{ 0 };
            if (shirtIndex < static_cast<int>(item::armor::base_type::Count))
            {
                item::armor::ArmorInfo ai(item::armor_type::Shirt);
                ai.base = static_cast<item::armor::base_type::Enum>(shirtIndex);
                ai.is_shirt = true;
                sf::Texture texture;
                iimPtr->Load(texture, iimPtr->GetImageFilename(ai));
                game::LoopManager::Instance()->TestingImageSet(
                    texture, true, "item", "shirt", item::armor::base_type::ToString(ai.base));
                game::LoopManager::Instance()->TestingStrIncrement(TEST_PRE_STR);
                ++shirtIndex;
                return false;
            }

            // test misc items
            static auto miscIndex{ 1 };
            static auto isJeweled{ true };
            static auto isBone{ false };
            if (miscIndex < static_cast<int>(item::misc_type::Count))
            {
                auto const ENUM{ static_cast<item::misc_type::Enum>(miscIndex) };
                auto const ENUM_STR{ item::misc_type::ToString(ENUM) };
                auto const FILENAMES_VEC = iimPtr->GetImageFilenames(ENUM, isJeweled, isBone);

                M_ASSERT_OR_LOGANDTHROW_SS(
                    (FILENAMES_VEC.empty() == false),
                    "sfml_util::gui::ItemImageManager::Test() While testing misc item #"
                        << miscIndex << " \"" << ENUM_STR << "\", is_jeweled=" << std::boolalpha
                        << isJeweled << ", GetImageFilenames() returned an empty vector.");

                static std::size_t fileIndex{ 0 };
                if (fileIndex < FILENAMES_VEC.size())
                {
                    auto const NEXT_FILENAME{ FILENAMES_VEC[fileIndex] };

                    M_ASSERT_OR_LOGANDTHROW_SS(
                        (NEXT_FILENAME.empty() == false),
                        "sfml_util::gui::ItemImageManager::Test() (rand)  "
                            << "While testing misc item #" << miscIndex << " \"" << ENUM_STR
                            << "\", filename #" << fileIndex << ", is_jeweled=" << std::boolalpha
                            << isJeweled << ", found an empty filename string.");

                    std::ostringstream ss;
                    ss << "ItemImageManager Test of " << ENUM_STR << " #" << fileIndex
                       << ((isJeweled) ? "jeweled" : "not-jeweled");

                    game::LoopManager::Instance()->TestingStrIncrement(ss.str());

                    sf::Texture texture;
                    iimPtr->Load(texture, NEXT_FILENAME);
                    game::LoopManager::Instance()->TestingImageSet(
                        texture, true, "items/misc", ENUM_STR, NEXT_FILENAME);

                    ++fileIndex;
                    return false;
                }

                if (false == isJeweled)
                {
                    isJeweled = true;
                    return false;
                }
                else
                {
                    isJeweled = false;
                }

                fileIndex = 0;
                ++miscIndex;
                return false;
            }

            // test item ToString() functions
            {
                static unsigned int categoryIndex{ 0 };
                if (categoryIndex <= item::category::ConsumedOnUse)
                {
                    auto const STR{ item::category::ToString(
                        static_cast<item::category::Enum>(categoryIndex), false) };

                    game::LoopManager::Instance()->TestingStrIncrement(
                        "ItemImageManager category::ToString(\"" + STR + "\") Test.");

                    if (0 == categoryIndex)
                    {
                        categoryIndex = 1;
                    }
                    else
                    {
                        categoryIndex = categoryIndex << 1;
                    }
                    return false;
                }

                static unsigned int weaponTypeIndex{ 0 };
                if (weaponTypeIndex <= item::weapon_type::Breath)
                {
                    auto const STR{ item::weapon_type::ToString(
                        static_cast<item::weapon_type::Enum>(weaponTypeIndex), false) };

                    game::LoopManager::Instance()->TestingStrIncrement(
                        "ItemImageManager weapon_type::ToString(\"" + STR + "\") Test.");

                    if (0 == weaponTypeIndex)
                    {
                        weaponTypeIndex = 1;
                    }
                    else
                    {
                        weaponTypeIndex = weaponTypeIndex << 1;
                    }
                    return false;
                }

                static unsigned int armorTypeIndex{ 0 };
                if (armorTypeIndex <= item::armor_type::Skin)
                {
                    auto const STR{ item::armor_type::ToString(
                        static_cast<item::armor_type::Enum>(armorTypeIndex), false) };

                    game::LoopManager::Instance()->TestingStrIncrement(
                        "ItemImageManager armor_type::ToString(\"" + STR + "\") Test.");

                    if (0 == armorTypeIndex)
                    {
                        armorTypeIndex = 1;
                    }
                    else
                    {
                        armorTypeIndex = armorTypeIndex << 1;
                    }
                    return false;
                }

                static unsigned int collecterTypeIndex{ 0 };
                if (collecterTypeIndex <= non_player::ownership::collector_type::Hoarder)
                {
                    auto const STR{ non_player::ownership::collector_type::ToString(
                        static_cast<non_player::ownership::collector_type::Enum>(
                            collecterTypeIndex)) };

                    game::LoopManager::Instance()->TestingStrIncrement(
                        "ItemImageManager non_player::ownership::collector_type::ToString(\"" + STR
                        + "\") Test.");

                    if (0 == collecterTypeIndex)
                    {
                        collecterTypeIndex = 1;
                    }
                    else
                    {
                        collecterTypeIndex = collecterTypeIndex << 1;
                    }
                    return false;
                }
            }

            game::LoopManager::Instance()->TestingStrAppend(
                "sfml_util::gui::ItemImageManager::Test()  ALL TESTS PASSED.");
            return true;
        }

        void ItemImageManager::SetItemImageDirectory(const std::string & PATH)
        {
            imagesDirectoryPath_ = PATH;
        }

        void
            ItemImageManager::Load(sf::Texture & texture, const std::string & IMAGE_FILE_NAME) const
        {
            namespace bfs = boost::filesystem;
            const bfs::path PATH_OBJ(
                bfs::system_complete(bfs::path(imagesDirectoryPath_) / bfs::path(IMAGE_FILE_NAME)));

            sfml_util::LoadTexture(texture, PATH_OBJ.string());
        }

        void ItemImageManager::Load(sf::Texture & texture, const item::ItemPtr_t ITEM_PTR) const
        {
            return Load(texture, ITEM_PTR->ImageFilename());
        }

        void ItemImageManager::Load(
            sf::Texture & texture,
            const item::misc_type::Enum ITEM_ENUM,
            const bool IS_JEWELED,
            const bool IS_BONE,
            const bool WILL_RANDOMIZE) const
        {
            Load(texture, GetImageFilename(ITEM_ENUM, IS_JEWELED, IS_BONE, WILL_RANDOMIZE));
        }

        const std::string ItemImageManager::GetImageFilename(
            const item::misc_type::Enum ITEM_ENUM,
            const bool IS_JEWELED,
            const bool IS_BONE,
            const bool WILL_RANDOMIZE,
            const creature::race::Enum RACE_ENUM,
            const creature::role::Enum ROLE_ENUM) const
        {
            auto const FILENAMES_VEC(
                GetImageFilenames(ITEM_ENUM, IS_JEWELED, IS_BONE, RACE_ENUM, ROLE_ENUM));

            M_ASSERT_OR_LOGANDTHROW_SS(
                (FILENAMES_VEC.empty() == false),
                "sfml_util::gui::ItemImageManager::GetImageFilename(misc, \""
                    << item::misc_type::ToString(ITEM_ENUM) << "\", is_jeweled=" << std::boolalpha
                    << IS_JEWELED << ", will_rand=" << WILL_RANDOMIZE << ", race=" << RACE_ENUM
                    << ", role=" << ROLE_ENUM
                    << ") unable to get any filenames for those settings.");

            if (WILL_RANDOMIZE)
            {
                return FILENAMES_VEC[static_cast<std::size_t>(
                    misc::random::Int(0, static_cast<int>(FILENAMES_VEC.size()) - 1))];
            }
            else
            {
                return FILENAMES_VEC[0];
            }
        }

        const std::string ItemImageManager::GetImageFilename(
            const item::ItemPtr_t ITEM_PTR, const bool WILL_RANDOMIZE) const
        {
            using namespace item;

            if (ITEM_PTR->IsWeapon() && (ITEM_PTR->Weapon_Info().type != weapon_type::NotAWeapon))
            {
                return GetImageFilename(ITEM_PTR->Weapon_Info(), ITEM_PTR->IsJeweled());
            }
            else if (ITEM_PTR->Armor_Info().type == armor_type::Skin)
            {
                return GetSkinImageFilename(ITEM_PTR);
            }
            else if (ITEM_PTR->Armor_Info().type != armor_type::NotArmor)
            {
                return GetImageFilename(ITEM_PTR->Armor_Info());
            }
            else if (ITEM_PTR->MiscType() != misc_type::NotMisc)
            {
                return GetImageFilename(
                    ITEM_PTR->MiscType(),
                    ITEM_PTR->IsJeweled(),
                    (ITEM_PTR->MaterialPrimary() == item::material::Bone),
                    WILL_RANDOMIZE,
                    ITEM_PTR->GetSummonInfo().Race(),
                    ITEM_PTR->GetSummonInfo().Role());
            }
            else
            {
                std::ostringstream ss;
                ss << "sfml_util::gui::ItemImageManager::GetImageFilename(item->Name()="
                   << ITEM_PTR->Name()
                   << ", item->Category=" << category::ToString(ITEM_PTR->Category(), false)
                   << ", desc=\"" << ITEM_PTR->Desc()
                   << "\") failed to be categorized.  (IsWeapon=" << std::boolalpha
                   << ITEM_PTR->IsWeapon() << ", WeaponInfo.type="
                   << weapon_type::ToString(ITEM_PTR->Weapon_Info().type, false)
                   << ", IsArmor=" << std::boolalpha << ITEM_PTR->IsArmor() << ", ArmorInfo.type="
                   << armor_type::ToString(ITEM_PTR->Armor_Info().type, false)
                   << ", MiscType()=" << ITEM_PTR->MiscType() << ")";

                throw std::runtime_error(ss.str());
            }
        }

        const std::string ItemImageManager::GetImageFilename(
            const item::weapon::WeaponInfo & WEAPON_INFO, const bool IS_JEWELED) const
        {
            using namespace item;

            if (WEAPON_INFO.type & weapon_type::Sword)
            {
                return weapon::sword_type::ToString(WEAPON_INFO.sword) + FILE_EXT_STR_;
            }

            if (WEAPON_INFO.type & weapon_type::Axe)
            {
                return weapon::axe_type::ToString(WEAPON_INFO.axe) + FILE_EXT_STR_;
            }

            if (WEAPON_INFO.type & weapon_type::Club)
            {
                return weapon::club_type::ToString(WEAPON_INFO.club) + FILE_EXT_STR_;
            }

            if (WEAPON_INFO.type & weapon_type::Whip)
            {
                return weapon::whip_type::ToString(WEAPON_INFO.whip) + FILE_EXT_STR_;
            }

            if (WEAPON_INFO.type & weapon_type::Projectile)
            {
                return weapon::projectile_type::ToString(WEAPON_INFO.projectile) + FILE_EXT_STR_;
            }

            if (WEAPON_INFO.type & weapon_type::BladedStaff)
            {
                return weapon::bladedstaff_type::ToString(WEAPON_INFO.bladedstaff) + FILE_EXT_STR_;
            }

            if (WEAPON_INFO.is_bite)
            {
                return "bite" + FILE_EXT_STR_;
            }

            if (WEAPON_INFO.is_breath)
            {
                return "breath" + FILE_EXT_STR_;
            }

            if (WEAPON_INFO.is_claws)
            {
                return "claws" + FILE_EXT_STR_;
            }

            if (WEAPON_INFO.is_dagger)
            {
                return "dagger" + FILE_EXT_STR_;
            }

            if (WEAPON_INFO.is_fists)
            {
                return "fists" + FILE_EXT_STR_;
            }

            if (WEAPON_INFO.is_knife)
            {
                return "knife" + FILE_EXT_STR_;
            }

            if (WEAPON_INFO.is_quarterstaff)
            {
                return "quarter-staff" + FILE_EXT_STR_;
            }

            if (WEAPON_INFO.is_staff)
            {
                if (IS_JEWELED)
                {
                    return "staff-2" + FILE_EXT_STR_;
                }
                else
                {
                    return "staff-plain" + FILE_EXT_STR_;
                }
            }

            if (WEAPON_INFO.is_tendrils)
            {
                return "tendrils" + FILE_EXT_STR_;
            }

            std::ostringstream ss;
            ss << "sfml_util::gui::ItemImageManager::GetImageFilename(WEAPON_INFO.type="
               << weapon_type::ToString(WEAPON_INFO.type, false)
               << ") failed to resolve a filename for weapon.";
            throw std::runtime_error(ss.str());
        }

        const std::string
            ItemImageManager::GetImageFilename(const item::armor::ArmorInfo & ARMOR_INFO) const
        {
            using namespace item;

            if (ARMOR_INFO.type & armor_type::Aventail)
            {
                return "aventail" + FILE_EXT_STR_;
            }

            if (ARMOR_INFO.type & armor_type::Boots)
            {
                return "Boots-" + armor::base_type::ToString(ARMOR_INFO.base) + FILE_EXT_STR_;
            }

            if ((ARMOR_INFO.type & armor_type::Bracer) || (ARMOR_INFO.is_bracer))
            {
                return "bracer" + FILE_EXT_STR_;
            }

            if (ARMOR_INFO.type & armor_type::Covering)
            {
                return armor::cover_type::ToString(ARMOR_INFO.cover) + FILE_EXT_STR_;
            }

            if ((ARMOR_INFO.type & armor_type::Gauntlets) || (ARMOR_INFO.is_gauntlets))
            {
                if (ARMOR_INFO.base == armor::base_type::Plain)
                {
                    return "gloves" + FILE_EXT_STR_;
                }
                else
                {
                    return "Gauntlets-" + armor::base_type::ToString(ARMOR_INFO.base)
                        + FILE_EXT_STR_;
                }
            }

            if (ARMOR_INFO.type & armor_type::Helm)
            {
                return armor::helm_type::ToString(ARMOR_INFO.helm) + "-Helm" + FILE_EXT_STR_;
            }

            if ((ARMOR_INFO.type & armor_type::Pants) || (ARMOR_INFO.is_pants))
            {
                return "pants" + FILE_EXT_STR_;
            }

            if (ARMOR_INFO.type & armor_type::Sheild)
            {
                return armor::shield_type::ToString(ARMOR_INFO.shield) + "-Shield" + FILE_EXT_STR_;
            }

            if ((ARMOR_INFO.type & armor_type::Shirt) || (ARMOR_INFO.is_shirt))
            {
                return "Shirt-" + armor::base_type::ToString(ARMOR_INFO.base) + FILE_EXT_STR_;
            }

            std::ostringstream ss;
            ss << "sfml_util::gui::ItemImageManager::GetImageFilename(ARMOR_INFO.type="
               << armor_type::ToString(ARMOR_INFO.type, false)
               << ") failed to resolve a filename for armor.";

            throw std::runtime_error(ss.str());
        }

        const std::string
            ItemImageManager::GetSkinImageFilename(const item::ItemPtr_t ITEM_PTR) const
        {
            using namespace item;

            auto const PRI_MATERIAL{ ITEM_PTR->MaterialPrimary() };

            if (PRI_MATERIAL == material::Plant)
            {
                return "plant" + FILE_EXT_STR_;
            }
            else if (PRI_MATERIAL == material::Flesh)
            {
                return "flesh" + FILE_EXT_STR_;
            }
            else if (PRI_MATERIAL == material::Scale)
            {
                return "scale" + FILE_EXT_STR_;
            }
            else if ((PRI_MATERIAL == material::Fur) || (PRI_MATERIAL == material::Hide))
            {
                return "hide" + FILE_EXT_STR_;
            }

            std::ostringstream ss;
            ss << "sfml_util::gui::ItemImageManager::GetSkinImageFilename(PRI_MATERIAL="
               << material::ToString(PRI_MATERIAL)
               << ") failed to resolve a filename for that material.";

            throw std::runtime_error(ss.str());
        }

        const std::vector<std::string> ItemImageManager::GetImageFilenames(
            const item::misc_type::Enum ITEM_ENUM,
            const bool IS_JEWELED,
            const bool IS_BONE,
            const creature::race::Enum RACE_ENUM,
            const creature::role::Enum ROLE_ENUM) const
        {
            using namespace item;

            std::vector<std::string> filenames;

            switch (ITEM_ENUM)
            {
                case misc_type::Amulet:
                case misc_type::Pendant:
                case misc_type::Medallion:
                {
                    for (auto i(1); i <= 23; ++i)
                    {
                        std::ostringstream ss;
                        ss << "amulet-" << i << FILE_EXT_STR_;
                        filenames.emplace_back(ss.str());
                    }
                    return filenames;
                }
                case misc_type::Necklace:
                {
                    filenames.emplace_back("necklace" + FILE_EXT_STR_);
                    return filenames;
                }
                case misc_type::Bag:
                {
                    for (auto i(1); i <= 8; ++i)
                    {
                        std::ostringstream ss;
                        ss << "bag-" << i << FILE_EXT_STR_;
                        filenames.emplace_back(ss.str());
                    }
                    return filenames;
                }
                case misc_type::Tome:
                case misc_type::Book:
                {
                    for (auto i(1); i <= 5; ++i)
                    {
                        std::ostringstream ss;
                        ss << "book-" << i << FILE_EXT_STR_;
                        filenames.emplace_back(ss.str());
                    }
                    return filenames;
                }
                case misc_type::Cape:
                {
                    filenames.emplace_back("cape" + FILE_EXT_STR_);
                    return filenames;
                }
                case misc_type::Cloak:
                {
                    filenames.emplace_back("cloak" + FILE_EXT_STR_);
                    return filenames;
                }
                case misc_type::Robe:
                {
                    filenames.emplace_back("robe" + FILE_EXT_STR_);
                    return filenames;
                }
                case misc_type::Crown:
                {
                    for (auto i(1); i <= 12; ++i)
                    {
                        std::ostringstream ss;
                        ss << "crown-" << i << FILE_EXT_STR_;
                        filenames.emplace_back(ss.str());
                    }
                    return filenames;
                }
                case misc_type::Doll:
                {
                    for (auto i(1); i <= 16; ++i)
                    {
                        std::ostringstream ss;
                        ss << "doll-" << i << FILE_EXT_STR_;
                        filenames.emplace_back(ss.str());
                    }
                    return filenames;
                }
                case misc_type::Drink:
                case misc_type::Potion:
                {
                    for (auto i(1); i <= 33; ++i)
                    {
                        std::ostringstream ss;
                        ss << "potion-" << i << FILE_EXT_STR_;
                        filenames.emplace_back(ss.str());
                    }
                    return filenames;
                }
                case misc_type::Goblet:
                {
                    for (auto i(1); i <= 8; ++i)
                    {
                        std::ostringstream ss;
                        ss << "goblet-" << i << FILE_EXT_STR_;
                        filenames.emplace_back(ss.str());
                    }
                    return filenames;
                }
                case misc_type::Herbs:
                {
                    for (auto i(1); i <= 34; ++i)
                    {
                        std::ostringstream ss;
                        ss << "herb-" << i << FILE_EXT_STR_;
                        filenames.emplace_back(ss.str());
                    }
                    return filenames;
                }
                case misc_type::Hourglass:
                {
                    for (auto i(1); i <= 3; ++i)
                    {
                        std::ostringstream ss;
                        ss << "hourglass-" << i << FILE_EXT_STR_;
                        filenames.emplace_back(ss.str());
                    }
                    return filenames;
                }
                case misc_type::Key:
                {
                    for (auto i(1); i <= 11; ++i)
                    {
                        std::ostringstream ss;
                        ss << "key-" << i << FILE_EXT_STR_;
                        filenames.emplace_back(ss.str());
                    }
                    return filenames;
                }
                case misc_type::Lantern:
                {
                    for (auto i(1); i <= 18; ++i)
                    {
                        std::ostringstream ss;
                        ss << "lantern-" << i << FILE_EXT_STR_;
                        filenames.emplace_back(ss.str());
                    }
                    return filenames;
                }
                case misc_type::Torch:
                {
                    for (auto i(1); i <= 2; ++i)
                    {
                        std::ostringstream ss;
                        ss << "torch-" << i << FILE_EXT_STR_;
                        filenames.emplace_back(ss.str());
                    }
                    return filenames;
                }
                case misc_type::Lockbox:
                {
                    for (auto i(1); i <= 8; ++i)
                    {
                        std::ostringstream ss;
                        ss << "lockbox-" << i << FILE_EXT_STR_;
                        filenames.emplace_back(ss.str());
                    }
                    return filenames;
                }
                case misc_type::Chest:
                {
                    for (auto i(1); i <= 14; ++i)
                    {
                        std::ostringstream ss;
                        ss << "chest-" << i << FILE_EXT_STR_;
                        filenames.emplace_back(ss.str());
                    }
                    return filenames;
                }
                case misc_type::LockPicks:
                {
                    for (auto i(1); i <= 7; ++i)
                    {
                        std::ostringstream ss;
                        ss << "lockpicks-" << i << FILE_EXT_STR_;
                        filenames.emplace_back(ss.str());
                    }
                    return filenames;
                }
                case misc_type::Mask:
                {
                    for (auto i(1); i <= 6; ++i)
                    {
                        std::ostringstream ss;
                        ss << "mask-" << i << FILE_EXT_STR_;
                        filenames.emplace_back(ss.str());
                    }
                    return filenames;
                }
                case misc_type::Mirror:
                {
                    for (auto i(1); i <= 10; ++i)
                    {
                        std::ostringstream ss;
                        ss << "mirror-" << i << FILE_EXT_STR_;
                        filenames.emplace_back(ss.str());
                    }
                    return filenames;
                }
                case misc_type::DrumLute:
                {
                    for (auto i(1); i <= 21; ++i)
                    {
                        std::ostringstream ss;
                        ss << "drumlute-" << i << FILE_EXT_STR_;
                        filenames.emplace_back(ss.str());
                    }
                    return filenames;
                }
                case misc_type::Scroll:
                {
                    for (auto i(1); i <= 13; ++i)
                    {
                        std::ostringstream ss;
                        ss << "scroll-" << i << FILE_EXT_STR_;
                        filenames.emplace_back(ss.str());
                    }
                    return filenames;
                }
                case misc_type::Orb:
                {
                    for (auto i(1); i <= 9; ++i)
                    {
                        std::ostringstream ss;
                        ss << "orb-" << i << FILE_EXT_STR_;
                        filenames.emplace_back(ss.str());
                    }
                    return filenames;
                }
                case misc_type::Ring:
                {
                    if (IS_JEWELED)
                    {
                        for (auto i(1); i <= 24; ++i)
                        {
                            std::ostringstream ss;
                            ss << "ring-jeweled-" << i << FILE_EXT_STR_;
                            filenames.emplace_back(ss.str());
                        }
                    }
                    else if (IS_BONE)
                    {
                        filenames.emplace_back("ring-bone" + FILE_EXT_STR_);
                    }
                    else
                    {
                        for (auto i(1); i <= 2; ++i)
                        {
                            std::ostringstream ss;
                            ss << "ring-plain-" << i << FILE_EXT_STR_;
                            filenames.emplace_back(ss.str());
                        }
                    }
                    return filenames;
                }
                case misc_type::Skull:
                {
                    for (auto i(1); i <= 11; ++i)
                    {
                        std::ostringstream ss;
                        ss << "skull-" << i << FILE_EXT_STR_;
                        filenames.emplace_back(ss.str());
                    }
                    return filenames;
                }
                case misc_type::Shard:
                {
                    for (auto i(1); i <= 7; ++i)
                    {
                        std::ostringstream ss;
                        ss << "shard-" << i << FILE_EXT_STR_;
                        filenames.emplace_back(ss.str());
                    }
                    return filenames;
                }
                case misc_type::Salve:
                {
                    for (auto i(1); i <= 9; ++i)
                    {
                        std::ostringstream ss;
                        ss << "mortar-" << i << FILE_EXT_STR_;
                        filenames.emplace_back(ss.str());
                    }
                    return filenames;
                }
                case misc_type::Wand:
                {
                    for (auto i(1); i <= 11; ++i)
                    {
                        std::ostringstream ss;
                        ss << "wand-" << i << FILE_EXT_STR_;
                        filenames.emplace_back(ss.str());
                    }
                    return filenames;
                }
                case misc_type::Scepter:
                {
                    for (auto i(1); i <= 26; ++i)
                    {
                        std::ostringstream ss;
                        ss << "scepter-" << i << FILE_EXT_STR_;
                        filenames.emplace_back(ss.str());
                    }
                    return filenames;
                }
                case misc_type::Balm_Pot:
                {
                    for (auto i(1); i <= 3; ++i)
                    {
                        std::ostringstream ss;
                        ss << "balm-pot-" << i << FILE_EXT_STR_;
                        filenames.emplace_back(ss.str());
                    }
                    return filenames;
                }
                case misc_type::Doll_Blessed:
                {
                    filenames.emplace_back("doll-1" + FILE_EXT_STR_);
                    filenames.emplace_back("doll-2" + FILE_EXT_STR_);
                    filenames.emplace_back("doll-4" + FILE_EXT_STR_);
                    filenames.emplace_back("doll-5" + FILE_EXT_STR_);
                    filenames.emplace_back("doll-6" + FILE_EXT_STR_);
                    filenames.emplace_back("doll-8" + FILE_EXT_STR_);
                    filenames.emplace_back("doll-9" + FILE_EXT_STR_);
                    filenames.emplace_back("doll-10" + FILE_EXT_STR_);
                    return filenames;
                }
                case misc_type::Doll_Cursed:
                {
                    filenames.emplace_back("doll-11" + FILE_EXT_STR_);
                    filenames.emplace_back("doll-12" + FILE_EXT_STR_);
                    filenames.emplace_back("doll-13" + FILE_EXT_STR_);
                    filenames.emplace_back("doll-14" + FILE_EXT_STR_);
                    filenames.emplace_back("doll-15" + FILE_EXT_STR_);
                    filenames.emplace_back("doll-16" + FILE_EXT_STR_);
                    return filenames;
                }
                case misc_type::Figurine_Blessed:
                {
                    for (auto i(1); i <= 15; ++i)
                    {
                        std::ostringstream ss;
                        ss << "figurine-" << i << FILE_EXT_STR_;
                        filenames.emplace_back(ss.str());
                    }
                    return filenames;
                }
                case misc_type::Figurine_Cursed:
                {
                    for (auto i(1); i <= 15; ++i)
                    {
                        std::ostringstream ss;
                        ss << "figurine-" << i << FILE_EXT_STR_;
                        filenames.emplace_back(ss.str());
                    }
                    for (auto i(1); i <= 4; ++i)
                    {
                        std::ostringstream ss;
                        ss << "figurine-evil-" << i << FILE_EXT_STR_;
                        filenames.emplace_back(ss.str());
                    }
                    return filenames;
                }
                case misc_type::Staff:
                {
                    for (auto i(1); i <= 21; ++i)
                    {
                        std::ostringstream ss;
                        ss << "staff-" << i << FILE_EXT_STR_;
                        filenames.emplace_back(ss.str());
                    }
                    return filenames;
                }
                case misc_type::Summoning_Statue:
                {
                    if ((RACE_ENUM == creature::race::Count)
                        || (ROLE_ENUM == creature::role::Count))
                    {
                        filenames.emplace_back("figurine-1" + FILE_EXT_STR_);
                    }
                    else
                    {
                        sfml_util::gui::CreatureImageManager::Instance()->GetFilenames(
                            filenames,
                            RACE_ENUM,
                            ROLE_ENUM,
                            ((misc::random::Bool()) ? creature::sex::Male : creature::sex::Female),
                            creature::wolfen_class::Adult,
                            creature::dragon_class::Adult);
                    }
                    return filenames;
                }
                case misc_type::Puppet_Blessed:
                case misc_type::Puppet_Cursed:
                {
                    filenames.emplace_back("puppet" + FILE_EXT_STR_);
                    return filenames;
                }
                case misc_type::Ankh_Necklace:
                case misc_type::Armband:
                case misc_type::Beard:
                case misc_type::Bell:
                case misc_type::Bird_Claw:
                case misc_type::Bone_Whistle:
                case misc_type::Bone:
                case misc_type::Bracelet_Crown:
                case misc_type::Bracelet_Feather:
                case misc_type::Bracelet_Fist:
                case misc_type::Bracelet_Hourglass:
                case misc_type::Bracelet_Key:
                case misc_type::Bracelet_Mask:
                case misc_type::Brooch_Crown:
                case misc_type::Brooch_Feather:
                case misc_type::Brooch_Fist:
                case misc_type::Brooch_Hourglass:
                case misc_type::Brooch_Key:
                case misc_type::Brooch_Mask:
                case misc_type::Braid:
                case misc_type::Bust:
                case misc_type::Cameo:
                case misc_type::Cat:
                case misc_type::Chains:
                case misc_type::Charm_Crown:
                case misc_type::Charm_Feather:
                case misc_type::Charm_Fist:
                case misc_type::Charm_Hourglass:
                case misc_type::Charm_Key:
                case misc_type::Charm_Mask:
                case misc_type::Chimes:
                case misc_type::Conch:
                case misc_type::Crumhorn:
                case misc_type::Devil_Horn:
                case misc_type::Dried_Head:
                case misc_type::Embryo:
                case misc_type::Egg:
                case misc_type::Eye:
                case misc_type::Feather:
                case misc_type::Finger:
                case misc_type::Fingerclaw:
                case misc_type::Flag:
                case misc_type::Frog:
                case misc_type::Gecko:
                case misc_type::Ghost_Sheet:
                case misc_type::Gizzard:
                case misc_type::Gong:
                case misc_type::Grave_Ornament:
                case misc_type::Handbag:
                case misc_type::Hanky:
                case misc_type::Headdress:
                case misc_type::Hide:
                case misc_type::Horn:
                case misc_type::Horseshoe:
                case misc_type::Hurdy_Gurdy:
                case misc_type::Icicle:
                case misc_type::Iguana:
                case misc_type::Imp_Tail:
                case misc_type::Leaf:
                case misc_type::Legtie:
                case misc_type::Litch_Hand:
                case misc_type::Lizard:
                case misc_type::Lyre:
                case misc_type::Magnifying_Glass:
                case misc_type::Mummy_Hand:
                case misc_type::Noose:
                case misc_type::Nose:
                case misc_type::Paw:
                case misc_type::Petrified_Snake:
                case misc_type::Pin_Crown:
                case misc_type::Pin_Feather:
                case misc_type::Pin_Fist:
                case misc_type::Pin_Hourglass:
                case misc_type::Pin_Key:
                case misc_type::Pin_Mask:
                case misc_type::Pipe_And_Tabor:
                case misc_type::Rabbit_Foot:
                case misc_type::Rainmaker:
                case misc_type::Rat_Juju:
                case misc_type::Rattlesnake_Tail:
                case misc_type::Recorder:
                case misc_type::Relic:
                case misc_type::Scale:
                case misc_type::Scythe:
                case misc_type::Seeds:
                case misc_type::Salamander:
                case misc_type::Shark_Tooth_Necklace:
                case misc_type::Shroud:
                case misc_type::Signet_Crown:
                case misc_type::Signet_Feather:
                case misc_type::Signet_Fist:
                case misc_type::Signet_Hourglass:
                case misc_type::Signet_Key:
                case misc_type::Signet_Mask:
                case misc_type::Skink:
                case misc_type::Spider_Eggs:
                case misc_type::Spyglass:
                case misc_type::Talisman:
                case misc_type::Tongue:
                case misc_type::Tooth_Necklace:
                case misc_type::Tooth:
                case misc_type::Troll_Figure:
                case misc_type::Trophy:
                case misc_type::Tuning_Fork:
                case misc_type::Turtle_Shell:
                case misc_type::Unicorn_Horn:
                case misc_type::Veil:
                case misc_type::Viol:
                case misc_type::Warhorse_Marionette:
                case misc_type::Weasel_Totem:
                case misc_type::Wolfen_Fur:
                {
                    auto const STR{ boost::replace_all_copy(
                        boost::to_lower_copy(misc_type::ToString(ITEM_ENUM)), "_", "-") };

                    filenames.emplace_back(STR + FILE_EXT_STR_);
                    return filenames;
                }
                case misc_type::NotMisc:
                case misc_type::Count:
                default:
                {
                    std::ostringstream ss;
                    ss << "sfml_util::gui::ItemImageManager::GetImageFilename(misc_type::Enum="
                       << ITEM_ENUM << ")_UnknownOrInvalidValueError.";

                    throw std::range_error(ss.str());
                }
            }
        }
    } // namespace gui
} // namespace sfml_util
} // namespace heroespath
