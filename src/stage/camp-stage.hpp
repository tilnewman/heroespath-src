// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_CAMPTAGE_HPP_INCLUDED
#define HEROESPATH_CAMPTAGE_HPP_INCLUDED
//
// camp-stage.hpp
//  A Stage class that allows characters to camp the game
//
#include "popup/i-popup-callback.hpp"
#include "sfml-util/gui/background-image.hpp"
#include "sfml-util/horiz-symbol.hpp"
#include "sfml-util/main-menu-buttons.hpp"
#include "sfml-util/main-menu-title.hpp"
#include "sfml-util/sfml-graphics.hpp"
#include "sfml-util/sliders.hpp"
#include "sfml-util/stage.hpp"

#include <memory>
#include <string>

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

    // A Stage class that allows the party to camp for resting and healing
    class CampStage
        : public sfml_util::Stage
        , public popup::IPopupHandler_t
    {
    public:
        CampStage(const CampStage &) = delete;
        CampStage(CampStage &&) = delete;
        CampStage & operator=(const CampStage &) = delete;
        CampStage & operator=(CampStage &&) = delete;

    public:
        CampStage();
        virtual ~CampStage();

        virtual const std::string HandlerName() const { return GetStageName(); }
        virtual bool HandleCallback(const popup::PopupResponse &);

        virtual void Setup();
        virtual void Draw(sf::RenderTarget & target, const sf::RenderStates & STATES);
        virtual void UpdateTime(const float ELAPSED_TIME_SECONDS);

    private:
        const std::string ComposeNewGamePopupText1();
        const std::string ComposeNewGamePopupText2();
        const std::string ComposeNewGamePopupText3();
        const std::string ComposeNewGamePopupText4();

    public:
        static const std::string NEWHEROESPATH_POPUP_NAME1_;
        static const std::string NEWHEROESPATH_POPUP_NAME2_;
        static const std::string NEWHEROESPATH_POPUP_NAME3_;
        static const std::string NEWHEROESPATH_POPUP_NAME4_;

    private:
        const float SCREEN_WIDTH_;
        const float SCREEN_HEIGHT_;
        sfml_util::MainMenuTitle mainMenuTitle_;
        sf::Texture campfireTexture_;
        sf::Sprite campfireSprite_;
        sfml_util::gui::BackgroundImage backgroundImage_;
        sfml_util::AnimationUPtr_t fireAnimUPtr_;
        bool showNewGamePopup1_;
        bool showNewGamePopup2_;
        bool showNewGamePopup3_;
        bool showNewGamePopup4_;
        sfml_util::OuroborosUPtr_t ouroborosUPtr_;
        sfml_util::BottomSymbol botSymbol_;
    };

} // namespace stage
} // namespace heroespath

#endif // HEROESPATH_CAMPTAGE_HPP_INCLUDED
