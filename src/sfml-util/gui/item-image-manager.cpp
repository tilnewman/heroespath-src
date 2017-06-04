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
#include <vector>


namespace sfml_util
{
namespace gui
{

    std::unique_ptr<ItemImageManager> ItemImageManager::instanceUPtr_{ nullptr };
    std::string ItemImageManager::imagesDirectoryPath_               { "" };
    const std::string ItemImageManager::FILE_EXT_STR_                { ".png" };


    ItemImageManager::ItemImageManager()
    {
        M_HP_LOG_DBG("Singleton Construction: ItemImageManager");
    }


    ItemImageManager::~ItemImageManager()
    {
        M_HP_LOG_DBG("Singleton Construction: ItemImageManager");
    }


    ItemImageManager * ItemImageManager::Instance()
    {
        if (instanceUPtr_.get() == nullptr)
        {
            M_HP_LOG_WRN("Singleton Instance() before Acquire(): ItemImageManager");
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
        else
        {
            M_HP_LOG_WRN("Singleton Acquire() after Construction: ItemImageManager");
        }
    }


    void ItemImageManager::Release()
    {
        M_ASSERT_OR_LOGANDTHROW_SS((instanceUPtr_.get() != nullptr),
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

        //test weapon items
        static auto axeIndex{ 0 };
        if (axeIndex < static_cast<int>(game::item::weapon::axe_type::Count))
        {
            auto const ENUM{ static_cast<game::item::weapon::axe_type::Enum>(axeIndex) };

            auto const STR { boost::algorithm::to_lower_copy(
                game::item::weapon::axe_type::ToString(ENUM)) };

            game::item::weapon::WeaponInfo wi(game::item::weapon_type::Axe, STR);
            wi.axe = ENUM;
            sf::Texture texture;
            iimPtr->Load(texture, iimPtr->GetImageFilename(wi, false));
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
            iimPtr->Load(texture, iimPtr->GetImageFilename(wi, false));
            game::LoopManager::Instance()->TestingImageSet(texture);
            game::LoopManager::Instance()->TestingStrIncrement(TEST_PRE_STR);
            hasTestedBite = true;
            return false;
        }

        static auto bladedStaffIndex{ 0 };
        if (bladedStaffIndex < static_cast<int>(game::item::weapon::bladedstaff_type::Count))
        {
            auto const ENUM{ static_cast<game::item::weapon::bladedstaff_type::Enum>(
                bladedStaffIndex) };

            auto const STR{ boost::algorithm::to_lower_copy(
                game::item::weapon::bladedstaff_type::ToString(ENUM)) };

            game::item::weapon::WeaponInfo wi(game::item::weapon_type::BladedStaff, STR);
            wi.bladedstaff = ENUM;
            sf::Texture texture;
            iimPtr->Load(texture, iimPtr->GetImageFilename(wi, false));
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
            iimPtr->Load(texture, iimPtr->GetImageFilename(wi, false));
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
            iimPtr->Load(texture, iimPtr->GetImageFilename(wi, false));
            game::LoopManager::Instance()->TestingImageSet(texture);
            game::LoopManager::Instance()->TestingStrIncrement(TEST_PRE_STR);
            hasTestedClaws = true;
            return false;
        }

        static auto clubIndex{ 0 };
        if (clubIndex < static_cast<int>(game::item::weapon::club_type::Count))
        {
            auto const ENUM{ static_cast<game::item::weapon::club_type::Enum>(clubIndex) };

            auto const STR{ boost::algorithm::to_lower_copy(
                game::item::weapon::club_type::ToString(ENUM)) };

            game::item::weapon::WeaponInfo wi(game::item::weapon_type::Club, STR);
            wi.club = ENUM;
            sf::Texture texture;
            iimPtr->Load(texture, iimPtr->GetImageFilename(wi, false));
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
            iimPtr->Load(texture, iimPtr->GetImageFilename(wi, false));
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
            iimPtr->Load(texture, iimPtr->GetImageFilename(wi, false));
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
            iimPtr->Load(texture, iimPtr->GetImageFilename(wi, false));
            game::LoopManager::Instance()->TestingImageSet(texture);
            game::LoopManager::Instance()->TestingStrIncrement(TEST_PRE_STR);
            hasTestedDagger = true;
            return false;
        }

        static auto projIndex{ 0 };
        if (projIndex < static_cast<int>(game::item::weapon::projectile_type::Count))
        {
            auto const ENUM{ static_cast<game::item::weapon::projectile_type::Enum>(projIndex) };

            auto const STR{ boost::algorithm::to_lower_copy(
                game::item::weapon::projectile_type::ToString(ENUM)) };

            game::item::weapon::WeaponInfo wi(game::item::weapon_type::Projectile, STR);
            wi.projectile = ENUM;
            sf::Texture texture;
            iimPtr->Load(texture, iimPtr->GetImageFilename(wi, false));
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
            iimPtr->Load(texture, iimPtr->GetImageFilename(wi, false));
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
            iimPtr->Load(texture, iimPtr->GetImageFilename(wi, false));
            game::LoopManager::Instance()->TestingImageSet(texture);
            game::LoopManager::Instance()->TestingStrIncrement(TEST_PRE_STR);
            hasTestedQStaff = true;
            return false;
        }

        static auto swordIndex{ 0 };
        if (swordIndex < static_cast<int>(game::item::weapon::sword_type::Count))
        {
            auto const ENUM{ static_cast<game::item::weapon::sword_type::Enum>(swordIndex) };

            auto const STR{ boost::algorithm::to_lower_copy(
                game::item::weapon::sword_type::ToString(ENUM)) };

            game::item::weapon::WeaponInfo wi(game::item::weapon_type::Sword, STR);
            wi.sword = ENUM;
            sf::Texture texture;
            iimPtr->Load(texture, iimPtr->GetImageFilename(wi, false));
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
            iimPtr->Load(texture, iimPtr->GetImageFilename(wi, false));
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
            iimPtr->Load(texture, iimPtr->GetImageFilename(ai));
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
            iimPtr->Load(texture, iimPtr->GetImageFilename(ai));
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
            iimPtr->Load(texture, iimPtr->GetImageFilename(ai));
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
            iimPtr->Load(texture, iimPtr->GetImageFilename(ai));
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
            iimPtr->Load(texture, iimPtr->GetImageFilename(ai));
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
            iimPtr->Load(texture, iimPtr->GetImageFilename(ai));
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
            iimPtr->Load(texture, iimPtr->GetImageFilename(ai));
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
            iimPtr->Load(texture, iimPtr->GetImageFilename(ai));
            game::LoopManager::Instance()->TestingImageSet(texture);
            game::LoopManager::Instance()->TestingStrIncrement(TEST_PRE_STR);
            ++shirtIndex;
            return false;
        }

        //test misc items
        static auto miscIndex{ 1 };
        static auto isJeweled{ false };
        if (miscIndex < static_cast<int>(game::item::misc_type::Count))
        {
            auto const ENUM{ static_cast<game::item::misc_type::Enum>(miscIndex) };
            auto const ENUM_STR{ game::item::misc_type::ToString(ENUM) };
            auto const FILENAMES_VEC = iimPtr->GetImageFilenames(ENUM, isJeweled);

            M_ASSERT_OR_LOGANDTHROW_SS((FILENAMES_VEC.empty() == false),
                "sfml_util::gui::ItemImageManager::Test() While testing misc item #" << miscIndex
                << " \"" << ENUM_STR << "\", is_jeweled=" << std::boolalpha << isJeweled
                << ", GetImageFilenames() returned an empty vector.");

            static std::size_t fileIndex{ 0 };
            if (fileIndex < FILENAMES_VEC.size())
            {
                auto const NEXT_FILENAME{ FILENAMES_VEC[fileIndex] };

                M_ASSERT_OR_LOGANDTHROW_SS((NEXT_FILENAME.empty() == false),
                    "sfml_util::gui::ItemImageManager::Test() (rand)  While testing misc item #"
                    << miscIndex << " \"" << ENUM_STR << "\", filename #" << fileIndex
                    << ", is_jeweled=" << std::boolalpha << isJeweled
                    << ", found an empty filename string.");

                std::ostringstream ss;
                ss << "ItemImageManager Test of " << ENUM_STR << " #" << fileIndex
                    << ((isJeweled) ? "jeweled" : "not-jeweled" );

                game::LoopManager::Instance()->TestingStrIncrement(ss.str());

                sf::Texture texture;
                iimPtr->Load(texture, NEXT_FILENAME);
                game::LoopManager::Instance()->TestingImageSet(texture);

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

        //test item ToString() functions
        {
            static unsigned int categoryIndex{ 0 };
            if (categoryIndex <= game::item::category::Edible)
            {
                auto const STR{ game::item::category::ToString(
                    static_cast<game::item::category::Enum>(categoryIndex), false) };

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
            if (weaponTypeIndex <= game::item::weapon_type::Breath)
            {
                auto const STR{ game::item::weapon_type::ToString(
                    static_cast<game::item::weapon_type::Enum>(weaponTypeIndex), false) };

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
            if (armorTypeIndex <= game::item::armor_type::Skin)
            {
                auto const STR{ game::item::armor_type::ToString(
                    static_cast<game::item::armor_type::Enum>(armorTypeIndex), false) };

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
            if (collecterTypeIndex <= game::non_player::ownership::collector_type::Hoarder)
            {
                auto const STR{ game::non_player::ownership::collector_type::ToString(
                    static_cast<game::non_player::ownership::collector_type::Enum>(
                        collecterTypeIndex)) };

                game::LoopManager::Instance()->TestingStrIncrement(
                    "ItemImageManager non_player::ownership::collector_type::ToString(\"" +
                    STR + "\") Test.");

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


    void ItemImageManager::Load(sf::Texture & texture, const std::string & IMAGE_FILE_NAME) const
    {
        namespace bfs = boost::filesystem;
        const bfs::path PATH_OBJ(bfs::system_complete(bfs::path(imagesDirectoryPath_) /
            bfs::path(IMAGE_FILE_NAME)));

        sfml_util::LoadImageOrTexture(texture, PATH_OBJ.string());
    }


    void ItemImageManager::Load(sf::Texture & texture, const game::item::ItemPtr_t ITEM_PTR) const
    {
        return Load(texture, ITEM_PTR->ImageFilename() + FILE_EXT_STR_);
    }


    void ItemImageManager::Load(sf::Texture &                     texture,
                                const game::item::misc_type::Enum ITEM_ENUM,
                                const bool                        IS_JEWELED,
                                const bool                        WILL_RANDOMIZE) const
    {
        Load(texture, GetImageFilename(ITEM_ENUM, IS_JEWELED, WILL_RANDOMIZE));
    }


    const std::string ItemImageManager::GetImageFilename(
        const game::item::misc_type::Enum ITEM_ENUM,
        const bool                        IS_JEWELED,
        const bool                        WILL_RANDOMIZE) const
    {
        auto const FILENAMES_VEC(GetImageFilenames(ITEM_ENUM, IS_JEWELED));

        M_ASSERT_OR_LOGANDTHROW_SS((FILENAMES_VEC.empty() == false),
            "sfml_util::gui::ItemImageManager::GetImageFilename(misc, \""
            << game::item::misc_type::ToString(ITEM_ENUM) << "\", is_jeweled="
            << std::boolalpha << IS_JEWELED << ", will_rand=" << WILL_RANDOMIZE
            << ") unable to get any filenames for those settings.");

        if (WILL_RANDOMIZE)
        {
            return FILENAMES_VEC[
                static_cast<std::size_t>(misc::random::Int(0,
                    static_cast<int>(FILENAMES_VEC.size()) - 1))];
        }
        else
        {
            return FILENAMES_VEC[0];
        }
    }


    const std::string ItemImageManager::GetImageFilename(
        const game::item::ItemPtr_t ITEM_PTR,
        const bool                  WILL_RANDOMIZE) const
    {
        using namespace game::item;

        if (ITEM_PTR->IsWeapon() && (ITEM_PTR->Weapon_Info().type != weapon_type::NotAWeapon))
        {
            return GetImageFilename(ITEM_PTR->Weapon_Info(), ITEM_PTR->IsJeweled());
        }
        else if (ITEM_PTR->IsArmor() && (ITEM_PTR->Armor_Info().type != armor_type::NotArmor))
        {
            return GetImageFilename(ITEM_PTR->Armor_Info());
        }
        else if (ITEM_PTR->MiscType() != misc_type::NotMisc)
        {
            return GetImageFilename(ITEM_PTR->MiscType(), ITEM_PTR->IsJeweled(), WILL_RANDOMIZE);
        }
        else
        {
            std::ostringstream ss;
            ss << "sfml_util::gui::ItemImageManager::GetImageFilename(item->Name()="
                << ITEM_PTR->Name() << ", item->Category="
                << category::ToString(ITEM_PTR->Category(), false) << ", desc=\""
                <<  ITEM_PTR->Desc() << "\") failed to be categorized.  (IsWeapon="
                << std::boolalpha << ITEM_PTR->IsWeapon() << ", WeaponInfo.type="
                << weapon_type::ToString(ITEM_PTR->Weapon_Info().type, false) << ", IsArmor="
                << std::boolalpha << ITEM_PTR->IsArmor() << ", ArmorInfo.type="
                << armor_type::ToString(ITEM_PTR->Armor_Info().type, false) << ", MiscType()="
                << ITEM_PTR->MiscType() << ")";

            throw std::runtime_error(ss.str());
        }
    }


    const std::string ItemImageManager::GetImageFilename(
        const game::item::weapon::WeaponInfo & WEAPON_INFO,
        const bool                             IS_JEWELED) const
    {
        using namespace game::item;

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
                return "staff-special-2" + FILE_EXT_STR_;
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
            << weapon_type::ToString(WEAPON_INFO.type, false) << ") failed to resolve a filename.";
        throw std::runtime_error(ss.str());
    }


    const std::string ItemImageManager::GetImageFilename(
        const game::item::armor::ArmorInfo & ARMOR_INFO) const
    {
        using namespace game::item;

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
                return "Gauntlets-" + armor::base_type::ToString(ARMOR_INFO.base) + FILE_EXT_STR_;
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
            << armor_type::ToString(ARMOR_INFO.type, false) << ") failed to resolve a filename.";

        throw std::runtime_error(ss.str());
    }


    const std::vector<std::string> ItemImageManager::GetImageFilenames(
        const game::item::misc_type::Enum ITEM_ENUM,
        const bool                        IS_JEWELED) const
    {
        std::vector<std::string> filenames;

        switch (ITEM_ENUM)
        {
            case game::item::misc_type::Charm:
            case game::item::misc_type::Amulet:
            case game::item::misc_type::Pendant:
            case game::item::misc_type::Medallion:
            {
                for (auto i(1); i <= 23; ++i)
                {
                    std::ostringstream ss;
                    ss << "amulet-" << i << FILE_EXT_STR_;
                    filenames.push_back(ss.str());
                }
                return filenames;
            }
            case game::item::misc_type::Necklas:
            {
                filenames.push_back("necklas-1" + FILE_EXT_STR_);
                return filenames;
            }
            case game::item::misc_type::Bag:
            {
                for (auto i(1); i <= 8; ++i)
                {
                    std::ostringstream ss;
                    ss << "bag-" << i << FILE_EXT_STR_;
                    filenames.push_back(ss.str());
                }
                return filenames;
            }
            case game::item::misc_type::Tome:
            case game::item::misc_type::Book:
            {
                for (auto i(1); i <= 5; ++i)
                {
                    std::ostringstream ss;
                    ss << "book-" << i << FILE_EXT_STR_;
                    filenames.push_back(ss.str());
                }
                return filenames;
            }
            case game::item::misc_type::Cape:
            {
                filenames.push_back("cape" + FILE_EXT_STR_);
                return filenames;
            }
            case game::item::misc_type::Cloak:
            {
                filenames.push_back("cloak" + FILE_EXT_STR_);
                return filenames;
            }
            case game::item::misc_type::Robe:
            {
                filenames.push_back("robe" + FILE_EXT_STR_);
                return filenames;
            }
            case game::item::misc_type::Crown:
            {
                for (auto i(1); i <= 12; ++i)
                {
                    std::ostringstream ss;
                    ss << "crown-" << i << FILE_EXT_STR_;
                    filenames.push_back(ss.str());
                }
                return filenames;
            }
            case game::item::misc_type::Doll:
            {
                for (auto i(1); i <= 16; ++i)
                {
                    std::ostringstream ss;
                    ss << "doll-" << i << FILE_EXT_STR_;
                    filenames.push_back(ss.str());
                }
                return filenames;
            }
            case game::item::misc_type::Drink:
            case game::item::misc_type::Potion:
            {
                for (auto i(1); i <= 30; ++i)
                {
                    std::ostringstream ss;
                    ss << "potion-" << i << FILE_EXT_STR_;
                    filenames.push_back(ss.str());
                }
                return filenames;
            }
            case game::item::misc_type::Goblet:
            {
                for (auto i(1); i <= 8; ++i)
                {
                    std::ostringstream ss;
                    ss << "goblet-" << i << FILE_EXT_STR_;
                    filenames.push_back(ss.str());
                }
                return filenames;
            }
            case game::item::misc_type::Herbs:
            {
                for (auto i(1); i <= 34; ++i)
                {
                    std::ostringstream ss;
                    ss << "herb-" << i << FILE_EXT_STR_;
                    filenames.push_back(ss.str());
                }
                return filenames;
            }
            case game::item::misc_type::Hourglass:
            {
                for (auto i(1); i <= 3; ++i)
                {
                    std::ostringstream ss;
                    ss << "hourglass-" << i << FILE_EXT_STR_;
                    filenames.push_back(ss.str());
                }
                return filenames;
            }
            case game::item::misc_type::Key:
            {
                for (auto i(1); i <= 11; ++i)
                {
                    std::ostringstream ss;
                    ss << "key-" << i << FILE_EXT_STR_;
                    filenames.push_back(ss.str());
                }
                return filenames;
            }
            case game::item::misc_type::Lantern:
            {
                for (auto i(1); i <= 18; ++i)
                {
                    std::ostringstream ss;
                    ss << "lantern-" << i << FILE_EXT_STR_;
                    filenames.push_back(ss.str());
                }
                return filenames;
            }
            case game::item::misc_type::Torch:
            {
                for (auto i(1); i <= 5; ++i)
                {
                    std::ostringstream ss;
                    ss << "torch-" << i << FILE_EXT_STR_;
                    filenames.push_back(ss.str());
                }
                return filenames;
            }
            case game::item::misc_type::Lockbox:
            {
                for (auto i(1); i <= 8; ++i)
                {
                    std::ostringstream ss;
                    ss << "lockbox-" << i << FILE_EXT_STR_;
                    filenames.push_back(ss.str());
                }
                return filenames;
            }
            case game::item::misc_type::Chest:
            {
                for (auto i(1); i <= 14; ++i)
                {
                    std::ostringstream ss;
                    ss << "chest-" << i << FILE_EXT_STR_;
                    filenames.push_back(ss.str());
                }
                return filenames;
            }
            case game::item::misc_type::LockPicks:
            {
                filenames.push_back("lockpicks" + FILE_EXT_STR_);
                return filenames;
            }
            case game::item::misc_type::Mask:
            {
                for (auto i(1); i <= 6; ++i)
                {
                    std::ostringstream ss;
                    ss << "mask-" << i << FILE_EXT_STR_;
                    filenames.push_back(ss.str());
                }
                return filenames;
            }
            case game::item::misc_type::Mirror:
            {
                for (auto i(1); i <= 10; ++i)
                {
                    std::ostringstream ss;
                    ss << "mirror-" << i << FILE_EXT_STR_;
                    filenames.push_back(ss.str());
                }
                return filenames;
            }
            case game::item::misc_type::Lute:
            {
                filenames.push_back("lute" + FILE_EXT_STR_);
                return filenames;
            }
            case game::item::misc_type::Panflute:
            {
                filenames.push_back("panflute" + FILE_EXT_STR_);
                return filenames;
            }
            case game::item::misc_type::Flute:
            {
                filenames.push_back("flute" + FILE_EXT_STR_);
                return filenames;
            }
            case game::item::misc_type::Sitar:
            {
                filenames.push_back("sitar" + FILE_EXT_STR_);
                return filenames;
            }
            case game::item::misc_type::Scroll:
            {
                for (auto i(1); i <= 13; ++i)
                {
                    std::ostringstream ss;
                    ss << "scroll-" << i << FILE_EXT_STR_;
                    filenames.push_back(ss.str());
                }
                return filenames;
            }
            case game::item::misc_type::Orb:
            {
                for (auto i(1); i <= 13; ++i)
                {
                    std::ostringstream ss;
                    ss << "orb-" << i << FILE_EXT_STR_;
                    filenames.push_back(ss.str());
                }
                return filenames;
            }
            case game::item::misc_type::Ring:
            {   
                if (IS_JEWELED)
                {
                    for (auto i(1); i <= 14; ++i)
                    {
                        std::ostringstream ss;
                        ss << "ring-jeweled-" << i << FILE_EXT_STR_;
                        filenames.push_back(ss.str());
                    }
                }
                else
                {
                    for (auto i(1); i <= 2; ++i)
                    {
                        std::ostringstream ss;
                        ss << "ring-plain-" << i << FILE_EXT_STR_;
                        filenames.push_back(ss.str());
                    }
                }
                return filenames;
            }
            case game::item::misc_type::Skull:
            {
                for (auto i(1); i <= 11; ++i)
                {
                    std::ostringstream ss;
                    ss << "skull-" << i << FILE_EXT_STR_;
                    filenames.push_back(ss.str());
                }
                return filenames;
            }
            case game::item::misc_type::Shard:
            {
                for (auto i(1); i <= 7; ++i)
                {
                    std::ostringstream ss;
                    ss << "shard-" << i << FILE_EXT_STR_;
                    filenames.push_back(ss.str());
                }
                return filenames;
            }
            case game::item::misc_type::Salve:
            {
                for (auto i(1); i <= 9; ++i)
                {
                    std::ostringstream ss;
                    ss << "mortar-" << i << FILE_EXT_STR_;
                    filenames.push_back(ss.str());
                }
                return filenames;
            }
            case game::item::misc_type::Wand:
            {
                for (auto i(1); i <= 9; ++i)
                {
                    std::ostringstream ss;
                    ss << "wand-" << i << FILE_EXT_STR_;
                    filenames.push_back(ss.str());
                }
                return filenames;
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
