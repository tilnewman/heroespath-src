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
#include <string>
#include <string_view>

namespace heroespath
{
namespace gui
{

    struct ContentImage
    {
        ContentImage() = delete;

        static void SetupFilesystemPaths();

        static constexpr std::string_view FilenameExtension() { return ".png"; }

        static constexpr std::string_view FilenameSeparator() { return separator_; }
        static constexpr char FilenameSeparatorChar() { return separator_[0]; }

        static constexpr std::string_view FilenameTodo(const bool WILL_INCLUDE_EXTENSION)
        {
            if (WILL_INCLUDE_EXTENSION)
            {
                return filenameTodo_;
            }
            else
            {
                return filenameFullTodo_;
            }
        }

        static constexpr std::string_view FilenameError(const bool WILL_INCLUDE_EXTENSION)
        {
            if (WILL_INCLUDE_EXTENSION)
            {
                return filenameError_;
            }
            else
            {
                return filenameFullError_;
            }
        }

        static const std::string PathDirTodo() { return pathMiscImageDir_; }
        static const std::string PathFullTodo() { return pathFullTodo_; }

        static const std::string PathDirError() { return pathMiscImageDir_; }
        static const std::string PathFullError() { return pathFullError_; }

    private:
        static inline constexpr std::string_view separator_ = "-";
        static inline constexpr std::string_view filenameTodo_ = "todo";
        static inline constexpr std::string_view filenameFullTodo_ = "todo.png";
        static inline constexpr std::string_view filenameError_ = "error";
        static inline constexpr std::string_view filenameFullError_ = "error.png";
        static std::string pathMiscImageDir_;
        static std::string pathFullTodo_;
        static std::string pathFullError_;
    };

} // namespace gui
} // namespace heroespath

#endif // HEROESPATH_GUI_CONTENT_IMAGES_HPP_INCLUDED
