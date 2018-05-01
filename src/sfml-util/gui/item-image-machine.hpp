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
#ifndef HEROESPATH_SFMLUTIL_GUI_ITEM_IMAGE_MACHINE_HPP_INCLUDED
#define HEROESPATH_SFMLUTIL_GUI_ITEM_IMAGE_MACHINE_HPP_INCLUDED
//
// item-image-machine.hpp
//
#include "sfml-util/sfml-graphics.hpp"

#include "creature/race-enum.hpp"
#include "creature/role-enum.hpp"
#include "item/armor-type-wrapper.hpp"
#include "item/item-type-enum.hpp"
#include "item/weapon-type-wrapper.hpp"
#include "misc/not-null.hpp"
#include "sfml-util/gui/image-dimmensions.hpp"

#include <memory>
#include <string>
#include <vector>

namespace heroespath
{
namespace item
{
    class Item;
    using ItemPtr_t = misc::NotNull<Item *>;
} // namespace item

namespace sfml_util
{
    namespace gui
    {

        // Responsible for loading item images.
        class ItemImageMachine
        {
        public:
            ItemImageMachine(const ItemImageMachine &) = delete;
            ItemImageMachine(ItemImageMachine &&) = delete;
            ItemImageMachine & operator=(const ItemImageMachine &) = delete;
            ItemImageMachine & operator=(ItemImageMachine &&) = delete;

            ItemImageMachine() = default;

            static bool Test();

            static float MaxDimmension() { return ImageDimmensions::ResourceStandardMax(); }

            void Load(sf::Texture & texture, const item::ItemPtr_t ITEM_PTR) const;

            // note that full paths are not returned, only the filenames, Load() will create a full
            // path from the filename
            const std::string
                Filename(const item::ItemPtr_t ITEM_PTR, const bool WILL_RANDOMIZE = true) const;

            void EnsureFileExists(const item::ItemPtr_t ITEM_PTR) const;

        private:
            static const std::vector<std::string> Filenames(
                const item::misc_type::Enum ITEM_ENUM, const bool IS_JEWELED, const bool IS_BONE);

            static const std::string Filename(
                const item::misc_type::Enum ITEM_ENUM,
                const bool IS_JEWELED,
                const bool IS_BONE,
                const bool WILL_RANDOMIZE);

            static const std::string GetSkinImageFilename(const item::material::Enum);

            static const std::string Filename(
                const item::weapon::WeaponTypeWrapper & WEAPON_INFO, const bool IS_JEWELED);

            static const std::string Filename(const item::armor::ArmorTypeWrapper & ARMOR_INFO);

            static void Load(sf::Texture &, const std::string & FILENAME);

            static const std::string MakeFullPathFromFilename(const std::string & FILENAME);

            static void EnsureValidDimmensions(const sf::Texture &, const std::string & ERROR_MSG);

            static const std::vector<std::string> MakeFilenames(
                const std::string & PREFIX, const int LAST_NUMBER, const int FIRST_NUMBER = 1);

            static const std::string FILE_EXT_STR_;
            static const std::string SEPARATOR_;
        };

    } // namespace gui
} // namespace sfml_util
} // namespace heroespath

#endif // HEROESPATH_SFMLUTIL_GUI_ITEM_IMAGE_MACHINE_HPP_INCLUDED
