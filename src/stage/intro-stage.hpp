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
#include "sfml-util/music-enum.hpp"
#include "stage/stage-base.hpp"

#include <SFML/Graphics/Sprite.hpp>

namespace heroespath
{
namespace stage
{

    // Responsible for drawing a single centered title image that fades in/out
    class IntroStage : public stage::StageBase
    {
    public:
        IntroStage(const IntroStage &) = delete;
        IntroStage(IntroStage &&) = delete;
        IntroStage & operator=(const IntroStage &) = delete;
        IntroStage & operator=(IntroStage &&) = delete;

        IntroStage();
        virtual ~IntroStage();

        void Setup() final;
        void Draw(sf::RenderTarget & target, const sf::RenderStates & STATES) final;
        void UpdateTime(const float ELAPSED_TIME_SECONDS) final;

        bool KeyPress(const sf::Event::KeyEvent &) final;
        void UpdateMouseDown(const sf::Vector2f &) final;

    private:
        const sfml_util::music::Enum backgroundMusic_;
        const float musicVolumeOrig_;
        const float timeFromStartToMusicSec_;
        const float timeFromMusicToAnimSec_;
        const float timeFromAnimToExitSec_;
        const float imageScaleSpeed_;
        sfml_util::CachedTexture titleCachedTexture_;
        sf::Sprite titleSprite_;
        float elapsedSec_;
        bool hasMusicStarted_;
        bool hasAnimStarted_;
        bool hasExited_;
        bool wasMusicVolumeChanged_;
    };

} // namespace stage
} // namespace heroespath

#endif // HEROESPATH_INTROSTAGE_HPP_INCLUDED
