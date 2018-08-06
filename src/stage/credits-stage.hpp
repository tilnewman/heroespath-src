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
#include "sfml-util/cached-texture.hpp"
#include "sfml-util/gui/box-entity.hpp"
#include "sfml-util/music-enum.hpp"
#include "sfml-util/sfml-util-primitives.hpp"
#include "sfml-util/stage.hpp"
#include "stage/credit.hpp"

#include <SFML/Graphics/Sprite.hpp>

#include <memory>
#include <vector>

namespace heroespath
{
namespace stage
{

    // A simple class that displays the credits
    class CreditsStage : public sfml_util::Stage
    {
    public:
        CreditsStage(const CreditsStage &) = delete;
        CreditsStage(CreditsStage &&) = delete;
        CreditsStage & operator=(const CreditsStage &) = delete;
        CreditsStage & operator=(CreditsStage &&) = delete;

    public:
        CreditsStage();
        virtual ~CreditsStage();

        virtual void Setup();
        virtual void UpdateTime(const float ELAPSED_TIME_SECONDS);
        virtual void Draw(sf::RenderTarget & target, const sf::RenderStates &);

        virtual bool KeyPress(const sf::Event::KeyEvent & KE);
        virtual bool KeyRelease(const sf::Event::KeyEvent & KE);

    private:
        static const float DEFAULT_SCROLL_SPEED_;
        static const float SCROLL_SPEED_MULT_;
        static const float SCROLL_SPEED_MAX_;
        static const float CREDIT_BOX_INNER_PAD_;
        //
        const float CREDIT_BOX_WIDTH_;
        const float CREDITBOX_POS_LEFT_;
        //
        float creditBoxPosTop_;
        float creditBoxHeight_;
        float totalHeight_;
        float heightTracker_;
        sfml_util::CachedTexture titleCachedTexture_;
        sf::Sprite bpTitleSprite_;
        sfml_util::gui::BoxEntityUPtr_t boxUPtr_;
        CreditUVec_t creditUVec_;
        float scrollSpeed_;
        bool isKeyHeldDown_;
        sf::RectangleShape aboveBlackRectangle_;
        sf::RectangleShape belowBlackRectangle_;
    };

} // namespace stage
} // namespace heroespath

#endif // HEROESPATH_CREDITSTAGE_HPP_INCLUDED
