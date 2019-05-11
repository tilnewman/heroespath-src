// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// spell-enum.cpp
//
#include "spell-enum.hpp"

#include "misc/config-file.hpp"
#include "misc/filesystem.hpp"
#include "misc/log-macros.hpp"

namespace heroespath
{
namespace spell
{

    const std::string Spells::ShortDesc(const Spells::Enum ENUM)
    {
        return misc::ConfigFile::Instance()->Value(
            "spell-" + NAMEOF_ENUM_STR(ENUM) + "-short-desc");
    }

    const std::string Spells::ExtraDesc(const Spells::Enum ENUM)
    {
        return misc::ConfigFile::Instance()->Value(
            "spell-" + NAMEOF_ENUM_STR(ENUM) + "-extra-desc");
    }

    const std::string Spells::ImageFilename(const Spells::Enum ENUM)
    {
        return misc::ToLowerCopy(NAMEOF_ENUM_STR(ENUM) + ".png");
    }

    const std::string Spells::ImageDirectory()
    {
        static std::string imageDir;

        if (imageDir.empty())
        {
            imageDir = misc::ConfigFile::Instance()->GetMediaPath("media-image-spell-dir");
        }

        return imageDir;
    }

    const std::string Spells::ImagePath(const Enum ENUM)
    {
        return misc::filesystem::CombinePaths(ImageDirectory(), ImageFilename(ENUM));
    }

} // namespace spell
} // namespace heroespath
