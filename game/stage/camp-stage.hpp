#ifndef GAME_CAMPTAGE_INCLUDED
#define GAME_CAMPTAGE_INCLUDED
//
// camp-stage.hpp
//  A Stage class that allows characters to camp the game
//
#include "sfml-util/sfml-graphics.hpp"
#include "sfml-util/stage.hpp"
#include "sfml-util/sliders.hpp"
#include "sfml-util/gui/background-image.hpp"

#include "game/horiz-symbol.hpp"
#include "game/main-menu-buttons.hpp"
#include "game/i-popup-callback.hpp"
#include "game/ouroboros.hpp"
#include "game/main-menu-title.hpp"

#include <memory>
#include <string>


namespace sfml_util
{
    class MultiTextureAnimation;
    using MultiTextureAnimationSPtr_t = std::shared_ptr<MultiTextureAnimation>;
}

namespace game
{
namespace stage
{

    //A Stage class that allows camping characters
    class CampStage
    :
        public sfml_util::Stage,
        public game::callback::IPopupHandler_t
    {
        //prevent copy construction
        CampStage(const CampStage &) =delete;

        //prevent copy assignment
        CampStage & operator=(const CampStage &) =delete;

    public:
        CampStage();
        virtual ~CampStage();

        inline virtual const std::string HandlerName() const { return GetStageName(); }
        virtual bool HandleCallback(const game::callback::PopupResponse &);

        virtual void Setup();
        virtual void Draw(sf::RenderTarget & target, const sf::RenderStates & STATES);
        virtual void UpdateTime(const float ELAPSED_TIME_SECONDS);

    private:
        const std::string ComposeNewGamePopupText1();
        const std::string ComposeNewGamePopupText2();
        const std::string ComposeNewGamePopupText3();
        const std::string ComposeNewGamePopupText4();

    public:
        static const std::string NEWGAME_POPUP_NAME1_;
        static const std::string NEWGAME_POPUP_NAME2_;
        static const std::string NEWGAME_POPUP_NAME3_;
        static const std::string NEWGAME_POPUP_NAME4_;

    private:
        const float SCREEN_WIDTH_;
        const float SCREEN_HEIGHT_;
        MainMenuTitle mainMenuTitle_;
        sf::Texture campfireTexture_;
        sf::Sprite  campfireSprite_;
        sfml_util::gui::BackgroundImage backgroundImage_;
        sfml_util::MultiTextureAnimationSPtr_t fireAnim1SPtr_;
        bool showNewGamePopup1_;
        bool showNewGamePopup2_;
        bool showNewGamePopup3_;
        bool showNewGamePopup4_;
        OuroborosSPtr_t ouroborosSPtr_;
        BottomSymbol botSymbol_;
    };

}
}
#endif //GAME_CAMPTAGE_INCLUDED

