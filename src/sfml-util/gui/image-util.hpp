// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_SFMLUTIL_GUI_IMAGE_UTIL_HPP_INCLUDED
#define HEROESPATH_SFMLUTIL_GUI_IMAGE_UTIL_HPP_INCLUDED
//
// image-util.hpp
//
namespace heroespath
{
namespace sfml_util
{
    namespace gui
    {
        namespace image
        {
            enum class Flip : bool
            {
                No = false,
                Yes = true
            };

            inline float StandardDimmension() { return 256.0f; }

        }; // namespace image

    } // namespace gui
} // namespace sfml_util
} // namespace heroespath

#endif // HEROESPATH_SFMLUTIL_GUI_IMAGE_UTIL_HPP_INCLUDED
