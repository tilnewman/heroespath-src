// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
///////////////////////////////////////////////////////////////////////////////
//
// Heroes' Path - Open-source, non-commercial, simple, game in the RPG style.
// Copyright (C) 2017 Ziesche Til Newman (tilnewman@gmail.com)
//
// This software is provided 'as-is', weaponInfothout any express or implied warranty.
// In no event weaponInfoll the authors be held liable for any damages arising from
// the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely, subject to the followeaponInfong restrictions:
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
// item-image-machine.cpp
//
#include "item-image-machine.hpp"

#include "creature/dragon-class-enum.hpp"
#include "creature/wolfen-class-enum.hpp"
#include "game/game-data-file.hpp"
#include "game/loop-manager.hpp"
#include "item/item.hpp"
#include "log/log-macros.hpp"
#include "misc/assertlogandthrow.hpp"
#include "misc/boost-string-includes.hpp"
#include "misc/enum-util.hpp"
#include "misc/random.hpp"
#include "non-player/ownership-profile.hpp"
#include "sfml-util/loaders.hpp"

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

        const std::string ItemImageMachine::FILE_EXT_STR_{ ".png" };
        const std::string ItemImageMachine::SEPARATOR_{ "-" };

        bool ItemImageMachine::Test()
        {
            using namespace item;

            static auto hasInitialPrompt{ false };
            if (false == hasInitialPrompt)
            {
                hasInitialPrompt = true;
                game::LoopManager::Instance()->TestingStrAppend(
                    "sfml_util::gui::ItemImageMachine::Test()  Starting tests...");
            }

            // TODO these enum tests (and many others) should be elsewhere in the testing system
            if (misc::enum_util::Test<category>::ToStringForFlagEnum(
                    misc::enum_util::AllowOneEmpty::Yes)
                == false)
            {
                return false;
            }

            if (misc::enum_util::Test<element_type>::ToStringForFlagEnum(
                    misc::enum_util::AllowOneEmpty::Yes)
                == false)
            {
                return false;
            }

            if (misc::enum_util::Test<weapon_type>::ToStringForFlagEnum(
                    misc::enum_util::AllowOneEmpty::No)
                == false)
            {
                return false;
            }

            if (misc::enum_util::Test<armor_type>::ToStringForCountingEnum(
                    misc::enum_util::AllowOneEmpty::No)
                == false)
            {
                return false;
            }

            if (misc::enum_util::Test<non_player::ownership::collector_type>::ToStringForFlagEnum(
                    misc::enum_util::AllowOneEmpty::No)
                == false)
            {
                return false;
            }

            const std::string TEST_PRE_STR{ "ItemImageMachine Test " };

            static auto const WEAPON_TYPE_WRAPPERS{ weapon::WeaponTypeWrapper::MakeCompleteSet() };

            static std::size_t weaponIndex{ 0 };
            if (weaponIndex < WEAPON_TYPE_WRAPPERS.size())
            {
                auto const & WEAPON_TYPE_WRAPPER{ WEAPON_TYPE_WRAPPERS.at(weaponIndex) };
                sf::Texture texture;
                Load(texture, Filename(WEAPON_TYPE_WRAPPER, false));

                game::LoopManager::Instance()->TestingImageSet(
                    texture, true, "item", WEAPON_TYPE_WRAPPER.ReadableName());

                game::LoopManager::Instance()->TestingStrIncrement(
                    TEST_PRE_STR + WEAPON_TYPE_WRAPPER.ReadableName());

                EnsureValidDimmensions(texture, WEAPON_TYPE_WRAPPER.ReadableName());
                ++weaponIndex;
                return false;
            }

            static auto const ARMOR_TYPE_WRAPPERS{ armor::ArmorTypeWrapper::MakeCompleteSet() };

            static std::size_t armorIndex{ 0 };
            if (armorIndex < ARMOR_TYPE_WRAPPERS.size())
            {
                auto const & ARMOR_TYPE_WRAPPER{ ARMOR_TYPE_WRAPPERS.at(armorIndex) };

                // skip testing skin images because we need creatures for that, and skin images will
                // get tested when we test creature creation
                if (ARMOR_TYPE_WRAPPER.IsSkin() == false)
                {
                    sf::Texture texture;
                    Load(texture, Filename(ARMOR_TYPE_WRAPPER));

                    game::LoopManager::Instance()->TestingImageSet(
                        texture, true, "item", ARMOR_TYPE_WRAPPER.ReadableName());

                    game::LoopManager::Instance()->TestingStrIncrement(
                        TEST_PRE_STR + ARMOR_TYPE_WRAPPER.ReadableName());

                    EnsureValidDimmensions(texture, ARMOR_TYPE_WRAPPER.ReadableName());
                }

                ++armorIndex;
                return false;
            }

            // test misc items
            static auto miscIndex{ 1 }; // start at 1 to avoid misc_type::NotMisc
            static auto isJeweled{ true };
            static auto isBone{ false };
            if (miscIndex < static_cast<int>(misc_type::Count))
            {
                auto const ENUM{ static_cast<misc_type::Enum>(miscIndex) };

                // skip misc items that are weapons or armor because they will require a TypeWrapper
                // object to construct the image filename, and will have already been tested by
                // ItemFactory
                if (misc_type::IsWeapon(ENUM) || misc_type::IsArmor(ENUM))
                {
                    ++miscIndex;
                    return false;
                }

                auto const ENUM_STR{ misc_type::ToString(ENUM) };
                auto const FILENAMES_VEC = Filenames(ENUM, isJeweled, isBone);

                M_ASSERT_OR_LOGANDTHROW_SS(
                    (FILENAMES_VEC.empty() == false),
                    "sfml_util::gui::ItemImageMachine::Test() While testing misc item #"
                        << miscIndex << " \"" << ENUM_STR << "\", is_jeweled=" << std::boolalpha
                        << isJeweled << ", Filenames() returned an empty vector.");

                static std::size_t fileIndex{ 0 };
                if (fileIndex < FILENAMES_VEC.size())
                {
                    auto const NEXT_FILENAME{ FILENAMES_VEC[fileIndex] };

                    M_ASSERT_OR_LOGANDTHROW_SS(
                        (NEXT_FILENAME.empty() == false),
                        "sfml_util::gui::ItemImageMachine::Test() (rand)  "
                            << "While testing misc item #" << miscIndex << " \"" << ENUM_STR
                            << "\", filename #" << fileIndex << ", is_jeweled=" << std::boolalpha
                            << isJeweled << ", found an empty filename string.");

                    std::ostringstream ss;
                    ss << "ItemImageMachine Testing \"" << ENUM_STR
                       << "\", file_index=" << fileIndex << " ("
                       << ((isJeweled) ? "jeweled" : "not-jeweled") << ")";

                    game::LoopManager::Instance()->TestingStrIncrement(ss.str());

                    sf::Texture texture;
                    Load(texture, NEXT_FILENAME);

                    game::LoopManager::Instance()->TestingImageSet(
                        texture, true, "items/misc", ENUM_STR, NEXT_FILENAME);

                    EnsureValidDimmensions(texture, ENUM_STR);

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

            game::LoopManager::Instance()->TestingStrAppend(
                "sfml_util::gui::ItemImageMachine::Test()  ALL TESTS PASSED.");

            return true;
        }

        void ItemImageMachine::Load(sf::Texture & texture, const item::ItemPtr_t ITEM_PTR) const
        {
            Load(texture, ITEM_PTR->ImageFilename());
        }

        const std::string ItemImageMachine::Filename(
            const item::ItemPtr_t ITEM_PTR, const bool WILL_RANDOMIZE) const
        {
            // it is important to check for weapon/armor status first, because some misc_types are
            // weapons/armor and their image filenames will not be correctly identified by the
            // misc_type logic

            if (ITEM_PTR->IsWeapon())
            {
                return Filename(ITEM_PTR->WeaponInfo(), ITEM_PTR->IsJeweled());
            }
            else if (ITEM_PTR->IsArmor())
            {
                if (ITEM_PTR->ArmorInfo().IsSkin())
                {
                    return GetSkinImageFilename(ITEM_PTR->MaterialPrimary());
                }
                else
                {
                    return Filename(ITEM_PTR->ArmorInfo());
                }
            }
            else if (ITEM_PTR->IsMisc())
            {
                return Filename(
                    ITEM_PTR->MiscType(),
                    ITEM_PTR->IsJeweled(),
                    (ITEM_PTR->MaterialPrimary() == item::material::Bone),
                    WILL_RANDOMIZE);
            }

            std::ostringstream ss;
            ss << "sfml_util::gui::ItemImageMachine::Filename(item={" << ITEM_PTR->ToString()
               << "}, will_randomize=" << std::boolalpha << WILL_RANDOMIZE
               << ") failed to find the image filename for that item because it was not weapon, "
                  "armor, or misc type.";

            throw std::runtime_error(ss.str());
        }

        bool ItemImageMachine::DoesFileExists(const item::ItemPtr_t ITEM_PTR) const
        {
            return DoesFileExists(ITEM_PTR->ImageFilename());
        }

        bool ItemImageMachine::DoesFileExists(const std::string & FILENAME) const
        {
            namespace bfs = boost::filesystem;
            auto const PATH{ bfs::path(MakeFullPathFromFilename(FILENAME)) };
            return (bfs::exists(PATH) && bfs::is_regular_file(PATH));
        }

        const std::vector<std::string> ItemImageMachine::Filenames(
            const item::misc_type::Enum MISC_TYPE, const bool IS_JEWELED, const bool IS_BONE)
        {
            using namespace item;

            switch (MISC_TYPE)
            {
                case misc_type::Amulet:
                case misc_type::Pendant:
                {
                    return MakeFilenames("amulet", 23);
                }
                case misc_type::Necklace:
                {
                    return { ("necklace" + FILE_EXT_STR_) };
                }
                case misc_type::Bag:
                {
                    return MakeFilenames("bag", 8);
                }
                case misc_type::Tome:
                case misc_type::Book:
                {
                    return MakeFilenames("book", 5);
                }
                case misc_type::Cape:
                {
                    return { ("cape" + FILE_EXT_STR_) };
                }
                case misc_type::Cloak:
                {
                    return { ("cloak" + FILE_EXT_STR_) };
                }
                case misc_type::Robe:
                {
                    return { ("robe" + FILE_EXT_STR_) };
                }
                case misc_type::Doll:
                {
                    return MakeFilenames("doll", 16);
                }
                case misc_type::Potion:
                {
                    return MakeFilenames("potion", 33);
                }
                case misc_type::Goblet:
                {
                    return MakeFilenames("goblet", 8);
                }
                case misc_type::Key:
                {
                    return MakeFilenames("key", 11);
                }
                case misc_type::LockPicks:
                {
                    return MakeFilenames("lockpicks", 7);
                }
                case misc_type::Mask:
                {
                    return MakeFilenames("mask", 6);
                }
                case misc_type::Mirror:
                {
                    return MakeFilenames("mirror", 10);
                }
                case misc_type::DrumLute:
                {
                    return MakeFilenames("drumlute", 21);
                }
                case misc_type::Scroll:
                {
                    return MakeFilenames("scroll", 13);
                }
                case misc_type::Orb:
                {
                    return MakeFilenames("orb", 9);
                }
                case misc_type::Ring:
                {
                    if (IS_JEWELED)
                    {
                        return MakeFilenames("ring-jeweled", 24);
                    }
                    else if (IS_BONE)
                    {
                        return { ("ring-bone" + FILE_EXT_STR_) };
                    }
                    else
                    {
                        return MakeFilenames("ring-plain", 2);
                    }
                }
                case misc_type::Shard:
                {
                    return MakeFilenames("shard", 7);
                }
                case misc_type::Wand:
                {
                    return MakeFilenames("wand", 11);
                }
                case misc_type::Scepter:
                {
                    return MakeFilenames("scepter", 26);
                }
                case misc_type::Doll_Blessed:
                {
                    // note that certain numbers are intentionally missing from this sequence
                    return { ("doll-1" + FILE_EXT_STR_), ("doll-2" + FILE_EXT_STR_),
                             ("doll-4" + FILE_EXT_STR_), ("doll-5" + FILE_EXT_STR_),
                             ("doll-6" + FILE_EXT_STR_), ("doll-8" + FILE_EXT_STR_),
                             ("doll-9" + FILE_EXT_STR_), ("doll-10" + FILE_EXT_STR_) };
                }
                case misc_type::Doll_Cursed:
                {
                    return MakeFilenames("doll", 16, 11);
                }
                case misc_type::Figurine_Blessed:
                {
                    return MakeFilenames("figurine", 15);
                }
                case misc_type::Figurine_Cursed:
                {
                    auto filenames{ MakeFilenames("figurine", 15) };

                    for (auto const & FILENAME : MakeFilenames("figurine-evil", 4))
                    {
                        filenames.emplace_back(FILENAME);
                    }

                    return filenames;
                }
                case misc_type::Staff:
                {
                    return MakeFilenames("staff", 21);
                }
                case misc_type::Summoning_Statue:
                {
                    return { ("figurine-1" + FILE_EXT_STR_) };
                }
                case misc_type::Puppet_Blessed:
                case misc_type::Puppet_Cursed:
                {
                    return { ("puppet" + FILE_EXT_STR_) };
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
                case misc_type::Angel_Braid:
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
                case misc_type::Golem_Finger:
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
                case misc_type::Scales:
                {
                    auto const TYPE_STR{ boost::replace_all_copy(
                        boost::to_lower_copy(misc_type::ToString(MISC_TYPE)), "_", SEPARATOR_) };

                    return { (TYPE_STR + FILE_EXT_STR_) };
                }
                case misc_type::NotMisc:
                case misc_type::Count:
                default:
                {
                    std::ostringstream ss;
                    ss << "sfml_util::gui::ItemImageMachine::Filenames(misc_type="
                       << ((MISC_TYPE == misc_type::Count) ? "Count"
                                                           : misc_type::ToString(MISC_TYPE))
                       << ", is_jeweled=" << std::boolalpha << IS_JEWELED << ", is_bone=" << IS_BONE
                       << ") but that misc_type is somehow invalid.";

                    throw std::range_error(ss.str());
                }
            }
        }

        const std::string ItemImageMachine::Filename(
            const item::misc_type::Enum MISC_TYPE,
            const bool IS_JEWELED,
            const bool IS_BONE,
            const bool WILL_RANDOMIZE)
        {
            auto const FILENAMES(Filenames(MISC_TYPE, IS_JEWELED, IS_BONE));

            M_ASSERT_OR_LOGANDTHROW_SS(
                (FILENAMES.empty() == false),
                "sfml_util::gui::ItemImageMachine::Filename(misc_type="
                    << ((MISC_TYPE == item::misc_type::Count)
                            ? "Count"
                            : item::misc_type::ToString(MISC_TYPE))
                    << ", is_jeweled=" << std::boolalpha << IS_JEWELED << ", is_bone=" << IS_BONE
                    << ", will_rand=" << WILL_RANDOMIZE
                    << ") unable to get any filenames for those settings.");

            if (WILL_RANDOMIZE)
            {
                return misc::Vector::SelectRandom(FILENAMES);
            }
            else
            {
                return FILENAMES[0];
            }
        }

        const std::string
            ItemImageMachine::GetSkinImageFilename(const item::material::Enum PRIMARY_MATERIAL)
        {
            using namespace item;

            auto materialToUseForName{ PRIMARY_MATERIAL };
            if (PRIMARY_MATERIAL == material::Fur)
            {
                materialToUseForName = material::Hide;
            }

            return boost::algorithm::to_lower_copy(
                material::ToString(materialToUseForName) + FILE_EXT_STR_);
        }

        const std::string ItemImageMachine::Filename(
            const item::weapon::WeaponTypeWrapper & WEAPON_TYPE_WRAPPER, const bool IS_JEWELED)
        {
            if (WEAPON_TYPE_WRAPPER.IsStaff())
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
            else
            {
                return WEAPON_TYPE_WRAPPER.ImageFilename(SEPARATOR_, FILE_EXT_STR_);
            }
        }

        const std::string
            ItemImageMachine::Filename(const item::armor::ArmorTypeWrapper & ARMOR_TYPE_WRAPPER)
        {
            if (ARMOR_TYPE_WRAPPER.IsPants() || ARMOR_TYPE_WRAPPER.IsBracers()
                || ARMOR_TYPE_WRAPPER.IsAventail())
            {
                return ARMOR_TYPE_WRAPPER.SpecificName() + FILE_EXT_STR_;
            }
            else
            {
                return ARMOR_TYPE_WRAPPER.ImageFilename(SEPARATOR_, FILE_EXT_STR_);
            }
        }

        void ItemImageMachine::Load(sf::Texture & texture, const std::string & FILENAME)
        {
            sfml_util::LoadTexture(texture, MakeFullPathFromFilename(FILENAME));
        }

        const std::string ItemImageMachine::MakeFullPathFromFilename(const std::string & FILENAME)
        {
            namespace bfs = boost::filesystem;

            auto const IMAGES_DIR{ game::GameDataFile::Instance()->GetMediaPath(
                "media-images-items-dir") };

            const bfs::path PATH{
                bfs::system_complete(bfs::path(IMAGES_DIR) / bfs::path(FILENAME)).normalize()
            };

            return PATH.string();
        }

        void ItemImageMachine::EnsureValidDimmensions(
            const sf::Texture & TEXTURE, const std::string & ERROR_MSG)
        {
            auto const SIZE{ TEXTURE.getSize() };

            auto const DIMMENSION{ static_cast<unsigned>(MaxDimmension()) };

            M_ASSERT_OR_LOGANDTHROW_SS(
                ((SIZE.x == DIMMENSION) || (SIZE.y == DIMMENSION)),
                "sfml_util::gui::ItemImageMachine::EnsureValidDimmensions() was given an image of "
                "size="
                    << SIZE.x << "x" << SIZE.y
                    << " but neither of those dimmensions was the required " << DIMMENSION
                    << ".  This error occured during testing \"" << ERROR_MSG << "\".");
        }

        const std::vector<std::string> ItemImageMachine::MakeFilenames(
            const std::string & PREFIX, const int LAST_NUMBER, const int FIRST_NUMBER)
        {
            std::vector<std::string> filenames;
            filenames.reserve(static_cast<std::size_t>((LAST_NUMBER - FIRST_NUMBER) + 1));

            for (int i(FIRST_NUMBER); i <= LAST_NUMBER; ++i)
            {
                std::ostringstream ss;
                ss << PREFIX << "-" << i << FILE_EXT_STR_;
                filenames.emplace_back(ss.str());
            }

            return filenames;
        }

    } // namespace gui
} // namespace sfml_util
} // namespace heroespath
