#ifndef GAME_PARTYSTAGE_INCLUDED
#define GAME_PARTYSTAGE_INCLUDED
//
// party-stage.hpp
//  A Stage class that allows grouping of saved characters into a party.
//
#include "sfml-util/sfml-graphics.hpp"
#include "sfml-util/sfml-system.hpp"
#include "sfml-util/stage.hpp"
#include "sfml-util/gui/background-image.hpp"
#include "sfml-util/gui/list-box.hpp"
#include "sfml-util/sliders.hpp"

#include "game/main-menu-buttons.hpp"
#include "game/i-popup-callback.hpp"
#include "game/ouroboros.hpp"
#include "game/horiz-symbol.hpp"
#include "game/main-menu-title.hpp"

#include <memory>

namespace game
{
    namespace player
    {
        class Character;
        using CharacterSPtr_t = std::shared_ptr<Character>;
    }

namespace stage
{

    //A Stage class that displays saved characters and allows grouping them into a party of six
    class PartyStage
    :
        public sfml_util::Stage,
        public sfml_util::gui::callback::IListBoxCallbackHandler,
        public sfml_util::gui::callback::IFourStateButtonCallbackHandler_t,
        public game::callback::IPopupHandler_t
    {
        //prevent copy construction
        PartyStage(const PartyStage &) =delete;

        //prevent copy assignment
        PartyStage & operator=(const PartyStage &) =delete;

    public:
        PartyStage();
        virtual ~PartyStage();

        inline virtual const std::string HandlerName() const { return GetStageName(); }
        virtual bool HandleCallback(const sfml_util::gui::callback::ListBoxEventPackage &);
        virtual bool HandleCallback(const sfml_util::gui::callback::FourStateButtonCallbackPackage_t &);
        virtual bool HandleCallback(const game::callback::PopupResponse &);
        //
        virtual bool HandleCallback_BackButton();
        virtual bool HandleCallback_StartButton();
        virtual bool HandleCallback_DeleteButton();

        virtual void Setup();
        virtual void Draw(sf::RenderTarget & target, const sf::RenderStates &);
        virtual std::size_t NumCharactersInTheParty() const;
        virtual sfml_util::gui::ListBoxItemSPtr_t GetSelectedItemSPtr() const;
        virtual player::CharacterSPtr_t GetSelectedCharacter() const;

        virtual void UpdateTime(const float ELAPSED_TIME_SECONDS);

        virtual void UpdateMousePos(const sf::Vector2f & MOUSE_POS_V);

        virtual void StartNewGame();

        virtual bool KeyRelease(const sf::Event::KeyEvent &);

    private:
        void ResetMouseOverPopupState();

    private:
        static const std::string POPUP_NAME_STR_NOT_ENOUGH_CHARS_;
        static const std::string POPUP_NAME_STR_TOO_MANY_CHARS_;
        static const std::string POPUP_NAME_STR_DELETE_CONFIRM_;
        static const float MOUSE_OVER_POPUP_DELAY_SEC_;
        static const float MOUSE_OVER_SLIDER_SPEED_;
        static const float MOUSE_OVER_POPUP_POS_LEFT_;
        static const float MOUSE_OVER_POPUP_POS_TOP_;
        static const float MOUSE_OVER_IMAGE_PAD_;
        static const float MOUSE_OVER_BOX_ALPHA_;
        //
        const float SCREEN_WIDTH_;
        const float SCREEN_HEIGHT_;
        MainMenuTitle mainMenuTitle_;
        sfml_util::gui::BackgroundImage backgroundImage_;
        sfml_util::gui::FourStateButtonSPtr_t backButtonSPtr_;
        sfml_util::gui::FourStateButtonSPtr_t startButtonSPtr_;
        sfml_util::gui::FourStateButtonSPtr_t deleteButtonSPtr_;
        sfml_util::gui::ListBoxSPtr_t characterListBoxSPtr_;
        sfml_util::gui::ListBoxSPtr_t partyListBoxSPtr_;
        sfml_util::gui::TextRegionSPtr_t insTextRegionSPtr_;
        sfml_util::gui::TextInfo warningTextInfo_;
        sfml_util::gui::TextRegionSPtr_t warningTextRegionSPtr_;
        sfml_util::sliders::Slider2<sf::Uint8, float> warningTextSlider_;
        OuroborosSPtr_t ouroborosSPtr_;
        BottomSymbol bottomSymbol_;
        bool willDisplayCharacterCountWarningText_;

        //mouseover popup and animation members
        bool            willShowMouseOverPopup_;
        float           mouseOverPopupTimerSec_;
        sf::VertexArray mouseOverQuad_;
        float           mouseOverBoxWidth_;
        float           mouseOverBoxHeight_;
        sf::Vector2f    mouseOverPosV_;
        sf::Sprite      mouseOverSprite_;
        player::CharacterSPtr_t mouseOverCharSPtr_;
        sfml_util::TextureSPtr_t mouseOverTextureSPtr_;
        sfml_util::gui::TextRegionSPtr_t mouseOverTextRegionSPtr_;
        sfml_util::sliders::ZeroSliderOnce<float> mouseOverSlider_;
    };

}
}
#endif //GAME_PARTYSTAGE_INCLUDED
