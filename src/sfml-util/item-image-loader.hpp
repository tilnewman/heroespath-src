// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_SFMLUTIL_ITEM_IMAGE_LOADER_HPP_INCLUDED
#define HEROESPATH_SFMLUTIL_ITEM_IMAGE_LOADER_HPP_INCLUDED
//
// item-image-loader.hpp
//
#include "creature/race-enum.hpp"
#include "creature/role-enum.hpp"
#include "item/armor-type-wrapper.hpp"
#include "item/item-type-enum.hpp"
#include "item/weapon-type-wrapper.hpp"
#include "misc/not-null.hpp"
#include "sfml-util/image-util.hpp"

#include <memory>
#include <string>
#include <vector>

namespace sf
{
class Texture;
}

namespace heroespath
{
namespace item
{
    class Item;
    using ItemPtr_t = misc::NotNull<Item *>;
} // namespace item

namespace sfml_util
{

    // Responsible for loading Item images.
    class ItemImageLoader
    {
    public:
        ItemImageLoader(const ItemImageLoader &) = delete;
        ItemImageLoader(ItemImageLoader &&) = delete;
        ItemImageLoader & operator=(const ItemImageLoader &) = delete;
        ItemImageLoader & operator=(ItemImageLoader &&) = delete;

        ItemImageLoader();

        bool Test() const;

        float MaxDimmension() const { return StandardImageDimmension(); }

        void Load(sf::Texture & texture, const item::ItemPtr_t ITEM_PTR) const;

        const std::string Path(const item::ItemPtr_t ITEM_PTR) const;
        const std::string Path(const std::string & FILE_NAME) const;

        const std::string
            Filename(const item::ItemPtr_t ITEM_PTR, const bool WILL_RANDOMIZE = true) const;

        bool DoesFileExist(const item::ItemPtr_t ITEM_PTR) const;
        bool DoesFileExist(const std::string & IMAGE_FILE_NAME) const;

    private:
        const std::vector<std::string> Filenames(
            const item::misc_type::Enum ITEM_ENUM, const bool IS_JEWELED, const bool IS_BONE) const;

        const std::string Filename(
            const item::misc_type::Enum ITEM_ENUM,
            const bool IS_JEWELED,
            const bool IS_BONE,
            const bool WILL_RANDOMIZE) const;

        const std::string GetSkinImageFilename(const item::material::Enum) const;

        const std::string Filename(
            const item::weapon::WeaponTypeWrapper & WEAPON_INFO, const bool IS_JEWELED) const;

        const std::string Filename(const item::armor::ArmorTypeWrapper & ARMOR_INFO) const;

        void Load(sf::Texture &, const std::string & FILENAME) const;

        const std::string MakeFullPathFromFilename(const std::string & FILENAME) const;

        void EnsureValidDimmensions(const sf::Texture &, const std::string & ERROR_MSG) const;

        const std::vector<std::string> MakeFilenames(
            const std::string & PREFIX, const int LAST_NUMBER, const int FIRST_NUMBER = 1) const;

        static const std::string FILE_EXT_STR_;
        static const std::string SEPARATOR_;

        std::string imageDirectoryPath_;
    };

} // namespace sfml_util
} // namespace heroespath

#endif // HEROESPATH_SFMLUTIL_ITEM_IMAGE_LOADER_HPP_INCLUDED
