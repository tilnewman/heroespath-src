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

#include "game/log-macros.hpp"
#include "game/item/item.hpp"
#include "game/loop-manager.hpp"
#include "game/non-player/ownership-profile.hpp"

#include "misc/random.hpp"
#include "misc/assertlogandthrow.hpp"
#include "misc/boost-string-includes.hpp"

#include <boost/filesystem.hpp>

#include <exception>
#include <sstream>


namespace sfml_util
{
namespace gui
{

    std::unique_ptr<ItemImageManager> ItemImageManager::instanceUPtr_{ nullptr };
    std::string ItemImageManager::imagesDirectoryPath_{ "" };
    const std::string ItemImageManager::EXT_{ ".png" };


    ItemImageManager::ItemImageManager()
    {}


    ItemImageManager * ItemImageManager::Instance()
    {
        if (instanceUPtr_.get() == nullptr)
        {
            Acquire();
        }

        return instanceUPtr_.get();
    }


    void ItemImageManager::Acquire()
    {
        if (instanceUPtr_.get() == nullptr)
        {
            instanceUPtr_.reset(new ItemImageManager);
        }
    }


    void ItemImageManager::Release()
    {
        M_ASSERT_OR_LOGANDTHROW_SS((instanceUPtr_.get() != nullptr), "sfml_util::gui::ItemImageManager::Release() found instanceUPtr that was null.");
        instanceUPtr_.reset();
    }


    bool ItemImageManager::Test()
    {
        static auto hasInitialPrompt{ false };
        if (false == hasInitialPrompt)
        {
            hasInitialPrompt = true;
            game::LoopManager::Instance()->TestingStrAppend("sfml_util::gui::ItemImageManager::Test()  Starting tests...");
        }

        auto iimPtr{ ItemImageManager::Instance() };

        const std::string TEST_PRE_STR{ "ItemImageManager Test #" };

        //test weapon items
        static auto axeIndex{ 0 };
        if (axeIndex < static_cast<int>(game::item::weapon::axe_type::Count))
        {
            auto const ENUM{ static_cast<game::item::weapon::axe_type::Enum>(axeIndex) };
            auto const STR { boost::algorithm::to_lower_copy(game::item::weapon::axe_type::ToString(ENUM)) };
            game::item::weapon::WeaponInfo wi(game::item::weapon_type::Axe, STR);
            wi.axe = ENUM;
            sf::Texture texture;
            iimPtr->Load(texture, iimPtr->GetImageFilename(wi, false, false));
            game::LoopManager::Instance()->TestingImageSet(texture);
            game::LoopManager::Instance()->TestingStrIncrement(TEST_PRE_STR);
            ++axeIndex;
            return false;
        }

        static auto hasTestedBite{ false };
        if (false == hasTestedBite)
        {
            game::item::weapon::WeaponInfo wi(game::item::weapon_type::Bite, "bite");
            wi.is_bite = true;
            sf::Texture texture;
            iimPtr->Load(texture, iimPtr->GetImageFilename(wi, false, false));
            game::LoopManager::Instance()->TestingImageSet(texture);
            game::LoopManager::Instance()->TestingStrIncrement(TEST_PRE_STR);
            hasTestedBite = true;
            return false;
        }

        static auto bladedStaffIndex{ 0 };
        if (bladedStaffIndex < static_cast<int>(game::item::weapon::bladedstaff_type::Count))
        {
            auto const ENUM{ static_cast<game::item::weapon::bladedstaff_type::Enum>(bladedStaffIndex) };
            auto const STR{ boost::algorithm::to_lower_copy(game::item::weapon::bladedstaff_type::ToString(ENUM)) };
            game::item::weapon::WeaponInfo wi(game::item::weapon_type::BladedStaff, STR);
            wi.bladedstaff = ENUM;
            sf::Texture texture;
            iimPtr->Load(texture, iimPtr->GetImageFilename(wi, false, false));
            game::LoopManager::Instance()->TestingImageSet(texture);
            game::LoopManager::Instance()->TestingStrIncrement(TEST_PRE_STR);
            ++bladedStaffIndex;
            return false;
        }

        static auto hasTestedBreath{ false };
        if (false == hasTestedBreath)
        {
            game::item::weapon::WeaponInfo wi(game::item::weapon_type::Breath, "breath");
            wi.is_breath = true;
            sf::Texture texture;
            iimPtr->Load(texture, iimPtr->GetImageFilename(wi, false, false));
            game::LoopManager::Instance()->TestingImageSet(texture);
            game::LoopManager::Instance()->TestingStrIncrement(TEST_PRE_STR);
            hasTestedBreath = true;
            return false;
        }

        static auto hasTestedClaws{ false };
        if (false == hasTestedClaws)
        {
            game::item::weapon::WeaponInfo wi(game::item::weapon_type::Claws, "claws");
            wi.is_claws = true;
            sf::Texture texture;
            iimPtr->Load(texture, iimPtr->GetImageFilename(wi, false, false));
            game::LoopManager::Instance()->TestingImageSet(texture);
            game::LoopManager::Instance()->TestingStrIncrement(TEST_PRE_STR);
            hasTestedClaws = true;
            return false;
        }

        static auto clubIndex{ 0 };
        if (clubIndex < static_cast<int>(game::item::weapon::club_type::Count))
        {
            auto const ENUM{ static_cast<game::item::weapon::club_type::Enum>(clubIndex) };
            auto const STR{ boost::algorithm::to_lower_copy(game::item::weapon::club_type::ToString(ENUM)) };
            game::item::weapon::WeaponInfo wi(game::item::weapon_type::Club, STR);
            wi.club = ENUM;
            sf::Texture texture;
            iimPtr->Load(texture, iimPtr->GetImageFilename(wi, false, false));
            game::LoopManager::Instance()->TestingImageSet(texture);
            game::LoopManager::Instance()->TestingStrIncrement(TEST_PRE_STR);
            ++clubIndex;
            return false;
        }

        static auto hasTestedFists{ false };
        if (false == hasTestedFists)
        {
            game::item::weapon::WeaponInfo wi(game::item::weapon_type::Fists, "fists");
            wi.is_fists = true;
            sf::Texture texture;
            iimPtr->Load(texture, iimPtr->GetImageFilename(wi, false, false));
            game::LoopManager::Instance()->TestingImageSet(texture);
            game::LoopManager::Instance()->TestingStrIncrement(TEST_PRE_STR);
            hasTestedFists = true;
            return false;
        }

        static auto hasTestedKnife{ false };
        if (false == hasTestedKnife)
        {
            game::item::weapon::WeaponInfo wi(game::item::weapon_type::Knife, "knife");
            wi.is_knife = true;
            sf::Texture texture;
            iimPtr->Load(texture, iimPtr->GetImageFilename(wi, false, false));
            game::LoopManager::Instance()->TestingImageSet(texture);
            game::LoopManager::Instance()->TestingStrIncrement(TEST_PRE_STR);
            hasTestedKnife = true;
            return false;
        }

        static auto hasTestedDagger{ false };
        if (false == hasTestedDagger)
        {
            game::item::weapon::WeaponInfo wi(game::item::weapon_type::Knife, "dagger");
            wi.is_dagger = true;
            sf::Texture texture;
            iimPtr->Load(texture, iimPtr->GetImageFilename(wi, false, false));
            game::LoopManager::Instance()->TestingImageSet(texture);
            game::LoopManager::Instance()->TestingStrIncrement(TEST_PRE_STR);
            hasTestedDagger = true;
            return false;
        }

        static auto projIndex{ 0 };
        if (projIndex < static_cast<int>(game::item::weapon::projectile_type::Count))
        {
            auto const ENUM{ static_cast<game::item::weapon::projectile_type::Enum>(projIndex) };
            auto const STR{ boost::algorithm::to_lower_copy(game::item::weapon::projectile_type::ToString(ENUM)) };
            game::item::weapon::WeaponInfo wi(game::item::weapon_type::Projectile, STR);
            wi.projectile = ENUM;
            sf::Texture texture;
            iimPtr->Load(texture, iimPtr->GetImageFilename(wi, false, false));
            game::LoopManager::Instance()->TestingImageSet(texture);
            game::LoopManager::Instance()->TestingStrIncrement(TEST_PRE_STR);
            ++projIndex;
            return false;
        }

        static auto hasTestedStaff{ false };
        if (false == hasTestedStaff)
        {
            game::item::weapon::WeaponInfo wi(game::item::weapon_type::Staff, "staff");
            wi.is_staff = true;
            sf::Texture texture;
            iimPtr->Load(texture, iimPtr->GetImageFilename(wi, false, false));
            game::LoopManager::Instance()->TestingImageSet(texture);
            game::LoopManager::Instance()->TestingStrIncrement(TEST_PRE_STR);
            hasTestedStaff = true;
            return false;
        }

        static auto hasTestedQStaff{ false };
        if (false == hasTestedQStaff)
        {
            game::item::weapon::WeaponInfo wi(game::item::weapon_type::Staff, "quarterstaff");
            wi.is_quarterstaff = true;
            sf::Texture texture;
            iimPtr->Load(texture, iimPtr->GetImageFilename(wi, false, false));
            game::LoopManager::Instance()->TestingImageSet(texture);
            game::LoopManager::Instance()->TestingStrIncrement(TEST_PRE_STR);
            hasTestedQStaff = true;
            return false;
        }

        static auto swordIndex{ 0 };
        if (swordIndex < static_cast<int>(game::item::weapon::sword_type::Count))
        {
            auto const ENUM{ static_cast<game::item::weapon::sword_type::Enum>(swordIndex) };
            auto const STR{ boost::algorithm::to_lower_copy(game::item::weapon::sword_type::ToString(ENUM)) };
            game::item::weapon::WeaponInfo wi(game::item::weapon_type::Sword, STR);
            wi.sword = ENUM;
            sf::Texture texture;
            iimPtr->Load(texture, iimPtr->GetImageFilename(wi, false, false));
            game::LoopManager::Instance()->TestingImageSet(texture);
            game::LoopManager::Instance()->TestingStrIncrement(TEST_PRE_STR);
            ++swordIndex;
            return false;
        }

        static auto hasTestedTendrils{ false };
        if (false == hasTestedTendrils)
        {
            game::item::weapon::WeaponInfo wi(game::item::weapon_type::Tendrils, "tendrils");
            wi.is_tendrils = true;
            sf::Texture texture;
            iimPtr->Load(texture, iimPtr->GetImageFilename(wi, false, false));
            game::LoopManager::Instance()->TestingImageSet(texture);
            game::LoopManager::Instance()->TestingStrIncrement(TEST_PRE_STR);
            hasTestedTendrils = true;
            return false;
        }

        //test armor items
        static auto aventailIndex{ 0 };
        if (aventailIndex < static_cast<int>(game::item::armor::base_type::Count))
        {
            game::item::armor::ArmorInfo ai(game::item::armor_type::Aventail);
            ai.base = static_cast<game::item::armor::base_type::Enum>(aventailIndex);
            ai.is_aventail = true;
            sf::Texture texture;
            iimPtr->Load(texture, iimPtr->GetImageFilename(ai, false, false));
            game::LoopManager::Instance()->TestingImageSet(texture);
            game::LoopManager::Instance()->TestingStrIncrement(TEST_PRE_STR);
            ++aventailIndex;
            return false;
        }

        static auto bootsIndex{ 0 };
        if (bootsIndex < static_cast<int>(game::item::armor::base_type::Count))
        {
            game::item::armor::ArmorInfo ai(game::item::armor_type::Boots);
            ai.base = static_cast<game::item::armor::base_type::Enum>(bootsIndex);
            ai.is_boots = true;
            sf::Texture texture;
            iimPtr->Load(texture, iimPtr->GetImageFilename(ai, false, false));
            game::LoopManager::Instance()->TestingImageSet(texture);
            game::LoopManager::Instance()->TestingStrIncrement(TEST_PRE_STR);
            ++bootsIndex;
            return false;
        }

        static auto bracerIndex{ 0 };
        if (bracerIndex < static_cast<int>(game::item::armor::base_type::Count))
        {
            game::item::armor::ArmorInfo ai(game::item::armor_type::Bracer);
            ai.base = static_cast<game::item::armor::base_type::Enum>(bracerIndex);
            ai.is_bracer = true;
            sf::Texture texture;
            iimPtr->Load(texture, iimPtr->GetImageFilename(ai, false, false));
            game::LoopManager::Instance()->TestingImageSet(texture);
            game::LoopManager::Instance()->TestingStrIncrement(TEST_PRE_STR);
            ++bracerIndex;
            return false;
        }

        static auto coverIndex{ 0 };
        if (coverIndex < static_cast<int>(game::item::armor::cover_type::Count))
        {
            game::item::armor::ArmorInfo ai(game::item::armor_type::Covering);
            ai.cover = static_cast<game::item::armor::cover_type::Enum>(coverIndex);
            sf::Texture texture;
            iimPtr->Load(texture, iimPtr->GetImageFilename(ai, false, false));
            game::LoopManager::Instance()->TestingImageSet(texture);
            game::LoopManager::Instance()->TestingStrIncrement(TEST_PRE_STR);
            ++coverIndex;
            return false;
        }

        static auto helmIndex{ 0 };
        if (helmIndex < static_cast<int>(game::item::armor::helm_type::Count))
        {
            game::item::armor::ArmorInfo ai(game::item::armor_type::Helm);
            ai.helm = static_cast<game::item::armor::helm_type::Enum>(helmIndex);
            sf::Texture texture;
            iimPtr->Load(texture, iimPtr->GetImageFilename(ai, false, false));
            game::LoopManager::Instance()->TestingImageSet(texture);
            game::LoopManager::Instance()->TestingStrIncrement(TEST_PRE_STR);
            ++helmIndex;
            return false;
        }

        static auto pantIndex{ 0 };
        if (pantIndex < static_cast<int>(game::item::armor::base_type::Count))
        {
            game::item::armor::ArmorInfo ai(game::item::armor_type::Pants);
            ai.base = static_cast<game::item::armor::base_type::Enum>(pantIndex);
            ai.is_pants = true;
            sf::Texture texture;
            iimPtr->Load(texture, iimPtr->GetImageFilename(ai, false, false));
            game::LoopManager::Instance()->TestingImageSet(texture);
            game::LoopManager::Instance()->TestingStrIncrement(TEST_PRE_STR);
            ++pantIndex;
            return false;
        }

        static auto shieldIndex{ 0 };
        if (shieldIndex < static_cast<int>(game::item::armor::shield_type::Count))
        {
            game::item::armor::ArmorInfo ai(game::item::armor_type::Sheild);
            ai.shield = static_cast<game::item::armor::shield_type::Enum>(shieldIndex);
            sf::Texture texture;
            iimPtr->Load(texture, iimPtr->GetImageFilename(ai, false, false));
            game::LoopManager::Instance()->TestingImageSet(texture);
            game::LoopManager::Instance()->TestingStrIncrement(TEST_PRE_STR);
            ++shieldIndex;
            return false;
        }

        static auto shirtIndex{ 0 };
        if (shirtIndex < static_cast<int>(game::item::armor::base_type::Count))
        {
            game::item::armor::ArmorInfo ai(game::item::armor_type::Shirt);
            ai.base = static_cast<game::item::armor::base_type::Enum>(shirtIndex);
            ai.is_shirt = true;
            sf::Texture texture;
            iimPtr->Load(texture, iimPtr->GetImageFilename(ai, false, false));
            game::LoopManager::Instance()->TestingImageSet(texture);
            game::LoopManager::Instance()->TestingStrIncrement(TEST_PRE_STR);
            ++shirtIndex;
            return false;
        }

        //test misc items
        auto const RAND_REPEAT_COUNT{ 501 };
        static auto miscIndex{ 1 };
        static auto randIndex{ 0 };
        if (miscIndex < static_cast<int>(game::item::misc_type::Count))
        {
            auto const ENUM{ static_cast<game::item::misc_type::Enum>(miscIndex) };
            auto const FILENAME_NONRAND{ iimPtr->GetImageFilename(ENUM, false, false) };
            M_ASSERT_OR_LOGANDTHROW_SS((FILENAME_NONRAND.empty() == false), "sfml_util::gui::ItemImageManager::Test() (non-rand)  While testing misc items #" << miscIndex << " \"" << game::item::misc_type::ToString(ENUM) << "\", GetImageFilename() returned an empty string.");
            sf::Texture textureBase;
            iimPtr->Load(textureBase, FILENAME_NONRAND);
            
            if (randIndex < RAND_REPEAT_COUNT)
            {
                auto const FILENAME_RAND{ iimPtr->GetImageFilename(ENUM, false, true) };
                M_ASSERT_OR_LOGANDTHROW_SS((FILENAME_RAND.empty() == false),     "sfml_util::gui::ItemImageManager::Test() (rand)  While testing misc items #" << randIndex << " \"" << game::item::misc_type::ToString(ENUM) << "\", GetImageFilename() returned an empty string.");
                sf::Texture textureRand;
                iimPtr->Load(textureRand, FILENAME_RAND);
                game::LoopManager::Instance()->TestingImageSet(textureRand);

                auto const ENUM_STR{ game::item::misc_type::ToString(ENUM) };

                std::ostringstream ss;
                ss << "ItemImageManager Rand Test of " << ENUM_STR << "#";
                game::LoopManager::Instance()->TestingStrIncrement(ss.str());

                ++randIndex;
                return false;
            }

            randIndex = 0;
            ++miscIndex;
            return false;
        }

        //test item ToString() functions
        {
            static unsigned int categoryIndex{ 0 };
            if (categoryIndex <= game::item::category::Edible)
            {
                auto const STR{ game::item::category::ToString(static_cast<game::item::category::Enum>(categoryIndex), false) };
                game::LoopManager::Instance()->TestingStrIncrement("ItemImageManager category::ToString(\"" + STR + "\") Test.");
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
            if (weaponTypeIndex <= game::item::weapon_type::Breath)
            {
                auto const STR{ game::item::weapon_type::ToString(static_cast<game::item::weapon_type::Enum>(weaponTypeIndex), false) };
                game::LoopManager::Instance()->TestingStrIncrement("ItemImageManager weapon_type::ToString(\"" + STR + "\") Test.");
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
            if (armorTypeIndex <= game::item::armor_type::Skin)
            {
                auto const STR{ game::item::armor_type::ToString(static_cast<game::item::armor_type::Enum>(armorTypeIndex), false) };
                game::LoopManager::Instance()->TestingStrIncrement("ItemImageManager armor_type::ToString(\"" + STR + "\") Test.");
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
            if (collecterTypeIndex <= game::non_player::ownership::collector_type::Hoarder)
            {
                auto const STR{ game::non_player::ownership::collector_type::ToString(static_cast<game::non_player::ownership::collector_type::Enum>(collecterTypeIndex)) };
                game::LoopManager::Instance()->TestingStrIncrement("ItemImageManager non_player::ownership::collector_type::ToString(\"" + STR + "\") Test.");
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

        game::LoopManager::Instance()->TestingStrAppend("sfml_util::gui::ItemImageManager::Test()  ALL TESTS PASSED.");
        return true;
    }


    void ItemImageManager::SetItemImageDirectory(const std::string & PATH)
    {
        imagesDirectoryPath_ = PATH;
    }


    void ItemImageManager::Load(sf::Texture & texture, const std::string & IMAGE_FILE_NAME) const
    {
        namespace bfs = boost::filesystem;
        const bfs::path PATH_OBJ(bfs::system_complete(bfs::path(imagesDirectoryPath_) / bfs::path(IMAGE_FILE_NAME)));
        sfml_util::LoadImageOrTexture(texture, PATH_OBJ.string());
    }


    void ItemImageManager::Load(sf::Texture & texture, const game::item::ItemPtr_t ITEM_PTR) const
    {
        return Load(texture, ITEM_PTR->ImageFilename() + EXT_);
    }


    void ItemImageManager::Load(sf::Texture &                     texture,
                                const game::item::misc_type::Enum ITEM_ENUM,
                                const bool                        IS_JEWELED,
                                const bool                        WILL_RANDOMIZE) const
    {
        Load(texture, GetImageFilename(ITEM_ENUM, IS_JEWELED, WILL_RANDOMIZE) );
    }


    const std::string ItemImageManager::GetImageFilename(const game::item::ItemPtr_t ITEM_PTR,
                                                         const bool                  WILL_RANDOMIZE) const
    {
        using namespace game::item;

        if (ITEM_PTR->IsWeapon() && (ITEM_PTR->Weapon_Info().type != weapon_type::NotAWeapon))
        {
            return GetImageFilename(ITEM_PTR->Weapon_Info(), ITEM_PTR->IsJeweled(), WILL_RANDOMIZE);
        }
        else if (ITEM_PTR->IsArmor() && (ITEM_PTR->Armor_Info().type != armor_type::NotArmor))
        {
            return GetImageFilename(ITEM_PTR->Armor_Info(), ITEM_PTR->IsJeweled(), WILL_RANDOMIZE);
        }
        else if (ITEM_PTR->MiscType() != misc_type::NotMisc)
        {
            return GetImageFilename(ITEM_PTR->MiscType(), ITEM_PTR->IsJeweled(), WILL_RANDOMIZE);
        }
        else
        {
            std::ostringstream ss;
            ss << "sfml_util::gui::ItemImageManager::GetImageFilename(item->Name()=" << ITEM_PTR->Name()
                << ", item->Category=" << category::ToString(ITEM_PTR->Category(), false) << ", desc=\""
                <<  ITEM_PTR->Desc() << "\") failed to be categorized.  (IsWeapon=" << std::boolalpha
                << ITEM_PTR->IsWeapon() << ", WeaponInfo.type="
                << weapon_type::ToString(ITEM_PTR->Weapon_Info().type, false) << ", IsArmor="
                << std::boolalpha << ITEM_PTR->IsArmor() << ", ArmorInfo.type="
                << armor_type::ToString(ITEM_PTR->Armor_Info().type, false) << ", MiscType()="
                << ITEM_PTR->MiscType() << ")";

            throw std::runtime_error(ss.str());
        }
    }


    const std::string ItemImageManager::GetImageFilename(const game::item::weapon::WeaponInfo & WEAPON_INFO, const bool IS_JEWELED, const bool) const
    {
        using namespace game::item;

        if ((WEAPON_INFO.type & weapon_type::Sword) != 0)
            return weapon::sword_type::ToString(WEAPON_INFO.sword) + EXT_;

        if ((WEAPON_INFO.type & weapon_type::Axe) != 0)
            return weapon::axe_type::ToString(WEAPON_INFO.axe) + EXT_;

        if ((WEAPON_INFO.type & weapon_type::Club) != 0)
            return weapon::club_type::ToString(WEAPON_INFO.club) + EXT_;

        if ((WEAPON_INFO.type & weapon_type::Whip) != 0)
            return weapon::whip_type::ToString(WEAPON_INFO.whip) + EXT_;

        if ((WEAPON_INFO.type & weapon_type::Projectile) != 0)
            return weapon::projectile_type::ToString(WEAPON_INFO.projectile) + EXT_;

        if ((WEAPON_INFO.type & weapon_type::BladedStaff) != 0)
            return weapon::bladedstaff_type::ToString(WEAPON_INFO.bladedstaff) + EXT_;

        if (WEAPON_INFO.is_bite)
            return "bite" + EXT_;

        if (WEAPON_INFO.is_breath)
            return "breath" + EXT_;

        if (WEAPON_INFO.is_claws)
            return "claws" + EXT_;

        if (WEAPON_INFO.is_dagger)
            return "dagger" + EXT_;

        if (WEAPON_INFO.is_fists)
            return "fists" + EXT_;

        if (WEAPON_INFO.is_knife)
            return "knife" + EXT_;

        if (WEAPON_INFO.is_quarterstaff)
            return "quarter-staff" + EXT_;

        if (WEAPON_INFO.is_staff)
        {
            if (IS_JEWELED)
                return "staff-special-2" + EXT_;
            else
                return "staff-plain" + EXT_;
        }

        if (WEAPON_INFO.is_tendrils)
            return "tendrils" + EXT_;

        std::ostringstream ss;
        ss << "sfml_util::gui::ItemImageManager::GetImageFilename(WEAPON_INFO.type=" << weapon_type::ToString(WEAPON_INFO.type, false) << ") failed to resolve a filename.";
        throw std::runtime_error(ss.str());
    }


    const std::string ItemImageManager::GetImageFilename(const game::item::armor::ArmorInfo & ARMOR_INFO, const bool, const bool) const
    {
        using namespace game::item;

        if ((ARMOR_INFO.type & armor_type::Aventail) != 0)
            return "aventail" + EXT_;

        if ((ARMOR_INFO.type & armor_type::Boots) != 0)
            return "Boots-" + armor::base_type::ToString(ARMOR_INFO.base) + EXT_;

        if (((ARMOR_INFO.type & armor_type::Bracer) != 0) || (ARMOR_INFO.is_bracer))
            return "bracer" + EXT_;

        if ((ARMOR_INFO.type & armor_type::Covering) != 0)
            return armor::cover_type::ToString(ARMOR_INFO.cover) + EXT_;

        if (((ARMOR_INFO.type & armor_type::Gauntlets) != 0) || (ARMOR_INFO.is_gauntlets))
        {
            if (ARMOR_INFO.base == armor::base_type::Plain)
                return "gloves" + EXT_;
            else
                return "Gauntlets-" + armor::base_type::ToString(ARMOR_INFO.base) + EXT_;
        }

        if ((ARMOR_INFO.type & armor_type::Helm) != 0)
            return armor::helm_type::ToString(ARMOR_INFO.helm) + "-Helm" + EXT_;

        if (((ARMOR_INFO.type & armor_type::Pants) != 0) || (ARMOR_INFO.is_pants))
            return "pants" + EXT_;

        if ((ARMOR_INFO.type & armor_type::Sheild) != 0)
            return armor::shield_type::ToString(ARMOR_INFO.shield) + "-Shield" + EXT_;

        if (((ARMOR_INFO.type & armor_type::Shirt) != 0) || (ARMOR_INFO.is_shirt))
            return "Shirt-" + armor::base_type::ToString(ARMOR_INFO.base) + EXT_;

        std::ostringstream ss;
        ss << "sfml_util::gui::ItemImageManager::GetImageFilename(ARMOR_INFO.type=" << armor_type::ToString(ARMOR_INFO.type, false) << ") failed to resolve a filename.";
        throw std::runtime_error(ss.str());
    }


    const std::string ItemImageManager::GetImageFilename(const game::item::misc_type::Enum ITEM_ENUM, const bool IS_JEWELED, const bool WILL_RANDOMIZE) const
    {
        switch (ITEM_ENUM)
        {
            case game::item::misc_type::Charm:
            case game::item::misc_type::Amulet:
            case game::item::misc_type::Pendant:
            case game::item::misc_type::Medallion:
            {
                if (WILL_RANDOMIZE)
                {
                    std::ostringstream ss;
                    ss << "amulet-" << misc::random::Int(1, 23) << EXT_;
                    return ss.str();
                }
                else
                    return "amulet-20" + EXT_;
            }
            case game::item::misc_type::Necklas:
            {
                return "necklas-1" + EXT_;
            }
            case game::item::misc_type::Bag:
            {
                if (WILL_RANDOMIZE)
                {
                    std::ostringstream ss;
                    ss << "bag-" << misc::random::Int(1, 8) << EXT_;
                    return ss.str();
                }
                else
                    return "bag-2" + EXT_;
            }
            case game::item::misc_type::Tome:
            case game::item::misc_type::Book:
            {
                if (WILL_RANDOMIZE)
                {
                    std::ostringstream ss;
                    ss << "book-" << misc::random::Int(1, 5) << EXT_;
                    return ss.str();
                }
                else
                    return "book-2" + EXT_;
            }
            case game::item::misc_type::Cape:
            {
                return "cape" + EXT_;
            }
            case game::item::misc_type::Cloak:
            {
                return "cloak" + EXT_;
            }
            case game::item::misc_type::Robe:
            {
                return "robe" + EXT_;
            }
            case game::item::misc_type::Crown:
            {
                if (WILL_RANDOMIZE)
                {
                    std::ostringstream ss;
                    ss << "crown-" << misc::random::Int(1, 12) << EXT_;
                    return ss.str();
                }
                else
                    return "crown-5" + EXT_;
            }
            case game::item::misc_type::Doll:
            {
                if (WILL_RANDOMIZE)
                {
                    std::ostringstream ss;
                    ss << "doll-" << misc::random::Int(1, 16) << EXT_;
                    return ss.str();
                }
                else
                    return "doll-1" + EXT_;
            }
            case game::item::misc_type::Drink:
            case game::item::misc_type::Potion:
            {
                if (WILL_RANDOMIZE)
                {
                    std::ostringstream ss;
                    ss << "potion-" << misc::random::Int(1, 30) << EXT_;
                    return ss.str();
                }
                    return "potion-13" + EXT_;
            }
            case game::item::misc_type::Goblet:
            {
                if (WILL_RANDOMIZE)
                {
                    std::ostringstream ss;
                    ss << "goblet-" << misc::random::Int(1, 8) << EXT_;
                    return ss.str();
                }
                else
                    return "goblet-5" + EXT_;
            }
            case game::item::misc_type::Herbs:
            {
                if (WILL_RANDOMIZE)
                {
                    std::ostringstream ss;
                    ss << "herb-" << misc::random::Int(1, 34) << EXT_;
                    return ss.str();
                }
                    return "herb-26" + EXT_;
            }
            case game::item::misc_type::Hourglass:
            {
                if (WILL_RANDOMIZE)
                {
                    std::ostringstream ss;
                    ss << "hourglass-" << misc::random::Int(1, 3) << EXT_;
                    return ss.str();
                }
                else
                    return "hourglass-1" + EXT_;
            }
            case game::item::misc_type::Key:
            {
                if (WILL_RANDOMIZE)
                {
                    std::ostringstream ss;
                    ss << "key-" << misc::random::Int(1, 11) << EXT_;
                    return ss.str();
                }
                else
                    return "key-2" + EXT_;
            }
            case game::item::misc_type::Lantern:
            {
                if (WILL_RANDOMIZE)
                {
                    std::ostringstream ss;
                    ss << "lantern-" << misc::random::Int(1, 18) << EXT_;
                    return ss.str();
                }
                else
                    return "lantern-10" + EXT_;
            }
            case game::item::misc_type::Torch:
            {
                if (WILL_RANDOMIZE)
                {
                    std::ostringstream ss;
                    ss << "torch-" << misc::random::Int(1, 5) << EXT_;
                    return ss.str();
                }
                    return "torch-1" + EXT_;
            }
            case game::item::misc_type::Lockbox:
            {
                if (WILL_RANDOMIZE)
                {
                    std::ostringstream ss;
                    ss << "lockbox-" << misc::random::Int(1, 8) << EXT_;
                    return ss.str();
                }
                else
                    return "lockbox-2" + EXT_;
            }
            case game::item::misc_type::Chest:
            {
                if (WILL_RANDOMIZE)
                {
                    std::ostringstream ss;
                    ss << "chest-" << misc::random::Int(1, 14) << EXT_;
                    return ss.str();
                }
                else
                    return "chest-4" + EXT_;
            }
            case game::item::misc_type::LockPicks:
            {
                return "lockpicks" + EXT_;
            }
            case game::item::misc_type::Mask:
            {
                if (WILL_RANDOMIZE)
                {
                    std::ostringstream ss;
                    ss << "mask-" << misc::random::Int(1, 6) << EXT_;
                    return ss.str();
                }
                else
                    return "mask-3" + EXT_;
            }
            case game::item::misc_type::Mirror:
            {
                if (WILL_RANDOMIZE)
                {
                    std::ostringstream ss;
                    ss << "mirror-" << misc::random::Int(1, 10) << EXT_;
                    return ss.str();
                }
                else
                    return "mirror-7" + EXT_;
            }
            case game::item::misc_type::Lute:
            {
                return "lute" + EXT_;
            }
            case game::item::misc_type::Panflute:
            {
                return "panflute" + EXT_;
            }
            case game::item::misc_type::Flute:
            {
                return "flute" + EXT_;
            }
            case game::item::misc_type::Sitar:
            {
                return "sitar" + EXT_;
            }
            case game::item::misc_type::Scroll:
            {
                if (WILL_RANDOMIZE)
                {
                    std::ostringstream ss;
                    ss << "scroll-" << misc::random::Int(1, 13) << EXT_;
                    return ss.str();
                }
                else
                    return "scroll-6" + EXT_;
            }
            case game::item::misc_type::Orb:
            {
                if (WILL_RANDOMIZE)
                {
                    std::ostringstream ss;
                    ss << "orb-" << misc::random::Int(1, 13) << EXT_;
                    return ss.str();
                }
                else
                    return "orb-4" + EXT_;
            }
            case game::item::misc_type::Ring:
            {
                if (WILL_RANDOMIZE)
                {
                    std::ostringstream ss;
                    if (IS_JEWELED)
                        ss << "ring-jeweled-" << misc::random::Int(1, 14) << EXT_;
                    else
                        ss << "ring-plain-" << misc::random::Int(1, 2) << EXT_;
                    return ss.str();
                }
                else
                {
                    if (IS_JEWELED)
                        return "ring-jeweled-4" + EXT_;
                    else
                        return "ring-plain-2" + EXT_;
                }
            }
            case game::item::misc_type::Skull:
            {
                if (WILL_RANDOMIZE)
                {
                    std::ostringstream ss;
                    ss << "skull-" << misc::random::Int(1, 11) << EXT_;
                    return ss.str();
                }
                else
                    return "skull-3" + EXT_;
            }
            case game::item::misc_type::Shard:
            {
                if (WILL_RANDOMIZE)
                {
                    std::ostringstream ss;
                    ss << "shard-" << misc::random::Int(1, 7) << EXT_;
                    return ss.str();
                }
                else
                    return "shard-1" + EXT_;
            }
            case game::item::misc_type::Salve:
            {
                if (WILL_RANDOMIZE)
                {
                    std::ostringstream ss;
                    ss << "mortar-" << misc::random::Int(1, 9) << EXT_;
                    return ss.str();
                }
                else
                    return "mortar-3" + EXT_;
            }
            case game::item::misc_type::Wand:
            {
                if (WILL_RANDOMIZE)
                {
                    std::ostringstream ss;
                    ss << "wand-" << misc::random::Int(1, 9) << EXT_;
                    return ss.str();
                }
                else
                    return "wand-7" + EXT_;
            }
            case game::item::misc_type::NotMisc:
            case game::item::misc_type::Count:
            default:
            {
                std::ostringstream ss;
                ss << "sfml_util::gui::ItemImageManager::GetImageFilename(misc_type::Enum=" << ITEM_ENUM << ")_UnknownOrInvalidValueError.";
                throw std::range_error(ss.str());
            }
        }
    }

}
}
