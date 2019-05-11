// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// condition-enum.cpp
//
#include "condition-enum.hpp"

#include "misc/config-file.hpp"
#include "misc/filesystem.hpp"
#include "misc/log-macros.hpp"

namespace heroespath
{
namespace creature
{

    const std::string Conditions::Desc(const Conditions::Enum ENUM)
    {
        return misc::ConfigFile::Instance()->Value(
            "creature-condition-" + NAMEOF_ENUM_STR(ENUM) + "-desc");
    }

    const std::string Conditions::ImageFilename(const Conditions::Enum ENUM)
    {
        std::string str(NAMEOF_ENUM(ENUM));
        misc::ToLower(str);
        str += ".png";
        return str;
    }

    const std::string Conditions::ImageDirectory()
    {
        static std::string imageDir;

        if (imageDir.empty())
        {
            imageDir = misc::ConfigFile::Instance()->GetMediaPath("media-image-condition-dir");
        }

        return imageDir;
    }

    const std::string Conditions::ImagePath(const Enum ENUM)
    {
        return misc::filesystem::CombinePaths(ImageDirectory(), ImageFilename(ENUM));
    }

} // namespace creature
} // namespace heroespath
