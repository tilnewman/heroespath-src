// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_SFUTIL_VIDEO_MODE_HPP_INCLUDED
#define HEROESPATH_SFUTIL_VIDEO_MODE_HPP_INCLUDED
//
// video-mode.hpp
//
#include "misc/boost-optional-that-throws.hpp"
#include "misc/strings.hpp"
#include "sfutil/vector-and-rect.hpp"

#include <SFML/Window/VideoMode.hpp>

namespace sf
{

inline std::ostream & operator<<(std::ostream & os, const sf::VideoMode & VM)
{
    os << "(" << VM.width << "x" << VM.height << ":" << VM.bitsPerPixel << ")";
    return os;
}

// operator== and operator!= provided by sfml

} // namespace sf

namespace heroespath
{

using VideoModeOpt_t = boost::optional<sf::VideoMode>;

namespace sfutil
{

    inline const std::string ToString(
        const sf::VideoMode & VM,
        const misc::ToStringPrefix::Enum OPTIONS = misc::ToStringPrefix::Default)
    {
        std::ostringstream ss;
        ss << misc::MakeToStringPrefix(OPTIONS, "VideoMode") << VM;
        return ss.str();
    }

} // namespace sfutil
} // namespace heroespath

#endif // HEROESPATH_SFUTIL_VIDEO_MODE_HPP_INCLUDED
