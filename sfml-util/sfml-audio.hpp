#ifndef SFMLUTIL_SFMLAUDIO_HPP_INCLUDED
#define SFMLUTIL_SFMLAUDIO_HPP_INCLUDED
//
// sfml-audio.hpp
//

//prevent warnings that can be ignored in SFML
#include "sfml-util/platform.hpp"
#ifdef SFMLUTIL_PLATFORMDETECT__APPLE_OS
#pragma GCC diagnostic ignored "-Wundef"
#pragma GCC diagnostic ignored "-Wfloat-equal"
#endif

#include <SFML/Audio.hpp>

#ifdef SFMLUTIL_PLATFORMDETECT__APPLE_OS
#pragma GCC diagnostic warning "-Wfloat-equal"
#pragma GCC diagnostic warning "-Wundef"
#endif

#include <memory>
#include <vector>


namespace sfml_util
{

    using SoundSPtr_t = std::shared_ptr<sf::Sound>;
    using SoundSVec_t = std::vector<SoundSPtr_t>;
    //
    using SoundBufferSPtr_t = std::shared_ptr<sf::SoundBuffer>;
    using SoundBufferSVec_t = std::vector<SoundBufferSPtr_t>;
    //
    using MusicSPtr_t = std::shared_ptr<sf::Music>;
    using MusicSVec_t = std::vector<MusicSPtr_t>;

}


#endif //SFMLUTIL_SFMLAUDIO_HPP_INCLUDED
