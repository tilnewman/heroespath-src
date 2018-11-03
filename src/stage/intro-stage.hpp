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
#include "gui/cached-texture.hpp"
#include "gui/music-enum.hpp"
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
        void draw(sf::RenderTarget &, sf::RenderStates) const final;
        void UpdateTime(const float ELAPSED_TIME_SECONDS) final;

        bool KeyPress(const sf::Event::KeyEvent &) final;
        void UpdateMouseDown(const sf::Vector2f &) final;

    private:
        const gui::music::Enum backgroundMusic_;
        const float musicVolumeOrig_;
        const float timeFromStartToMusicSec_;
        const float timeFromMusicToAnimSec_;
        const float timeFromAnimToExitSec_;
        const float imageScaleSpeed_;
        const float fadeSpeed_;
        gui::CachedTexture titleCachedTexture_;
        sf::Sprite titleSprite_;
        float elapsedSec_;
        float fadeColorAlpha_;
        bool hasMusicStarted_;
        bool hasAnimStarted_;
        bool hasExited_;
        bool wasMusicVolumeChanged_;
    };

} // namespace stage
} // namespace heroespath

#endif // HEROESPATH_INTROSTAGE_HPP_INCLUDED
