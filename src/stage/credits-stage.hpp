// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_CREDITSTAGE_HPP_INCLUDED
#define HEROESPATH_CREDITSTAGE_HPP_INCLUDED
//
// credits-stage.hpp
//  A Stage class that displays the rolling credits of the app.
//
#include "sfml-util/box-entity.hpp"
#include "sfml-util/cached-texture.hpp"
#include "sfml-util/colored-rect.hpp"
#include "sfml-util/music-enum.hpp"
#include "stage/credit.hpp"
#include "stage/stage-base.hpp"

#include <SFML/Graphics/Sprite.hpp>

#include <memory>
#include <vector>

namespace heroespath
{
namespace stage
{

    // A simple class that displays the credits
    class CreditsStage : public stage::StageBase
    {
    public:
        CreditsStage(const CreditsStage &) = delete;
        CreditsStage(CreditsStage &&) = delete;
        CreditsStage & operator=(const CreditsStage &) = delete;
        CreditsStage & operator=(CreditsStage &&) = delete;

        CreditsStage();
        virtual ~CreditsStage();

        void Setup() final;
        void UpdateTime(const float ELAPSED_TIME_SECONDS) final;
        void Draw(sf::RenderTarget & target, const sf::RenderStates &) final;

        bool KeyPress(const sf::Event::KeyEvent &) final;
        bool KeyRelease(const sf::Event::KeyEvent &) final;
        void UpdateMouseDown(const sf::Vector2f &) final;

    private:
        void UpdateCreditAnimations(const float ELAPSED_TIME_SECONDS);
        void UpdateCreditPositions(const float ELAPSED_TIME_SECONDS);
        void UpdateScrollSpeed(const float ELAPSED_TIME_SECONDS);
        bool IsCreditVisible(const Credit &) const;
        void MoveCreditsToStartPos();
        void MoveCreditsToReverseStartPos();

        static const float DEFAULT_SCROLL_SPEED_;
        static const float SCROLL_SPEED_MULT_;
        static const float SCROLL_SPEED_MAX_;

        gui::CachedTexture titleCachedTexture_;
        sf::Sprite bpTitleSprite_;
        gui::BoxEntityUPtr_t boxUPtr_;
        gui::BoxEntityUPtr_t boxBorderUPtr_;
        CreditUVec_t creditUVec_;
        float scrollSpeed_;
        bool isKeyHeldArrowUp_;
        bool isKeyHeldArrowDown_;
        sf::Clock keyPressClock_;
        gui::ColoredRect blackRectUpper_;
        gui::ColoredRect blackRectLower_;
    };

} // namespace stage
} // namespace heroespath

#endif // HEROESPATH_CREDITSTAGE_HPP_INCLUDED
