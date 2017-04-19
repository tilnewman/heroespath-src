//
// item-image-manager.cpp
//
#include "item-image-manager.hpp"

#include "sfml-util/loaders.hpp"
#include "sfml-util/random.hpp"
#include "sfml-util/boost-string-includes.hpp"

#include "heroespath/log-macros.hpp"
#include "heroespath/assertlogandthrow.hpp"
#include "heroespath/loop-manager.hpp"

#include <boost/filesystem.hpp>

#include <exception>
#include <sstream>


namespace sfml_util
{
namespace gui
{

    std::string            ItemImageManager::imagesDirectoryPath_;
    ItemImageManagerSPtr_t ItemImageManager::instanceSPtr_;
    const std::string      ItemImageManager::EXT_(".gif");


    ItemImageManager::ItemImageManager()
    {}


    ItemImageManager::~ItemImageManager()
    {}


    ItemImageManagerSPtr_t ItemImageManager::Instance()
    {
        if (nullptr == instanceSPtr_.get())
            instanceSPtr_.reset( new ItemImageManager );

        return instanceSPtr_;
    }


    bool ItemImageManager::Test()
    {
        static auto hasInitialPrompt{ false };
        if (false == hasInitialPrompt)
        {
            hasInitialPrompt = true;
            heroespath::LoopManager::Instance()->TestingStrAppend("sfml_util::gui::ItemImageManager::Test()  Starting tests...");
        }

        ItemImageManagerSPtr_t iimSPtr{ ItemImageManager::Instance() };

        const std::string TEST_PRE_STR{ "ItemImageManager Test #" };

        //test weapon items
        static auto axeIndex{ 0 };
        if (axeIndex < static_cast<int>(heroespath::item::weapon::axe_type::Count))
        {
            auto const ENUM{ static_cast<heroespath::item::weapon::axe_type::Enum>(axeIndex) };
            auto const STR { boost::algorithm::to_lower_copy(heroespath::item::weapon::axe_type::ToString(ENUM)) };
            heroespath::item::weapon::WeaponInfo wi(heroespath::item::weapon_type::Axe, STR);
            wi.axe = ENUM;
            auto const TEXTURE_SPTR{ iimSPtr->Load(iimSPtr->GetImageFilename(wi, false, false)) };
            M_ASSERT_OR_LOGANDTHROW_SS((TEXTURE_SPTR.get() != nullptr), "sfml_util::gui::ItemImageManager::Test() \"" << STR << "\"");
            heroespath::LoopManager::Instance()->TestingImageSet(TEXTURE_SPTR);
            heroespath::LoopManager::Instance()->TestingStrIncrement(TEST_PRE_STR);
            ++axeIndex;
            return false;
        }

        static auto hasTestedBite{ false };
        if (false == hasTestedBite)
        {
            heroespath::item::weapon::WeaponInfo wi(heroespath::item::weapon_type::Bite, "bite");
            wi.is_bite = true;
            auto const TEXTURE_SPTR{ iimSPtr->Load(iimSPtr->GetImageFilename(wi, false, false)) };
            M_ASSERT_OR_LOGANDTHROW_SS((TEXTURE_SPTR.get() != nullptr), "sfml_util::gui::ItemImageManager::Test() \"bite\"");
            heroespath::LoopManager::Instance()->TestingImageSet(TEXTURE_SPTR);
            heroespath::LoopManager::Instance()->TestingStrIncrement(TEST_PRE_STR);
            hasTestedBite = true;
            return false;
        }

        static auto bladedStaffIndex{ 0 };
        if (bladedStaffIndex < static_cast<int>(heroespath::item::weapon::bladedstaff_type::Count))
        {
            auto const ENUM{ static_cast<heroespath::item::weapon::bladedstaff_type::Enum>(bladedStaffIndex) };
            auto const STR{ boost::algorithm::to_lower_copy(heroespath::item::weapon::bladedstaff_type::ToString(ENUM)) };
            heroespath::item::weapon::WeaponInfo wi(heroespath::item::weapon_type::BladedStaff, STR);
            wi.bladedstaff = ENUM;
            auto const TEXTURE_SPTR{ iimSPtr->Load(iimSPtr->GetImageFilename(wi, false, false)) };
            M_ASSERT_OR_LOGANDTHROW_SS((TEXTURE_SPTR.get() != nullptr), "sfml_util::gui::ItemImageManager::Test() \"" << STR << "\"");
            heroespath::LoopManager::Instance()->TestingImageSet(TEXTURE_SPTR);
            heroespath::LoopManager::Instance()->TestingStrIncrement(TEST_PRE_STR);
            ++bladedStaffIndex;
            return false;
        }

        static auto hasTestedBreath{ false };
        if (false == hasTestedBreath)
        {
            heroespath::item::weapon::WeaponInfo wi(heroespath::item::weapon_type::Breath, "breath");
            wi.is_breath = true;
            auto const TEXTURE_SPTR{ iimSPtr->Load(iimSPtr->GetImageFilename(wi, false, false)) };
            M_ASSERT_OR_LOGANDTHROW_SS((TEXTURE_SPTR.get() != nullptr), "sfml_util::gui::ItemImageManager::Test() \"breath\"");
            heroespath::LoopManager::Instance()->TestingImageSet(TEXTURE_SPTR);
            heroespath::LoopManager::Instance()->TestingStrIncrement(TEST_PRE_STR);
            hasTestedBreath = true;
            return false;
        }

        static auto hasTestedClaws{ false };
        if (false == hasTestedClaws)
        {
            heroespath::item::weapon::WeaponInfo wi(heroespath::item::weapon_type::Claws, "claws");
            wi.is_claws = true;
            auto const TEXTURE_SPTR{ iimSPtr->Load(iimSPtr->GetImageFilename(wi, false, false)) };
            M_ASSERT_OR_LOGANDTHROW_SS((TEXTURE_SPTR.get() != nullptr), "sfml_util::gui::ItemImageManager::Test() \"claws\"");
            heroespath::LoopManager::Instance()->TestingImageSet(TEXTURE_SPTR);
            heroespath::LoopManager::Instance()->TestingStrIncrement(TEST_PRE_STR);
            hasTestedClaws = true;
            return false;
        }

        static auto clubIndex{ 0 };
        if (clubIndex < static_cast<int>(heroespath::item::weapon::club_type::Count))
        {
            auto const ENUM{ static_cast<heroespath::item::weapon::club_type::Enum>(clubIndex) };
            auto const STR{ boost::algorithm::to_lower_copy(heroespath::item::weapon::club_type::ToString(ENUM)) };
            heroespath::item::weapon::WeaponInfo wi(heroespath::item::weapon_type::Club, STR);
            wi.club = ENUM;
            auto const TEXTURE_SPTR{ iimSPtr->Load(iimSPtr->GetImageFilename(wi, false, false)) };
            M_ASSERT_OR_LOGANDTHROW_SS((TEXTURE_SPTR.get() != nullptr), "sfml_util::gui::ItemImageManager::Test() \"" << STR << "\"");
            heroespath::LoopManager::Instance()->TestingImageSet(TEXTURE_SPTR);
            heroespath::LoopManager::Instance()->TestingStrIncrement(TEST_PRE_STR);
            ++clubIndex;
            return false;
        }

        static auto hasTestedFists{ false };
        if (false == hasTestedFists)
        {
            heroespath::item::weapon::WeaponInfo wi(heroespath::item::weapon_type::Fists, "fists");
            wi.is_fists = true;
            auto const TEXTURE_SPTR{ iimSPtr->Load(iimSPtr->GetImageFilename(wi, false, false)) };
            M_ASSERT_OR_LOGANDTHROW_SS((TEXTURE_SPTR.get() != nullptr), "sfml_util::gui::ItemImageManager::Test() \"fists\"");
            heroespath::LoopManager::Instance()->TestingImageSet(TEXTURE_SPTR);
            heroespath::LoopManager::Instance()->TestingStrIncrement(TEST_PRE_STR);
            ++hasTestedFists;
            return false;
        }

        static auto hasTestedKnife{ false };
        if (false == hasTestedKnife)
        {
            heroespath::item::weapon::WeaponInfo wi(heroespath::item::weapon_type::Knife, "knife");
            wi.is_knife = true;
            auto const TEXTURE_SPTR{ iimSPtr->Load(iimSPtr->GetImageFilename(wi, false, false)) };
            M_ASSERT_OR_LOGANDTHROW_SS((TEXTURE_SPTR.get() != nullptr), "sfml_util::gui::ItemImageManager::Test() \"knife\"");
            heroespath::LoopManager::Instance()->TestingImageSet(TEXTURE_SPTR);
            heroespath::LoopManager::Instance()->TestingStrIncrement(TEST_PRE_STR);
            hasTestedKnife = true;
            return false;
        }

        static auto hasTestedDagger{ false };
        if (false == hasTestedDagger)
        {
            heroespath::item::weapon::WeaponInfo wi(heroespath::item::weapon_type::Knife, "dagger");
            wi.is_dagger = true;
            auto const TEXTURE_SPTR{ iimSPtr->Load(iimSPtr->GetImageFilename(wi, false, false)) };
            M_ASSERT_OR_LOGANDTHROW_SS((TEXTURE_SPTR.get() != nullptr), "sfml_util::gui::ItemImageManager::Test() \"dagger\"");
            heroespath::LoopManager::Instance()->TestingImageSet(TEXTURE_SPTR);
            heroespath::LoopManager::Instance()->TestingStrIncrement(TEST_PRE_STR);
            hasTestedDagger = true;
            return false;
        }

        static auto projIndex{ 0 };
        if (projIndex < static_cast<int>(heroespath::item::weapon::projectile_type::Count))
        {
            auto const ENUM{ static_cast<heroespath::item::weapon::projectile_type::Enum>(projIndex) };
            auto const STR{ boost::algorithm::to_lower_copy(heroespath::item::weapon::projectile_type::ToString(ENUM)) };
            heroespath::item::weapon::WeaponInfo wi(heroespath::item::weapon_type::Projectile, STR);
            wi.projectile = ENUM;
            auto const TEXTURE_SPTR{ iimSPtr->Load(iimSPtr->GetImageFilename(wi, false, false)) };
            M_ASSERT_OR_LOGANDTHROW_SS((TEXTURE_SPTR.get() != nullptr), "sfml_util::gui::ItemImageManager::Test() \"" << STR << "\"");
            heroespath::LoopManager::Instance()->TestingImageSet(TEXTURE_SPTR);
            heroespath::LoopManager::Instance()->TestingStrIncrement(TEST_PRE_STR);
            ++projIndex;
            return false;
        }

        static auto hasTestedStaff{ false };
        if (false == hasTestedStaff)
        {
            heroespath::item::weapon::WeaponInfo wi(heroespath::item::weapon_type::Staff, "staff");
            wi.is_staff = true;
            auto const TEXTURE_SPTR{ iimSPtr->Load(iimSPtr->GetImageFilename(wi, false, false)) };
            M_ASSERT_OR_LOGANDTHROW_SS((TEXTURE_SPTR.get() != nullptr), "sfml_util::gui::ItemImageManager::Test() \"staff\"");
            heroespath::LoopManager::Instance()->TestingImageSet(TEXTURE_SPTR);
            heroespath::LoopManager::Instance()->TestingStrIncrement(TEST_PRE_STR);
            hasTestedStaff = true;
            return false;
        }

        static auto hasTestedQStaff{ false };
        if (false == hasTestedQStaff)
        {
            heroespath::item::weapon::WeaponInfo wi(heroespath::item::weapon_type::Staff, "quarterstaff");
            wi.is_quarterstaff = true;
            auto const TEXTURE_SPTR{ iimSPtr->Load(iimSPtr->GetImageFilename(wi, false, false)) };
            M_ASSERT_OR_LOGANDTHROW_SS((TEXTURE_SPTR.get() != nullptr), "sfml_util::gui::ItemImageManager::Test() \"quarterstaff\"");
            heroespath::LoopManager::Instance()->TestingImageSet(TEXTURE_SPTR);
            heroespath::LoopManager::Instance()->TestingStrIncrement(TEST_PRE_STR);
            hasTestedQStaff = true;
            return false;
        }

        static auto swordIndex{ 0 };
        if (swordIndex < static_cast<int>(heroespath::item::weapon::sword_type::Count))
        {
            auto const ENUM{ static_cast<heroespath::item::weapon::sword_type::Enum>(swordIndex) };
            auto const STR{ boost::algorithm::to_lower_copy(heroespath::item::weapon::sword_type::ToString(ENUM)) };
            heroespath::item::weapon::WeaponInfo wi(heroespath::item::weapon_type::Sword, STR);
            wi.sword = ENUM;
            auto const TEXTURE_SPTR{ iimSPtr->Load(iimSPtr->GetImageFilename(wi, false, false)) };
            M_ASSERT_OR_LOGANDTHROW_SS((TEXTURE_SPTR.get() != nullptr), "sfml_util::gui::ItemImageManager::Test() \"" << STR << "\"");
            heroespath::LoopManager::Instance()->TestingImageSet(TEXTURE_SPTR);
            heroespath::LoopManager::Instance()->TestingStrIncrement(TEST_PRE_STR);
            ++swordIndex;
            return false;
        }

        static auto hasTestedTendrils{ false };
        if (false == hasTestedTendrils)
        {
            heroespath::item::weapon::WeaponInfo wi(heroespath::item::weapon_type::Tendrils, "tendrils");
            wi.is_tendrils = true;
            auto const TEXTURE_SPTR{ iimSPtr->Load(iimSPtr->GetImageFilename(wi, false, false)) };
            M_ASSERT_OR_LOGANDTHROW_SS((TEXTURE_SPTR.get() != nullptr), "sfml_util::gui::ItemImageManager::Test() \"tendrils\"");
            heroespath::LoopManager::Instance()->TestingImageSet(TEXTURE_SPTR);
            heroespath::LoopManager::Instance()->TestingStrIncrement(TEST_PRE_STR);
            hasTestedTendrils = true;
            return false;
        }

        //test armor items
        static auto aventailIndex{ 0 };
        if (aventailIndex < static_cast<int>(heroespath::item::armor::base_type::Count))
        {
            heroespath::item::armor::ArmorInfo ai(heroespath::item::armor_type::Aventail);
            ai.base = static_cast<heroespath::item::armor::base_type::Enum>(aventailIndex);
            ai.is_aventail = true;
            auto const TEXTURE_SPTR{ iimSPtr->Load(iimSPtr->GetImageFilename(ai, false, false)) };
            M_ASSERT_OR_LOGANDTHROW_SS((TEXTURE_SPTR.get() != nullptr), "sfml_util::gui::ItemImageManager::Test() aventail \"" << heroespath::item::armor::base_type::ToString(ai.base) << "\"");
            heroespath::LoopManager::Instance()->TestingImageSet(TEXTURE_SPTR);
            heroespath::LoopManager::Instance()->TestingStrIncrement(TEST_PRE_STR);
            ++aventailIndex;
            return false;
        }

        static auto bootsIndex{ 0 };
        if (bootsIndex < static_cast<int>(heroespath::item::armor::base_type::Count))
        {
            heroespath::item::armor::ArmorInfo ai(heroespath::item::armor_type::Boots);
            ai.base = static_cast<heroespath::item::armor::base_type::Enum>(bootsIndex);
            ai.is_boots = true;
            auto const TEXTURE_SPTR{ iimSPtr->Load(iimSPtr->GetImageFilename(ai, false, false)) };
            M_ASSERT_OR_LOGANDTHROW_SS((TEXTURE_SPTR.get() != nullptr), "sfml_util::gui::ItemImageManager::Test() boots \"" << heroespath::item::armor::base_type::ToString(ai.base) << "\"");
            heroespath::LoopManager::Instance()->TestingImageSet(TEXTURE_SPTR);
            heroespath::LoopManager::Instance()->TestingStrIncrement(TEST_PRE_STR);
            ++bootsIndex;
            return false;
        }

        static auto bracerIndex{ 0 };
        if (bracerIndex < static_cast<int>(heroespath::item::armor::base_type::Count))
        {
            heroespath::item::armor::ArmorInfo ai(heroespath::item::armor_type::Bracer);
            ai.base = static_cast<heroespath::item::armor::base_type::Enum>(bracerIndex);
            ai.is_bracer = true;
            auto const TEXTURE_SPTR{ iimSPtr->Load(iimSPtr->GetImageFilename(ai, false, false)) };
            M_ASSERT_OR_LOGANDTHROW_SS((TEXTURE_SPTR.get() != nullptr), "sfml_util::gui::ItemImageManager::Test() bracer \"" << heroespath::item::armor::base_type::ToString(ai.base) << "\"");
            heroespath::LoopManager::Instance()->TestingImageSet(TEXTURE_SPTR);
            heroespath::LoopManager::Instance()->TestingStrIncrement(TEST_PRE_STR);
            ++bracerIndex;
            return false;
        }

        static auto coverIndex{ 0 };
        if (coverIndex < static_cast<int>(heroespath::item::armor::cover_type::Count))
        {
            heroespath::item::armor::ArmorInfo ai(heroespath::item::armor_type::Covering);
            ai.cover = static_cast<heroespath::item::armor::cover_type::Enum>(coverIndex);
            auto const TEXTURE_SPTR{ iimSPtr->Load(iimSPtr->GetImageFilename(ai, false, false)) };
            M_ASSERT_OR_LOGANDTHROW_SS((TEXTURE_SPTR.get() != nullptr), "sfml_util::gui::ItemImageManager::Test() cover \"" << heroespath::item::armor::cover_type::ToString(ai.cover) << "\"");
            heroespath::LoopManager::Instance()->TestingImageSet(TEXTURE_SPTR);
            heroespath::LoopManager::Instance()->TestingStrIncrement(TEST_PRE_STR);
            ++coverIndex;
            return false;
        }

        static auto helmIndex{ 0 };
        if (helmIndex < static_cast<int>(heroespath::item::armor::helm_type::Count))
        {
            heroespath::item::armor::ArmorInfo ai(heroespath::item::armor_type::Helm);
            ai.helm = static_cast<heroespath::item::armor::helm_type::Enum>(helmIndex);
            auto const TEXTURE_SPTR{ iimSPtr->Load(iimSPtr->GetImageFilename(ai, false, false)) };
            M_ASSERT_OR_LOGANDTHROW_SS((TEXTURE_SPTR.get() != nullptr), "sfml_util::gui::ItemImageManager::Test() helm \"" << heroespath::item::armor::helm_type::ToString(ai.helm) << "\"");
            heroespath::LoopManager::Instance()->TestingImageSet(TEXTURE_SPTR);
            heroespath::LoopManager::Instance()->TestingStrIncrement(TEST_PRE_STR);
            ++helmIndex;
            return false;
        }

        static auto pantIndex{ 0 };
        if (pantIndex < static_cast<int>(heroespath::item::armor::base_type::Count))
        {
            heroespath::item::armor::ArmorInfo ai(heroespath::item::armor_type::Pants);
            ai.base = static_cast<heroespath::item::armor::base_type::Enum>(pantIndex);
            ai.is_pants = true;
            auto const TEXTURE_SPTR{ iimSPtr->Load(iimSPtr->GetImageFilename(ai, false, false)) };
            M_ASSERT_OR_LOGANDTHROW_SS((TEXTURE_SPTR.get() != nullptr), "sfml_util::gui::ItemImageManager::Test() pants \"" << heroespath::item::armor::base_type::ToString(ai.base) << "\"");
            heroespath::LoopManager::Instance()->TestingImageSet(TEXTURE_SPTR);
            heroespath::LoopManager::Instance()->TestingStrIncrement(TEST_PRE_STR);
            ++pantIndex;
            return false;
        }

        static auto shieldIndex{ 0 };
        if (shieldIndex < static_cast<int>(heroespath::item::armor::shield_type::Count))
        {
            heroespath::item::armor::ArmorInfo ai(heroespath::item::armor_type::Sheild);
            ai.shield = static_cast<heroespath::item::armor::shield_type::Enum>(shieldIndex);
            auto const TEXTURE_SPTR{ iimSPtr->Load(iimSPtr->GetImageFilename(ai, false, false)) };
            M_ASSERT_OR_LOGANDTHROW_SS((TEXTURE_SPTR.get() != nullptr), "sfml_util::gui::ItemImageManager::Test() shield \"" << heroespath::item::armor::shield_type::ToString(ai.shield) << "\"");
            heroespath::LoopManager::Instance()->TestingImageSet(TEXTURE_SPTR);
            heroespath::LoopManager::Instance()->TestingStrIncrement(TEST_PRE_STR);
            ++shieldIndex;
            return false;
        }

        static auto shirtIndex{ 0 };
        if (shirtIndex < static_cast<int>(heroespath::item::armor::base_type::Count))
        {
            heroespath::item::armor::ArmorInfo ai(heroespath::item::armor_type::Shirt);
            ai.base = static_cast<heroespath::item::armor::base_type::Enum>(shirtIndex);
            ai.is_shirt = true;
            auto const TEXTURE_SPTR{ iimSPtr->Load(iimSPtr->GetImageFilename(ai, false, false)) };
            M_ASSERT_OR_LOGANDTHROW_SS((TEXTURE_SPTR.get() != nullptr), "sfml_util::gui::ItemImageManager::Test() shirt \"" << heroespath::item::armor::base_type::ToString(ai.base) << "\"");
            heroespath::LoopManager::Instance()->TestingImageSet(TEXTURE_SPTR);
            heroespath::LoopManager::Instance()->TestingStrIncrement(TEST_PRE_STR);
            ++shirtIndex;
            return false;
        }

        //test misc items
        auto const RAND_REPEAT_COUNT{ 100 };
        static auto miscIndex{ 1 };
        static auto randIndex{ 0 };
        if (miscIndex < static_cast<int>(heroespath::item::misc_type::Count))
        {
            auto const ENUM{ static_cast<heroespath::item::misc_type::Enum>(miscIndex) };
            auto const FILENAME_NONRAND{ iimSPtr->GetImageFilename(ENUM, false, false) };
            M_ASSERT_OR_LOGANDTHROW_SS((FILENAME_NONRAND.empty() == false), "sfml_util::gui::ItemImageManager::Test() (non-rand)  While testing misc items #" << miscIndex << " \"" << heroespath::item::misc_type::ToString(ENUM) << "\", GetImageFilename() returned an empty string.");
            auto const TEXTURE_BASE_SPTR{ iimSPtr->Load(FILENAME_NONRAND) };
            M_ASSERT_OR_LOGANDTHROW_SS((TEXTURE_BASE_SPTR.get() != nullptr), "sfml_util::gui::ItemImageManager::Test() (non-rand)  While testing misc items #" << miscIndex << " \"" << heroespath::item::misc_type::ToString(ENUM) << "\", GetImage() returned a nullptr texture.");

            if (randIndex < RAND_REPEAT_COUNT)
            {
                auto const FILENAME_RAND{ iimSPtr->GetImageFilename(ENUM, false, true) };
                M_ASSERT_OR_LOGANDTHROW_SS((FILENAME_RAND.empty() == false),     "sfml_util::gui::ItemImageManager::Test() (rand)  While testing misc items #" << randIndex << " \"" << heroespath::item::misc_type::ToString(ENUM) << "\", GetImageFilename() returned an empty string.");
                auto const TEXTURE_RAND_SPTR{ iimSPtr->Load(FILENAME_RAND) };
                M_ASSERT_OR_LOGANDTHROW_SS((TEXTURE_RAND_SPTR.get() != nullptr), "sfml_util::gui::ItemImageManager::Test() (rand)  While testing misc items #" << randIndex << " \"" << heroespath::item::misc_type::ToString(ENUM) << "\", GetImage() returned a nullptr texture.");
                heroespath::LoopManager::Instance()->TestingImageSet(TEXTURE_RAND_SPTR);

                auto const ENUM_STR{ heroespath::item::misc_type::ToString(ENUM) };

                std::ostringstream ss;
                ss << "ItemImageManager Rand Test of " << ENUM_STR << "#";
                heroespath::LoopManager::Instance()->TestingStrIncrement(ss.str());

                ++randIndex;
                return false;
            }

            randIndex = 0;
            ++miscIndex;
            return false;
        }

        heroespath::LoopManager::Instance()->TestingStrAppend("sfml_util::gui::ItemImageManager::Test()  ALL TESTS PASSED.");
        return true;
    }


    void ItemImageManager::SetItemImageDirectory(const std::string & PATH)
    {
        imagesDirectoryPath_ = PATH;
    }


    TextureSPtr_t ItemImageManager::Load(const std::string & IMAGE_FILE_NAME) const
    {
        namespace bfs = boost::filesystem;
        const bfs::path PATH_OBJ(bfs::system_complete(bfs::path(imagesDirectoryPath_) / bfs::path(IMAGE_FILE_NAME)));

        TextureSPtr_t textureSPtr;
        sfml_util::LoadImageOrTextureSPtr(textureSPtr, PATH_OBJ.string());
        return textureSPtr;
    }


    TextureSPtr_t ItemImageManager::Load(const heroespath::item::misc_type::Enum ITEM_ENUM, const bool IS_JEWELED, const bool WILL_RANDOMIZE) const
    {
        return Load( GetImageFilename(ITEM_ENUM, IS_JEWELED, WILL_RANDOMIZE) );
    }


    const std::string ItemImageManager::GetImageFilename(const heroespath::item::ItemSPtr_t & ITEM_SPTR, const bool WILL_RANDOMIZE) const
    {
        using namespace heroespath::item;

        if (ITEM_SPTR->IsWeapon() && (ITEM_SPTR->Weapon_Info().type != weapon_type::NotAWeapon))
        {
            return GetImageFilename(ITEM_SPTR->Weapon_Info(), ITEM_SPTR->IsJeweled(), WILL_RANDOMIZE);
        }
        else if (ITEM_SPTR->IsArmor() && (ITEM_SPTR->Armor_Info().type != armor_type::NotArmor))
        {
            return GetImageFilename(ITEM_SPTR->Armor_Info(), ITEM_SPTR->IsJeweled(), WILL_RANDOMIZE);
        }
        else if (ITEM_SPTR->MiscType() != misc_type::NotMisc)
        {
            return GetImageFilename(ITEM_SPTR->MiscType(), ITEM_SPTR->IsJeweled(), WILL_RANDOMIZE);
        }
        else
        {
            std::ostringstream ss;
            ss << "sfml_util::gui::ItemImageManager::GetImageFilename(item->Name()=" << ITEM_SPTR->Name() << ", item->Category=" << category::ToString(ITEM_SPTR->Category(), false) << ", desc=\"" <<  ITEM_SPTR->Desc() << "\") failed to be categorized.  (IsWeapon=" << std::boolalpha << ITEM_SPTR->IsWeapon() << ", WeaponInfo.type=" << weapon_type::ToString(ITEM_SPTR->Weapon_Info().type, false) << ", IsArmor=" << std::boolalpha << ITEM_SPTR->IsArmor() << ", ArmorInfo.type=" << armor_type::ToString(ITEM_SPTR->Armor_Info().type, false) << ", MiscType()=" << ITEM_SPTR->MiscType() << ")";
            throw std::runtime_error(ss.str());
        }
    }


    const std::string ItemImageManager::GetImageFilename(const heroespath::item::weapon::WeaponInfo & WEAPON_INFO, const bool IS_JEWELED, const bool) const
    {
        using namespace heroespath::item;

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


    const std::string ItemImageManager::GetImageFilename(const heroespath::item::armor::ArmorInfo & ARMOR_INFO, const bool, const bool) const
    {
        using namespace heroespath::item;

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


    const std::string ItemImageManager::GetImageFilename(const heroespath::item::misc_type::Enum ITEM_ENUM, const bool IS_JEWELED, const bool WILL_RANDOMIZE) const
    {
        switch (ITEM_ENUM)
        {
            case heroespath::item::misc_type::Charm:
            case heroespath::item::misc_type::Amulet:
            case heroespath::item::misc_type::Pendant:
            case heroespath::item::misc_type::Medallion:
            {
                if (WILL_RANDOMIZE)
                {
                    std::ostringstream ss;
                    ss << "amulet-" << sfml_util::rand::Int(1, 23) << EXT_;
                    return ss.str();
                }
                else
                    return "amulet-20" + EXT_;
            }
            case heroespath::item::misc_type::Necklas:
            {
                return "necklas-1" + EXT_;
            }
            case heroespath::item::misc_type::Bag:
            {
                if (WILL_RANDOMIZE)
                {
                    std::ostringstream ss;
                    ss << "bag-" << sfml_util::rand::Int(1, 8) << EXT_;
                    return ss.str();
                }
                else
                    return "bag-2" + EXT_;
            }
            case heroespath::item::misc_type::Tome:
            case heroespath::item::misc_type::Book:
            {
                if (WILL_RANDOMIZE)
                {
                    std::ostringstream ss;
                    ss << "book-" << sfml_util::rand::Int(1, 5) << EXT_;
                    return ss.str();
                }
                else
                    return "book-2" + EXT_;
            }
            case heroespath::item::misc_type::Cape:
            {
                return "cape" + EXT_;
            }
            case heroespath::item::misc_type::Cloak:
            {
                return "cloak" + EXT_;
            }
            case heroespath::item::misc_type::Robe:
            {
                return "robe" + EXT_;
            }
            case heroespath::item::misc_type::Crown:
            {
                if (WILL_RANDOMIZE)
                {
                    std::ostringstream ss;
                    ss << "crown-" << sfml_util::rand::Int(1, 12) << EXT_;
                    return ss.str();
                }
                else
                    return "crown-5" + EXT_;
            }
            case heroespath::item::misc_type::Doll:
            {
                if (WILL_RANDOMIZE)
                {
                    std::ostringstream ss;
                    ss << "doll-" << sfml_util::rand::Int(1, 16) << EXT_;
                    return ss.str();
                }
                else
                    return "doll-1" + EXT_;
            }
            case heroespath::item::misc_type::Drink:
            case heroespath::item::misc_type::Potion:
            {
                if (WILL_RANDOMIZE)
                {
                    std::ostringstream ss;
                    ss << "potion-" << sfml_util::rand::Int(1, 30) << EXT_;
                    return ss.str();
                }
                    return "potion-13" + EXT_;
            }
            case heroespath::item::misc_type::Goblet:
            {
                if (WILL_RANDOMIZE)
                {
                    std::ostringstream ss;
                    ss << "goblet-" << sfml_util::rand::Int(1, 8) << EXT_;
                    return ss.str();
                }
                else
                    return "goblet-5" + EXT_;
            }
            case heroespath::item::misc_type::Herbs:
            {
                if (WILL_RANDOMIZE)
                {
                    std::ostringstream ss;
                    ss << "herb-" << sfml_util::rand::Int(1, 34) << EXT_;
                    return ss.str();
                }
                    return "herb-26" + EXT_;
            }
            case heroespath::item::misc_type::Hourglass:
            {
                if (WILL_RANDOMIZE)
                {
                    std::ostringstream ss;
                    ss << "hourglass-" << sfml_util::rand::Int(1, 3) << EXT_;
                    return ss.str();
                }
                else
                    return "hourglass-1" + EXT_;
            }
            case heroespath::item::misc_type::Key:
            {
                if (WILL_RANDOMIZE)
                {
                    std::ostringstream ss;
                    ss << "key-" << sfml_util::rand::Int(1, 11) << EXT_;
                    return ss.str();
                }
                else
                    return "key-2" + EXT_;
            }
            case heroespath::item::misc_type::Lantern:
            {
                if (WILL_RANDOMIZE)
                {
                    std::ostringstream ss;
                    ss << "lantern-" << sfml_util::rand::Int(1, 18) << EXT_;
                    return ss.str();
                }
                else
                    return "lantern-10" + EXT_;
            }
            case heroespath::item::misc_type::Torch:
            {
                if (WILL_RANDOMIZE)
                {
                    std::ostringstream ss;
                    ss << "torch-" << sfml_util::rand::Int(1, 5) << EXT_;
                    return ss.str();
                }
                    return "torch-1" + EXT_;
            }
            case heroespath::item::misc_type::Lockbox:
            {
                if (WILL_RANDOMIZE)
                {
                    std::ostringstream ss;
                    ss << "lockbox-" << sfml_util::rand::Int(1, 8) << EXT_;
                    return ss.str();
                }
                else
                    return "lockbox-2" + EXT_;
            }
            case heroespath::item::misc_type::Chest:
            {
                if (WILL_RANDOMIZE)
                {
                    std::ostringstream ss;
                    ss << "chest-" << sfml_util::rand::Int(1, 14) << EXT_;
                    return ss.str();
                }
                else
                    return "chest-4" + EXT_;
            }
            case heroespath::item::misc_type::LockPicks:
            {
                return "lockpicks" + EXT_;
            }
            case heroespath::item::misc_type::Mask:
            {
                if (WILL_RANDOMIZE)
                {
                    std::ostringstream ss;
                    ss << "mask-" << sfml_util::rand::Int(1, 6) << EXT_;
                    return ss.str();
                }
                else
                    return "mask-3" + EXT_;
            }
            case heroespath::item::misc_type::Mirror:
            {
                if (WILL_RANDOMIZE)
                {
                    std::ostringstream ss;
                    ss << "mirror-" << sfml_util::rand::Int(1, 10) << EXT_;
                    return ss.str();
                }
                else
                    return "mirror-7" + EXT_;
            }
            case heroespath::item::misc_type::Lute:
            {
                return "lute" + EXT_;
            }
            case heroespath::item::misc_type::Panflute:
            {
                return "panflute" + EXT_;
            }
            case heroespath::item::misc_type::Flute:
            {
                return "flute" + EXT_;
            }
            case heroespath::item::misc_type::Sitar:
            {
                return "sitar" + EXT_;
            }
            case heroespath::item::misc_type::Scroll:
            {
                if (WILL_RANDOMIZE)
                {
                    std::ostringstream ss;
                    ss << "scroll-" << sfml_util::rand::Int(1, 13) << EXT_;
                    return ss.str();
                }
                else
                    return "scroll-6" + EXT_;
            }
            case heroespath::item::misc_type::Orb:
            {
                if (WILL_RANDOMIZE)
                {
                    std::ostringstream ss;
                    ss << "orb-" << sfml_util::rand::Int(1, 13) << EXT_;
                    return ss.str();
                }
                else
                    return "orb-4" + EXT_;
            }
            case heroespath::item::misc_type::Ring:
            {
                if (WILL_RANDOMIZE)
                {
                    std::ostringstream ss;
                    if (IS_JEWELED)
                        ss << "ring-jeweled-" << sfml_util::rand::Int(1, 14) << EXT_;
                    else
                        ss << "ring-plain-" << sfml_util::rand::Int(1, 2) << EXT_;
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
            case heroespath::item::misc_type::Skull:
            {
                if (WILL_RANDOMIZE)
                {
                    std::ostringstream ss;
                    ss << "skull-" << sfml_util::rand::Int(1, 11) << EXT_;
                    return ss.str();
                }
                else
                    return "skull-3" + EXT_;
            }
            case heroespath::item::misc_type::Shard:
            {
                if (WILL_RANDOMIZE)
                {
                    std::ostringstream ss;
                    ss << "shard-" << sfml_util::rand::Int(1, 7) << EXT_;
                    return ss.str();
                }
                else
                    return "shard-1" + EXT_;
            }
            case heroespath::item::misc_type::Salve:
            {
                if (WILL_RANDOMIZE)
                {
                    std::ostringstream ss;
                    ss << "mortar-" << sfml_util::rand::Int(1, 9) << EXT_;
                    return ss.str();
                }
                else
                    return "mortar-3" + EXT_;
            }
            case heroespath::item::misc_type::Wand:
            {
                if (WILL_RANDOMIZE)
                {
                    std::ostringstream ss;
                    ss << "wand-" << sfml_util::rand::Int(1, 9) << EXT_;
                    return ss.str();
                }
                else
                    return "wand-7" + EXT_;
            }
            case heroespath::item::misc_type::NotMisc:
            case heroespath::item::misc_type::Count:
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
