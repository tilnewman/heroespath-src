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
#include <string>
#include <vector>

namespace heroespath
{
namespace item
{
    class ItemProfile;
} // namespace item

namespace gui
{

    class ItemImagePaths
    {
    public:
        ItemImagePaths() = delete;

        static void SetupFilesystemPaths();

        static const std::string
            Filename(const item::ItemProfile & PROFILE, const bool WILL_RANDOMIZE);

        static const std::vector<std::string> Filenames(const item::ItemProfile & PROFILE);

        static const std::string PathFromFilename(const std::string & FILE_NAME);

    private:
        static const std::vector<std::string> MiscFilenames(const item::ItemProfile & PROFILE);

        static const std::string WeaponFilename(const item::ItemProfile & PROFILE);
        static const std::string ArmorFilename(const item::ItemProfile & PROFILE);
        static const std::string SkinArmorFilename(const item::ItemProfile & PROFILE);

        static const std::vector<std::string> MakeNumberedFilenames(
            const std::string_view PREFIX, const int LAST_NUMBER, const int FIRST_NUMBER = 1);

        static std::string itemImageDir_;
    };

} // namespace gui
} // namespace heroespath

#endif // HEROESPATH_GUI_ITEM_IMAGE_PATHS_HPP_INCLUDED
