///////////////////////////////////////////////////////////////////////////////
//
// Heroes' Path - Open-source, non-commercial, simple, game in the RPG style.
// Copyright (C) 2017 Ziesche Til Newman (tilnewman@gmail.com)
//
// This software is provided 'as-is', without any express or implied warranty.
// In no event will the authors be held liable for any damages arising from
// the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely, subject to the following restrictions:
//
//  1. The origin of this software must not be misrepresented; you must not
//     claim that you wrote the original software.  If you use this software
//     in a product, an acknowledgment in the product documentation would be
//     appreciated but is not required.
//
//  2. Altered source versions must be plainly marked as such, and must not
//     be misrepresented as being the original software.
//
//  3. This notice may not be removed or altered from any source distribution.
//
///////////////////////////////////////////////////////////////////////////////
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
#include "popup/i-popup-callback.hpp"
#include "game/ouroboros.hpp"
#include "game/horiz-symbol.hpp"
#include "game/main-menu-title.hpp"

#include <memory>
#include <string>
#include <set>


namespace sfml_util
{
namespace gui
{
    class TextRegion;
    using TextRegionUPtr_t = std::unique_ptr<TextRegion>;
}
}

namespace game
{
namespace player
{
    class Character;
    using CharacterPtr_t = Character *;
    using CharacterPSet_t = std::set<CharacterPtr_t>;
}

namespace stage
{

    //A Stage class that displays saved characters and allows grouping them into a party of six
    class PartyStage
    :
        public sfml_util::Stage,
        public sfml_util::gui::callback::IListBoxCallbackHandler,
        public sfml_util::gui::callback::IFourStateButtonCallbackHandler_t,
        public popup::IPopupHandler_t
    {
        PartyStage(const PartyStage &) =delete;
        PartyStage & operator=(const PartyStage &) =delete;

    public:
        PartyStage();
        virtual ~PartyStage();

        inline virtual const std::string HandlerName() const { return GetStageName(); }
        virtual bool HandleCallback(const sfml_util::gui::callback::ListBoxEventPackage &);
        virtual bool HandleCallback(const sfml_util::gui::callback::FourStateButtonCallbackPackage_t &);
        virtual bool HandleCallback(const popup::PopupResponse &);
        //
        virtual bool HandleCallback_BackButton();
        virtual bool HandleCallback_StartButton();
        virtual bool HandleCallback_DeleteButton();

        virtual void Setup();
        virtual void Draw(sf::RenderTarget & target, const sf::RenderStates &);
        virtual std::size_t NumCharactersInTheParty() const;
        virtual sfml_util::gui::ListBoxItemSPtr_t GetSelectedItemSPtr() const;
        virtual player::CharacterPtr_t GetSelectedCharacter() const;

        virtual void UpdateTime(const float ELAPSED_TIME_SECONDS);

        virtual void UpdateMousePos(const sf::Vector2i & MOUSE_POS_V);

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
        sfml_util::gui::FourStateButtonUPtr_t backButtonUPtr_;
        sfml_util::gui::FourStateButtonUPtr_t startButtonUPtr_;
        sfml_util::gui::FourStateButtonUPtr_t deleteButtonUPtr_;
        sfml_util::gui::ListBoxUPtr_t characterListBoxUPtr_;
        sfml_util::gui::ListBoxUPtr_t partyListBoxUPtr_;
        sfml_util::gui::TextRegionUPtr_t insTextRegionUPtr_;
        sfml_util::gui::TextRegionUPtr_t upTextRegionUPtr_;
        sfml_util::gui::TextRegionUPtr_t partyTextRegionUPtr_;
        sfml_util::gui::TextInfo warningTextInfo_;
        sfml_util::gui::TextRegionUPtr_t warningTextRegionUPtr_;
        sfml_util::sliders::Slider<sf::Uint8, float> warningTextSlider_;
        OuroborosUPtr_t ouroborosUPtr_;
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
        player::CharacterPtr_t mouseOverCharPtr_;
        sf::Texture     mouseOverTexture_;
        bool            isMouseOverTexture_;
        sfml_util::gui::TextRegionUPtr_t mouseOverTextRegionUPtr_;
        sfml_util::sliders::ZeroSliderOnce<float> mouseOverSlider_;

        //
        game::player::CharacterPSet_t charactersPSet_;
    };

}
}

#endif //GAME_PARTYSTAGE_INCLUDED
