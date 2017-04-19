#ifndef APPBASE_HEROESPATH_CAMPTAGE_INCLUDED
#define APPBASE_HEROESPATH_CAMPTAGE_INCLUDED
//
// camp-stage.hpp
//  A Stage class that allows characters to camp the game
//
#include "sfml-util/sfml-graphics.hpp"
#include "sfml-util/stage.hpp"
#include "sfml-util/sliders.hpp"
#include "sfml-util/gui/background-image.hpp"

#include "heroespath/horiz-symbol.hpp"
#include "heroespath/main-menu-buttons.hpp"
#include "heroespath/i-popup-callback.hpp"
#include "heroespath/ouroboros.hpp"
#include "heroespath/main-menu-title.hpp"

#include <memory>
#include <string>


namespace sfml_util
{
    class MultiTextureAnimation;
    using MultiTextureAnimationSPtr_t = std::shared_ptr<MultiTextureAnimation>;
}

namespace heroespath
{
namespace stage
{

    //A Stage class that allows camping characters
    class CampStage
    :
        public sfml_util::Stage,
        public heroespath::callback::IPopupHandler_t
    {
        //prevent copy construction
        CampStage(const CampStage &);

        //prevent copy assignment
        CampStage & operator=(const CampStage &);

    public:
        CampStage();
        virtual ~CampStage();

        inline virtual const std::string HandlerName() const { return GetStageName(); }
        virtual bool HandleCallback(const heroespath::callback::PopupResponse &);

        virtual void Setup();
        virtual void Draw(sf::RenderTarget & target, sf::RenderStates states);
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
#endif //APPBASE_HEROESPATH_CAMPTAGE_INCLUDED

