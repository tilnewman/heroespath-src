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
#include <SFML/Window/VideoMode.hpp>

#include <ostream>

namespace sf
{

inline std::ostream & operator<<(std::ostream & os, const sf::VideoMode & VM)
{
    os << "(" << VM.width << "x" << VM.height << ":" << VM.bitsPerPixel << ")";
    return os;
}

// operator== and operator!= provided by sfml

} // namespace sf

#endif // HEROESPATH_SFUTIL_VIDEO_MODE_HPP_INCLUDED
