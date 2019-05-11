// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// song-enum.cpp
//
#include "song-enum.hpp"

#include "misc/config-file.hpp"
#include "misc/filesystem.hpp"
#include "misc/strings.hpp"

namespace heroespath
{
namespace song
{

    const std::string Songs::ShortDesc(const Songs::Enum ENUM)
    {
        return misc::ConfigFile::Instance()->Value("song-" + NAMEOF_ENUM_STR(ENUM) + "-short-desc");
    }

    const std::string Songs::ExtraDesc(const Songs::Enum ENUM)
    {
        return misc::ConfigFile::Instance()->Value("song-" + NAMEOF_ENUM_STR(ENUM) + "-extra-desc");
    }

    const std::string Songs::ImageFilename(const Songs::Enum ENUM)
    {
        return misc::ToLowerCopy(NAMEOF_ENUM_STR(ENUM) + ".png");
    }

    const std::string Songs::ImageDirectory()
    {
        static std::string imageDir;

        if (imageDir.empty())
        {
            imageDir = misc::ConfigFile::Instance()->GetMediaPath("media-image-song-dir");
        }

        return imageDir;
    }

    const std::string Songs::ImagePath(const Enum ENUM)
    {
        return misc::filesystem::CombinePaths(ImageDirectory(), ImageFilename(ENUM));
    }

} // namespace song
} // namespace heroespath
