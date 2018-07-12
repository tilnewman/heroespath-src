// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_INNSTAGE_HPP_INCLUDED
#define HEROESPATH_INNSTAGE_HPP_INCLUDED
//
// inn-stage.hpp
//  A Stage class that allows starting the game
//
#include "sfml-util/gui/background-image.hpp"
#include "sfml-util/sfml-graphics.hpp"
#include "sfml-util/stage.hpp"

#include "popup/i-popup-callback.hpp"
#include "sfml-util/gui/main-menu-buttons.hpp"
#include "sfml-util/horiz-symbol.hpp"

#include <memory>

namespace heroespath
{
namespace sfml_util
{
    class Animation;
    using AnimationUPtr_t = std::unique_ptr<Animation>;

    class Ouroboros;
    using OuroborosUPtr_t = std::unique_ptr<Ouroboros>;
} // namespace sfml_util

namespace stage
{

    // A Stage class that allows starting the game
    class InnStage : public sfml_util::Stage
    {
    public:
        InnStage(const InnStage &) = delete;
        InnStage(InnStage &&) = delete;
        InnStage & operator=(const InnStage &) = delete;
        InnStage & operator=(InnStage &&) = delete;

    public:
        InnStage();
        virtual ~InnStage();

        virtual void Setup();
        virtual void Draw(sf::RenderTarget & target, const sf::RenderStates & STATES);

    private:
        const float SCREEN_WIDTH_;
        const float SCREEN_HEIGHT_;
        //
        sf::Texture titleSymbolTexture_;
        sf::Sprite titleSymbolSprite_;
        sfml_util::gui::BackgroundImage backgroundImage_;
        sf::Texture candleTexture_;
        sf::Sprite candleSprite_;
        sfml_util::AnimationUPtr_t candleAnimUPtr_;
        sfml_util::OuroborosUPtr_t ouroborosUPtr_;
        sfml_util::BottomSymbol bottomSymbol_;
    };
} // namespace stage
} // namespace heroespath

#endif // HEROESPATH_INNSTAGE_HPP_INCLUDED
