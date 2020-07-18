// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// content-images.cpp
//
#include "content-images.hpp"

#include "misc/assertlogandthrow.hpp"
#include "misc/config-file.hpp"
#include "misc/filesystem.hpp"

namespace heroespath
{
namespace gui
{

    std::string ContentImage::pathMiscImageDir_ {};
    std::string ContentImage::pathFullTodo_ {};
    std::string ContentImage::pathFullError_ {};

    void ContentImage::SetupFilesystemPaths()
    {
        pathMiscImageDir_ = misc::ConfigFile::Instance()->GetMediaPath("media-image-misc-dir");

        pathFullTodo_ = misc::filesystem::CombinePathsAndClean(
            pathMiscImageDir_, std::string(FilenameTodo(true)));

        pathFullError_ = misc::filesystem::CombinePathsAndClean(
            pathMiscImageDir_, std::string(FilenameError(true)));
    }

} // namespace gui
} // namespace heroespath
