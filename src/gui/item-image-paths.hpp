// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_GUI_ITEM_IMAGE_PATHS_HPP_INCLUDED
#define HEROESPATH_GUI_ITEM_IMAGE_PATHS_HPP_INCLUDED
//
// item-image-paths.hpp
//
#include "item/item-type-enum.hpp"
#include "misc/not-null.hpp"

#include <memory>
#include <string>
#include <vector>

namespace heroespath
{
namespace item
{
    class Item;

    namespace armor
    {
        class ArmorTypeWrapper;
    }

    namespace weapon
    {
        class WeaponTypeWrapper;
    }

} // namespace item

namespace gui
{

    class ItemImagePaths
    {
    public:
        ItemImagePaths() = delete;

        static void SetupFilesystemPaths();
        static const std::string PathFromFilename(const std::string & FILE_NAME);
        static const std::string Filename(const item::Item & ITEM, const bool WILL_RANDOMIZE);
        static const std::vector<std::string> Filenames(const item::Item & ITEM);

        static const std::string
            Filename(const item::weapon::WeaponTypeWrapper & WEAPON_INFO, const bool IS_JEWELED);

    private:
        static const std::vector<std::string> MiscFilenames(
            const item::misc_type::Enum ITEM_ENUM, const bool IS_JEWELED, const bool IS_BONE);

        static const std::string Filename(
            const item::misc_type::Enum ITEM_ENUM,
            const bool IS_JEWELED,
            const bool IS_BONE,
            const bool WILL_RANDOMIZE);

        static const std::string GetSkinImageFilename(const item::material::Enum);

        static const std::vector<std::string> MakeFilenames(
            const std::string & PREFIX, const int LAST_NUMBER, const int FIRST_NUMBER = 1);

        static const std::string SEPARATOR_;
        static std::string imageDirectoryPath_;
    };

} // namespace gui
} // namespace heroespath

#endif // HEROESPATH_GUI_ITEM_IMAGE_PATHS_HPP_INCLUDED
