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

    std::string ContentImage::todoPath_ { "" };
    std::string ContentImage::errorPath_ { "" };

    void ContentImage::SetupFilesystemPaths()
    {
        todoPath_ = misc::ConfigFile::Instance()->GetMediaPath("media-image-misc-todo");

        M_HP_ASSERT_OR_LOG_AND_THROW(
            misc::filesystem::ExistsAndIsFile(todoPath_),
            "Default todo content image path does not exist or is not a file."
                + M_HP_VAR_STR(todoPath_));

        errorPath_ = misc::ConfigFile::Instance()->GetMediaPath("media-image-misc-error");

        M_HP_ASSERT_OR_LOG_AND_THROW(
            misc::filesystem::ExistsAndIsFile(errorPath_),
            "Default error content image path does not exist or is not a file."
                + M_HP_VAR_STR(todoPath_));
    }

    const std::string ContentImage::FilenameExtension() { return ".png"; }

    const std::string ContentImage::FilenameSeparator() { return "-"; }

    const std::string ContentImage::TodoFilename() { return "todo" + FilenameExtension(); }

    const std::string ContentImage::TodoPath() { return todoPath_; }

    const std::string ContentImage::ErrorFilename() { return "error" + FilenameExtension(); }

    const std::string ContentImage::ErrorPath() { return errorPath_; }

} // namespace gui
} // namespace heroespath
