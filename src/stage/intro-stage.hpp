// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_INTROSTAGE_HPP_INCLUDED
#define HEROESPATH_INTROSTAGE_HPP_INCLUDED
//
// intro-stage.hpp
//
#include "sfml-util/cached-texture.hpp"
#include "sfml-util/stage.hpp"

namespace heroespath
{
namespace stage
{

    // Responsible for drawing a single centered title image that fades in/out
    class IntroStage : public sfml_util::Stage
    {
    public:
        IntroStage(const IntroStage &) = delete;
        IntroStage(IntroStage &&) = delete;
        IntroStage & operator=(const IntroStage &) = delete;
        IntroStage & operator=(IntroStage &&) = delete;

        IntroStage();
        virtual ~IntroStage();

        virtual void Setup();
        virtual void Draw(sf::RenderTarget & target, const sf::RenderStates & STATES);
        virtual void UpdateTime(const float ELAPSED_TIME_SECONDS);

    private:
        sfml_util::CachedTexture titleCachedTexture_;
        sf::Sprite titleSprite_;
        std::size_t initialDrawHoldCounter_;
    };

} // namespace stage
} // namespace heroespath

#endif // HEROESPATH_INTROSTAGE_HPP_INCLUDED
