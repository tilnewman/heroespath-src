// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_SFMLUTIL_SFMLAUDIO_HPP_INCLUDED
#define HEROESPATH_SFMLUTIL_SFMLAUDIO_HPP_INCLUDED
//
// sfml-audio.hpp
//

// prevent warnings that can be ignored in SFML
#include "misc/platform.hpp"
#ifdef HEROESPATH_PLATFORM_DETECTED_IS_APPLE
#pragma GCC diagnostic ignored "-Wundef"
#pragma GCC diagnostic ignored "-Wfloat-equal"
#endif

#include <SFML/Audio.hpp>

#ifdef HEROESPATH_PLATFORM_DETECTED_IS_APPLE
#pragma GCC diagnostic warning "-Wfloat-equal"
#pragma GCC diagnostic warning "-Wundef"
#endif

#include <memory>
#include <vector>

namespace heroespath
{
namespace sfml_util
{
    using MusicUPtr_t = std::unique_ptr<sf::Music>;
    using SoundUPtr_t = std::unique_ptr<sf::Sound>;
    using SoundBufferUPtr_t = std::unique_ptr<sf::SoundBuffer>;
} // namespace sfml_util
} // namespace heroespath

#endif // HEROESPATH_SFMLUTIL_SFMLAUDIO_HPP_INCLUDED
