// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// level-enum.cpp
//
#include "level-enum.hpp"

#include "misc/config-file.hpp"
#include "misc/filesystem.hpp"

namespace heroespath
{
namespace map
{

    const std::string Level::Path(const Level::Enum LEVEL)
    {
        static std::string dirPath;

        if (dirPath.empty())
        {
            dirPath = misc::ConfigFile::Instance()->GetMediaPath("media-maps-dir");
        }

        return misc::filesystem::CombinePaths(
            dirPath, NAMEOF_ENUM_STR(LEVEL) + std::string(Level::FILENAME_EXTENSION));
    }

} // namespace map
} // namespace heroespath
