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
#ifndef SFMLUTIL_GUI_ITEMIMAGEMANAGER_HPP_INCLUDED
#define SFMLUTIL_GUI_ITEMIMAGEMANAGER_HPP_INCLUDED
//
// item-image-manager.hpp
//  Code that manages loading and lifetime of item images.
//
#include "sfml-util/sfml-graphics.hpp"

#include "item/item-type-enum.hpp"
#include "item/weapon-info.hpp"
#include "item/armor-info.hpp"
#include "creature/race-enum.hpp"
#include "creature/role-enum.hpp"

#include <memory>
#include <string>
#include <vector>


namespace heroespath
{
namespace item
{
    class Item;
    using ItemPtr_t = Item *;
}

namespace sfml_util
{
namespace gui
{

    //A class that loads, stores, and distributes creature images.
    class ItemImageManager
    {
        ItemImageManager(const ItemImageManager &) =delete;
        ItemImageManager & operator=(const ItemImageManager &) =delete;

    public:
        ItemImageManager();
        ~ItemImageManager();

        static ItemImageManager * Instance();
        static void Acquire();
        static void Release();

        static bool Test();

        static void SetItemImageDirectory(const std::string & PATH);
        static inline float GetMaxDimmension() { return 256.0f; }
        static inline const std::string FileExtension() { return FILE_EXT_STR_; }

        void Load(sf::Texture & texture, const std::string & IMAGE_FILE_NAME) const;

        void Load(sf::Texture & texture, const heroespath::item::ItemPtr_t ITEM_PTR) const;

        void Load(
            sf::Texture &                     texture,
            const heroespath::item::misc_type::Enum ITEM_ENUM,
            const bool                        IS_JEWELED = false,
            const bool                        IS_BONE = false,
            const bool                        WILL_RANDOMIZE = true) const;

        const std::string GetImageFilename(
            const heroespath::item::ItemPtr_t ITEM_PTR,
            const bool                  WILL_RANDOMIZE = true) const;

        const std::string GetImageFilename(
            const heroespath::item::weapon::WeaponInfo & WEAPON_INFO,
            const bool                             IS_JEWELED = false) const;

        const std::string GetImageFilename(
            const heroespath::item::armor::ArmorInfo & ARMOR_INFO) const;

        const std::string GetSkinImageFilename(
            const heroespath::item::ItemPtr_t ITEM_PTR) const;

        const std::string GetImageFilename(
            const heroespath::item::misc_type::Enum ITEM_ENUM,
            const bool                        IS_JEWELED     = false,
            const bool                        IS_BONE        = false,
            const bool                        WILL_RANDOMIZE = false,
            const heroespath::creature::race::Enum  RACE_ENUM = heroespath::creature::race::Count,
            const heroespath::creature::role::Enum  ROLE_ENUM = heroespath::creature::role::Count) const;

        const std::vector<std::string> GetImageFilenames(
            const heroespath::item::misc_type::Enum ITEM_ENUM,
            const bool                        IS_JEWELED = false,
            const bool                        IS_BONE    = false,
            const heroespath::creature::race::Enum  RACE_ENUM  = heroespath::creature::race::Count,
            const heroespath::creature::role::Enum  ROLE_ENUM  = heroespath::creature::role::Count) const;

    private:
        static std::unique_ptr<ItemImageManager> instanceUPtr_;
        static std::string imagesDirectoryPath_;
        static const std::string FILE_EXT_STR_;
    };

}
}
}

#endif //SFMLUTIL_GUI_ITEMIMAGEMANAGER_HPP_INCLUDED
