// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// title-enum.cpp
//
#include "title-enum.hpp"

#include "creature/title-holder.hpp"
#include "creature/title.hpp"
#include "misc/config-file.hpp"
#include "misc/filesystem.hpp"

namespace heroespath
{
namespace creature
{

    const std::string Titles::Desc(const Titles::Enum ENUM)
    {
        return misc::ConfigFile::Instance()->Value(
            "creature-title-" + NAMEOF_ENUM_STR(ENUM) + "-desc");
    }

    const std::string Titles::ImageFilename(const Titles::Enum ENUM)
    {
        return title::Holder::Get(ENUM)->ImageFilename();
    }

    const std::string Titles::ImageDirectory()
    {
        static std::string imageDir;

        if (imageDir.empty())
        {
            imageDir = misc::ConfigFile::Instance()->GetMediaPath("media-image-title-dir");
        }

        return imageDir;
    }

    const std::string Titles::ImagePath(const Enum ENUM)
    {
        return misc::filesystem::CombinePaths(ImageDirectory(), ImageFilename(ENUM));
    }

} // namespace creature
} // namespace heroespath
