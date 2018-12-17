// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_GUI_CONTENT_IMAGES_HPP_INCLUDED
#define HEROESPATH_GUI_CONTENT_IMAGES_HPP_INCLUDED
//
// content-images.hpp
//
#include <SFML/System/Vector2.hpp>

#include <string>

namespace heroespath
{
namespace gui
{

    struct ContentImage
    {
        ContentImage() = delete;

        static void SetupFilesystemPaths();

        static const std::string FilenameExtension();
        static const std::string FilenameSeparator();
        static const std::string TodoFilename();
        static const std::string TodoPath();
        static const std::string ErrorFilename();
        static const std::string ErrorPath();

    private:
        static std::string todoPath_;
        static std::string errorPath_;
    };

} // namespace gui
} // namespace heroespath

#endif // HEROESPATH_GUI_CONTENT_IMAGES_HPP_INCLUDED
